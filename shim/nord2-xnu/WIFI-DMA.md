# Nord2 post-ROM DMA preparation

Opt-in `nord2wifidma=1` requires `nord2wifirom=1`. The loader validates the
current original FDT, selects the source-sized dynamic Wi-Fi pool at
`0x90000000+0xf20000`, and records a distinct 24-byte
`/chosen/nord2-wifi-dma-claim`. It excludes fixed reservations, the original
FDT, loader/stack, Linux image, payload, pstore and high XNU RAM. This is an
explicit iOS allocation policy, not a recovered bootloader allocation.

After the same native ROM owner has reached ROM_DONE, the new owner wakes
AP/WF and checks the live release/power/protection/ID state. A read-only
snapshot covers both WFDMA globals and all 27 rings affected by the selected
Android initialization. A complete inherited or unstable configuration
returns status 1 after balanced sleep, without setting memory protection or
writing the pool. Unknown failures retain resources for manual reset.

The accepted unprogrammed profile requires zero base addresses, extensions
and producer/consumer indices, with counts either zero or the observed 512.
512 is an observed boot value, not a source-documented reset default. Both
engines must remain disabled and stable. Unknown counts are refused.

Only this unprogrammed, stable snapshot permits source-ordered region18 SET calls
(group 1, then group 0). Both raw x0 status values must be zero; all returned
registers are logged. SET may lock and is never retried or rolled back. The
full pool is mapped once outside XNU's high DRAM window using
`ml_io_map_unmappable(..., VM_WIMG_IO)`. This keeps Device attributes while
allocating virtual space in `kernel_map`; ordinary `ml_io_map` uses an 8 MiB
permanent submap and cannot hold this 15.125 MiB claim. The mapping is retained
for the entire DMA lifetime, including uncertain failures. Only the first
128 KiB is zeroed and read back as aligned 32-bit words. The final hardware
operation releases the new AP vote.

This phase does not enable DMA, submit descriptors, copy the main firmware,
calibrate the radio, scan, or expose a network interface. The pool's CPU
readback alone does not establish radio DMA visibility. Subsequent transport
must retain the 32-bit address and no-cache-alias contracts.

Source: selected Android kernel commit
`04d00c2565764c3922c9b4826b5acff1106e533f`, MT6893 `plat_priv.c:289-313`
(region18), SOC3_0 manual prefetch/ring setup and AXI DMA32 allocation.
The private source audit and hardware evidence live in `research/nord2/wifi`.

Run `python3 tools/test-nord2-wifi-dma.py` for FDT collision/claim encoding,
112-read preflight faults, actual native binding, receipt/claim guards,
128 KiB boundaries and retained-failure tests. Full kernel CI and exact
image verification remain necessary before each hardware test.

The first accepted count-512 hardware run completed both SET calls, then
panicked with `KERN_NO_SPACE` while mapping the whole pool through the
8 MiB submap. Its preserved boot log establishes this allocation failure;
it does not establish zeroing, DMA enable or main-firmware startup. The
kernel-map correction still needs its own hardware test.
