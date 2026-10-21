# Retained scan-service host fixture

Run `python3 tools/test-nord2-wifi-scan-service.py`. This uses current canonical shim sources, the existing real-owner normal fixture, and the authenticated board file read privately. It never opens a device. Temporary copied sources instrument only the existing simulated hardware/readback seams; native ownership and checked I/O guards remain active.

`continuation-main.inc` adds repeated scans, five-ring and segmented cursor continuity, packet/TX wrap, finite completed-counter exhaustion, canceled queued/inflight tickets, deferred callback release, preparation refusal/restore, timeout and fatal drain. It also checks pending-DDONE and continuous incoming4096-frame prepublication refusal while preserving already-rearmed cursors. A separate invocation verifies the default-off boot26/park path. Clang ASan/UBSan use fail-fast behavior. `--log-dir` optionally records synthetic logs; the default writes no repository output.

The host-only IOLock/wakeup/ready hook seam lives in the shared normal fixture's `nss_host.h`. It models locking and scheduler boundaries; this is not a proof of live IOKit scheduling, Apple user-client ABI, association or networking.
