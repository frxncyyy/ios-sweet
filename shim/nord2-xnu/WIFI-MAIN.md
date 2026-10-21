# Nord 2 Wi-Fi main firmware

`nord2wifimain=1` adds a polling firmware-loader path after the existing ROM
and DMA-pool stages. It requires both `nord2wifirom=1` and `nord2wifidma=1`.
The older `nord2wififw=1` remains the read-only firmware verification option.

The implementation follows the selected MT6893/SOC3_0 Android source at
`04d00c2565764c3922c9b4826b5acff1106e533f`. It acquires a fresh AP/WF and
driver lifetime from the retained ROM/pool receipts, initializes the nine
source DMA rings and DMASHDL scheduler, and polls completion with peripheral
interrupt output disabled. Source-defined coherent DMA faults stop commands;
no interrupt handler or network interface is installed by this phase.

The firmware sequence is patch payload/date, patch-finish CID40, FORCE_CAL
CID8 with the board's saved calibration, bounded backup of the returned EMI
region, source PCCIF writes, five N9 spans, N9-finish CID41, then SW_SYNC0
FW_READY bits 0 and 1. FORCE_CAL operates the RF hardware. Firmware input
comes from the already verified vendor extents: main blob0 and patch blob1;
ROM blobs2/3 retain their original roles. Private board calibration is added
to the new image's ADT and checked by exact length/hash. Calibration and MAC
bytes are never stored in the source repository.

A successful FW_READY result establishes firmware startup only. Scanning,
association, data traffic and iOS network service integration require further
work. There is no fabricated ready response. Any uncertain failure after
acquiring hardware or changing DMA/firmware state retains the owned memory
and resources and requires a manual reset; it never chains to Android.
A clean refusal before mutation may continue booting iOS.

Run `python3 tools/test-nord2-wifi-main.py` for host protocol, scheduler,
initializer and transport checks. Full kernel CI, packed-image verification
and a separately authorized hardware test are still required. UFS and the
working display/input paths are unchanged.
