#ifndef WIFI_STATION_SERVICE_H
#define WIFI_STATION_SERVICE_H
#include "wifi_ios_join.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Native snapshots only: GET/run/carrier require an actual current lease and
 * authenticated assoc + PTK EID24 + GTK source completion + data-path receipt.
 * Structs are kernel-only; never copy their padding/keys/addresses to logs. */
enum njs_phase {NJS_UNAVAILABLE=0,NJS_IDLE,NJS_QUEUED,NJS_AUTHENTICATING,
 NJS_ASSOCIATING,NJS_SECURING,NJS_AUTHORIZED,NJS_LEAVING,NJS_FAILED};
enum njs_notice {NJS_JOINED=1,NJS_JOIN_FAILED,NJS_LEFT,NJS_RETIRED};
struct njs_snapshot {
 uint64_t native_generation,client_epoch,ticket;
 unsigned phase,result,associated,ptk_ready,gtk_ready,data_ready;
 unsigned involuntary;uint32_t reason,subreason;
 struct niwj_target peer;
};
typedef void (*njs_completion)(void*,uint64_t client_epoch,uint64_t ticket,enum njs_notice);
/* 0 accepted,1 unavailable,2 busy,-1 invalid; rejection must be atomic.
 * Native immediately copies the complete secret input and owns/wipes its copy.
 * No inline completion. Cookie retained by IOUC until final RETIRED notice,
 * exactly once, even on canceled/closed delivery. Callbacks outside native queue
 * lock, actual retained worker only; no radio calls from callback/client thread.
 * JOINED/FAILED/LEFT snapshot committed before notice. Retired means no future
 * callback can use cookie; it is not by itself a firmware/DMA retirement proof.
 */
int nord2_wifi_join_enqueue(const struct niwj_request*,void*,uint64_t,
 njs_completion,uint64_t*);
/* Idempotent while ticket retained: preauthorization cancel closes port and
 * requests bounded cleanup; after authorization detach delivery only, preserving
 * global native link. RETIRED follows any current callback before cookie reuse. */
int nord2_wifi_join_cancel(uint64_t ticket);
/* Manager-entitled native caller only. Closes controlled port/carrier before
 * asynchronous cleanup and LEFT notification; must not reset shared firmware. */
int nord2_wifi_join_disconnect(void);
/* Copy authoritative global current state under native serialization. Shared
 * queries across manager clients are allowed; only callback delivery binds to
 * matching ticket/client epoch. No pointer into mutable RX/secret state escapes.
 * 0 copied,1 unavailable,-1 invalid. Never returns success from a fake backend. */
int nord2_wifi_join_copy_state(struct njs_snapshot*);
/* Installed implementation inventory only, never live association/readiness.
 * Exact reviewed CCMP128 + RSN-IE path; no WPA1/TKIP/WEP/GCMP/SAE/PMF claim.
 * Native source owns entries, client cannot provide or widen them. No IO.
 * 0 copied,1 native service unavailable,-1 invalid; failure leaves out unchanged. */
struct niwc_manifest;
int nord2_wifi_join_card_inventory(struct niwc_manifest*);
#ifdef __cplusplus
}
#endif
#endif
