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
`python3 tools/test-nord2-battery.py`. The actual gauge/charger reads have
not yet been observed on the phone.
