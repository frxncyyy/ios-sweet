/* Included after NSS/NJS/native station definitions. All execution belongs to
 * the existing retained owner; the USB interrupt-masked decoder only queues. */
#include "wifi_live_control.h"
/* Selected AXI CSR0x18000000 + bus WTBLON0x430000. Only fixed
 * selector page and WLAN5 DW0/DW2 page; mappings retained, never written. */
static struct {unsigned mode,windows,pending,peer_attempts;
 uintptr_t peer_selector,peer_entry;} nwl_native;
static unsigned nwl_join_ready_locked(void){
 return !!(nss_native.powered&&njs_native.notice_lock&&!nss_native.failed&&
  !njs_native.active&&!njs_native.queued&&!njs_native.retiring&&!njs_native.completion&&
  njs_native.serial<255&&(!njs_native.used||njs_rejoin_ready_locked()));
}
/* No generic MMIO interface: exact three physical read locations only.
 * Native owner proof is checked afresh before each actual read. */
static int nwl_peer_read(unsigned slot,uint32_t*out){
 if(!out||slot>2||!nst_native_probe_window(&nwfn_native)||ncd_native_owner(&ncd_native))return -1;
 *out=slot?ni_rd(nwl_native.peer_entry,slot==1?0x500:0x508):ni_rd(nwl_native.peer_selector,0);
 return 0;
}
static unsigned nwl_peer_probe_locked(uint32_t*meta,unsigned*error){
 struct nst*s=&nst_native.state;*meta=0;*error=NWL_PEER_PRECONDITION;
 if(!njs_native.active||njs_native.queued||njs_native.leave||njs_native.canceled||
  !nst_native.active||!s->prepared||s->halt||s->error||s->cleanup_started||
  s->generation!=njs_native.native_generation||s->peer.wlan!=5||s->phase!=NST_IDLE||s->active||
  s->wire_busy||s->air_pending||s->callback_active||s->peer_kind||s->activation_pending||
  s->submitted!=s->dma_completed||!nstx_idle(&s->normal))return NWL_REFUSED;
 if(nwl_native.peer_attempts==NWL_PEER_PROBES)return NWL_LIMIT;
 if(!nwl_native.peer_selector||!nwl_native.peer_entry){*error=NWL_PEER_MAP;return NWL_UNAVAILABLE;}
 nwl_native.peer_attempts++;
 uint32_t before,d0,d2,after;unsigned result=NWL_REFUSED;*error=NWL_PEER_OWNER;
 if(nst_native_probe_begin())return NWL_REFUSED;
 if(nwl_peer_read(0,&before))goto done;
 if(before&7u){*error=NWL_PEER_SELECTOR;goto done;}
 if(nwl_peer_read(1,&d0)||nwl_peer_read(2,&d2)||nwl_peer_read(0,&after))goto done;
 if((after&7u)!=0){*error=NWL_PEER_CHANGED;goto done;}
 *meta=((d0>>26)&3u)|(((d0>>16)&63u)<<2)|(((d2>>16)&31u)<<8)|(((d2>>27)&1u)<<13);
 *error=NWL_PEER_NONE;result=NWL_OK;
 done:nst_native_probe_end();return result;
}
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
 if(!nwl_initialize(nonce)){
  nwl_native.peer_selector=ml_io_map(0x18434000,0x1000);
  nwl_native.peer_entry=ml_io_map(0x18438000,0x1000);
  printf("NORD2 WIFI LIVE: ready=1 protocol=2\n");
 }
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
 w[NWL_F_JOIN_READY]=nwl_join_ready_locked();
 w[NWL_F_NEXT_DATA_PROFILE]=nst_native_next_data_profile();
 w[NWL_F_DATA_PROFILE]=nst_native_data_profile();
}
static void nwl_native_work(void){
 struct nwl_request q;if(!nwl_take(&q))return;
 struct nwl_snapshot snapshot={0};unsigned result=NWL_OK,disconnect=0,peer_error=0;uint32_t peer_meta=0;
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
 else if(q.op==NWL_PEER_PROBE)result=nwl_peer_probe_locked(&peer_meta,&peer_error);
 else if(q.op==NWL_DATA_ETHERNET||q.op==NWL_DATA_TODS){
  if(!nwl_join_ready_locked()||nst_native_set_next_data_profile(q.op==NWL_DATA_TODS))result=NWL_REFUSED;
 }
 IOLockUnlock(nss_native.lock);
 /* Existing API takes its own locks, revokes port/carrier and schedules
  * normal asynchronous cleanup. ACK/OK does not claim that cleanup is done. */
 if(disconnect&&nord2_wifi_join_disconnect())result=NWL_REFUSED;
 IOLockLock(nss_native.lock);nwl_snapshot_locked(&snapshot);IOLockUnlock(nss_native.lock);
 if(q.op==NWL_PEER_PROBE){snapshot.word[NWL_F_PEER_VALID]=result==NWL_OK;
  snapshot.word[NWL_F_PEER_META]=peer_meta;snapshot.word[NWL_F_PEER_ERROR]=peer_error;}
 (void)nwl_complete(&q,result,&snapshot);
 printf("NORD2 WIFI LIVE: token=%u op=%u expected=%u result=%u generation=%llu\n",q.token,q.op,q.generation,result,(unsigned long long)generation);
}
