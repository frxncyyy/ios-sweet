#ifndef WIFI_WPA_NST_H
#define WIFI_WPA_NST_H
#include "wifi_wpa_owner.h"
#include "wifi_station_transport.h"
/* Real source peer sequencer owned by native transport.0 accepted/1 busy/-1
 * refused. Completion is delivered later by nwn_peer_complete, never inline. */
typedef int (*nwn_peer_submit)(void*,const struct stc_action*,const struct stc*);
struct nwn {struct nwo owner;struct nst *transport;nwn_peer_submit peer_submit;void *peer_context;struct nwo_install install;unsigned key_pairwise,cleanup_started;uint64_t now_ms,overall_deadline;};
int nwn_begin(struct nwn*,struct nst*,struct stc_arbiter*,const struct stc_config*,nwn_peer_submit,void*,uint64_t);
int nwn_pump(struct nwn*,uint64_t);
int nwn_event(struct nwn*,const struct nst_event*,uint64_t);
int nwn_peer_complete(struct nwn*,uint64_t,uint64_t,unsigned,int,uint64_t);
int nwn_cancel(struct nwn*,uint64_t,uint64_t);
#endif
