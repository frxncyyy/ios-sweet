#ifndef WIFI_IOS_STATION_VIEW_H
#define WIFI_IOS_STATION_VIEW_H
#include "wifi_station_service.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Pure snapshot checks, never an ownership/cipher/data-path authority. Native
 * service must create these fields from its real serialized receipts. */
int niwj_authorized(const struct njs_snapshot*);
/*0 encoded,1 no associated/current state,-1 shape,-2 unsupported. used=0 on
 * refusal, output unchanged. now_us must be actual monotonic uptime. Current
 * RSSI/IEs remain the recorded observation; age is never reset to manufacture
 * a fresh measurement. request13/21 are raw4 scalar output, payload_bytes0. */
int niwj_state_reply(const struct njs_snapshot*,unsigned request,size_t payload_bytes,
 unsigned scanning,uint64_t now_us,void*,size_t,size_t*);
/* Consumer-compatible copied pipe records. Native caller commits snapshot,
 * validates current client epoch/ticket under gate, publishes and notifies.
 * Event9 no-primary48B is safe through selected consumer's zeroed scratch.
 * Event4 carries32B, up only authorized; caller closes real port before down. */
int niwj_notice_record(const struct njs_snapshot*,unsigned event,const char[16],void*,size_t,size_t*);
#ifdef __cplusplus
}
#endif
#endif
