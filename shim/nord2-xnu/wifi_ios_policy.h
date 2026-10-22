#ifndef WIFI_IOS_POLICY_H
#define WIFI_IOS_POLICY_H
#include "wifi_scan_service.h"
#include "wifi_ios_mapping.h"
int niwi_set_shape(unsigned,const void*,size_t,const char[16],uint32_t*);
int niwi_scan_envelope(unsigned,const void*,size_t,const char[16]);
int niwi_scan_admit(const uint8_t*,size_t,const struct nss_info*,struct nss_request*);
const struct niwp_rate_map*niwi_rate_map(size_t*);

/* Pure shape only: caller must independently require the real owner ready. */
int niwi_power_on_admit(const void *, size_t);
#endif
