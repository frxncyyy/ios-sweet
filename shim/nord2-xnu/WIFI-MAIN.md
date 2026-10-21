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

The pre-copy SW_SYNC0 check treats only the full mask `3` as an already
running main firmware. Bit0 is INIT_DONE and bit1 is N9_DONE; either bit
alone is not the selected driver's ready condition. Refusing the full mask
before download is an additional conservative policy: the selected ROM/patch
download path omits that precheck. Receipt and ownership checks still apply.
`PRESPAN` and `PREORDER` log captured refusal reasons, SW_SYNC0 validity/value,
state groups and the original native error before the coordinator marks the
shared lifetime failed. These diagnostics perform no additional MMIO reads.
The b60dd9bc hardware test completed DMA initialization but stopped before
patch copy; its original SW_SYNC0 value was not logged, so this source defect
is not yet proven to be that test's cause.

Run `python3 tools/test-nord2-wifi-main.py` for host protocol, scheduler,
initializer and transport checks, or add `--only pre-span` for the focused
native readiness/refusal regression. Full kernel CI, packed-image verification
and a separately authorized hardware test are still required. UFS and the
working display/input paths are unchanged.

Uncertain failures now allow a 30-second log-drain interval before the existing
terminal panic. The failed normal thread sleeps to a fixed absolute deadline
only when preemption and interrupts are enabled. The shared radio lifetime
stays halted and retains its DMA memory; there are no new radio or USB accesses,
retries or cleanup operations. The existing CPU0 USB thread can continue
servicing the host while this thread sleeps. Unsafe contexts still panic
immediately. This provides a capture opportunity, not a delivery acknowledgment
or a remedy for a preceding bus hang. `--only drain` tests the actual helper's
fixed deadline, early wakeups, one-use and unsafe-context behavior.

The 0ce01268 hardware test also bootlooped. Its initial USB capture ended during
MAIN ARM2, and its final log did not survive restoration. Its actual failure
stage, original error and SW_SYNC0 value remain unknown. The phone was restored
to e0d3 and confirmed healthy; the drain change does not claim to fix that radio
failure. UFS and boot-speed tuning remain closed and unchanged.
