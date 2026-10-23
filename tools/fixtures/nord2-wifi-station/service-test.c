#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include "wifi_wpa_nst.h"
#include "wifi_station_service.h"
#include "wifi_ifnet_link.h"
#define NJS_HOST_TEST 1
#define NSS_QUEUED 1
#define NSS_RUNNING 2
#define NSEC_PER_USEC 1000
#define THREAD_UNINT 0
#define THREAD_CONTINUE_NULL 0
typedef void*event_t;
typedef struct {pthread_mutex_t mutex;}IOLock;
static IOLock*IOLockAlloc(void){IOLock*p=calloc(1,sizeof *p);assert(p&&!pthread_mutex_init(&p->mutex,0));return p;}
static void IOLockLock(IOLock*p){assert(!pthread_mutex_lock(&p->mutex));}
static void IOLockUnlock(IOLock*p){assert(!pthread_mutex_unlock(&p->mutex));}
static void thread_wakeup(event_t p){assert(p);}
static void clock_interval_to_deadline(unsigned a,unsigned b,uint64_t*c){*c=(uint64_t)a*b;}
static void assert_wait_deadline(event_t p,unsigned a,uint64_t b){assert(p&&!a&&b);}
static void thread_block(unsigned a){assert(!a);}
static void advance_entropy_clock(void);
static void read_random(void*p,unsigned n){memset(p,0x41,n);advance_entropy_clock();}
/* Same slot/storage shape as NSS; this seam refuses physical scan work. */
struct nss_slot {unsigned state,canceled,callback_busy,retire;uint64_t ticket,generation,captured;void*cookie;nss_completion completion;enum nss_status status;struct nss_request request;struct nsb_scan scan;};
static struct {IOLock*lock;unsigned failed,powered;uint64_t power_epoch;uint8_t scan_sequence;struct nss_info info;struct nss_slot slot[NSS_SLOTS];struct ncs_frames frames;}nss_native;
static struct {struct ncr_state rx;}ncb_native;
static struct {struct nst state;unsigned attempted,active;}nst_native;
/* Actual bounded key metadata helpers; the complete physical rejoin/scan
 * boundary is exercised by test-nord2-wifi-rejoin, not this service seam. */
#include "wifi_station_rejoin.h"
static struct nsr_ledger nsr_native;
static struct {struct nct_scan_return origin;unsigned attempted,ready,consumed;}nss_return;
static struct {unsigned active;struct nct_session session;}nct_native;
int nct_done_receipt(const struct nct_session*s){assert(s==&nct_native.session);return 0;}
static int njs_rejoin_ready_locked(void){return 0;} /* Never fabricate physical readiness. */
static unsigned refresh_calls,constructor_commits,constructor_aborts,prepare_expiry;
static void njs_rejoin_refresh(void){refresh_calls++;}
static struct nst constructor_backup;
static void nst_native_commit_prepare(void){constructor_commits++;memset(&constructor_backup,0,sizeof constructor_backup);}
static int nst_native_abort_prepare(uint64_t generation){
 struct nst*n=&nst_native.state;if(n->generation!=generation||n->mutated||n->submitted||n->wire_busy||n->air_pending||n->callback_active)return -1;
 constructor_aborts++;nst_native.state=constructor_backup;memset(&constructor_backup,0,sizeof constructor_backup);return 0;
}
static unsigned source_ready=1,prepared_calls,ifnet_down,ifnet_up,ifnet_error,notices[5],release_reentry;
static uint64_t clock_us=200,entropy_advance_us;
static unsigned prepare_success;
static void advance_entropy_clock(void){clock_us+=entropy_advance_us;}
static uint64_t njs_test_uptime_us(void){return clock_us;}
static pthread_mutex_t barrier=PTHREAD_MUTEX_INITIALIZER;static pthread_cond_t barrier_cond=PTHREAD_COND_INITIALIZER;static unsigned block_up,up_entered,release_up,disconnect_started,disconnect_done;
static struct {struct ncd_state diag;}ncd_native={.diag={.events=1}};
static const struct ncd_io ncd_io={0};
static int nss_available(void){return source_ready;}
static int ncd_native_now(void*p,uint64_t*out){assert(p==&ncd_native);*out=clock_us;return 0;}
static uint64_t njs_native_deadline_test;
static int nst_native_prepare(const struct niwj_target*t,uint64_t g,uint64_t now,uint64_t deadline,struct nst_profile*out){assert(deadline==njs_native_deadline_test);assert(deadline>now);prepared_calls++;if(!prepare_success)return -1;
 /* Pure constructor seam: real engine begins after a later millisecond. */
 memset(out,0,sizeof *out);constructor_backup=nst_native.state;struct nst*n=&nst_native.state;memset(n,0,sizeof *n);n->prepared=1;n->generation=g;n->peer.bss=0;n->peer.wlan=5;n->peer.group_wlan=4;memcpy(n->peer.ap,t->record.bssid,6);memcpy(n->peer.self,nss_native.info.mac,6);
 memcpy(out->assoc.peer.self,n->peer.self,6);memcpy(out->assoc.peer.ap,n->peer.ap,6);out->assoc.ssid=t->record.ssid;out->assoc.ssid_len=t->record.ssid_len;out->assoc.rates=t->rates;out->assoc.rates_len=t->rate_count;out->assoc.capability=17;out->assoc.listen_interval=10;clock_us=prepare_expiry?deadline:clock_us+1000;prepare_expiry=0;return 0;}
int nst_peer_begin(struct nst*t,uint64_t ticket,unsigned kind,const struct nst_profile*p,const uint8_t*r,size_t n){(void)t;(void)ticket;(void)kind;(void)p;(void)r;(void)n;return prepare_success?1:-1;}
int nst_command(struct nst*t,uint64_t ticket,uint8_t cid,const uint8_t*p,size_t n){(void)t;(void)ticket;(void)cid;(void)p;(void)n;return -1;}
int nst_management(struct nst*t,uint64_t ticket,const uint8_t*p,size_t n){(void)t;(void)ticket;(void)p;(void)n;return -1;}
int nst_eapol(struct nst*t,uint64_t ticket,const uint8_t*p,size_t n,unsigned x){(void)t;(void)ticket;(void)p;(void)n;(void)x;return -1;}
int nst_data(struct nst*t,uint64_t ticket,const uint8_t*p,size_t n){(void)t;(void)ticket;(void)p;(void)n;return -1;}
int nst_replay_install(struct nst*t,unsigned a,uint8_t b,const uint8_t*r){(void)t;(void)a;(void)b;(void)r;return -1;}
/* Rekey ownership is exercised by the actual transport suite, not this seam. */
int nst_replay_replace_group(struct nst*t,uint8_t key,const uint8_t*r){(void)t;(void)key;(void)r;return -1;}
int nst_authorize(struct nst*t){(void)t;return -1;}
int nst_cleanup_begin(struct nst*t,uint64_t now,uint64_t deadline){assert(deadline>now);t->deadline=deadline;return 0;}
void nst_close_port(struct nst*t){t->controlled_port=0;}
int nst_step(struct nst*t,const struct ncd_io*i,void*c,unsigned b,nst_event_fn f,void*x){(void)t;(void)i;(void)c;(void)b;(void)f;(void)x;return 0;}
/* No descriptor/radio scan proof is synthesized by service doubles. Actual
 * associated-scan/DMA/cancel behavior belongs to the lifecycle owner test. */
static unsigned scan_build_calls,scan_native_calls,scan_delivery_calls;
int ncs_build(const uint8_t*f,size_t fn,uint8_t seq,const uint8_t mac[6],const struct ncs_channel*c,size_t count,uint8_t scan_type,uint8_t phy,uint8_t scan_seq,unsigned dbdc,uint16_t dwell,uint16_t rest,struct ncs_frames*out){
 (void)f;(void)fn;(void)seq;(void)mac;(void)c;(void)count;(void)scan_type;(void)phy;(void)scan_seq;(void)dbdc;(void)dwell;(void)rest;(void)out;scan_build_calls++;return -1;
}
int nst_associated_scan(struct nst*t,struct nsb_scan*scan,const uint8_t*p,size_t n){(void)t;(void)scan;(void)p;(void)n;scan_native_calls++;return -1;}
int nst_associated_scan_cancel(struct nst*t){(void)t;scan_native_calls++;return -1;}
int nst_associated_scan_detach(struct nst*t,struct nsb_scan*scan){(void)t;(void)scan;scan_native_calls++;return -1;}
static void nss_deliver(struct nss_slot*slot,enum nss_status status){(void)slot;(void)status;scan_delivery_calls++;assert(!"unexpected physical scan delivery in service seam");}
#include "wifi_station_service_bind.inc"
static struct niwj_request request(void){struct niwj_request q={0};static const uint8_t rsn[22]={48,20,1,0,0,15,172,4,1,0,0,15,172,4,1,0,0,15,172,2,0,0};q.ssid_length=4;memcpy(q.ssid,"test",4);memset(q.pmk,0x42,32);memcpy(q.station_rsn,rsn,22);q.security=1;return q;}
static void complete(void*cookie,uint64_t epoch,uint64_t ticket,enum njs_notice notice){assert(cookie==(void*)1&&epoch==7&&ticket);assert(notice>=1&&notice<=4);notices[notice]++;struct njs_snapshot snap;assert(!nord2_wifi_join_copy_state(&snap));if(notice==NJS_RETIRED){struct niwj_request q=request();uint64_t next=0;assert(nord2_wifi_join_enqueue(&q,(void*)1,7,complete,&next)==(njs_native.used?1:2));release_reentry++;}}
int nord2_wifi_ifnet_link(uint64_t generation,int authorized){assert(generation);uint8_t ethernet[14]={0};ethernet[12]=8;memcpy(ethernet+6,nss_native.info.mac,6);
 if(authorized){ifnet_up++;struct njs_snapshot v;assert(!nord2_wifi_join_copy_state(&v));assert(njs_native.tx_admit&&!njs_native.linked&&v.phase!=NJS_AUTHORIZED&&v.result!=1&&!v.data_ready);assert(!nord2_wifi_link_send(ethernet,14,generation));if(block_up){pthread_mutex_lock(&barrier);up_entered=1;pthread_cond_broadcast(&barrier_cond);while(!release_up)pthread_cond_wait(&barrier_cond,&barrier);pthread_mutex_unlock(&barrier);}return ifnet_error;}
 ifnet_down++;assert(nord2_wifi_link_send(ethernet,14,generation)==NWIL_SEND_NOT_READY);return 0;}

int nord2_wifi_ifnet_receive(uint64_t generation,const uint8_t*p,size_t n){assert(generation&&p&&n>=14);return 0;}
static unsigned handover_failed,handover_retired;
static void handover_canceled(void*cookie,uint64_t epoch,uint64_t ticket,enum njs_notice notice){
 assert(cookie==(void*)2&&epoch==9&&ticket&&ticket!=njs_native.ticket);struct njs_snapshot v;assert(!nord2_wifi_join_copy_state(&v)&&v.ticket==ticket&&v.phase==NJS_FAILED&&v.result==2&&!v.data_ready);struct niwj_request zero={0};assert(!memcmp(&zero,&njs_handover.request,sizeof zero));if(notice==NJS_JOIN_FAILED)handover_failed++;else{assert(notice==NJS_RETIRED);handover_retired++;}
}
static int owner_close(void*p){(void)p;nst_close_port(&nst_native.state);return 0;}
static int owner_submit(void*p,const struct stc_action*a,const struct nwo_install*m){(void)p;(void)a;(void)m;return -1;}
static void seed_ready(uint64_t g){
 njs_native.active=njs_native.used=1;njs_native.queued=njs_native.linked=njs_native.tx_admit=njs_native.canceled=njs_native.leave=njs_native.detach=njs_native.joined=njs_native.terminal=njs_native.retiring=njs_native.cookie_retired=0;njs_native.native_generation=njs_native.ticket=g;njs_native.epoch=7;njs_native.cookie=(void*)1;njs_native.completion=complete;
 njs_native.arbiter=(struct stc_arbiter){.mode=2,.connection_used=1,.generation=g};njs_native.driver=(struct nwn){0};njs_native.driver.transport=&nst_native.state;njs_native.driver.owner.ops=(struct nwo_ops){owner_submit,owner_close};
 struct stc*t=&njs_native.driver.owner.transaction;t->attempted=1;t->generation=g;t->arbiter=&njs_native.arbiter;t->phase=STC_CONNECTED;t->outcome=STC_AUTHORIZED;t->associated=1;t->cleanup_timeout_ms=30000;
 nst_native.state.controlled_port=nst_native.state.sta_active=nst_native.state.ptk_ready=nst_native.state.ptk.installed=nst_native.state.gtk.installed=1;
 /* Readiness-only seam uses the same per-index representation as native RX. */
 nst_native.state.gtk.key_id=1;nst_native.state.gtk_keys[1]=nst_native.state.gtk;nst_native.state.gtk_wlan[1]=nst_native.state.peer.group_wlan;nst_native.state.gtk_mapped|=1u<<1;
 njs_native.snapshot=(struct njs_snapshot){.native_generation=g,.client_epoch=7,.ticket=g,.phase=NJS_SECURING};}
static void*owner_thread(void*x){(void)x;assert(njs_owner_work()==1);return NULL;}
static void*disconnect_thread(void*x){(void)x;pthread_mutex_lock(&barrier);disconnect_started=1;pthread_cond_broadcast(&barrier_cond);pthread_mutex_unlock(&barrier);assert(!nord2_wifi_join_disconnect());pthread_mutex_lock(&barrier);disconnect_done=1;pthread_cond_broadcast(&barrier_cond);pthread_mutex_unlock(&barrier);return NULL;}
int main(void){nss_native.powered=1;nss_native.power_epoch=1;nss_native.lock=IOLockAlloc();njs_initialize_locked();assert(njs_native.notice_lock);struct niwj_request q=request();uint64_t ticket=0;
/* Exact actual binder: successful repeated idle leave is pure, including
 * untouched first-attempt ledger and no ifnet callback or native prepare. */
static unsigned char idle_service[sizeof njs_native],idle_native[sizeof nst_native];
memcpy(idle_service,&njs_native,sizeof njs_native);memcpy(idle_native,&nst_native,sizeof nst_native);
for(unsigned k=0;k<256;k++){assert(!nord2_wifi_join_disconnect());assert(!memcmp(idle_service,&njs_native,sizeof njs_native)&&!memcmp(idle_native,&nst_native,sizeof nst_native));assert(!ifnet_down&&!prepared_calls);}
source_ready=0;assert(nord2_wifi_join_disconnect()==1);source_ready=1;
nss_native.failed=1;assert(nord2_wifi_join_disconnect()==1);nss_native.failed=0;
njs_native.used=1;assert(nord2_wifi_join_disconnect()==1);njs_native.used=0;
nst_native.attempted=1;assert(nord2_wifi_join_disconnect()==1);nst_native.attempted=0;
nst_native.active=1;assert(nord2_wifi_join_disconnect()==1);nst_native.active=0;
njs_native.snapshot.phase=NJS_FAILED;assert(nord2_wifi_join_disconnect()==1);njs_native.snapshot.phase=NJS_IDLE;
assert(!memcmp(idle_service,&njs_native,sizeof njs_native)&&!memcmp(idle_native,&nst_native,sizeof nst_native));
printf("PASS256 idle disconnects: byteidentical service/native ledger; unavailable/failed/used/nonidle refused; no ifnet/MMIO/prepare\n");
/* Same production binder and admissionlock: old scans never make SET20 EBUSY.
 * Native prepare remains a refusal-only seam; actual DMA proof is separate. */
 for(unsigned state=NSS_QUEUED;state<=NSS_RUNNING;state++){
  nss_native.slot[0].state=state;assert(!nord2_wifi_join_enqueue(&q,(void*)1,7,complete,&ticket));uint64_t accepted=ticket;
  assert(njs_scan_blocked_locked()&&njs_pending_locked()&&njs_native.request.pmk[0]==0x42);
  uint64_t unchanged=UINT64_C(0xdead);assert(nord2_wifi_join_enqueue(&q,(void*)1,7,complete,&unchanged)==2&&unchanged==UINT64_C(0xdead));
  assert(njs_owner_work()==0&&!prepared_calls&&njs_native.queued);
  assert(!nord2_wifi_join_cancel(accepted));assert(njs_owner_work()==1&&!njs_native.queued&&!njs_native.active&&!njs_native.used&&njs_native.cookie_retired);assert(!njs_native.completion&&!njs_native.cookie);struct niwj_request z={0};assert(!memcmp(&njs_native.request,&z,sizeof z));
 }
 /* Expired or backward-clock queued work retires once, even while scanning. */
 assert(!nord2_wifi_join_enqueue(&q,(void*)1,7,complete,&ticket));uint64_t deadline=njs_native.deadline_us;clock_us=deadline;assert(njs_owner_work()==1&&njs_native.snapshot.result==9&&!prepared_calls&&!njs_native.used);assert(njs_owner_work()==0);
 clock_us=1000;assert(!nord2_wifi_join_enqueue(&q,(void*)1,7,complete,&ticket));clock_us=999;assert(njs_owner_work()==1&&njs_native.snapshot.result==2&&!prepared_calls);clock_us=2000;
 assert(notices[NJS_RETIRED]==4&&release_reentry==4);
 /* Source-owned completed target: deferred debt remains in first prepare's
  * deadline, rather than starting a fresh8/30second budget after scans. */
 struct nsb_scan target_scan={0};target_scan.entered=target_scan.done=1;target_scan.completion.state=7;target_scan.started=1;target_scan.last=101;target_scan.deadline=1000;target_scan.count=target_scan.frames=1;
 struct nsb_record*r=&target_scan.records[0];r->bssid[0]=2;r->bssid[5]=1;r->ssid_len=4;r->ssid_present=1;memcpy(r->ssid,"test",4);r->subtype=8;r->channel=r->hw_channel=1;r->privacy=r->rsn_present=1;r->capability=17;r->observed_us=100;r->rxv_valid=1;r->group_valid=4;r->rxv[4]=100;r->rxv[5]=255;r->ies[0]=1;r->ies[1]=1;r->ies[2]=0x82;memcpy(r->ies+3,q.station_rsn,22);r->ie_bytes=r->ie_total=25;nss_native.info.count=1;nss_native.info.channels[0].band=nss_native.info.channels[0].channel=1;njs_cache_scan_locked(&target_scan);
 clock_us=UINT64_C(3000000);assert(!nord2_wifi_join_enqueue(&q,(void*)1,7,complete,&ticket));njs_native_deadline_test=njs_native.deadline_us;clock_us+=UINT64_C(2000000);assert(njs_owner_work()==0&&!prepared_calls);nss_native.slot[0].state=0;assert(njs_owner_work()==1&&prepared_calls==1&&njs_native_deadline_test==UINT64_C(11000000)&&!njs_native.used&&njs_native.cookie_retired);assert(notices[NJS_RETIRED]==5);/* Advance both pure prepare and the RNG used by real nwn_begin. The old
  * owner timestamp predates transaction.last_ms and must not reach pump. */
 prepare_success=1;entropy_advance_us=1000;clock_us=6000000;nss_native.info.mac[0]=2;nss_native.info.mac[5]=2;
 assert(!nord2_wifi_join_enqueue(&q,(void*)1,7,complete,&ticket));njs_native_deadline_test=njs_native.deadline_us;uint64_t before_constructor=clock_us;
 assert(njs_owner_work()==1&&njs_native.active&&njs_native.used&&!njs_native.driver.owner.halted&&!nst_native.state.halt);
 assert(clock_us==before_constructor+2000&&njs_native.driver.owner.transaction.phase==STC_PREPARE&&njs_native.driver.owner.transaction.last_ms==clock_us/1000);
 printf("PASS constructor/RNG clock advance: first owner pump resamples monotonic time after real nwn_begin; no native publication\n");
 /* Only host doubles were adopted and submit remained BUSY; no firmware
  * epoch or key is claimed. Reset this isolated seam for legacy cases. */
 nwp_cancel(&njs_native.driver.owner.transaction.wpa);memset(&njs_native.driver,0,sizeof njs_native.driver);memset(&njs_native.arbiter,0,sizeof njs_native.arbiter);memset(&nst_native,0,sizeof nst_native);niwj_clear(&njs_native.request,sizeof njs_native.request);njs_native.active=njs_native.used=njs_native.queued=0;njs_native.completion=0;njs_native.cookie=0;prepare_success=0;entropy_advance_us=0;memset(nss_native.info.mac,0,6);
 /* New pure pre-publication rollback seam: constructor expires after
  * preparation, retires/wipes once, and preserves the unused old native object. */
 prepare_success=prepare_expiry=1;clock_us=UINT64_C(7000000);assert(!nord2_wifi_join_enqueue(&q,(void*)1,7,complete,&ticket));njs_native_deadline_test=njs_native.deadline_us;
 unsigned before_abort=constructor_aborts;assert(njs_owner_work()==1&&constructor_aborts==before_abort+1&&!nst_native.state.prepared&&!nst_native.state.mutated&&!njs_native.used&&!njs_native.active&&njs_native.cookie_retired&&njs_native.snapshot.result==9);
 struct niwj_request after_expiry={0};assert(!memcmp(&after_expiry,&njs_native.request,sizeof after_expiry));prepare_success=0;
 memset(njs_native.cache,0,sizeof njs_native.cache);njs_native.cache_next=0;prepared_calls=0;
 memset(notices,0,sizeof notices);release_reentry=0;

assert(!nord2_wifi_join_enqueue(&q,(void*)1,7,complete,&ticket));memset(&q,0,sizeof q);assert(njs_native.request.pmk[0]==0x42&&njs_scan_blocked_locked()&&njs_pending_locked());assert(!nord2_wifi_join_cancel(ticket));assert(njs_owner_work()==1&&!njs_native.queued&&!njs_native.used);assert(notices[NJS_JOIN_FAILED]==1&&notices[NJS_RETIRED]==1&&release_reentry==1&&!prepared_calls);struct niwj_request zero={0};assert(!memcmp(&njs_native.request,&zero,sizeof zero));
q=request();assert(!nord2_wifi_join_enqueue(&q,(void*)1,7,complete,&ticket));assert(!nord2_wifi_join_disconnect()&&ifnet_down==1);assert(njs_native.snapshot.phase==NJS_LEAVING&&!njs_native.snapshot.data_ready);assert(njs_owner_work()==1&&notices[NJS_RETIRED]==2);
/* Only completed overflow source scans are cacheable; records remain identical. */
struct nsb_scan scan={0};scan.entered=scan.done=1;scan.completion.state=7;scan.started=1;scan.last=101;scan.deadline=1000;scan.count=NSB_RESULTS;scan.frames=NSB_RESULTS+1;scan.partial=scan.dropped=1;scan.records[0].bssid[0]=2;unsigned cache_before=njs_native.cache_next;njs_cache_scan_locked(&scan);assert(njs_native.cache_next==cache_before+1&&!memcmp(&scan,&njs_native.cache[cache_before%4],sizeof scan));
scan.timed_out=1;njs_cache_scan_locked(&scan);assert(njs_native.cache_next==cache_before+1);scan.timed_out=0;scan.done=0;njs_cache_scan_locked(&scan);assert(njs_native.cache_next==cache_before+1);
/* Synthetic authoritative-ready seam tests synchronous revoke, not RF readiness. */
njs_native.active=njs_native.used=njs_native.linked=1;njs_native.native_generation=3;njs_native.ticket=3;njs_native.epoch=7;njs_native.cookie=(void*)1;njs_native.completion=complete;njs_native.snapshot.phase=NJS_AUTHORIZED;njs_native.snapshot.data_ready=njs_native.snapshot.associated=njs_native.snapshot.ptk_ready=njs_native.snapshot.gtk_ready=1;njs_native.leave=njs_native.canceled=0;nst_native.state.controlled_port=1;njs_native.driver.owner.transaction.phase=STC_CONNECTED;njs_native.driver.owner.transaction.outcome=STC_AUTHORIZED;
uint8_t frame[14]={0};frame[12]=8;uint8_t invalid[14]={0};assert(nord2_wifi_link_send(invalid,14,3)==NWIL_SEND_INVALID);invalid[12]=8;invalid[6]=1;assert(nord2_wifi_link_send(invalid,14,3)==NWIL_SEND_INVALID);assert(!nord2_wifi_link_send(frame,14,3)&&njs_native.tx_count==1);assert(!nord2_wifi_join_disconnect());assert(!nst_native.state.controlled_port&&!njs_native.tx_count&&!njs_native.linked&&!njs_native.snapshot.data_ready&&!njs_native.snapshot.associated&&ifnet_down==2);assert(nord2_wifi_link_send(frame,14,3)==NWIL_SEND_NOT_READY);/* Explicit leave now admits ONE bounded next request, without replacing
 * the old native identity or granting physical readiness in this seam. */
unsigned before_prepare=prepared_calls;assert(!nord2_wifi_join_enqueue(&q,(void*)2,9,handover_canceled,&ticket)&&njs_handover.state==1&&njs_native.ticket==3&&njs_native.native_generation==3&&prepared_calls==before_prepare);assert(!nord2_wifi_join_cancel(ticket));assert(njs_owner_work()==1&&handover_failed==1&&handover_retired==1&&!njs_handover.state&&prepared_calls==before_prepare&&njs_native.ticket==3&&njs_native.native_generation==3);
/* Authorized clientclose detaches delivery without closing connection. */
njs_native.leave=0;njs_native.active=njs_native.linked=1;njs_native.snapshot.phase=NJS_AUTHORIZED;nst_native.state.controlled_port=1;assert(!nord2_wifi_join_cancel(3)&&njs_native.detach&&!njs_native.canceled&&nst_native.state.controlled_port);
seed_ready(4);assert(njs_owner_work()==1&&njs_native.linked&&njs_native.snapshot.phase==NJS_AUTHORIZED&&njs_native.snapshot.result==1&&njs_native.tx_count==1&&notices[NJS_JOINED]==1);assert(!nord2_wifi_join_disconnect()&&!njs_native.tx_count);
seed_ready(5);ifnet_error=1;assert(njs_owner_work()==1&&!njs_native.linked&&!njs_native.tx_admit&&!njs_native.tx_count&&njs_native.snapshot.result!=1&&!nst_native.state.controlled_port);assert(njs_owner_work()==1&&njs_native.cookie_retired&&notices[NJS_JOINED]==1);assert(ifnet_up==2);
seed_ready(6);ifnet_error=0;block_up=1;pthread_t owner_worker,closer;assert(!pthread_create(&owner_worker,NULL,owner_thread,NULL));pthread_mutex_lock(&barrier);while(!up_entered)pthread_cond_wait(&barrier_cond,&barrier);pthread_mutex_unlock(&barrier);assert(!pthread_create(&closer,NULL,disconnect_thread,NULL));pthread_mutex_lock(&barrier);while(!disconnect_started)pthread_cond_wait(&barrier_cond,&barrier);assert(!disconnect_done);assert(pthread_mutex_trylock(&njs_native.notice_lock->mutex)==EBUSY);release_up=1;pthread_cond_broadcast(&barrier_cond);pthread_mutex_unlock(&barrier);assert(!pthread_join(owner_worker,NULL)&&!pthread_join(closer,NULL));assert(disconnect_done&&!njs_native.linked&&!njs_native.tx_admit&&!nst_native.state.controlled_port&&!njs_native.tx_count&&njs_native.snapshot.phase==NJS_LEAVING);/* Typed data retirement never creates an RF-success statistic or authority. */
unsigned local=njs_native.tx_local,ok=njs_native.tx_ok,results=njs_native.tx_results,failed=njs_native.tx_fail,dropped=njs_native.queued_drop,logs=njs_native.logs;struct nst_event local_event={.kind=NST_DATA_SUBMITTED,.ticket=UINT64_MAX,.generation=njs_native.native_generation};assert(!njs_event(&njs_native,&local_event));assert(njs_native.tx_local==local+1&&njs_native.tx_ok==ok&&njs_native.tx_results==results&&njs_native.tx_fail==failed&&njs_native.logs==logs+1&&!nst_native.state.controlled_port);local_event.status=1;assert(!njs_event(&njs_native,&local_event));assert(njs_native.queued_drop==dropped+1&&njs_native.tx_local==local+1);local_event.generation++;assert(!njs_event(&njs_native,&local_event)&&njs_native.queued_drop==dropped+1);
assert(refresh_calls&&constructor_commits&&constructor_aborts);assert(!scan_build_calls&&!scan_native_calls&&!scan_delivery_calls);
printf("PASS actual live service binder admission/copy/wipe/reentrant-retire/synchronous-disconnect/authorized-detach; mocked native IO only\n");IOLock*notice=njs_native.notice_lock;assert(!pthread_mutex_destroy(&notice->mutex));free(notice);assert(!pthread_mutex_destroy(&nss_native.lock->mutex));free(nss_native.lock);return 0;}
