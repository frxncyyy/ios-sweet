/* Included after NSS/NJS/native station definitions. All execution belongs to
 * the existing retained owner; the USB interrupt-masked decoder only queues. */
#include "wifi_live_control.h"
static struct {unsigned mode,windows,pending;} nwl_native;
static void nwl_configure_station(void){
 if(!nwl_enabled()||!nwl_native.mode)return;
 /* Each new station consumes a bounded capture window too. A command armed
  * while idle reserved its window already; never grant a free per-join reset. */
 if(nwl_native.pending)nwl_native.pending=0;
 else if(nwl_native.windows<NWL_CAPTURE_WINDOWS)nwl_native.windows++;
 else nwl_native.mode=0;
 nsdd_capture(&nst_native.state.data_diag,nwl_native.mode,nwl_native.windows);
}
static void nwl_native_start(void){
 unsigned option=0;PE_parse_boot_argn("nord2wifilive",&option,sizeof option);
 if(option!=1||ncd_native.worker!=current_thread())return;
 uint64_t nonce=0;read_random(&nonce,sizeof nonce);if(!nonce)return;
 if(!nwl_initialize(nonce))printf("NORD2 WIFI LIVE: ready=1 protocol=1\n");
}
/* Caller owns the NSS lock. Copy explicitly public scalars, never native
 * structs with padding, MACs, SSIDs, pointers, nonces, keys or replay values. */
static void nwl_snapshot_locked(struct nwl_snapshot*out){
 *out=(struct nwl_snapshot){0};uint32_t*w=out->word;
 struct nst*s=&nst_native.state;struct nsdd*d=&s->data_diag;
 const struct njs_snapshot*j=&njs_native.snapshot;
 uint64_t g=njs_native.native_generation,t=njs_uptime_us()/1000;
 w[NWL_F_GENERATION_LO]=(uint32_t)g;w[NWL_F_GENERATION_HI]=(uint32_t)(g>>32);
 w[NWL_F_TIME_LO]=(uint32_t)t;w[NWL_F_TIME_HI]=(uint32_t)(t>>32);
 w[NWL_F_SERVICE]=j->phase;w[NWL_F_NATIVE_PHASE]=s->phase;
 w[NWL_F_ERROR]=s->error;w[NWL_F_HALT]=!!s->halt;
 w[NWL_F_FLAGS]=!!njs_native.active|(!!njs_native.linked<<1)|
 (!!j->associated<<2)|(!!j->ptk_ready<<3)|(!!j->gtk_ready<<4)|
 (!!s->controlled_port<<5)|(!!njs_native.used<<6)|
 (!!njs_native.retiring<<7)|(!!nss_native.failed<<8);
 w[NWL_F_TX_PENDING]=njs_native.tx_count;w[NWL_F_RX_PENDING]=njs_native.rx_count;
 /* dma_completed already includes HW0 header DDONE. Normal.completed is
  * subsequent token retirement for those same packets, not extra transfers. */
 w[NWL_F_SUBMITTED]=s->submitted;w[NWL_F_COMPLETED]=s->dma_completed;
 w[NWL_F_RX_UNICAST]=d->rx_unicast;w[NWL_F_RX_GROUP]=d->rx_group;
 w[NWL_F_TX_DATA]=d->tx_data;w[NWL_F_TX_SECURITY]=d->tx_security;
 w[NWL_F_TX_DHCP]=d->tx_dhcp;w[NWL_F_RX_DHCP]=d->rx_dhcp;
 w[NWL_F_OFFER]=d->reply_match[2];w[NWL_F_ACK]=d->reply_match[5];w[NWL_F_NAK]=d->reply_match[6];
 w[NWL_F_RX_ERROR]=d->rejected[NSDD_RX_ERROR];w[NWL_F_RX_PEER]=d->rejected[NSDD_RX_PEER];
 w[NWL_F_RX_KEY]=d->rejected[NSDD_RX_KEY];w[NWL_F_RX_REPLAY]=d->rejected[NSDD_RX_REPLAY];
 w[NWL_F_RX_PLAIN]=d->rejected[NSDD_RX_PLAINTEXT];
 w[NWL_F_TXS_OK]=d->txs_ok;w[NWL_F_TXS_FAIL]=d->txs_fail;w[NWL_F_TXS_AMBIGUOUS]=d->ambiguous;
 w[NWL_F_CAPTURE]=nwl_native.mode;w[NWL_F_WINDOWS_LEFT]=NWL_CAPTURE_WINDOWS-nwl_native.windows;
 w[NWL_F_OFLD_DROP]=s->ofld_drop;w[NWL_F_DATA_CURSOR]=s->normal.next;w[NWL_F_COMMAND_CURSOR]=s->tx_next;
 for(unsigned k=0;k<5;k++)w[NWL_F_RX0+k]=s->queue[k].next;
 w[NWL_F_DATA_PUBLISHED]=!!s->normal.published;w[NWL_F_DATA_REPORT]=!!s->normal.report_done;
 w[NWL_F_DATA_DDONE]=!!s->normal.dma_done;w[NWL_F_AIR_PENDING]=!!s->air_pending;
 w[NWL_F_WIRE_BUSY]=!!s->wire_busy;w[NWL_F_TX_ACCEPTED]=njs_native.tx_accepted;
 w[NWL_F_TX_LOCAL]=njs_native.tx_local;w[NWL_F_RX_DELIVERED]=njs_native.rx_delivered;
 w[NWL_F_JOIN_READY]=!!(njs_native.notice_lock&&!nss_native.failed&&
  !njs_native.active&&!njs_native.queued&&!njs_native.retiring&&!njs_native.completion&&
  njs_native.serial<255&&(!njs_native.used||njs_rejoin_ready_locked()));
}
static void nwl_native_work(void){
 struct nwl_request q;if(!nwl_take(&q))return;
 struct nwl_snapshot snapshot={0};unsigned result=NWL_OK,disconnect=0;
 if(!nss_available()||ncd_native.worker!=current_thread()){
  (void)nwl_complete(&q,NWL_UNAVAILABLE,&snapshot);return;
 }
 IOLockLock(nss_native.lock);
 uint64_t generation=njs_native.native_generation;
 if(q.op!=NWL_STATUS&&(generation>NWL_MAX_GENERATION||q.generation!=generation))result=NWL_STALE;
 else if(q.op!=NWL_STATUS&&(nss_native.failed||nst_native.state.halt||njs_native.retiring))result=NWL_REFUSED;
 else if(q.op>=NWL_CAPTURE_DHCP&&q.op<=NWL_CAPTURE_STOP){
  if(q.op!=NWL_CAPTURE_STOP&&nwl_native.windows==NWL_CAPTURE_WINDOWS)result=NWL_LIMIT;
  else{nwl_native.mode=q.op==NWL_CAPTURE_STOP?0:q.op==NWL_CAPTURE_DHCP?1:2;
   nwl_native.pending=0;
   if(nwl_native.mode){nwl_native.windows++;
    nwl_native.pending=!njs_native.active||!nst_native.state.prepared||nst_native.state.cleanup_started;}
   nsdd_capture(&nst_native.state.data_diag,nwl_native.pending?0:nwl_native.mode,nwl_native.windows);}
 }else if(q.op==NWL_OFLD_ALLOW||q.op==NWL_OFLD_DROP){
  if(!njs_native.active||!nst_native.state.prepared||nst_native.state.cleanup_started||njs_native.leave||njs_native.canceled)result=NWL_REFUSED;
  else nst_native.state.ofld_drop=q.op==NWL_OFLD_DROP;
 }else if(q.op==NWL_DISCONNECT)disconnect=1;
 IOLockUnlock(nss_native.lock);
 /* Existing API takes its own locks, revokes port/carrier and schedules
  * normal asynchronous cleanup. ACK/OK does not claim that cleanup is done. */
 if(disconnect&&nord2_wifi_join_disconnect())result=NWL_REFUSED;
 IOLockLock(nss_native.lock);nwl_snapshot_locked(&snapshot);IOLockUnlock(nss_native.lock);
 (void)nwl_complete(&q,result,&snapshot);
 printf("NORD2 WIFI LIVE: token=%u op=%u expected=%u result=%u generation=%llu\n",q.token,q.op,q.generation,result,(unsigned long long)generation);
}
