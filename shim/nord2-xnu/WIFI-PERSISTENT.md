# Persistent MT6893 radio initialization

This opt-in path configures the shared connectivity hardware while keeping
WFSYS/BGFSYS processors off and reset. It does not load firmware, transmit
packets, register an iOS network interface, or establish effective EMI access.
Hardware acceptance remains a separate step from compilation and host tests.

`nord2wifi=2 nord2wifipersist=1` selects this path in the existing input thread's
owned power window, before the restoring common/CHIP_ID diagnostic. Other boots
retain the diagnostic behavior. The persistent mode does not fall back to that
diagnostic if its prerequisites fail.

The source sequence includes EMI address-remap registers, D-die setup, the
common register table, a 16 KiB connectivity SYSRAM clear, A-die E3 setup,
read-mode efuse data, AFE calibration, PLL, subsystem status, RC low-power
configuration, top-clock ownership and hardware-control handoff. The selected
Wi-Fi voltage callback and final PMIC low-power hook are no-ops; no extra PMIC
writes are synthesized for them. The platform build word is 3. Saved vendor
configuration and the source parser select 26 MHz COTMS, without the optional
GPIO155 external-TCXO mux.

The persistent PMIC path sets SW_LP before enabling each rail, preserving the
source VCN18/VCN13 order. The legacy diagnostic keeps its existing operation.
Full-register writes, masked fields, delays and the final no-extra-read handoff
follow the selected source. Transport errors and ambiguous semaphore/SPI
completion stop the sequence and require manual reset.

`wifi_persistent.c` contains the checked callback core;
`wifi_persistent_adapter.c` restricts register access and implements bounded
semaphore/TOP-SPI operations. Both are separate guarded BSD translation units.
`wifi_persistent_bind.inc` maps the selected MMIO/SYSRAM apertures and binds them
to the existing power owner. It never maps the CONN or MCIF DRAM ranges.

Historical CONN `0x8fb00000` and MCIF `0x8a000000` bases are used only as
source-backed remap values. MCIF belongs to the modem's CCCI reservation, not
XNU. Their exclusion from the XNU allocator is not proof of exclusive hardware
ownership or effective permissions. Firmware placement, DMA and CPU release
need separate implementation and evidence.

Before the first radio mutation, the binding latches retained ownership.
Success leaves the power and configuration in place; it must not invoke the
old restoring cleanup. Uncertain failure retains state and panics for manual
reset. `nw_restore`, input-return cleanup and reentry honor this latch. After
the final handoff, no generic CONN read/check is performed: only the source's
last CFG1860 read/write is allowed. Later accesses require a source-backed
wakeup sequence.

Host tests use the actual canonical core and adapter with injected I/O, plus
the exact PMIC helper extracted from the generated XNU patch. They cover core
callback failures, native transport bounds and ownership, terminal handoff,
default-off behavior and PMIC ordering/failures. They do not execute MMIO/SMC
or establish electrical behavior. Run:

```sh
python3 tools/test-nord2-wifi-persistent.py
```

Selected Android source is commit
`04d00c2565764c3922c9b4826b5acff1106e533f` in
[android_kernel_oplus_mt6893](https://github.com/frxncyyy/android_kernel_oplus_mt6893/tree/04d00c2565764c3922c9b4826b5acff1106e533f).
Relevant files are `drivers/misc/mediatek/connectivity/conninfra/platform/`
`mt6893/mt6893_pos.c`, `mt6893_pmic.c`, `consys_hw.c`, and the conninfra
`Kbuild`, configuration parser and platform EMI manager.
