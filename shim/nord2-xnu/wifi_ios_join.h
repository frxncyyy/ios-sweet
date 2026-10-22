#ifndef WIFI_IOS_JOIN_H
#define WIFI_IOS_JOIN_H
#include <stddef.h>
#include <stdint.h>
#include "wifi_scan_bss.h"
#include "wifi_scan_service.h"
#ifdef __cplusplus
extern "C" {
#endif
#define NIWJ_ASSOC_BYTES 592u
#define NIWJ_AP_RSN_BYTES 257u
#define NIWJ_WPA2_PSK_CCMP 1u
/* Copied kernel-only secret input; never a user pointer or authority receipt.
 * Framework supplies PMK32 for both ordinary passphrase and ready-PMK input.
 * Caller and backend must explicitly clear their copies on every retirement. */
struct niwj_request {
 uint8_t ssid_length,ssid[32],has_target,target[6],pmk[32],station_rsn[22],association_reason;
 uint32_t security,native_flags;
};
enum niwj_refusal {NIWJ_OK=0,NIWJ_SHAPE=-1,NIWJ_PROFILE=-2,
 NIWJ_CREDENTIAL=-3,NIWJ_SECURITY=-4,NIWJ_OPTIONS=-5,NIWJ_TARGET=-6};
struct niwj_diagnostic {
 uint32_t mode,auth_lower,auth_upper,rsn_bytes;
 uint32_t reason,key_kind,key_bytes,native_flags,association_flags,association_reason;
 uint32_t cached_pmk_bytes,blacklist_count,scope_id,conditional_option,transition_disable,band_preference;
};
/* Secret output cleared before refusal; no full-record zero sweep. Disjoint
 * buffers required. Diagnostic contains numeric metadata only, never secrets. */
int niwj_decode(const void*,size_t,struct niwj_request*,struct niwj_diagnostic*);
void niwj_clear(void*,size_t);
struct niwj_target {
 struct nsb_record record;
 uint8_t band,channel,rate_count,rates[12]; /*500kbps; basic flags retained*/
 uint16_t ap_rsn_bytes;uint8_t ap_rsn[NIWJ_AP_RSN_BYTES];
 int16_t rssi_dbm;
};
/* Pure retained-record eligibility, never native ownership/readiness. Source
 * nsb_add's full-cache overflow preserves all16 actual records. With matched
 * state7 completion and monotonic complete lifetime, overflow is the only
 * partial cause remaining; timeout/backward-clock/incomplete scan refuse. */
static inline int niwj_scan_usable(const struct nsb_scan*s){
 return s&&s->entered&&s->done&&!s->active&&!s->timed_out&&
  s->completion.state==7&&s->completion.sequence==s->sequence&&
  s->count<=NSB_RESULTS&&s->count<=s->frames&&s->dropped<=s->frames-s->count&&
  s->last>=s->started&&s->deadline>s->started&&s->last<s->deadline&&
  (s->partial?(s->dropped&&s->count==NSB_RESULTS):!s->dropped);
}
/* Owner-thread pure selection from its immutable actual complete scan and
 * allowed-channel snapshot. Does not establish freshness/airtime/ownership.
 * Cached AP must be authenticated/associated and its RSN verified by native
 * transaction; a beacon never establishes connection or permits ordinary TX.
 * Atomic output; no target/country/rate/index is invented. */
int niwj_select(const struct niwj_request*,const struct nsb_scan*,
 const struct nss_info*,uint64_t now_us,struct niwj_target*);
#ifdef __cplusplus
}
#endif
#endif
