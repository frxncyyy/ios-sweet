#ifndef PRIVATE_WIFI_SCAN_SERVICE_H
#define PRIVATE_WIFI_SCAN_SERVICE_H
#include "wifi_scan_plan.h"
#include "wifi_scan_bss.h"
#ifdef __cplusplus
extern "C" {
#endif
#define NSS_SLOTS 4u
#define NSS_CHANNELS 39u
struct nss_request {unsigned count;struct ncs_channel channels[NSS_CHANNELS];uint16_t dwell_tu;}; /*0=actual full allowed list; passive only/default or108TU timing; no SSID/random*/
struct nss_info {uint8_t mac[6],band5,nss,max_bw,path;unsigned count;struct ncs_channel channels[NSS_CHANNELS];unsigned scanning;}; /* running scan transaction, not RF emission */
enum nss_status {NSS_COMPLETE=0,NSS_PARTIAL=1,NSS_CANCELED=2,NSS_REFUSED=3,NSS_FATAL=4};
typedef void (*nss_completion)(void *cookie,uint64_t client_generation,uint64_t ticket,enum nss_status);
struct nss_view {const struct nsb_scan *scan;uint64_t captured_us,client_generation;enum nss_status status;};
/* Kernel-only; native IOUC validates task/ABI and retains cookie until callback
 * returns. Callback runs actual radio worker, outside queue lock; must be
 * bounded/no radio calls, and recheck client generation before event publish.
 * 0 accepted,1 backend unavailable,2 queue full,-1 invalid. Refusal atomic. */
int nord2_wifi_scan_info(struct nss_info *);
int nord2_wifi_scan_enqueue(const struct nss_request *,void *,uint64_t,nss_completion,uint64_t *);
/* Cancel closes delivery; queued gets canceled callback, inflight radio finishes
 * bounded without abort/reset. Client's gate still rejects late event delivery. */
int nord2_wifi_scan_cancel(uint64_t);
/* View only DONE and not canceled. Pointer stays immutable until release;
 * caller must retain ticket through outside-gate conversion. */
int nord2_wifi_scan_view(uint64_t,struct nss_view *);
/* After readers/converters finish. Allowed DURING callback: marks deferred
 * release, actual slot reuse only after callback returns. */
int nord2_wifi_scan_release(uint64_t);
#ifdef __cplusplus
}
#endif
#endif
