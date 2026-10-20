# Nord 2 input and LK overlay handoff

2026-10-20: first implementation, **not yet tested on hardware**.

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
| Power | MT6359 TOPSTATUS; `pmic_get_pwrkey_deb` | PWRAP WACS2 read of `0x2a`, bit1 active-low |
| Volume up | `kpd.c` sets `homekey_as_vol_up` | Same PMIC register, bit3 active-low |
| Volume down | keypad DT, `kpd.c` | GPIO155 active-low |

The misleading keypad DT `volume-up=<20>` is **not** used by the Linux
driver when HOMEKEY is volume-up. GPIO20 belongs to touch reset.

`nord2input=1`, full-userland mode23, and the eight-core readiness check are
all required. A single polling thread starts two seconds after root device
publication. It preserves pinmux, direction, output latch and input-enable
bits. GPIO I2C uses a zero output latch and switches direction to release
each line, never drives a high level, supports bounded clock stretching,
ACK/NACK and repeated START, and finishes transactions with STOP.
It does not configure MediaTek I2C/APDMA, clocks or PMIC regulators.

Touch reads exactly the Oplus packet sequence: 15 bytes from register01,
then either one byte from3e to acknowledge, or 47 bytes from10. The decoder
validates IDs, event flags, duplicates, coordinates and active count before
dispatching through `sweet_hid_touch`. Existing Sweet HID normalization and
edge gestures remain unchanged. Keys are debounced across two polls and
dispatch through `sweet_hid_key`. Persistent read failures release held
contacts/PMIC keys. GPIO polling and I2C transactions are not a latency or
throughput claim; real interaction still needs verification.

The MT6360 LDO2 VTP supply is marked default-on in the device tree, but
its state at XNU entry is **not established**. This implementation leaves
the supply unchanged and only pulses the board's touch reset pin. If ID
reads fail, logs report pin state, bus idle and ID; touch polling is withheld
while buttons continue. Do not infer a working supply from the DT default.
No flash, firmware, calibration, persistent PMIC or touchscreen mode writes
are made.

Return-to-rescue first stops other CPUs, drains any owned PWRAP read,
releases the software bus and restores only saved pin fields. It refuses
to chain into Linux while a wrapper transaction remains active.

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

## Validation

`tools/nord2-check-input.py` compiles the exact C transport, decoder and
cleanup code with modeled GPIO/PWRAP/HID and ASan/UBSan. It covers x4 edge
coordinates, ten fingers, up events, malformed/duplicate IDs, invalid counts,
100,000 random reports, change-only dispatch, I2C read/address NACK/clock
timeout/STOP/length bounds, read-only PMIC commands, wrapper ownership and
refusing a return with an active transaction.

`tools/nord2-check-display.py` covers the original GUI gates and DSI timeouts
plus two-pipe overlay validation, leaving unexpected layouts unchanged,
preserving GUI layers and exact restoration. These are host models, not
hardware validation.
