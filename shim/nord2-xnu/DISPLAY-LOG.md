# Nord 2 on-screen early log — specification

Goal: see the XNU early console on the panel during a boot that hangs, so one
boot (and a phone photo) replaces the gate-per-reboot walk. The pmsg ring is
lost on the PMIC cold reset that follows a hang (`bootreason=PMIC_cold_reboot`,
see `tools/nord2-pull-logs.sh`); the panel keeps its last frame until reset.

Status (2026-10-20): **phases 1 and 2 verified on hardware.** The console
shows XNU through IOKit initialization. After `arm_vm_init`, permanent
TTBR1 mappings keep it working across thread switches; see `README.md` and
the active handoff in `NOTES.md`. The numbered phases below preserve the
original implementation plan, not a list of remaining tasks.
Phase-1 results are in `NOTES.md` (session 5) and
`backup/nord2/logs/20261019-151659-display-phase1/`: dual pipe, four
540x2400 layers, two full-screen physical buffers in the carve-out
(`0x76e10000` pitch 4352, `0x77806000` pitch 4320), 32 bpp, no mutex enabled
at the stop. Sources: the running
kernel's driver (`../nord2-current-kernel/drivers/gpu/drm/mediatek`, HEAD
04d00c25) and `dtb/nord2/live.dts`. Facts below are cited; anything about the
state LK leaves behind is a hypothesis until phase 1 has read it back.

## Why writing the framebuffer is not enough

The panel (AMS643YE05, `oplus20615_samsung_ams643ye05_1080p_dsi_cmd`) is DSI
**command mode**: the panel has its own RAM and only updates when the host
pushes a frame. Android does this per frame from a GCE (CMDQ) loop
(`mtk_crtc_start_trig_loop`, mtk_drm_crtc.c:4212). At XNU time no GCE loop
runs, so after drawing, the CPU must start one frame transfer itself.

## Hardware facts (live DT)

| block | base | DT node |
|---|---|---|
| DISP_OVL0 | 0x14000000 | `disp_ovl@14000000` |
| DISP_OVL0_2L | 0x14001000 | `disp_ovl@14001000` |
| DISP_OVL2_2L | 0x14002000 | |
| DISP_RDMA0 | 0x14003000 | |
| DISP_OVL1 / OVL1_2L (pipe 1) | 0x14100000 / 0x14101000 | |
| DSI0 | 0x1400e000 (size 0x1000) | `mediatek,mt6885-dsi` |
| DISP_MUTEX | 0x14117000 (size 0x1000) | `mediatek,mt6885-disp-mutex` |
| framebuffer carve-out | 0x76e10000 + 0x1df0000 | `mblock-18-framebuffer` (reserved) |

The framebuffer carve-out is outside the XNU RAM window used by the current
loader (kernel at 0x143004000 in the v5 log). Android drives this panel with
**dual pipe + DSC** (dmesg: `connect dual pipe path`, `DDP_COMPONENT_DSC0,
w:1080, h:2400, slice(540,30)`); whether LK does the same is unknown.

## Register offsets (driver source)

DSI0 (mtk_dsi.c): `DSI_START` 0x00, `DSI_INTSTA` 0x0c, `DSI_MODE_CTRL` 0x14;
`DSI_INTSTA` bits: `CMD_DONE` bit1, `TE_RDY` bit2, `FRAME_DONE` bit4,
`BUFFER_UNDERRUN` bit12, `DSI_BUSY` bit31. `mtk_dsi_start()` writes 0 then 1
to `DSI_START` (line 1735).

MUTEX (mtk_drm_ddp.c, MT6885 data): `MUTEX_EN(n)` = 0x20 + 0x20·n,
`MUTEX_SOF(n)` = 0x2c + 0x20·n, `MUTEX_MOD0(n)` = 0x30 + 0x20·n,
`MUTEX_MOD2(n)` = 0x34 + 0x20·n (name as in the source). Android's crtc0 uses mutex id 0
(`pipe * DDP_PATH_NR + i`, mtk_drm_crtc.c:8376); `mtk_disp_mutex_enable()`
writes 1 to `MUTEX_EN(id)`.

OVL (mtk_disp_ovl.c): `OVL_EN` 0x0c, `ROI_SIZE` 0x20, `SRC_CON` 0x2c (bit n =
layer n enabled), `L_CON(n)` 0x30 + 0x20·n (bits 15:12 colour format, 24 byte
swap, 25 RGB swap), `L_SRC_SIZE(n)` 0x38 + 0x20·n (h<<16 | w),
`L_PITCH(n)` 0x44 + 0x20·n (low 16 bits), `L_ADDR(n)` 0xf40 + 0x20·n (MT6885).

Per-frame sequence in command mode (trigger loop, mtk_drm_crtc.c:4335):
`MUTEX_EN(mutex) = 1`, then DSI trigger (`DSI_START = 1`), then wait for the
frame-done event.

## Phase 1 — read-only dump (no hang risk, log survives)

Inside the existing `cpu_init` stop (stage 6), which always chains back to
Android with the pmsg ring preserved (v5/v7/v8), read and log, without writing:

- DSI0: `DSI_START`, `DSI_INTSTA`, `DSI_MODE_CTRL`;
- MUTEX 0..9: `EN`, `SOF`, `MOD0`, `MOD2`;
- OVL0, OVL0_2L, OVL1, OVL1_2L: `OVL_EN`, `ROI_SIZE`, `SRC_CON`, and for
  layers 0..3 `L_CON`, `L_SRC_SIZE`, `L_PITCH`, `L_ADDR`.

Reading these is safe only because the display is powered (the LK logo is on
screen); do not touch any other display block. The MMIO needs a Device-nGnRE
mapping of 0x14000000–0x14117fff (one 32 MiB block at 0x14000000 covers it)
in the bootstrap map, next to the existing pmsg block.

What phase 1 must establish before phase 2:
1. which mutex is in use (`EN`/`MOD0` non-zero, `SOF` = DSI0 SOF);
2. which OVL layer(s) are enabled and whether `L_ADDR` falls inside
   0x76e10000–0x78c00000 — if it does not, LK is using an IOMMU address and the
   plan below must change;
3. pitch, size and colour format of the active layer (expected 1080×2400,
   pitch 4352 = 1088·4, 32 bpp — unverified for LK);
4. single or dual pipe (are OVL1/OVL1_2L layers enabled?).

## Phase 2 — draw and push

1. Map the active buffer (from phase 1, inside the reserved carve-out) Normal
   Non-Cacheable, like the pmsg block; draw text with `iso_font` as Sweet's
   `sweet_early.c` did, using the measured pitch and format.
2. After each line (or each `sweet_early_mark`): `dsb sy`; if `DSI_INTSTA` shows
   the previous frame still running (`DSI_BUSY`), poll with a bounded count;
   write `MUTEX_EN(mutex) = 1`; write `DSI_START` 0 then 1; poll `FRAME_DONE`
   (bounded), then clear it as the driver does.
3. Ignore TE (worst case: tearing). Never block forever on a status bit: a
   bounded poll that gives up keeps the log path from becoming a new hang.
4. Keep logging to pmsg as well, so successful boots still leave the text log.
5. After `arm_vm_init` drops TTBR0 the V=P mappings disappear; the console
   then needs a kernel-VA alias of the buffer and the MMIO block (Sweet did the
   same for its framebuffer in `sweet_early_map_final`).

## Open risks

- LK may leave the DSI engine or mutex in a state where a CPU trigger is
  ignored or misbehaves; phase 1 is meant to show that before any write.
- With DSC/dual pipe, a single mutex trigger might refresh only half the
  panel; phase 1 point 4 decides.
- If the buffer is in RGB565 or byte-swapped, the drawing code must follow
  `L_CON`.
