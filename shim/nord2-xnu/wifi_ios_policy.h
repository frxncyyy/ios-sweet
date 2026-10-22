#ifndef WIFI_IOS_POLICY_H
#define WIFI_IOS_POLICY_H
#include "wifi_scan_service.h"
#include "wifi_ios_mapping.h"
/* Numeric scan-admission diagnostics only; never log request payloads. */
enum niwi_admission {
 NIWI_ADMIT_ARGUMENT=-1,NIWI_ADMIT_PROFILE=-2,NIWI_ADMIT_OWNER=-3,
 NIWI_ADMIT_OPTION=-4,NIWI_ADMIT_FLAGS=-5,NIWI_ADMIT_PEER=-6,
 NIWI_ADMIT_CHANNEL=-7,NIWI_ADMIT_CHANNEL_FLAGS=-8,NIWI_ADMIT_ALLOWLIST=-9
};
/* Same-IPSW builder: SCAN_LOW_PRIORITY/SCAN_LOW_LATENCY booleans. */
#define NIWI_SCAN_LOW_PRIORITY 0x2u
#define NIWI_SCAN_LOW_LATENCY 0x20u
int niwi_set_shape(unsigned,const void*,size_t,const char[16],uint32_t*);
int niwi_scan_envelope(unsigned,const void*,size_t,const char[16]);
int niwi_scan_admit(const uint8_t*,size_t,const struct nss_info*,struct nss_request*);
const struct niwp_rate_map*niwi_rate_map(size_t*);

/* Pure shape only: caller must independently require the real owner ready. */
int niwi_power_on_admit(const void *, size_t);
#endif
