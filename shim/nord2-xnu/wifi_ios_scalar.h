#ifndef WIFI_IOS_SCALAR_H
#define WIFI_IOS_SCALAR_H
#include "wifi_ios_control.h"
#include "wifi_scan_service.h"
/* Driver-owned observed native state, never copied from an IOUC request.
 * SCANNING is the native RUNNING transaction (including preparation/drain),
 * not a queued client ticket or a claim of physical RF emission.
 * This pure encoder authenticates neither the owner nor the observation. */
enum niws_native_state { NIWS_UNAVAILABLE=0, NIWS_IDLE=1, NIWS_SCANNING=2 };
/* GET13 state (INIT0/SCAN1) and GET15 opmode (STA1), raw LE32 response.
 * No carrier/tail output and no GET96/97 guess. Atomic refusal leaves output
 * and used unchanged. The native adapter validates task/generation/lifetime. */
int niws_reply(const struct niw_request *,const char native_name[16],
 enum niws_native_state,void *output,size_t capacity,size_t *used);
#define NIWS_CHANNEL_BYTES 4824u
/* GET27/254: authoritative allowed channel subset for this passive-only driver.
 * Raw version1/count +401 channel records(12B) +zero trailer. No ACTIVE/DFS.
 * Caller owns output allocation (4824B; avoid a kernel stack temporary).
 * All validation precedes output mutation. No owner/lifetime authentication. */
int niws_channels(const struct niw_request *,const char native_name[16],
 const struct nss_info *,void *output,size_t capacity,size_t *used);
#endif
