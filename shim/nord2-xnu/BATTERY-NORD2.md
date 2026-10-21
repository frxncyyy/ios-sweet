# Nord 2 battery observation

`nord2battery=1` replaces the Nord 2's fabricated 100%/external-power
report with direct gauge observations. It is separate from the existing
Qualcomm `sweetbattery` path and does not use PM6150 registers.

The selected board source places the gauge on I2C7, address `0x55`.
The provider reads SOC twice from register `0x2c`, accepting 0–100% only
when the samples agree within three points. Voltage is the two-cell pack
voltage; temperature has an independent validity check. Current stays
unknown until the gauge model has actually been identified.

The optional MP2650 charger on I2C11, address `0x5c`, supplies stable
status/fault/status observations. An unavailable charger does not erase a
valid SOC. Charging, external power and termination are derived from those
observations, never from an invented 100% fallback. Failed SOC is published
as unavailable with an error condition, not as an asserted empty battery.

The polled PIO controller path derives timing from the existing clock,
requires the expected pinmux and an idle controller, preserves its initial
configuration, and restores only its owned clock-gate bit after a known
completion. It does not configure the gauge, charger, GPIO mux, PLL or DMA.
After uncertain completion it leaves that bus offline rather than retrying
or resetting it. Captured first-attempt/changed-error diagnostics expose
the raw sample, stage, clock and controller state without extra MMIO.

This relies on the selected Android driver's initial VOOC policy for a
normal PC USB connection; future fast-charge ownership needs explicit
coordination. Hardware IRQ/FIFO behavior and iOS's rendering of unavailable
values still need testing on the next approved image.

Source pin: `frxncyyy/android_kernel_oplus_mt6893` commit
`04d00c2565764c3922c9b4826b5acff1106e533f`. Host verification:
`python3 tools/test-nord2-battery.py`. Actual gauge/charger observations are
recorded below; they do not establish charging control or full UI validation.

## First hardware handoff result

The initial23a9bd30 boot refused bus7 before any gauge command: INTR_STAT
contained only TRANSAC_COMP (bit0), with START idle and DMA off. This is an
inherited completion latch, not a gauge NACK. The selected Android driver
clears latched status before transfers. Native preflight now adopts only
exact status1 after empty FIFO and repeated idle/DMAdisabled/FIFOempty/status1
checks, acknowledges precisely bit0, and requires status0 readback. Errors,
changing state, nonempty FIFO and uncertain callbacks still leave the bus
offline. No reset, retry, FIFO discard or pinmux change is introduced.
Captured NORD2 BAT PREFLIGHT reports the prior latch and returned acknowledgement;
actual SOC/charger success still requires a new hardware test.

The V2 FIFO preflight uses the low five receive-count bits, following the
selected tree's `drivers/i3c/master/i3c-master-mtk.c` receive paths at lines
776 and 843. Its expanded register map matches `i2c-mtk.h` V2, including
FIFO_STAT at 0xf4. Applying that convention to I2Cv2 is a shared-IP source
inference; no MT6893 field specification was located. Upper bits remain
unnamed. Captured FIFO_STAT 0x840 has count zero under this convention;
nonzero counts are refused. START, DMA, interrupt-error and completion-latch
checks remain independent, and there is no drain/reset/retry fallback.

## Observed reads with the FIFO correction

The 368062de and b60dd9bc images successfully read both I2C buses before
their separate Wi-Fi failures. The latter captured SOC100/100, pack voltage
`0x21e7` (8679 mV), temperature `0xbf6` (3062 decikelvin), charger status
`0x3f/0x3f` and faults0. This is a measured 100%, not the prior constant
fallback. The source's active-low MP2650 power-good bit was not asserted;
that observation alone does not establish USB cable detection or charging
configuration. Current remains unavailable pending gauge identification.
The restored e0d3 fallback predates this FIFO fix, so its battery behaviour
must not be used as validation of the newer provider.
