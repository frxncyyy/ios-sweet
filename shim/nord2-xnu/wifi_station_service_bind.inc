/* Included after NSS declarations/delivery and authenticated NST native binder.
 * Same actual retained CAP worker, same admission lock. No second worker. */
#include "wifi_station_service.h"
#include "wifi_ios_card.h"
#include "wifi_wpa_nst.h"
#include "wifi_ifnet_link.h"
#ifndef NJS_HOST_TEST
#include <sys/random.h>
#include <kern/clock.h>
#endif
/* Same clock_get_uptime conversion as ncd_native_now, usable from IOUC:
 * reading monotonic time does not borrow worker authority or access MMIO. */
static uint64_t njs_uptime_us(void){
#ifdef NJS_HOST_TEST
 return njs_test_uptime_us();
#else
 uint64_t absolute,ns;clock_get_uptime(&absolute);absolutetime_to_nanoseconds(absolute,&ns);return ns/1000;
#endif
}
#ifndef NJS_RETURN_SCANS
#define NJS_RETURN_SCANS() 0
#endif
#ifndef NJS_LIVE_CONFIGURE
#define NJS_LIVE_CONFIGURE() ((void)0)
#endif
#define NJS_JOIN_BUDGET_US UINT64_C(8000000)
#define NJS_HANDOVER_BUDGET_US UINT64_C(30000000)
#define NJS_TX_SLOTS 8u
#define NJS_CACHE_SLOTS 4u
#define NJS_RX_SLOTS 16u
static struct {
 IOLock*notice_lock;
 unsigned queued,active,canceled,leave,detach,used,joined,terminal,retiring,cache_next,scan_returned;
 unsigned linked,tx_admit,tx_head,tx_count,rx_count;
 unsigned logs,last_phase,last_kind,last_outcome,last_error,last_halt,last_ready,last_event,last_status,event_count,tx_accepted,tx_results,tx_ok,tx_fail,tx_local,queued_drop,rx_delivered,data_logs,data_retire_logs,cookie_retired,last_service,last_linked,last_result;
 uint64_t admitted_us,deadline_us;
 uint64_t serial,ticket,epoch,native_generation,rx_generation[NJS_RX_SLOTS];
 void*cookie;njs_completion completion;
 struct niwj_request request;
 struct niwj_target target,candidate;
 struct njs_snapshot snapshot;
 struct nsb_scan cache[NJS_CACHE_SLOTS];
 struct nwo_install install;
 struct nwn driver,constructor_saved;struct stc_arbiter arbiter,arbiter_saved;
 struct nst_profile profile;
 uint8_t tx[NJS_TX_SLOTS][NWIL_ETHERNET_MAX];unsigned tx_bytes[NJS_TX_SLOTS];
 uint8_t rx[NJS_RX_SLOTS][NWIL_ETHERNET_MAX];unsigned rx_bytes[NJS_RX_SLOTS];
} njs_native;
/* One copied next attempt; old callback/firmware ownership stays intact until
 * retirement plus the existing real all-queue/fresh-scan barrier. */
static struct {unsigned state,canceled;uint64_t admitted,deadline,ticket,epoch;
 void*cookie;njs_completion completion;struct niwj_request request;struct njs_snapshot snapshot;}njs_handover;

/* At most112 samples (five short state parts each), never packet/secret
 * bytes. State rows only on phase/action/outcome/error/readiness transitions. */
static void njs_trace_locked(unsigned force){
 struct stc*t=&njs_native.driver.owner.transaction;struct nst*n=&nst_native.state;
 unsigned ready=n->sta_active|(n->ptk_ready<<1)|(n->gtk.installed<<2)|(n->controlled_port<<3);
 if(njs_native.logs>=110)return;
 if(!force&&njs_native.last_phase==t->phase&&njs_native.last_kind==t->action.kind&&njs_native.last_outcome==t->outcome&&njs_native.last_error==n->error&&njs_native.last_halt==n->halt&&njs_native.last_ready==ready&&njs_native.last_service==njs_native.snapshot.phase&&njs_native.last_linked==njs_native.linked&&njs_native.last_result==njs_native.snapshot.result)return;
 njs_native.logs++;njs_native.last_phase=t->phase;njs_native.last_kind=t->action.kind;njs_native.last_outcome=t->outcome;njs_native.last_error=n->error;njs_native.last_halt=n->halt;njs_native.last_ready=ready;njs_native.last_service=njs_native.snapshot.phase;njs_native.last_linked=njs_native.linked;njs_native.last_result=njs_native.snapshot.result;
 /* Kernel printf stages only256 bytes. Every part is <=230 bytes even at
  * maximal integer widths; the boot-global sample binds one locked snapshot. */
 printf("NORD2 WIFI IOS STATIONPART: sample=%u part=1 generation=%llu ticket=%llu service=%u result=%u linked=%u retired=%u transaction=%u\n",njs_native.logs,(unsigned long long)njs_native.native_generation,(unsigned long long)njs_native.ticket,njs_native.snapshot.phase,njs_native.snapshot.result,njs_native.linked,njs_native.cookie_retired,t->phase);
 printf("NORD2 WIFI IOS STATIONPART: sample=%u part=2 action=%u outcome=%u owner_halt=%u native_phase=%u error=%u halt=%u peer_step=%u\n",njs_native.logs,t->action.kind,t->outcome,njs_native.driver.owner.halted,n->phase,n->error,n->halt,n->peer_step);
 printf("NORD2 WIFI IOS STATIONPART: sample=%u part=3 cid=%u sequence=%u submitted=%u dma=%u air_pending=%u associated=%u sta=%u ptk=%u gtk=%u port=%u\n",njs_native.logs,n->wire_cid,n->wire_sequence,n->submitted,n->dma_completed,n->air_pending,t->associated,n->sta_active,n->ptk_ready,n->gtk.installed,n->controlled_port);
 printf("NORD2 WIFI IOS STATIONPART: sample=%u part=4 event=%u status=%u events=%u tx_accept=%u tx_result=%u tx_ok=%u tx_fail=%u queued_drop=%u tx_local=%u\n",njs_native.logs,njs_native.last_event,njs_native.last_status,njs_native.event_count,njs_native.tx_accepted,njs_native.tx_results,njs_native.tx_ok,njs_native.tx_fail,njs_native.queued_drop,njs_native.tx_local);
 printf("NORD2 WIFI IOS STATIONPART: sample=%u part=5 rx_delivered=%u rx_raw=%u rx_raw_eapol=%u rx_raw_drop=%u t_ms=%llu security=%u rx_raw_data=%u\n",njs_native.logs,njs_native.rx_delivered,n->rx_untranslated,n->rx_untranslated_eapol,n->rx_untranslated_drop,(unsigned long long)njs_native.driver.now_ms,njs_native.snapshot.security,n->rx_untranslated_data);
}
static void njs_link_error(unsigned up,int error){IOLockLock(nss_native.lock);unsigned log=njs_native.logs<112;if(log)njs_native.logs++;IOLockUnlock(nss_native.lock);if(log)printf("NORD2 WIFI IOS LINK: generation=%llu up=%u error=%d\n",(unsigned long long)njs_native.native_generation,up,error);}
static int njs_pending_locked(void){return njs_native.queued||njs_native.active||njs_handover.state;}
static int njs_associated_scan_ready_locked(void){return njs_native.active&&njs_native.linked&&!njs_native.queued&&!njs_native.leave&&!njs_native.canceled&&nwo_authorized(&njs_native.driver.owner)&&nst_native.state.controlled_port&&!nst_native.state.cleanup_started;}
static int njs_scan_blocked_locked(void){return !nss_native.powered||njs_native.queued||(njs_native.active&&!njs_associated_scan_ready_locked())||(njs_native.used&&!njs_native.active&&!njs_native.scan_returned);}
static void njs_cache_scan_locked(const struct nsb_scan*s){
 if(!niwj_scan_usable(s))return;
 njs_native.cache[njs_native.cache_next++%NJS_CACHE_SLOTS]=*s;
 /* A later genuine UI scan may recover from a refused/canceled refresh. This
  * is still source-proven all-queue drain+new scan, never a copied UI cache.
  * nss_deliver sets callback_busy before releasing this same lock. */
 if(nsr_native.closed_generation&&njs_native.scan_returned&&!nst_native.active&&!njs_native.active&&
  nss_return.ready&&nss_return.consumed&&s->started>=nsr_native.closed_us&&
  nct_done_receipt(&nct_native.session)&&!ncb_native.rx.active&&!ncb_native.rx.halt&&!ncb_native.rx.error)
  nsr_native.scan_ready=1;
}
static void njs_initialize_locked(void){njs_native.notice_lock=IOLockAlloc();njs_cache_scan_locked(&ncb_native.rx.scan);njs_native.snapshot.phase=NJS_IDLE;}
static int njs_request_ok(const struct niwj_request*q){
 static const uint8_t rsn[22]={48,20,1,0,0,15,172,4,1,0,0,15,172,4,1,0,0,15,172,2,0,0};
 return q&&q->ssid_length&&q->ssid_length<=32&&q->has_target<=1&&q->association_reason<=2&&((q->security==NIWJ_WPA2_PSK_CCMP&&(q->native_flags==0||q->native_flags==4)&&!memcmp(q->station_rsn,rsn,22))||
 (q->security==NIWJ_OPEN&&!q->native_flags&&!memcmp(q->station_rsn,(const uint8_t[22]){0},22)&&!memcmp(q->pmk,(const uint8_t[32]){0},32)))&&(!q->has_target||(!(q->target[0]&1)&&(q->target[0]|q->target[1]|q->target[2]|q->target[3]|q->target[4]|q->target[5])));
}
static void njs_accept_locked(const struct niwj_request*q,void*cookie,uint64_t epoch,njs_completion completion,uint64_t next_ticket,uint64_t now_us){
 njs_native.admitted_us=now_us;njs_native.deadline_us=(now_us/1000+8000)*1000;
 njs_native.cookie_retired=0;njs_native.request=*q;njs_native.ticket=next_ticket;njs_native.native_generation=njs_native.ticket;
 if(njs_native.used)nsr_native.admitted_generation=njs_native.native_generation;
 njs_native.epoch=epoch;njs_native.cookie=cookie;njs_native.completion=completion;njs_native.queued=1;njs_native.canceled=njs_native.leave=njs_native.detach=njs_native.terminal=njs_native.joined=njs_native.retiring=0;
 njs_native.snapshot=(struct njs_snapshot){.native_generation=njs_native.native_generation,.client_epoch=epoch,.ticket=njs_native.ticket,.phase=NJS_QUEUED,.security=q->security};

}
int nord2_wifi_join_enqueue(const struct niwj_request*q,void*cookie,uint64_t epoch,njs_completion completion,uint64_t*ticket){
 if(!njs_request_ok(q)||!cookie||!epoch||!completion||!ticket)return -1;
 if(!nss_available())return 1;uint64_t now_us=njs_uptime_us();if(now_us>UINT64_MAX-NJS_JOIN_BUDGET_US)return 1;IOLockLock(nss_native.lock);
 if(!nss_native.powered||!njs_native.notice_lock||nss_native.failed||nst_native.state.halt||nst_native.state.error||nsr_native.bad||njs_native.serial>=255){IOLockUnlock(nss_native.lock);return 1;}
 if(njs_handover.state){IOLockUnlock(nss_native.lock);return 2;}
 if(njs_native.used&&!njs_rejoin_ready_locked()){
  /* Only an explicit old leave or completed teardown awaiting its real scan
   * permits handover. Never replace a healthy link or overwrite an attempt. */
  if(!njs_native.leave&&!(njs_native.scan_returned&&!njs_native.active&&!njs_native.queued)){IOLockUnlock(nss_native.lock);return 1;}
  if(now_us>UINT64_MAX-NJS_HANDOVER_BUDGET_US){IOLockUnlock(nss_native.lock);return 1;}
  njs_handover.state=1;njs_handover.admitted=now_us;njs_handover.deadline=now_us+NJS_HANDOVER_BUDGET_US;
  njs_handover.ticket=++njs_native.serial;njs_handover.epoch=epoch;njs_handover.cookie=cookie;njs_handover.completion=completion;njs_handover.request=*q;
  njs_handover.snapshot=(struct njs_snapshot){.native_generation=njs_handover.ticket,.client_epoch=epoch,.ticket=njs_handover.ticket,.phase=NJS_QUEUED,.security=q->security};
  *ticket=njs_handover.ticket;IOLockUnlock(nss_native.lock);thread_wakeup((event_t)&nss_native);return 0;
 }
 unsigned busy=njs_native.active||njs_native.queued||njs_native.retiring||njs_native.completion;
 if(busy){IOLockUnlock(nss_native.lock);return 2;}
 njs_accept_locked(q,cookie,epoch,completion,++njs_native.serial,now_us);
 *ticket=njs_native.ticket;IOLockUnlock(nss_native.lock);thread_wakeup((event_t)&nss_native);return 0;
}

int nord2_wifi_join_cancel(uint64_t ticket){if(!ticket||!nss_available())return 1;IOLockLock(nss_native.lock);int r=1;if(njs_handover.state==1&&ticket==njs_handover.ticket){njs_handover.canceled=1;niwj_clear(&njs_handover.request,sizeof njs_handover.request);r=0;}else if(ticket==njs_native.ticket&&(njs_native.queued||njs_native.active)){if(njs_native.linked&&njs_native.snapshot.phase==NJS_AUTHORIZED)njs_native.detach=1;else njs_native.canceled=1;r=0;}IOLockUnlock(nss_native.lock);if(!r)thread_wakeup((event_t)&nss_native);return r;}
static void njs_drop_tx_locked(void);
static int njs_disconnect_notice_locked(void){
 IOLockLock(nss_native.lock);
 int r=1;unsigned changed=0,wake=0;uint64_t generation=0;if(njs_handover.state==1){njs_handover.canceled=1;niwj_clear(&njs_handover.request,sizeof njs_handover.request);wake=1;r=0;}
 if(njs_native.queued||njs_native.active){changed=1;njs_native.leave=1;generation=njs_native.native_generation;
  if(njs_native.active)nst_close_port(&nst_native.state);
  njs_drop_tx_locked();njs_native.linked=njs_native.tx_admit=0;njs_native.snapshot.associated=njs_native.snapshot.ptk_ready=njs_native.snapshot.gtk_ready=njs_native.snapshot.data_ready=0;njs_native.snapshot.phase=NJS_LEAVING;r=0;}
 /* A pre-join leave on an initialized, healthy, untouched service is
  * already satisfied. Do not claim native work, change generation/used,
  * notify ifnet with generation0, or consume the single native attempt. */
 else if(!nss_native.failed&&njs_native.snapshot.phase==NJS_IDLE&&!njs_native.used&&!nst_native.attempted&&!nst_native.active&&!njs_native.retiring&&!njs_native.completion&&!njs_native.cookie&&!njs_native.linked&&!njs_native.tx_admit)r=0;
 IOLockUnlock(nss_native.lock);
 if(changed&&nord2_wifi_ifnet_link(generation,0))r=-1;
 if(changed||wake)thread_wakeup((event_t)&nss_native);return r;
}
int nord2_wifi_join_disconnect(void){
 if(!nss_available()||!njs_native.notice_lock)return 1;
 IOLockLock(njs_native.notice_lock);int r=njs_disconnect_notice_locked();IOLockUnlock(njs_native.notice_lock);return r;
}
int nord2_wifi_power_set(unsigned on){
 if(on>1||!nss_available()||!njs_native.notice_lock)return 1;
 /* Serialize rapid power requests with existing revoke/link-up notices.
  * Never hold NSS across the synchronous BSD callback. */
 IOLockLock(njs_native.notice_lock);IOLockLock(nss_native.lock);
 if(nss_native.failed||(nss_native.powered!=on&&nss_native.power_epoch==UINT64_MAX)){IOLockUnlock(nss_native.lock);IOLockUnlock(njs_native.notice_lock);return 1;}
 unsigned was_on=nss_native.powered;
 if(was_on!=on)nss_native.power_epoch++;nss_native.powered=on;
 if(on&&!was_on&&njs_native.used&&njs_native.scan_returned&&!njs_native.active&&!njs_native.queued&&!nsr_native.scan_ready){
  nsr_native.refresh_requested_us=njs_uptime_us();
  if(nsr_native.scan_state==2)nsr_native.refresh_after_scan=1;
  else{nsr_native.scan_state=1;nsr_native.refresh_after_scan=0;}
 }
 if(!on)for(unsigned k=0;k<NSS_SLOTS;k++)if(nss_native.slot[k].state==NSS_QUEUED||nss_native.slot[k].state==NSS_RUNNING)nss_native.slot[k].canceled=1;
 IOLockUnlock(nss_native.lock);
 int r=on?0:njs_disconnect_notice_locked();IOLockUnlock(njs_native.notice_lock);
 thread_wakeup((event_t)&nss_native);return r<0?-1:0;
}
/* Only owner calls this under admission lock; never borrows NCT while linked. */
static int njs_associated_scan_start_locked(void){
 if(!nss_native.powered||!njs_associated_scan_ready_locked()||nst_native.state.associated_scan||nst_native.state.wire_busy||nst_native.state.air_pending||nst_native.state.peer_kind)return 0;
 struct nss_slot*slot=0;for(unsigned k=0;k<NSS_SLOTS;k++)if(nss_native.slot[k].state==NSS_QUEUED&&!nss_native.slot[k].canceled&&(!slot||nss_native.slot[k].ticket<slot->ticket))slot=&nss_native.slot[k];if(!slot)return 0;
 const struct ncs_channel*channels=slot->request.count?slot->request.channels:nss_native.info.channels;size_t count=slot->request.count?slot->request.count:nss_native.info.count;
 uint8_t sequence=(uint8_t)(nss_native.scan_sequence+1);
 if(ncs_build(ncd_native.diag.frame,ncd_native.diag.frame_bytes,ncd_native.diag.history[ncd_native.diag.events-1].sequence,nss_native.info.mac,channels,count,1,2,sequence,1,slot->request.dwell_tu,0,&nss_native.frames))return -1;
 int r=nst_associated_scan(&nst_native.state,&slot->scan,nss_native.frames.scan+64,NCS_SCAN_BYTES-64);if(r)return r<0?-1:0;
 nss_native.scan_sequence=sequence;slot->state=NSS_RUNNING;return 0;
}
static struct nss_slot*njs_associated_scan_done_locked(void){
 struct nsb_scan*scan=nst_native.state.associated_scan;if(!scan||(!scan->done&&!nst_native.state.associated_scan_canceled)||scan->active||(nst_native.state.wire_busy&&(nst_native.state.wire_cid==3||nst_native.state.wire_cid==NST_CID_SCAN_CANCEL)))return 0;
 for(unsigned k=0;k<NSS_SLOTS;k++)if(&nss_native.slot[k].scan==scan){if(nst_associated_scan_detach(&nst_native.state,scan))return 0;nss_native.slot[k].captured=scan->last;return &nss_native.slot[k];}return 0;
}
/* Compiled native module implements all six operations for this checked
 * WPA2-PSK/CCMP profile. Bits describe installed handlers, not reached keys,
 * current carrier, all RSN authentication modes or a hardware-test result. */
int nord2_wifi_join_card_inventory(struct niwc_manifest*out){
 if(!out)return -1;
 if(!nss_available())return 1;
 IOLockLock(nss_native.lock);
 if(nss_native.failed||!njs_native.notice_lock){IOLockUnlock(nss_native.lock);return 1;}
 const struct niwc_manifest installed={2,{
  {NIWC_CCMP,NIWC_END_TO_END},{NIWC_RSN_IE,NIWC_END_TO_END}}};
 *out=installed;IOLockUnlock(nss_native.lock);return 0;
}
int nord2_wifi_join_copy_state(struct njs_snapshot*out){if(!out)return -1;if(!nss_available())return 1;IOLockLock(nss_native.lock);*out=njs_handover.state?njs_handover.snapshot:njs_native.snapshot;IOLockUnlock(nss_native.lock);return 0;}
int nord2_wifi_link_send(const uint8_t*p,size_t n,uint64_t generation){
 if(!p||n<14||n>NWIL_ETHERNET_MAX||!generation||(p[12]==0x88&&p[13]==0x8e))return NWIL_SEND_INVALID;
 if(!nss_available())return NWIL_SEND_NOT_READY;IOLockLock(nss_native.lock);
 if((((unsigned)p[12]<<8)|p[13])<1536||memcmp(p+6,nss_native.info.mac,6)){IOLockUnlock(nss_native.lock);return NWIL_SEND_INVALID;}
 int r=NWIL_SEND_NOT_READY;
 if(nss_native.powered&&!nss_native.failed&&njs_native.active&&!njs_native.canceled&&!njs_native.leave&&(njs_native.linked||njs_native.tx_admit)&&njs_native.native_generation==generation&&nwo_authorized(&njs_native.driver.owner)){
  if(njs_native.tx_count==NJS_TX_SLOTS)r=NWIL_SEND_BUSY;
  else{unsigned k=(njs_native.tx_head+njs_native.tx_count)%NJS_TX_SLOTS;memcpy(njs_native.tx[k],p,n);njs_native.tx_bytes[k]=(unsigned)n;njs_native.tx_count++;njs_native.tx_accepted++;if(njs_native.data_logs<8){njs_native.data_logs++;njs_trace_locked(1);}r=NWIL_SEND_ACCEPTED;}}
 IOLockUnlock(nss_native.lock);if(!r)thread_wakeup((event_t)&nss_native);return r;
}
static int njs_rng(void*unused,uint8_t*out,size_t n){(void)unused;if(!out||n!=32)return -1;read_random(out,(unsigned)n);return 0;}
static int njs_peer(void*ctx,const struct stc_action*a,const struct stc*transaction){
 if(ctx!=&njs_native||transaction!=&njs_native.driver.owner.transaction)return -1;
 unsigned kind=a->kind==STC_PREPARE_PEER?NST_PREPARE_PEER:a->kind==STC_CONFIGURE_PEER?NST_CONFIGURE_PEER:a->kind==STC_RETIRE_PEER?NST_RETIRE_PEER:0;
 if(!kind)return -1;
 return nst_peer_begin(&nst_native.state,a->ticket,kind,kind==NST_PREPARE_PEER?&njs_native.profile:0,kind==NST_CONFIGURE_PEER?a->payload:0,kind==NST_CONFIGURE_PEER?a->bytes:0);
}
static void njs_notice(enum njs_notice notice){
 IOLockLock(nss_native.lock);njs_completion fn=njs_native.completion;void*cookie=njs_native.cookie;uint64_t epoch=njs_native.epoch,ticket=njs_native.ticket;IOLockUnlock(nss_native.lock);
 if(fn)fn(cookie,epoch,ticket,notice);
 if(notice==NJS_RETIRED){IOLockLock(nss_native.lock);njs_native.completion=0;njs_native.cookie=0;njs_native.retiring=0;njs_native.cookie_retired=1;njs_trace_locked(1);IOLockUnlock(nss_native.lock);}
}
static void njs_drop_tx_locked(void){njs_native.queued_drop+=njs_native.tx_count;niwj_clear(njs_native.tx,sizeof njs_native.tx);memset(njs_native.tx_bytes,0,sizeof njs_native.tx_bytes);njs_native.tx_count=njs_native.tx_head=0;}
static void njs_snapshot_locked(void){
 struct stc*s=&njs_native.driver.owner.transaction;struct njs_snapshot*v=&njs_native.snapshot;
 unsigned current=s->phase<STC_DEAUTH&&!njs_native.canceled&&!njs_native.leave;
 v->security=njs_native.target.security;v->peer=njs_native.target;v->associated=current&&s->associated;v->ptk_ready=current&&nst_native.state.ptk_ready&&nst_native.state.ptk.installed;v->gtk_ready=current&&nst_native.state.gtk.installed;v->data_ready=current&&njs_native.linked&&nst_native.state.controlled_port&&nwo_authorized(&njs_native.driver.owner);
 if((njs_native.leave||njs_native.canceled)&&s->phase<STC_DEAUTH)v->phase=NJS_LEAVING;
 else if(v->data_ready)v->phase=NJS_AUTHORIZED;
 else if(s->phase>=STC_DEAUTH)v->phase=s->phase==STC_DONE||s->phase==STC_HALTED?NJS_FAILED:NJS_LEAVING;
 else if(s->phase>=STC_CONFIGURE)v->phase=NJS_SECURING;
 else if(s->phase>=STC_ASSOC_TX)v->phase=NJS_ASSOCIATING;
 else v->phase=NJS_AUTHENTICATING;
 v->result=v->data_ready?1:(s->outcome==STC_PENDING||s->outcome==STC_AUTHORIZED)?0:s->outcome==STC_TIMEOUT?9:2;
}
/* Called by nst_step under admission lock: pure engine updates or bounded copy,
 * never client callback/ifnet invocation/recursive transport publication. */
static int njs_event(void*ctx,const struct nst_event*e){
 if(ctx!=&njs_native||e->generation!=njs_native.native_generation)return 0;
 if(!nss_native.powered&&e->kind==NST_EAPOL)return 0;
 if(e->kind==NST_COMMAND_DMA&&e->cid==7&&nsr_key_complete(&nsr_native,e))return -1;
 njs_native.last_event=e->kind;njs_native.last_status=e->status;njs_native.event_count++;if(e->kind==NST_FRAME_RESULT&&e->ticket==UINT64_MAX){njs_native.tx_results++;if(e->status==0)njs_native.tx_ok++;else njs_native.tx_fail++;if(njs_native.data_logs<8){njs_native.data_logs++;njs_trace_locked(1);}}
 if(e->kind==NST_DATA_SUBMITTED&&e->ticket==UINT64_MAX){if(e->status==0)njs_native.tx_local++;else njs_native.queued_drop++;if(njs_native.data_retire_logs<8){njs_native.data_retire_logs++;njs_trace_locked(1);}return 0;} /* local retirement, not tx_ok */
 if(e->kind==NST_ETHERNET){if(!njs_native.canceled&&!njs_native.leave&&njs_native.linked&&nwo_authorized(&njs_native.driver.owner)&&e->body&&e->bytes>=14&&e->bytes<=NWIL_ETHERNET_MAX){if(njs_native.rx_count==NJS_RX_SLOTS)return 1;unsigned k=njs_native.rx_count++;memcpy(njs_native.rx[k],e->body,e->bytes);njs_native.rx_bytes[k]=(unsigned)e->bytes;njs_native.rx_generation[k]=e->generation;}return 0;}
 unsigned type=e->kind==NST_PEER_PREPARED?NWO_PREPARED:e->kind==NST_PEER_CONFIGURED?NWO_CONFIGURED:e->kind==NST_PEER_RETIRED?NWO_RETIRED:0;
 if(type)return nwn_peer_complete(&njs_native.driver,e->generation,e->ticket,type,e->status==0,njs_native.driver.now_ms)<0?-1:0;
 int result=nwn_event(&njs_native.driver,e,njs_native.driver.now_ms);
 if(e->kind==NST_LINK_LOST){njs_native.snapshot.involuntary=1;njs_native.snapshot.reason=0;njs_native.snapshot.subreason=e->status;}
 if(e->kind==NST_MANAGEMENT&&njs_native.driver.owner.transaction.outcome==STC_DISCONNECTED&&e->body&&e->bytes>=26){njs_native.snapshot.involuntary=1;njs_native.snapshot.reason=e->body[24]|(uint32_t)e->body[25]<<8;}
 return result<0?-1:0;
}
static void njs_early_finish(unsigned result){
 IOLockLock(nss_native.lock);njs_native.queued=njs_native.active=0;njs_native.retiring=1;njs_native.snapshot.phase=NJS_FAILED;njs_native.snapshot.result=result;niwj_clear(&njs_native.request,sizeof njs_native.request);IOLockUnlock(nss_native.lock);
 njs_notice(NJS_JOIN_FAILED);njs_notice(NJS_RETIRED);
}
static int njs_start_owner(uint64_t now_us){
 int found=0;IOLockLock(nss_native.lock);
 if(njs_native.canceled||njs_native.leave||now_us<njs_native.admitted_us||now_us>=njs_native.deadline_us){
  unsigned result=!njs_native.canceled&&!njs_native.leave&&now_us>=njs_native.deadline_us?9:2;
  IOLockUnlock(nss_native.lock);njs_early_finish(result);return 0;}
 for(unsigned k=0;k<NJS_CACHE_SLOTS;k++)if(!niwj_select(&njs_native.request,&njs_native.cache[k],&nss_native.info,now_us,&njs_native.candidate)&&(!found||njs_native.candidate.rssi_dbm>njs_native.target.rssi_dbm)){njs_native.target=njs_native.candidate;found=1;}
 /* Holding admission lock across these pure constructors serializes cancel
  * with first-attempt ownership. No native publish or callback occurs here. */
 if(!found||nst_native_prepare(&njs_native.target,njs_native.native_generation,now_us,njs_native.deadline_us,&njs_native.profile)){
  IOLockUnlock(nss_native.lock);njs_early_finish(2);return 0;}
 unsigned old_used=njs_native.used,old_returned=njs_native.scan_returned;
 njs_native.constructor_saved=njs_native.driver;njs_native.arbiter_saved=njs_native.arbiter;
 NJS_LIVE_CONFIGURE();
 if(njs_native.used){niwj_clear(&njs_native.driver,sizeof njs_native.driver);niwj_clear(&njs_native.arbiter,sizeof njs_native.arbiter);}
 njs_native.scan_returned=0;njs_native.used=1;
 uint64_t fresh_us=njs_uptime_us();if(fresh_us<now_us||fresh_us>=njs_native.deadline_us){
  int undo=nst_native_abort_prepare(njs_native.native_generation);njs_native.used=old_used;njs_native.scan_returned=old_returned;
  njs_native.driver=njs_native.constructor_saved;njs_native.arbiter=njs_native.arbiter_saved;niwj_clear(&njs_native.constructor_saved,sizeof njs_native.constructor_saved);niwj_clear(&njs_native.arbiter_saved,sizeof njs_native.arbiter_saved);
  IOLockUnlock(nss_native.lock);njs_early_finish(fresh_us>=njs_native.deadline_us?9:2);return undo?-1:0;}
 struct stc_config config={0};config.generation=njs_native.native_generation;config.deadline_ms=njs_native.deadline_us/1000;config.cleanup_timeout_ms=30000;config.assoc=njs_native.profile.assoc;config.security=config.assoc.security;
 config.channel=(struct sw_channel){.bss=nst_native.state.peer.bss,.token=(uint8_t)njs_native.native_generation,.band=njs_native.target.band,.channel=njs_native.target.channel,.dbdc_band=4,.interval_ms=4000};
 memcpy(config.group_spare,nst_native.state.group_spare,sizeof config.group_spare);config.sta_wlan=nst_native.state.peer.wlan;config.group_wlan=nst_native.state.peer.group_wlan;if(config.security!=SW_SECURITY_OPEN){config.pmk=njs_native.request.pmk;config.station_rsn=njs_native.request.station_rsn;config.ap_rsn=njs_native.target.ap_rsn;config.ap_rsn_bytes=njs_native.target.ap_rsn_bytes;config.rng=njs_rng;}
 config.ap_ies=njs_native.target.record.ies;config.ap_ies_bytes=njs_native.target.record.ie_bytes;
 int result=nwn_begin(&njs_native.driver,&nst_native.state,&njs_native.arbiter,&config,njs_peer,&njs_native,fresh_us/1000);
 niwj_clear(&config,sizeof config);niwj_clear(&njs_native.request,sizeof njs_native.request);njs_native.queued=0;njs_native.active=result==1;
 int undo=0;if(result==1){nst_native_commit_prepare();njs_snapshot_locked();}
 else{undo=nst_native_abort_prepare(njs_native.native_generation);niwj_clear(&njs_native.driver,sizeof njs_native.driver);njs_native.driver=njs_native.constructor_saved;njs_native.arbiter=njs_native.arbiter_saved;njs_native.used=old_used;njs_native.scan_returned=old_returned;}
 niwj_clear(&njs_native.constructor_saved,sizeof njs_native.constructor_saved);niwj_clear(&njs_native.arbiter_saved,sizeof njs_native.arbiter_saved);IOLockUnlock(nss_native.lock);
 if(result!=1){njs_early_finish(2);return undo?-1:0;}
 IOLockLock(nss_native.lock);njs_trace_locked(1);IOLockUnlock(nss_native.lock);return 1;
}
/* One independent terminal sample; routine transition logging can exhaust
 * during repeated joins. No payload, address, key, nonce or replay bytes. */
static unsigned njs_fault_logged;
static int njs_fatal_owner(unsigned source){
 IOLockLock(njs_native.notice_lock);IOLockLock(nss_native.lock);
 if(!njs_fault_logged){
  njs_fault_logged=1;const struct nst*n=&nst_native.state;const struct nwo*o=&njs_native.driver.owner;
  printf("NORD2 WIFI IOS STATIONFAULT: part=1 source=%u generation=%llu native=%u error=%u halt=%u transaction=%u owner=%u in_kind=%u in_ticket=%llu\n",source,(unsigned long long)n->generation,n->phase,n->error,n->halt,o->transaction.phase,o->halted,o->in_kind,(unsigned long long)o->in_ticket);
  printf("NORD2 WIFI IOS STATIONFAULT: part=2 last=%llu deadline=%llu operation_deadline=%llu cleanup_deadline_ms=%llu\n",(unsigned long long)n->last,(unsigned long long)n->deadline,(unsigned long long)n->operation_deadline,(unsigned long long)o->transaction.cleanup_deadline);
  printf("NORD2 WIFI IOS STATIONFAULT: part=3 submit=%u dma=%u busy=%u pub=%u ddone=%u air_pending=%u air_done=%u air_status=%u cleanup=%u associated_scan=%u\n",n->submitted,n->dma_completed,n->wire_busy,n->published,n->dma_done,n->air_pending,n->air_done,n->air_status,n->cleanup_started,(unsigned)(n->associated_scan!=0));
 }
 if(njs_handover.state){njs_handover.canceled=1;niwj_clear(&njs_handover.request,sizeof njs_handover.request);}
 nst_close_port(&nst_native.state);njs_native.linked=njs_native.tx_admit=0;njs_drop_tx_locked();
 nwp_cancel(&njs_native.driver.owner.transaction.wpa);niwj_clear(&njs_native.driver.owner.transaction.action,sizeof njs_native.driver.owner.transaction.action);niwj_clear(njs_native.driver.owner.transaction.queued_eapol,sizeof njs_native.driver.owner.transaction.queued_eapol);niwj_clear(&njs_native.request,sizeof njs_native.request);
 njs_trace_locked(1);
#ifndef NJS_HOST_TEST
 if(njs_native.logs<112){njs_native.logs++;printf("NORD2 WIFI IOS STATIONGUARD: reason=%u source=%u stage=%u error=%u valid=%u addr=%#x raw=%#x mask=%#x expected=%#x\n",nwfn_native.guard_reason,nwfn_native.guard_source,nwfn_native.failure_stage,nwfn_native.error,nwfn_native.guard_valid,nwfn_native.guard_address,nwfn_native.guard_raw,nwfn_native.guard_mask,nwfn_native.guard_expected);}
#endif
 njs_native.snapshot.phase=NJS_FAILED;njs_native.snapshot.result=2;njs_native.snapshot.associated=njs_native.snapshot.ptk_ready=njs_native.snapshot.gtk_ready=njs_native.snapshot.data_ready=0;
 njs_native.active=njs_native.queued=0;njs_native.used=1;njs_native.retiring=1;IOLockUnlock(nss_native.lock);
 (void)nord2_wifi_ifnet_link(njs_native.native_generation,0);IOLockUnlock(njs_native.notice_lock);njs_notice(njs_native.joined?NJS_LEFT:NJS_JOIN_FAILED);njs_notice(NJS_RETIRED);return -1;
}
/* Returns1 when the actual NSS worker remains occupied by connection;0 idle;
 * -1 uncertain native DMA/lease failure requiring inherited retained drain. */
/* Worker only; callback delivery never borrows either native lock. */
static int njs_handover_work(void){
 IOLockLock(nss_native.lock);if(njs_handover.state!=1){IOLockUnlock(nss_native.lock);return 0;}
 uint64_t now=njs_uptime_us();unsigned failed=njs_handover.canceled||!nss_native.powered||nss_native.failed||nst_native.state.halt||nst_native.state.error||nsr_native.bad||now<njs_handover.admitted||now>=njs_handover.deadline||now>UINT64_MAX-NJS_JOIN_BUDGET_US;
 if(failed){
  njs_handover.state=2;njs_handover.snapshot.phase=NJS_FAILED;njs_handover.snapshot.result=!njs_handover.canceled&&nss_native.powered&&now>=njs_handover.deadline?9:2;
  niwj_clear(&njs_handover.request,sizeof njs_handover.request);
  njs_completion fn=njs_handover.completion;void*cookie=njs_handover.cookie;uint64_t epoch=njs_handover.epoch,ticket=njs_handover.ticket;
  IOLockUnlock(nss_native.lock);fn(cookie,epoch,ticket,NJS_JOIN_FAILED);fn(cookie,epoch,ticket,NJS_RETIRED);
  IOLockLock(nss_native.lock);niwj_clear(&njs_handover,sizeof njs_handover);IOLockUnlock(nss_native.lock);return 1;
 }
 if(!njs_native.active&&!njs_native.queued&&!njs_native.retiring&&!njs_native.completion&&njs_rejoin_ready_locked()){
  njs_accept_locked(&njs_handover.request,njs_handover.cookie,njs_handover.epoch,njs_handover.completion,njs_handover.ticket,now);
  niwj_clear(&njs_handover,sizeof njs_handover);
 }
 IOLockUnlock(nss_native.lock);return 0;
}
static int njs_owner_work(void){
 if(njs_handover_work())return 1;
 IOLockLock(nss_native.lock);unsigned queued=njs_native.queued,active=njs_native.active,scans=0,canceled=njs_native.canceled||njs_native.leave;
 if(queued&&!active)for(unsigned k=0;k<NSS_SLOTS;k++)if(nss_native.slot[k].state==NSS_QUEUED||nss_native.slot[k].state==NSS_RUNNING)scans=1;
 uint64_t admitted=njs_native.admitted_us,deadline=njs_native.deadline_us;IOLockUnlock(nss_native.lock);
 if(!queued&&!active){njs_rejoin_refresh();if(njs_handover.state){uint64_t pause;clock_interval_to_deadline(1000,NSEC_PER_USEC,&pause);IOLockLock(nss_native.lock);assert_wait_deadline((event_t)&nss_native,THREAD_UNINT,pause);IOLockUnlock(nss_native.lock);thread_block(THREAD_CONTINUE_NULL);}return 0;}
 if(queued){uint64_t observed=njs_uptime_us();if(canceled||observed<admitted||observed>=deadline){njs_early_finish(!canceled&&observed>=deadline?9:2);return 1;}
  if(scans)return 0; /* Drain only scans admitted before this first join. */
 }
 uint64_t now_us;if(ncd_native_now(&ncd_native,&now_us))return njs_fatal_owner(1);
 if(queued){int r=njs_start_owner(now_us);if(r<0)return -1;IOLockLock(nss_native.lock);active=njs_native.active;IOLockUnlock(nss_native.lock);if(!active)return 1;
  /* Pure prepare/RNG may advance beyond the constructor's fresh timestamp.
   * Never feed the pre-constructor sample back into the monotonic engine. */
  if(ncd_native_now(&ncd_native,&now_us))return njs_fatal_owner(2);
 }
 unsigned notify_join=0,notify_terminal=0,retire=0,detach=0,link_up=0,link_down=0;
 IOLockLock(nss_native.lock);
 if(njs_native.detach){njs_native.detach=0;detach=1;}
 if(nst_native.state.associated_scan&&(njs_native.canceled||njs_native.leave||nst_native.state.link_lost))(void)nst_associated_scan_cancel(&nst_native.state);
 if((njs_native.canceled||njs_native.leave)&&!nst_native.state.associated_scan){(void)nwn_cancel(&njs_native.driver,njs_native.native_generation,now_us/1000);njs_drop_tx_locked();}
 if(njs_associated_scan_start_locked()<0){IOLockUnlock(nss_native.lock);return njs_fatal_owner(3);}
 int result=(njs_native.canceled||njs_native.leave)&&nst_native.state.associated_scan?0:nwn_pump(&njs_native.driver,now_us/1000);
 if(result<0){IOLockUnlock(nss_native.lock);return njs_fatal_owner(4);}
 if(!njs_native.canceled&&!njs_native.leave&&nwo_authorized(&njs_native.driver.owner)&&!nst_native.state.wire_busy&&!nst_native.state.air_pending&&njs_native.tx_count){
  unsigned k=njs_native.tx_head;int sent=nst_data(&nst_native.state,UINT64_MAX,njs_native.tx[k],njs_native.tx_bytes[k]);
  if(sent<=0){if(sent<0)njs_native.queued_drop++;niwj_clear(njs_native.tx[k],sizeof njs_native.tx[k]);njs_native.tx_bytes[k]=0;njs_native.tx_head=(k+1)%NJS_TX_SLOTS;njs_native.tx_count--;}}
 nsr_key_capture(&nsr_native,&nst_native.state);
 if(nst_native.state.halt||nst_step(&nst_native.state,&ncd_io,&ncd_native,16,njs_event,&njs_native)<0){IOLockUnlock(nss_native.lock);return njs_fatal_owner(5);}
 struct nss_slot*scan_done=njs_associated_scan_done_locked();
 njs_snapshot_locked();njs_trace_locked(0);
 if(nwo_authorized(&njs_native.driver.owner)&&nst_native.state.controlled_port&&!njs_native.canceled&&!njs_native.leave&&!njs_native.linked)link_up=1;
 if((!njs_native.snapshot.data_ready||njs_native.canceled||njs_native.leave)&&njs_native.linked){njs_native.linked=njs_native.tx_admit=0;link_down=1;njs_drop_tx_locked();}
 unsigned terminal=(njs_native.driver.owner.transaction.phase==STC_DONE||njs_native.driver.owner.transaction.phase==STC_HALTED)&&!nst_native.state.associated_scan;
 if(terminal){if(!njs_native.terminal){njs_native.terminal=1;notify_terminal=1;}njs_native.active=0;njs_native.retiring=1;retire=1;niwj_clear(&njs_native.driver.owner.transaction.wpa,sizeof njs_native.driver.owner.transaction.wpa);njs_drop_tx_locked();}
 IOLockUnlock(nss_native.lock);
 if(scan_done)nss_deliver(scan_done,!scan_done->scan.done?NSS_REFUSED:scan_done->scan.completion.state==7&&!scan_done->scan.partial&&!scan_done->scan.dropped?NSS_COMPLETE:NSS_PARTIAL);
 /* Canceled queued slots must retire even while station owns the loop. */
 for(unsigned k=0;k<NSS_SLOTS;k++){IOLockLock(nss_native.lock);unsigned cancel_scan=nss_native.slot[k].state==NSS_QUEUED&&nss_native.slot[k].canceled;IOLockUnlock(nss_native.lock);if(cancel_scan)nss_deliver(&nss_native.slot[k],NSS_CANCELED);}
 if(link_down){IOLockLock(njs_native.notice_lock);int down_error=nord2_wifi_ifnet_link(njs_native.native_generation,0);IOLockUnlock(njs_native.notice_lock);if(down_error)njs_link_error(0,down_error);}
 if(link_up){
  IOLockLock(njs_native.notice_lock);IOLockLock(nss_native.lock);
  unsigned eligible=!njs_native.canceled&&!njs_native.leave&&nwo_authorized(&njs_native.driver.owner);
  /* Native station/key/port proof already exists. Admit output generated by
   * the synchronous BSD link event; do not announce JOINED until it returns. */
  if(eligible)njs_native.tx_admit=1;
  IOLockUnlock(nss_native.lock);
  int error=eligible?nord2_wifi_ifnet_link(njs_native.native_generation,1):0;
  IOLockLock(nss_native.lock);njs_native.tx_admit=0;
  if(error||!eligible||njs_native.canceled||njs_native.leave){njs_native.linked=njs_native.tx_admit=0;(void)nwn_cancel(&njs_native.driver,njs_native.native_generation,now_us/1000);njs_drop_tx_locked();njs_snapshot_locked();}
  else{njs_native.linked=1;njs_snapshot_locked();if(!njs_native.joined){njs_native.joined=1;notify_join=1;}}
  unsigned linked=njs_native.linked;IOLockUnlock(nss_native.lock);
  if(eligible&&!linked)(void)nord2_wifi_ifnet_link(njs_native.native_generation,0);
  IOLockUnlock(njs_native.notice_lock);
  if(error)njs_link_error(1,error);
 }
 for(unsigned k=0;k<njs_native.rx_count;k++){IOLockLock(nss_native.lock);unsigned deliver=njs_native.linked&&!njs_native.canceled&&!njs_native.leave&&njs_native.rx_generation[k]==njs_native.native_generation;IOLockUnlock(nss_native.lock);if(deliver&&!nord2_wifi_ifnet_receive(njs_native.rx_generation[k],njs_native.rx[k],njs_native.rx_bytes[k])){IOLockLock(nss_native.lock);njs_native.rx_delivered++;if(njs_native.data_logs<8){njs_native.data_logs++;njs_trace_locked(1);}IOLockUnlock(nss_native.lock);}niwj_clear(njs_native.rx[k],sizeof njs_native.rx[k]);njs_native.rx_bytes[k]=0;}njs_native.rx_count=0;
 if(detach)njs_notice(NJS_RETIRED);
 if(notify_join&&!detach){IOLockLock(nss_native.lock);unsigned live=njs_native.linked&&!njs_native.canceled&&!njs_native.leave;IOLockUnlock(nss_native.lock);if(live)njs_notice(NJS_JOINED);}
 if(notify_terminal)njs_notice(njs_native.joined?NJS_LEFT:NJS_JOIN_FAILED);
 if(retire)njs_notice(NJS_RETIRED);
 if(terminal&&njs_native.driver.owner.transaction.phase==STC_DONE&&NJS_RETURN_SCANS()<0)return njs_fatal_owner(6);
 if(!terminal){uint64_t deadline;clock_interval_to_deadline(1000,NSEC_PER_USEC,&deadline);IOLockLock(nss_native.lock);assert_wait_deadline((event_t)&nss_native,THREAD_UNINT,deadline);IOLockUnlock(nss_native.lock);thread_block(THREAD_CONTINUE_NULL);}
 return 1;
}
