#ifndef PRIVATE_NIWA_SNAPSHOT_H
#define PRIVATE_NIWA_SNAPSHOT_H
#include "wifi_scan_bss.h"
#include "wifi_ios_scan_contract.h"
#include "wifi_ios_cursor.h"
#include "wifi_ios_mapping.h"
enum niwa_status {NIWA_PENDING=1,NIWA_PARTIAL,NIWA_TIMEOUT,NIWA_COMPLETE};
enum {NIWA_OMIT_RSSI=1u,NIWA_OMIT_CHANNEL=2u,NIWA_OMIT_IES=4u,NIWA_OMIT_RATES=8u,NIWA_OMIT_OTHER=16u,NIWA_OMIT_BSS=32u};
struct niwa_snapshot {enum niwa_status status;unsigned entered,count,backend_count,dropped,omitted,omit_mask;
 uint8_t completion_state,completed_channels;uint64_t generation;
 struct niwc_cursor cursor;uint64_t observed_us[NSB_RESULTS],reference_us;
 unsigned retained_view,source_overflow;uint8_t rows[NSB_RESULTS][NIWS_SCAN_RESULT_BYTES];};
/* Copied immutable scan input + caller-owned zeroed output retained through
 * cursor lifetime. Caller serializes actual backend owner/generation and must
 * not relocate/copy this object once prepared. No runtime authority granted.
 * age_raw has one separately verified Apple age-unit value per actual record;
 * no unit conversion guessed. rate map must be separately proven, not supplied
 * as a permission boolean. Unknown RSSI/rates, channel mismatch or truncated IEs omit and count
 * that individual row; a completed all-omitted scan is a real empty cache.
 * Malformed scan/map/cursor state still refuses globally. Original IEs remain
 * byte-exact; known membership selectors are not fabricated numeric rates.
 * Channel flags contain proven band and selected gen4m RADAR-channel DFS
 * metadata (52..64/100..144); width stays unspecified. No TX permission. */
int niwa_snapshot_prepare(const struct nsb_scan *,uint64_t,
 const uint32_t *,size_t,const struct niwp_rate_map *,size_t,struct niwa_snapshot *);
/* Before publication only: filter a freshly prepared complete cursor, never a consumed cache. */
int niwa_snapshot_bss_filter(struct niwa_snapshot *,unsigned);
/* Port display-cache policy, not a proprietary Apple timing claim. Native
 * scan provenance/connection authority stays separate. Caller owns both heap
 * objects under its gate; they must be disjoint and not relocated while read. */
#define NIWA_HISTORY_TTL_US UINT64_C(30000000)
struct niwa_history {
 unsigned count;uint64_t generation,reference_us;
 uint64_t observed_us[NIWC_MAX_RECORDS];
 uint8_t rows[NIWC_MAX_RECORDS][NIWS_SCAN_RESULT_BYTES];
};
void niwa_history_clear(struct niwa_history *);
/* Only actual state7 complete or exact full-cache-overflow completion; no
 * failed/timed-out/canceled completion. Captured/reference share native uptime
 * microseconds. Keeps BSSID+SSID keys and original observation times. Atomic
 * refusal; per-row metadata omissions never refresh earlier observations. */
int niwa_history_prepare(struct niwa_history *,const struct nsb_scan *,uint64_t,
 uint64_t,uint64_t,const struct niwp_rate_map *,size_t,unsigned,struct niwa_snapshot *);
/* Gate-held1 copied row/0 exhausted/-1 refusal. Expired rows are skipped,
 * original timestamp ages recomputed per read; TTL is never refreshed by GET. */
int niwa_snapshot_take_timed(struct niwa_snapshot *,struct niwa_history *,uint64_t,uint64_t,uint8_t *,size_t);
#endif
