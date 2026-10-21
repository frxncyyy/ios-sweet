# Nord 2 Wi-Fi ROM startup

`nord2wifirom=1` extends the tested persistent CONN initialization through
authenticated ROM placement and the firmware's `ROM_DONE` marker. It does not
provide a network interface, scanning, association, or packet transport.
The option is absent by default. Use it with `nord2wifipersist=1`, without the
older `nord2wifiwake` or `nord2wifiwfsys` diagnostic options.

## Memory and policy

The loader validates the original current-boot LK FDT before explicitly
selecting `0x8fb00000 + 0x450000` for CONN. It checks DRAM containment,
the exact dynamic CONN reservation, fixed reservation overlap and memreserve
overlap. It refuses inherited fixed CONN tags in this route. The original
FDT is unchanged. A 24-byte little-endian `/chosen/nord2-conn-claim` records
base, size, source and validity in the runtime ADT; the packaged ADT starts
with a zero record. The XNU allocator stays in its existing high RAM window.
This explicit allocation policy does not prove the absence of undocumented
secure-world or other-master users.

Before powering WFSYS, the native path issues the selected Android source's
region-27 SET requests: SMC `0xc200050b`, action 0, group 1 then group 0.
Each raw result is captured, and only x0=0 is accepted. There is no alternate
FID, getter, unlock, retry or speculative rollback. Group 0 can establish a
monitor software lock before validation; any uncertain result requires a
manual reset. SET acceptance alone is not proof of effective CONN access.

The four authenticated ROM spans are copied in source order, totaling
346,720 bytes. Every original destination byte is saved before the first
write; copies use at most 4,096 bytes per chunk. Full readback and two DSBs
are required. The same native object's receipt must report two accepted
SETs, phase 7, 88 writes, 176 reads and two barriers before CPU release.
The device mapping, firmware buffers and backup persist after success.
With the pinned XNU high-RAM layout, `ml_io_map` selects Device-nGnRnE for
this low range. New aliases or allocator changes invalidate that assumption.

## Fresh radio lifetime

Firmware preparation runs in the sleepable input thread after UFS is ready,
before acquiring the new radio lifetime. The new owner is independent of
the earlier, already asleep diagnostic owners. It requires the retained
phase-10 common initialization and fresh AP/WF votes, readability, and five
cold reset/power observations before the SET requests.

After WFSYS power, clock and protection sequencing, local ID `0x20010000`
gates placement. A second receipt check gates AP2WF and reset writes.
`cpu_release_attempted` is latched before deasserting reset; an uncertain
write cannot be reported as safely held. `0x184c1604 == 0x1d1e` is polled
for at most 102 reads, matching the selected source's loop.

On success, WFSYS and the firmware CPU remain running. The host releases
its PLL, semaphore, WF and AP votes; AP sleep is the last MMIO operation.
Post-sleep reports use captured state only. On failure after side effects,
resources are retained and the kernel stops for manual reset. Ordinary
Android must not be booted over the installed iOS filesystem; use the
verified RAM rescue image when needed.

## Source and verification

Android source pin: `frxncyyy/android_kernel_oplus_mt6893`,
`04d00c2565764c3922c9b4826b5acff1106e533f`, principally
`drivers/misc/mediatek/connectivity/wlan/core/gen4m/chips/soc3_0/soc3_0.c`
and `connectivity/conninfra/platform/mt6893/mt6893_pos.c`.

Canonical host tests:

```
python3 tools/test-nord2-wifi-rom-loader.py
python3 tools/test-nord2-wifi-rom.py
```

They exercise the actual loader, contract, placement, native adapter,
power core and native binding with saved private input files and mocked
hardware. Fault injection includes both SET groups, every placement and
power callback, readback corruption, uncertain CPU release and ROM timeout.
Host tests and compilation do not establish hardware ROM startup; the next
approved boot must supply that evidence. `tools/nord2-build-loader.sh`
rebuilds the matching loader and FDT helper for each packaged image.
