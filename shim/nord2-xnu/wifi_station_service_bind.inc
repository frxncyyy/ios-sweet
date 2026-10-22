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
#define NJS_JOIN_BUDGET_US UINT64_C(8000000)
#define NJS_TX_SLOTS 8u
#define NJS_CACHE_SLOTS 4u
#define NJS_RX_SLOTS 16u
static struct {
 IOLock*notice_lock;
 unsigned queued,active,canceled,leave,detach,used,joined,terminal,retiring,cache_next;
 unsigned linked,tx_admit,tx_head,tx_count,rx_count;
 unsigned logs,last_phase,last_kind,last_outcome,last_error,last_halt,last_ready,last_event,last_status,event_count,tx_accepted,tx_results,tx_ok,tx_fail,queued_drop,rx_delivered,data_logs,cookie_retired,last_service,last_linked,last_result;
 uint64_t admitted_us,deadline_us;
 uint64_t serial,ticket,epoch,native_generation,rx_generation[NJS_RX_SLOTS];
 void*cookie;njs_completion completion;
 struct niwj_request request;
 struct niwj_target target,candidate;
 struct njs_snapshot snapshot;
 struct nsb_scan cache[NJS_CACHE_SLOTS];
 struct nwo_install install;
 struct nwn driver;struct stc_arbiter arbiter;
 struct nst_profile profile;
 uint8_t tx[NJS_TX_SLOTS][NWIL_ETHERNET_MAX];unsigned tx_bytes[NJS_TX_SLOTS];
 uint8_t rx[NJS_RX_SLOTS][NWIL_ETHERNET_MAX];unsigned rx_bytes[NJS_RX_SLOTS];
} njs_native;
/* At most112 aggregate rows for this first transaction, never packet/secret
 * bytes. State rows only on phase/action/outcome/error/readiness transitions. */
static void njs_trace_locked(unsigned force){
 struct stc*t=&njs_native.driver.owner.transaction;struct nst*n=&nst_native.state;
 unsigned ready=n->sta_active|(n->ptk_ready<<1)|(n->gtk.installed<<2)|(n->controlled_port<<3);
 if(njs_native.logs>=110)return;
 if(!force&&njs_native.last_phase==t->phase&&njs_native.last_kind==t->action.kind&&njs_native.last_outcome==t->outcome&&njs_native.last_error==n->error&&njs_native.last_halt==n->halt&&njs_native.last_ready==ready&&njs_native.last_service==njs_native.snapshot.phase&&njs_native.last_linked==njs_native.linked&&njs_native.last_result==njs_native.snapshot.result)return;
 njs_native.logs++;njs_native.last_phase=t->phase;njs_native.last_kind=t->action.kind;njs_native.last_outcome=t->outcome;njs_native.last_error=n->error;njs_native.last_halt=n->halt;njs_native.last_ready=ready;njs_native.last_service=njs_native.snapshot.phase;njs_native.last_linked=njs_native.linked;njs_native.last_result=njs_native.snapshot.result;
 printf("NORD2 WIFI IOS STATION: generation=%llu ticket=%llu service=%u result=%u linked=%u retired=%u transaction=%u action=%u outcome=%u owner_halt=%u native_phase=%u error=%u halt=%u peer_step=%u cid=%u sequence=%u submitted=%u dma=%u air_pending=%u associated=%u sta=%u ptk=%u gtk=%u port=%u event=%u status=%u events=%u tx_accept=%u tx_result=%u tx_ok=%u tx_fail=%u queued_drop=%u rx_delivered=%u rx_raw=%u rx_raw_eapol=%u rx_raw_drop=%u t_ms=%llu\n",(unsigned long long)njs_native.native_generation,(unsigned long long)njs_native.ticket,njs_native.snapshot.phase,njs_native.snapshot.result,njs_native.linked,njs_native.cookie_retired,t->phase,t->action.kind,t->outcome,njs_native.driver.owner.halted,n->phase,n->error,n->halt,n->peer_step,n->wire_cid,n->wire_sequence,n->submitted,n->dma_completed,n->air_pending,t->associated,n->sta_active,n->ptk_ready,n->gtk.installed,n->controlled_port,njs_native.last_event,njs_native.last_status,njs_native.event_count,njs_native.tx_accepted,njs_native.tx_results,njs_native.tx_ok,njs_native.tx_fail,njs_native.queued_drop,njs_native.rx_delivered,n->rx_untranslated,n->rx_untranslated_eapol,n->rx_untranslated_drop,(unsigned long long)njs_native.driver.now_ms);
}
static void njs_link_error(unsigned up,int error){IOLockLock(nss_native.lock);unsigned log=njs_native.logs<112;if(log)njs_native.logs++;IOLockUnlock(nss_native.lock);if(log)printf("NORD2 WIFI IOS LINK: generation=%llu up=%u error=%d\n",(unsigned long long)njs_native.native_generation,up,error);}
static int njs_pending_locked(void){return njs_native.queued||njs_native.active;}
static int njs_scan_blocked_locked(void){return njs_native.queued||njs_native.active||njs_native.used;}
static void njs_cache_scan_locked(const struct nsb_scan*s){
 if(!niwj_scan_usable(s))return;
 njs_native.cache[njs_native.cache_next++%NJS_CACHE_SLOTS]=*s;
}
static void njs_initialize_locked(void){njs_native.notice_lock=IOLockAlloc();njs_cache_scan_locked(&ncb_native.rx.scan);njs_native.snapshot.phase=NJS_IDLE;}
static int njs_request_ok(const struct niwj_request*q){
 static const uint8_t rsn[22]={48,20,1,0,0,15,172,4,1,0,0,15,172,4,1,0,0,15,172,2,0,0};
 return q&&q->ssid_length&&q->ssid_length<=32&&q->has_target<=1&&q->association_reason<=2&&q->security==NIWJ_WPA2_PSK_CCMP&&(q->native_flags==0||q->native_flags==4)&&!memcmp(q->station_rsn,rsn,22)&&(!q->has_target||(!(q->target[0]&1)&&(q->target[0]|q->target[1]|q->target[2]|q->target[3]|q->target[4]|q->target[5])));
}
int nord2_wifi_join_enqueue(const struct niwj_request*q,void*cookie,uint64_t epoch,njs_completion completion,uint64_t*ticket){
 if(!njs_request_ok(q)||!cookie||!epoch||!completion||!ticket)return -1;
 if(!nss_available())return 1;uint64_t now_us=njs_uptime_us();if(now_us>UINT64_MAX-NJS_JOIN_BUDGET_US)return 1;IOLockLock(nss_native.lock);
 if(!njs_native.notice_lock||nss_native.failed||njs_native.used||njs_native.serial==UINT64_MAX){IOLockUnlock(nss_native.lock);return 1;}
 unsigned busy=njs_native.active||njs_native.queued||njs_native.retiring||njs_native.completion;
 if(busy){IOLockUnlock(nss_native.lock);return 2;}
 njs_native.admitted_us=now_us;njs_native.deadline_us=(now_us/1000+8000)*1000;
 njs_native.cookie_retired=0;njs_native.request=*q;njs_native.ticket=++njs_native.serial;njs_native.native_generation=njs_native.ticket;
 njs_native.epoch=epoch;njs_native.cookie=cookie;njs_native.completion=completion;njs_native.queued=1;njs_native.canceled=njs_native.leave=njs_native.detach=njs_native.terminal=njs_native.joined=njs_native.retiring=0;
 njs_native.snapshot=(struct njs_snapshot){.native_generation=njs_native.native_generation,.client_epoch=epoch,.ticket=njs_native.ticket,.phase=NJS_QUEUED};
 *ticket=njs_native.ticket;IOLockUnlock(nss_native.lock);thread_wakeup((event_t)&nss_native);return 0;
}
int nord2_wifi_join_cancel(uint64_t ticket){if(!ticket||!nss_available())return 1;IOLockLock(nss_native.lock);int r=1;if(ticket==njs_native.ticket&&(njs_native.queued||njs_native.active)){if(njs_native.linked&&njs_native.snapshot.phase==NJS_AUTHORIZED)njs_native.detach=1;else njs_native.canceled=1;r=0;}IOLockUnlock(nss_native.lock);if(!r)thread_wakeup((event_t)&nss_native);return r;}
static void njs_drop_tx_locked(void);
int nord2_wifi_join_disconnect(void){
 if(!nss_available()||!njs_native.notice_lock)return 1;
 IOLockLock(njs_native.notice_lock);IOLockLock(nss_native.lock);
 int r=1;uint64_t generation=0;
 if(njs_native.queued||njs_native.active){njs_native.leave=1;generation=njs_native.native_generation;
  if(njs_native.active)nst_close_port(&nst_native.state);
  njs_drop_tx_locked();njs_native.linked=njs_native.tx_admit=0;njs_native.snapshot.associated=njs_native.snapshot.ptk_ready=njs_native.snapshot.gtk_ready=njs_native.snapshot.data_ready=0;njs_native.snapshot.phase=NJS_LEAVING;r=0;}
 IOLockUnlock(nss_native.lock);
 if(!r&&nord2_wifi_ifnet_link(generation,0))r=-1;
 IOLockUnlock(njs_native.notice_lock);if(r!=1)thread_wakeup((event_t)&nss_native);return r;
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
int nord2_wifi_join_copy_state(struct njs_snapshot*out){if(!out)return -1;if(!nss_available())return 1;IOLockLock(nss_native.lock);*out=njs_native.snapshot;IOLockUnlock(nss_native.lock);return 0;}
int nord2_wifi_link_send(const uint8_t*p,size_t n,uint64_t generation){
 if(!p||n<14||n>NWIL_ETHERNET_MAX||!generation||(p[12]==0x88&&p[13]==0x8e))return NWIL_SEND_INVALID;
 if(!nss_available())return NWIL_SEND_NOT_READY;IOLockLock(nss_native.lock);
 if((((unsigned)p[12]<<8)|p[13])<1536||memcmp(p+6,nss_native.info.mac,6)){IOLockUnlock(nss_native.lock);return NWIL_SEND_INVALID;}
 int r=NWIL_SEND_NOT_READY;
 if(!nss_native.failed&&njs_native.active&&!njs_native.canceled&&!njs_native.leave&&(njs_native.linked||njs_native.tx_admit)&&njs_native.native_generation==generation&&nwo_authorized(&njs_native.driver.owner)){
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
 v->peer=njs_native.target;v->associated=current&&s->associated;v->ptk_ready=current&&nst_native.state.ptk_ready&&nst_native.state.ptk.installed;v->gtk_ready=current&&nst_native.state.gtk.installed;v->data_ready=current&&njs_native.linked&&nst_native.state.controlled_port&&nwo_authorized(&njs_native.driver.owner);
 if(v->data_ready)v->phase=NJS_AUTHORIZED;
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
 njs_native.last_event=e->kind;njs_native.last_status=e->status;njs_native.event_count++;if(e->kind==NST_FRAME_RESULT&&e->ticket==UINT64_MAX){njs_native.tx_results++;if(e->status==0)njs_native.tx_ok++;else njs_native.tx_fail++;if(njs_native.data_logs<8){njs_native.data_logs++;njs_trace_locked(1);}}
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
 njs_native.used=1;
 uint64_t fresh_us=njs_uptime_us();if(fresh_us<now_us||fresh_us>=njs_native.deadline_us){IOLockUnlock(nss_native.lock);njs_early_finish(fresh_us>=njs_native.deadline_us?9:2);return 0;}
 struct stc_config config={0};config.generation=njs_native.native_generation;config.deadline_ms=njs_native.deadline_us/1000;config.cleanup_timeout_ms=30000;config.assoc=njs_native.profile.assoc;
 config.channel=(struct sw_channel){.bss=nst_native.state.peer.bss,.token=1,.band=njs_native.target.band,.channel=njs_native.target.channel,.dbdc_band=4,.interval_ms=4000};
 config.sta_wlan=nst_native.state.peer.wlan;config.group_wlan=nst_native.state.peer.group_wlan;config.pmk=njs_native.request.pmk;config.station_rsn=njs_native.request.station_rsn;config.ap_rsn=njs_native.target.ap_rsn;config.ap_rsn_bytes=njs_native.target.ap_rsn_bytes;config.rng=njs_rng;
 config.ap_ies=njs_native.target.record.ies;config.ap_ies_bytes=njs_native.target.record.ie_bytes;
 int result=nwn_begin(&njs_native.driver,&nst_native.state,&njs_native.arbiter,&config,njs_peer,&njs_native,fresh_us/1000);
 niwj_clear(&config,sizeof config);niwj_clear(&njs_native.request,sizeof njs_native.request);njs_native.queued=0;njs_native.active=result==1;if(result==1)njs_snapshot_locked();IOLockUnlock(nss_native.lock);
 if(result!=1){njs_early_finish(2);return nst_native.state.mutated||nst_native.state.submitted?-1:0;}
 IOLockLock(nss_native.lock);njs_trace_locked(1);IOLockUnlock(nss_native.lock);return 1;
}
static int njs_fatal_owner(void){
 IOLockLock(njs_native.notice_lock);IOLockLock(nss_native.lock);nst_close_port(&nst_native.state);njs_native.linked=njs_native.tx_admit=0;njs_drop_tx_locked();
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
static int njs_owner_work(void){
 IOLockLock(nss_native.lock);unsigned queued=njs_native.queued,active=njs_native.active,scans=0,canceled=njs_native.canceled||njs_native.leave;
 if(queued&&!active)for(unsigned k=0;k<NSS_SLOTS;k++)if(nss_native.slot[k].state==NSS_QUEUED||nss_native.slot[k].state==NSS_RUNNING)scans=1;
 uint64_t admitted=njs_native.admitted_us,deadline=njs_native.deadline_us;IOLockUnlock(nss_native.lock);
 if(!queued&&!active)return 0;
 if(queued){uint64_t observed=njs_uptime_us();if(canceled||observed<admitted||observed>=deadline){njs_early_finish(!canceled&&observed>=deadline?9:2);return 1;}
  if(scans)return 0; /* Drain only scans admitted before this first join. */
 }
 uint64_t now_us;if(ncd_native_now(&ncd_native,&now_us))return njs_fatal_owner();
 if(queued){int r=njs_start_owner(now_us);if(r<0)return -1;IOLockLock(nss_native.lock);active=njs_native.active;IOLockUnlock(nss_native.lock);if(!active)return 1;
  /* Pure prepare/RNG may advance beyond the constructor's fresh timestamp.
   * Never feed the pre-constructor sample back into the monotonic engine. */
  if(ncd_native_now(&ncd_native,&now_us))return njs_fatal_owner();
 }
 unsigned notify_join=0,notify_terminal=0,retire=0,detach=0,link_up=0,link_down=0;
 IOLockLock(nss_native.lock);
 if(njs_native.detach){njs_native.detach=0;detach=1;}
 if(njs_native.canceled||njs_native.leave){(void)nwn_cancel(&njs_native.driver,njs_native.native_generation,now_us/1000);njs_drop_tx_locked();}
 int result=nwn_pump(&njs_native.driver,now_us/1000);
 if(result<0){IOLockUnlock(nss_native.lock);return njs_fatal_owner();}
 if(!njs_native.canceled&&!njs_native.leave&&nwo_authorized(&njs_native.driver.owner)&&!nst_native.state.wire_busy&&!nst_native.state.air_pending&&njs_native.tx_count){
  unsigned k=njs_native.tx_head;int sent=nst_data(&nst_native.state,UINT64_MAX,njs_native.tx[k],njs_native.tx_bytes[k]);
  if(sent<=0){if(sent<0)njs_native.queued_drop++;niwj_clear(njs_native.tx[k],sizeof njs_native.tx[k]);njs_native.tx_bytes[k]=0;njs_native.tx_head=(k+1)%NJS_TX_SLOTS;njs_native.tx_count--;}}
 if(nst_native.state.halt||nst_step(&nst_native.state,&ncd_io,&ncd_native,16,njs_event,&njs_native)<0){IOLockUnlock(nss_native.lock);return njs_fatal_owner();}
 njs_snapshot_locked();njs_trace_locked(0);
 if(nwo_authorized(&njs_native.driver.owner)&&nst_native.state.controlled_port&&!njs_native.canceled&&!njs_native.leave&&!njs_native.linked)link_up=1;
 if((!njs_native.snapshot.data_ready||njs_native.canceled||njs_native.leave)&&njs_native.linked){njs_native.linked=njs_native.tx_admit=0;link_down=1;njs_drop_tx_locked();}
 unsigned terminal=njs_native.driver.owner.transaction.phase==STC_DONE||njs_native.driver.owner.transaction.phase==STC_HALTED;
 if(terminal){if(!njs_native.terminal){njs_native.terminal=1;notify_terminal=1;}njs_native.active=0;njs_native.retiring=1;retire=1;niwj_clear(&njs_native.driver.owner.transaction.wpa,sizeof njs_native.driver.owner.transaction.wpa);njs_drop_tx_locked();}
 IOLockUnlock(nss_native.lock);
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
 if(!terminal){uint64_t deadline;clock_interval_to_deadline(1000,NSEC_PER_USEC,&deadline);IOLockLock(nss_native.lock);assert_wait_deadline((event_t)&nss_native,THREAD_UNINT,deadline);IOLockUnlock(nss_native.lock);thread_block(THREAD_CONTINUE_NULL);}
 return 1;
}
