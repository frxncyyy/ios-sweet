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

The ebfa71f6 hardware test captured a narrower failure: both patch spans were
copied and verified, then CID40 advanced TX17 DIDX and set completion bit27.
Transport error16 rejected descriptor pointer/SDL0/DW2/DW3 before checking
DMADONE. Bit27 is ordinary TX completion; coherent error bits20/21 were clear.
The raw descriptor and DDONE were not logged, so the mismatching field remains
unknown. The phone was restored to e0d3 and confirmed healthy.

Completion now waits for DMADONE before applying the unchanged strict echo
checks, matching the selected driver's ordering. The 1000-poll bound, coherent
fault checks and retained failure lifetime remain. Captured TXDESC/TXSUBMIT/
TXOBSERVED lines record the constructed intended words, latest successfully
copied descriptor, descriptor address, DIDX, length, validity, mismatch mask and
DDONE wait count without extra backend reads. Valid bit0 means constructed,
not read-back; bit1 means observed. Mismatch bits0..3 identify pointer, SDL0,
DW2 and DW3 and are evaluated only after DDONE. A completed mismatch still
halts before any RX event processing or rearm. No command replay is added.

The 923d7cea hardware test then captured expected DIDX1 and DDONE1 with
submitted words `90005000/40440000/0/0` and observed words
`90005000/80000000/0/0`. SDL0 and LastSec0 had been consumed; the pointer and
unused words matched. The strict SDL0 echo caused error16. This evidence does
not support the earlier partial-writeback hypothesis. No firmware event was
received before the guard halted. The phone was restored to healthy e0d3.

The transport now accepts that exact DDONE-only control word as an additional
completed form, after expected DIDX, DDONE, pointer and unused-word checks.
Zero length with any other control bit and a wrong nonzero length still fail.
The original-length branch retains its previous policy. Software `used` remains
the authoritative buffer extent; consumed length zero never supplies an extent
or address. This is a narrow exception based on the measured device, not a
claim that every MT6893 descriptor uses this writeback form. A matching actual
RX event is still required to advance command completion; no response is
fabricated and no retry, reset or extra backend access is added.

`TXWB` records constructed/observed validity, DDONE, accepted consumed-zero
classification and software length. Mismatch bit4 rejects zero length with
unexpected control bits. The flag is set only after all completion checks
pass, and is independent of firmware acknowledgment. The captured failure
drain remains. Struct fields are appended and all consuming translation units
must rebuild together. Host tests include the measured shape with synthetic
RX, invalid zero-control forms and a wrong response sequence; hardware success
remains unverified.

The a8cc8050 test accepted the measured TX completion and copied a 36-byte
RX frame (RXD count36, INIT count12). It then failed the packet-type/EID/
sequence guard before event completion or status/CID validation. The raw
header was not logged, so none of those four bytes is known yet. No PHY or
main firmware-ready result was reached. The phone was restored to healthy
e0d3. The lengths fit the selected RXD24 + INIT8 + result4 layout but do not
establish response identity or success.

Captured-only RXDESC/RXWORDS/RXHEADER diagnostics now record the existing
read's descriptor, expected buffer, DIDX and bounded copied header fields.
Validity bits distinguish expected context (1), captured DIDX (2), descriptor
(4), header (8) and status/CID (16). Status/CID are recorded only for a copied
frame of at least34 bytes with packet type0xe000 and EID1; PHY payload is not
logged. The trace is the latest captured data, not an atomic hardware snapshot.
Acceptance, rearm, polling and backend accesses are unchanged. The selected
source checks EID, sequence and status; the native CID comparison is an extra
strict check, and native polling budgets are shorter than the source's
response budgets. Neither difference explains this earlier header refusal
without the missing raw data. Appended state requires all consumers rebuilt.
