# Nord 2 XNU early probe

This is a diagnostic boot image for the OnePlus Nord 2 (MediaTek MT6893), not
an iOS boot. It contains the Sweet XNU kernel built for MT6893, a small
Linux-Image loader and a preserved copy of the stock Android kernel. It
writes a log to the Android ramoops `pmsg` ring and, with `nord2fb=1`, to the
DSI command-mode display. The default early stop returns after `cpu_init()`;
later diagnostic images have reached the end of IOKit initialization and
returned to Android. They do not yet boot iOS userland.

The current blocker and exact test results are in the active handoff at the
top of `NOTES.md`. As of 2026-10-20, the corrected CPU-driver matching and
unambiguous `nord2stopat=8` argument have passed on hardware: XNU returns
from `kperf_init_early`, reaches the stop before BSD, and chains back to
Android with pmsg intact. Point 9 has also passed: BSD reaches root
filesystem selection, including network-thread scheduling. The next test,
point 10, only reads standard UFSHCI registers to inspect the MT6893
controller state inherited from LK.

The probe does not write storage or load an iOS ramdisk. It does write the
pmsg and framebuffer reservations, the DSI/MUTEX registers used to present
frames, and the MediaTek watchdog restart register. Past early bring-up it
also initializes the GIC and architected virtual timer.

## Artifact layout

`tools/nord2-pack-xnu.py` reuses the stock Android boot v2 header, ramdisk
and DT table, and replaces only the gzip kernel stream:

```
0x40080000  loader                      (this shim, zero-padded to 2 MiB)
0x40280000  stock Android Image         (byte-identical, BSS zero-padded to
                                         the header's image_size)
0x43744000  SWEETXNU payload            (16 KiB aligned; current stock kernel)
```

LK decompresses the whole stream to `0x40080000` and, because the loader's
Image header branches from its first instruction, runs the loader. The
loader finds the stock Image header at `0x40280000`. The original kernel is
preserved byte for byte at this relocated address, and the chain-back needs
no copying. The payload address is derived from the stock Image's
`image_size` (including BSS); it is not a hard-coded offset in the packer.
The decompressed end varies with the payload. The image checker reports it
and verifies that the blob stays below the ramdisk at `0x51100000`.

The loader places the payload's Mach-O segments in a 992 MiB window in the
upper DDR bank:

```
phys_base 0x1_42000000   mem_size 0x3e000000   virt_base 0xfffffe0006000000
```

`start.s` identity-maps 30 complete 32 MiB blocks after allowing for the
kernel's head offset (`0x1_42000000..0x1_7e000000`); the ADT and `boot_args` sit
just past the kernel image, and the chain-back trampoline is copied to
`0x1_42004000`, the first free page of the mapped window.

## Chain-back

The boot args carry `/chosen` with three zero placeholders that the loader
patches in RAM:

| property | size | value |
| --- | --- | --- |
| `nord2-stock-entry` | 8 | `0x40280000` (stock Image entry) |
| `nord2-stock-dtb` | 8 | the live FDT physical address |
| `nord2-xnu-region` | 16 | first mapped / end of the identity window |

`nord2_maybe_return()`, called from `arm_init.c` after `cpu_init()`, caches
them and, unless the boot argument `nord2stop=0` was given, returns to
Android. Later stops use the same cached values. Before entering
`nord2_leave_xnu()` in `start.s`, the return path masks interrupts and
restores the bootstrap TTBR0 table: kernel-thread switches otherwise
replace it with `invalid_ttep`. The assembly copies its trampoline into the
window, cleans the whole window to the point of coherency, turns the MMU off
with the `SCTLR_EL1` value the CPU probe observed at stock entry
(`0x30d00800`), zeroes `x1..x3`, and branches to the stock entry with
`x0 = dtb`. `nord2stop=1` is also the kernel default.

After `arm_vm_init()`, `nord2_console_remap()` gives pmsg, framebuffer and
console/watchdog MMIO permanent kernel mappings in TTBR1 so logging
survives context switches. Keep `cpus=1` for these tests: secondary CPU
bring-up and returning to Android with secondary CPUs active are unverified.

### Bounded arm_vm_init walk

`arm_vm_init` contains fifteen
numbered gates, compiled in and chosen by the boot argument `nord2vmstop`:
a gate stops the kernel and logs `NORD2-NEXT=0x…`. The loader reads the last
`NORD2-NEXT` out of the preserved ring and rewrites `nord2vmstop=` in the
payload command line, so `nord2-xnu-vmstep.img` advances one gate per plain
reboot; the last `NORD2-NEXT` printed before a hang names the failing step.
The full `arm_init` path has since passed on hardware; these gates remain
available for regressions.

### Later stops

Use `nord2stop=0 nord2vm=1 nord2fb=1 nord2stopat=N` to run to a later
stop and chain back. Use ordinary decimal or explicit hexadecimal (`0x08`),
never zero-padded decimal: XNU parses `0007` as octal 7 but `0008` as a
string, so it does not select point 8. The legacy automatic VM gate walker
also writes zero-padded decimal and must be corrected before reuse.

| N | Stop |
| --- | --- |
| 1 | End of `arm_init` |
| 2 | Before `SweetBattery::registerService` (also an older BSD IOKit hook) |
| 3 | After `SweetBattery::registerService` |
| 4 | After `StartIOKitMatching` |
| 5 | Entry to `bsd_init` |
| 6 | After the copyright print in `bsd_init` |
| 7 | After `vm_set_restrictions`, before kperf |
| 8 | After `kperf_init_early`, before `bsd_init` |
| 9 | In `bsd_init`, after `inittodr`, before selecting/mounting the root filesystem |
| 10 | Same location as point 9, after a read-only UFSHCI register snapshot |
| 11 | Same location, after a bounded LUN2 capacity/GPT read probe and controller-state restoration |
| 12 | After publishing/selecting `ufsroot0`, before mounting; restores the UFS host and returns |

Point numbers are identifiers, not execution order. Never infer success
from a reboot alone: inspect the stop marker and the Android return marker.

Point 10 has passed on the Nord 2: UFSHCI 2.0 is enabled, supports 64-bit
addressing, and has no pending requests. Point 11 also passed: capacity, boot/userdata extents and both GPT CRCs
match the Android baseline, followed by controller restoration and a normal
return to Android. It accepts only TUR, READ CAPACITY(10) and READ(10)
of GPT metadata in LBAs 1–63; checks GPT CRCs and bounds; then restores the
inherited request list and interrupt state before returning to Android.
If a timeout cannot be cleared or restoration fails, it retains its DMA
memory and waits for a manual reboot. No root filesystem is mounted.

Point 12 is prepared but untested. Use `rd=ufsroot0 ufsrw=0 rootrw=0`.
The Nord 2 root driver validates LUN2 capacity and GPT CRCs, accepts only
userdata, and omits Sweet's Qualcomm input/USB and background log startup.
The diagnostic additionally forces read-only operation in the driver.

## Reading the result

After Android starts, the log is `/sys/fs/pstore/pmsg-ramoops-0` (read it
before the next reboot). Run `tools/nord2-pull-logs.sh LABEL` immediately to
save it with Android's boot reason and kernel log. Successful late stops
show the requested marker followed by `RETURN TO STOCK ANDROID KERNEL`.
Watchdog/manual cold resets can clear DRAM and leave pstore empty; in that
case the on-screen log is the remaining evidence. Logs from the earlier
CPU probe are in `backup/nord2/probe-v2-*`.

## Building

Build the complete kernel with the repository's `xnu.yml` GitHub Actions
workflow on macOS (Apple clang). The current fork is `frxncyyy/ios-sweet`,
branch `codex/nord2-bringup`. Download both `xnu-kernel` and `xnu-buildtree`;
record the run ID and head commit in `NOTES.md`.

Package a CI kernel with the existing loader:

```sh
tools/nord2-image-from-kernel.sh PATH/TO/kernel.development.sweet \
  shim/out/nord2-xnu/NEW-TEST.img \
  debug=0xc cpus=1 keepsyms=1 -noprogress \
  nord2stop=0 nord2vm=1 nord2fb=1 nord2stopat=8
```

The helper strips symbols, builds the current ADT and payload, packs the
boot image, and runs `tools/nord2-check-xnu.py`. It refuses an existing output.
It needs the existing `shim/out/nord2-xnu/loader-Image`, cctools in
`../toolchains/cctools`, and the verified `backup/nord2/boot.img`.

Local C/assembly changes can be compiled with `tools/nord2-xnu-cc.py`.
`shim/nord2-xnu/build.sh` supports the older local relink path using the
extracted CI tree and archive in `research/ci-39158169212/`. It does not
rebuild every C++ object from the latest patches. Do not use it to package a
new C++ change, and do not substitute upstream-clang `__builtin_xnu_*`
fallbacks for Apple compiler metadata.

XNU-side edits are carried by `xnu/patches/0082-nord2-early-probe.patch`.
`tools/nord2-mkpatch.sh` regenerates it from the working buildtree, verifies
that it applies to the pristine 0001–0081 CI tree, and compares every
resulting file. Add any newly touched source file to its `FILES` list.

## Flashing

This directory builds an image; it is not permission to flash it. `fastboot
boot` fails on this phone even for the original image, so a test means
flashing **boot only**, entering fastboot manually, and afterwards restoring
`backup/nord2/boot.img` (33554432 bytes, SHA-256
`04e84c4a14e5c3cb77d0e587da93b5a7e720e8c5b2a3a51ab3ef4cda746e1f01`) and
checking the read-back hash and `sys.boot_completed`. Bootloader, recovery,
modem, persist and userdata are never touched.
