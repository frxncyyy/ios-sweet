/* Memory-only diagnostic; no boot hook is installed by this interface. */
#ifndef NORD2_WIFI_FIRMWARE_PROBE_H
#define NORD2_WIFI_FIRMWARE_PROBE_H
/* Caller must run on a dedicated, sleepable kernel thread after UFS root is
 * ready, with no UFS/input/PMIF locks held. Never call from input polling or
 * interrupt context. No readiness retries are attempted. The UFS adapter's
 * per-command timeouts bound IO; allocator mutex contention is still possible.
 * Bootarg must be exactly nord2wififw=1. A single enabled attempt is consumed
 * even on failure; concurrent/repeated attempts return EALREADY. Disabled
 * calls return EPERM and do not consume the attempt. No blobs are retained.
 */
int nord2_wifi_firmware_probe(void);
/* Call once after UFS becomes ready, outside all driver/input locks. Starts a
 * dedicated thread and releases its creation reference. Strict opt-in as above;
 * returns EPERM disabled, EALREADY after one start attempt, EAGAIN on creation
 * failure (no retry), or zero after successful creation. Thread runs the probe
 * then terminates itself; successful creation is not successful verification.
 */
int nord2_wifi_firmware_start(void);
#endif
