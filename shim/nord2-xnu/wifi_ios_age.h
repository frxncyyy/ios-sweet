#ifndef PRIVATE_NIWAA_AGE_H
#define PRIVATE_NIWAA_AGE_H
#include "wifi_ios_snapshot.h"
struct niwaa_ages {uint64_t captured_us,reference_us;unsigned count;uint32_t milliseconds[NSB_RESULTS];};
/* Actual immutable completed scan; timestamps must be from the same monotonic
 * microsecond clock as nsb_record.observed_us. Caller supplies actual capture
 * and reference instants. Pure validation cannot authenticate that provenance.
 * Floor to completed milliseconds; refuse negative/future or signed32 overflow.
 * No saturation, epoch/TSF substitution, clock reads, or native authority. */
int niwaa_age_plan(const struct nsb_scan *,uint64_t,uint64_t,struct niwaa_ages *);
/* Own output exactly as original snapshot API; rate map remains separately
 * proven input. No production map or backend/service hook supplied here. */
int niwaa_snapshot_prepare(const struct nsb_scan *,uint64_t,uint64_t,uint64_t,
 const struct niwp_rate_map *,size_t,struct niwa_snapshot *);
#endif
