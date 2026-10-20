# Nord 2 input and LK overlay handoff

2026-10-20: first image tested; **volume-down works** by both user confirmation
and captured press/release events. Power, volume-up and touch failed. Corrected
PMIF/MT6360 implementation below passes local checks but awaits hardware test.

Source evidence is the captured live Nord 2 DT (`dtb/nord2/live.dts`) and
the user's Android kernel checkout, branch
`KSUN-RUI-susfs-v2.2.0-manual-hook`, commit `04d00c25`.
No device identity or Apple files are needed to build this driver.

| Function | Source evidence | XNU implementation |
| --- | --- | --- |
| Touch | Oplus `Focal/ft3518/focal_drivers_ft3518.c`, `ft3518_core.h`; I2C0 DT | FT3518, address `0x38`, IDs `0x54:0x52` |
| Touch bus | `pinctrl-mt6885.c`, I2C0 DT | GPIO204 SCL, GPIO205 SDA; software open-drain I2C |
| Reset/IRQ | focaltech DT | GPIO20 reset, GPIO21 IRQ hint |
| Touch resolution | `high_resolution_support`, 4320×9600 sensor coordinates | Decode x4 protocol, divide by four to 1080×2400 |
| Power | MT6359 TOPSTATUS; `mtk-pmic-wrap_v1.c` | PMIF SWINF2 read of `0x2a`, bit1 active-low |
| Volume up | `kpd.c` sets `homekey_as_vol_up` | Same PMIC register, bit3 active-low |
| Volume down | keypad DT, `kpd.c` | GPIO155 active-low |
| Touch supply | MT6360 vendor LDO driver; VTP DT | I2C5 GPIO202/203, LDO2 at address `0x64`, 3.0 V |

The first image incorrectly used the legacy WACS2 register layout and logged
status zero. The actual selected `drivers/soc/mediatek/mtk-pmic-wrap_v1.c`
uses PMIF SWINF2 at `0x10026000`: ACC `0xc80`, RDATA `0xc94`,
VLD_CLR `0xca4`, STA `0xca8`; FSM is bits1..3 and INIT_DONE is bit15.
The read command is the raw PMIC address `0x2a`. No PMIC register writes
are used for keys. Pending results are acknowledged only when owned by XNU.

The misleading keypad DT `volume-up=<20>` is **not** used by the Linux
driver when HOMEKEY is volume-up. GPIO20 belongs to touch reset.

`nord2input=1`, full-userland mode23, and the eight-core readiness check are
all required. A single polling thread starts two seconds after root device
publication. It preserves pinmux, direction, output latch and input-enable
bits. GPIO I2C uses a zero output latch and switches direction to release
each line, never drives a high level, supports bounded clock stretching,
ACK/NACK and repeated START, and finishes transactions with STOP.
It does not configure MediaTek I2C/APDMA or clocks. A second software bus
accesses only the identified MT6360's dedicated VTP supply as described below.

Touch reads exactly the Oplus packet sequence: 15 bytes from register01,
then either one byte from3e to acknowledge, or 47 bytes from10. The decoder
validates IDs, event flags, duplicates, coordinates and active count before
dispatching through `sweet_hid_touch`. Existing Sweet HID normalization and
edge gestures remain unchanged. Keys are debounced across two polls and
dispatch through `sweet_hid_key`. Persistent read failures release held
contacts/PMIC keys. GPIO polling and I2C transactions are not a latency or
throughput claim; real interaction still needs verification.

The MT6360 LDO2 VTP node carries `regulator-default-on=<1>`, but that property
is **not evidence that the rail is powered** (the main-PMIC parser even uses
value1 for off, and the separate MT6360 driver does not parse it).
The first image left it unchanged and got FT3518 ID00:00/not-ready. That
does not prove a particular power state. The correction checks MT6360 PMU
ID at address0x34/register0, then reads LDO2 enable0x11 and selector0x15 at
address0x64 with CRC8 (polynomial07). The board DT requires3.0V despite the
`vdd_2v8` name; selector0xaa..0xb0 represents3.0V. An already enabled3.0V
rail is preserved. Otherwise reset is held low, saved enable bit0x40 is
cleared, selector0xaa is written, and bit0x40 is set. Every write follows the
vendor value/CRC/dummy-byte format and is read back. Status bit0x04 must
become ready within20 polls. Writes are restricted to the two saved LDO2
registers, the saved voltage or0xaa, and the enable bit; other modes and
supplies are preserved. Original voltage/enable are restored before rescue.
No touchscreen firmware, flash, calibration or mode writes are made.

Return-to-rescue first stops other CPUs, drains any owned PMIF read,
releases both software buses, restores VTP and then only saved pin fields.
It refuses to chain into Linux while a wrapper transaction remains active
or VTP restoration cannot be verified. Failed disable/readback stops the
restoration sequence before changing voltage or enabling an unverified rail.

## Orange warning

The saved hardware snapshot shows a second full-screen buffer in OVL0 and
OVL1 L1 at `0x77806000`/`0x77806870`, above the GUI's OVL0_2L/OVL1_2L L0 at
`0x76e10000`/`0x76e10870`. This is the suspected stale LK warning layer.

The display driver validates both pipes' source masks, addresses, pitches
and layer dimensions. Only after the first GUI copy and DSI becoming idle
does it disable L1's source bit/RDMA/AFBC bit, following `mtk_ovl_layer_off`.
It preserves the 2L layers, relay path, panel and all bootloader partitions.
The saved overlay registers are restored before returning to rescue.
The bootloader will still show its normal unlocked warning during boot;
the change targets only the stale layer left over the iOS interface.
The first input-image hardware log confirms both inherited layers disabled
and the first eight command-mode GUI presents completed. The user confirms
the orange warning is gone from the iOS lockscreen.

## Validation

`tools/nord2-check-input.py` compiles the exact C transport, decoder and
cleanup code with modeled GPIO/PMIF/HID and ASan/UBSan. It covers x4 edge
coordinates, ten fingers, up events, malformed/duplicate IDs, invalid counts,
100,000 random reports, change-only dispatch, I2C read/address NACK/clock
timeout/STOP/length bounds, read-only PMIC commands, wrapper ownership and
refusing a return with an active transaction.
It also checks masked GPIO/IES restoration with unrelated fields populated.
Power tests cover independent CRC vectors, bad identity/CRC rejection, the
write allowlist, inherited-rail preservation,3.0V enable/readback/restore,
and NACK or mismatched readback preventing an unverified restoration.

Full Apple-clang [CI39468314328](https://github.com/frxncyyy/ios-sweet/actions/runs/39468314328)
passed at source `066ccc735f366b5df58b7dbe46d4239e15e183b0`.
The frozen `nord2-ios-input-power-stop23.img` is33,544,192 bytes,
SHA-256 `fd6d75caf4c7e95431bdb40eb50ca79650e8272824f54fd5f783cdec49995f20`.
Static checks, instruction compatibility, partition size and byte-exact
rescue-content verification pass. The user authorized this exact image; boot
flash and reboot succeeded at20:09:33CEST on2026-10-20. Hardware validation is
in progress. Preserve the first input-image evidence.

Initial corrected-image hardware probes succeed: MT6360 ID0x53, inherited
VTP disabled (`enable=0x80`, `selector=0x40`), enabled3.0V status0xc4;
FT3518 ID54:52/ready1 and PMIF TOPSTATUS0xe/ok1. Eight-core scheduler test
also passes. Idle touch-report rejections still occur and their packet
contents are not logged; gestures and power/up actions await user testing.
The initial USB capture ended at87,806 bytes with four EP0 timeouts; capture
was restarted without rebooting. This is not evidence of a kernel panic.

`tools/nord2-check-display.py` covers the original GUI gates and DSI timeouts
plus two-pipe overlay validation, leaving unexpected layouts unchanged,
preserving GUI layers and exact restoration. These are host models, not
hardware validation.
