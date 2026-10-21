#ifndef PRIVATE_NIWA_SNAPSHOT_H
#define PRIVATE_NIWA_SNAPSHOT_H
#include "wifi_scan_bss.h"
#include "wifi_ios_scan_contract.h"
#include "wifi_ios_cursor.h"
#include "wifi_ios_mapping.h"
enum niwa_status {NIWA_PENDING=1,NIWA_PARTIAL,NIWA_TIMEOUT,NIWA_COMPLETE};
struct niwa_snapshot {enum niwa_status status;unsigned entered,count,backend_count,dropped;
 uint8_t completion_state,completed_channels;uint64_t generation;
 struct niwc_cursor cursor;uint8_t rows[NSB_RESULTS][NIWS_SCAN_RESULT_BYTES];};
/* Copied immutable scan input + caller-owned zeroed output retained through
 * cursor lifetime. Caller serializes actual backend owner/generation and must
 * not relocate/copy this object once prepared. No runtime authority granted.
 * age_raw has one separately verified Apple age-unit value per actual record;
 * no unit conversion guessed. rate map must be separately proven, not supplied
 * as a permission boolean. Unknown RSSI/rates/truncated IEs refuse encoding.
 * Channel flags contain only proven band; width/DFS stay unspecified. */
int niwa_snapshot_prepare(const struct nsb_scan *,uint64_t,
 const uint32_t *,size_t,const struct niwp_rate_map *,size_t,struct niwa_snapshot *);
#endif
