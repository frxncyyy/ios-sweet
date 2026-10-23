/* Same-worker verified teardown. A new model requires a further actual NSS
 * scan; historical attempt, DMA pool and boot PID ledger remain retained. */
static int nss_scan_return_origin_valid(const struct nct_session*s){
 if(!nss_return.ready)return !nst_native.attempted&&!nst_native.active&&s->continuation!=2&&!s->scan_return;
 return s==&nct_native.session&&s->scan_return==&nss_return.origin&&
  nss_return.attempted==1&&nss_return.ready==1&&nss_return.origin.sealed==1&&
  !nst_native.active&&!njs_native.active&&!njs_native.linked&&!njs_native.tx_admit&&
  ((njs_native.queued&&nsr_native.admitted_generation==njs_native.native_generation&&njs_native.native_generation>nsr_native.closed_generation)||
   (njs_native.cookie_retired&&!njs_native.retiring&&!njs_native.cookie&&!njs_native.completion))&&
  njs_native.driver.owner.transaction.phase==STC_DONE&&njs_native.scan_returned==1&&njs_native.used==1&&
  njs_native.arbiter.connection_used==1&&nst_native.attempted==1&&
  nss_return.origin.generation==nsr_native.closed_generation;
}
/* Pure prerequisite failure leaves scans blocked. A physical owner/read fault
 * keeps the existing common halt/drain path, never a guessed return origin. */
static int nss_return_scans(void){
 if(ncd_native.worker!=current_thread())return 1;
 IOLockLock(nss_native.lock);
 if(njs_native.scan_returned){IOLockUnlock(nss_native.lock);return 0;}
 struct nst*n=&nst_native.state;struct nwo*o=&njs_native.driver.owner;struct stc*t=&o->transaction;
 struct nwt_state*w=&nwfn_native.init.transport;
 if(nsr_native.ptk||nsr_native.gtk||nsr_native.pending||nsr_native.bad||nss_return.attempted||nss_return.ready||nss_native.failed||nct_native.active||
  !ncd_native.active||ncd_native.parked||ncd_native.halt||nwfn_native.halt||nwfn_native.stage!=NWFN_RETAINED||!nwfn_native.ap_owned||!nwfn_native.wf_owned||nwfn_native.asleep||
  !nfp_native.ready||nfp_native.halt||nfp_native.state.halt||nfp_native.state.error||!nfp_native.finish_done||
  !njs_native.used||njs_native.queued||njs_native.active||!njs_native.terminal||njs_native.retiring||!njs_native.cookie_retired||njs_native.cookie||njs_native.completion||njs_native.linked||njs_native.tx_admit||njs_native.tx_count||njs_native.rx_count||
  njs_native.snapshot.phase!=NJS_FAILED||njs_native.snapshot.data_ready||njs_native.snapshot.associated||njs_native.snapshot.ptk_ready||njs_native.snapshot.gtk_ready||
  !njs_native.native_generation||njs_native.driver.transport!=n||t->generation!=njs_native.native_generation||t->phase!=STC_DONE||t->action.kind||o->halted||o->issuing||o->in_ticket||o->in_kind||!o->closed||
  t->arbiter!=&njs_native.arbiter||njs_native.arbiter.mode||njs_native.arbiter.generation||!njs_native.arbiter.connection_used||
  nst_native.attempted!=1||nst_native.active!=1||!n->prepared||n->halt||n->error||n->active||n->phase!=NST_IDLE||n->callback_active||n->generation!=njs_native.native_generation||n->init!=&nwfn_native.init||n->previous!=&nct_native.session||n->previous_rx!=&ncb_native.rx||
  !nct_done_receipt(n->previous)||n->previous->basic!=&nbc_native.state||n->previous->cap!=&ncd_native.diag||n->previous->init!=&nwfn_native.init||n->previous->firmware!=&nfp_native||
  n->base_completed!=n->previous->base_completed+n->previous->count||
  !n->cleanup_started||n->wire_cid!=0x17||n->wire_type||n->wire_sequence!=(uint8_t)(w->next_sequence-1u)||n->peer_step!=3||!n->dma_done||n->wire_busy||n->air_pending||n->peer_kind||n->activation_pending||n->channel_held||n->sta_active||n->ptk_ready||n->controlled_port||n->ptk.installed||n->gtk.installed||
  !nstx_idle(&n->normal)||!n->submitted||n->submitted!=n->dma_completed||n->base_completed>UINT32_MAX-n->dma_completed||w->completed!=n->base_completed+n->dma_completed-n->normal.dma_completed||w->completed==UINT32_MAX||w->tx_index!=n->tx_next||w->tx_index>=1024||w->submit_attempted||w->tx_done||w->rx_done||w->halt||nwfn_native.init.halt||
  n->credit_reserved||n->credit_available!=n->credit_capacity||n->credit_acquired!=n->credit_released||n->credit_capacity!=n->previous->capacity||n->credit_capacity!=nbc_native.state.plan.pse_cmd-1||
  w->rx_index!=n->queue[1].next){IOLockUnlock(nss_native.lock);return 1;}
 for(unsigned k=0;k<5;k++){
  const struct ncr_queue*q=&n->queue[k];const struct nwi_ring*r=&nwfn_native.init.rings[k+4];
  if(q->ring.descriptors!=r->descriptors||q->ring.buffers!=r->buffers||q->ring.base!=r->base||q->ring.count!=r->count||q->ring.ext!=r->ext||q->ring.rx!=r->rx||q->next>=r->count||q->segmented>1){IOLockUnlock(nss_native.lock);return 1;}
 }
 nss_return.attempted=1;unsigned phase=n->phase,active=n->active;int result=-1;uint32_t v;uint64_t now;uint8_t descriptor[16];
 n->active=1;n->phase=NST_TX_READ;
 /* Same checked native callbacks/lease, no writes and no returned addresses. */
 if(ncd_io.bus->owner(&ncd_native)||ncd_io.now_us(&ncd_native,&now)||now<n->last||now>=n->deadline)goto restore;
 n->last=now;
 if(ncd_io.bus->reg_read(&ncd_native,0x180600f0,&v)||(v&3)!=3)goto restore;
 for(unsigned k=0;k<2;k++)if(ncd_io.bus->reg_read(&ncd_native,0x18024200+4096u*k,&v)||(v&0x00300000))goto restore;
 if(ncd_io.bus->reg_read(&ncd_native,0x18025418,&v)||v!=n->tx_next||ncd_io.bus->reg_read(&ncd_native,0x1802541c,&v)||v!=n->tx_next)goto restore;
 uint64_t cell=w->tx_ring+16u*n->tx_next;
 if(ncd_io.bus->sync(&ncd_native,cell,16,2)||ncd_io.bus->mem_read(&ncd_native,cell,descriptor,16)||!(descriptor[7]&128))goto restore;
 n->phase=NST_DATA_READ;
 if(ncd_io.bus->reg_read(&ncd_native,0x18025308,&v)||v!=n->normal.next||ncd_io.bus->reg_read(&ncd_native,0x1802530c,&v)||v!=n->normal.next)goto restore;
 cell=n->normal.ring+16u*n->normal.next;
 if(ncd_io.bus->sync(&ncd_native,cell,16,2)||ncd_io.bus->mem_read(&ncd_native,cell,descriptor,16)||!(descriptor[7]&128))goto restore;
 result=0;
restore:
 n->phase=phase;n->active=active;
 if(result){IOLockUnlock(nss_native.lock);return -1;}
 /* Seal exact current counters independently: frames do not advance command
  * sequence. Old nct.session may now be replaced only through this origin. */
 nss_return.origin=(struct nct_scan_return){.basic=&nbc_native.state,.cap=&ncd_native.diag,.init=&nwfn_native.init,.firmware=&nfp_native,
  .sealed=1,.completed=w->completed,.tx_index=w->tx_index,.pse_total=nbc_native.state.plan.pse_cmd,.capacity=n->credit_capacity,.next_sequence=w->next_sequence,.generation=n->generation};
 for(unsigned k=0;k<5;k++)nss_return.queue[k]=n->queue[k];
 nsr_native.closed_generation=n->generation;nsr_native.closed_us=n->last;nsr_native.refresh_requested_us=n->last;nsr_native.refresh_after_scan=0;
 nsr_native.hw0_next=n->normal.next;nsr_native.token_next=n->normal.token_next;nsr_native.next_pid=n->next_pid;
 nsr_native.scan_ticket=0;nsr_native.scan_ready=0;nsr_native.scan_state=1;
 niwj_clear(njs_native.cache,sizeof njs_native.cache);njs_native.cache_next=0;
 n->pm_pending=0;nst_native.active=0;nss_return.ready=1;njs_native.scan_returned=1;
 printf("NORD2 WIFI IOS SCANRETURN: generation=%llu completed=%u tx=%u sequence=%u rx_event=%u used=1\n",(unsigned long long)n->generation,w->completed,w->tx_index,w->next_sequence,w->rx_index);
 IOLockUnlock(nss_native.lock);njs_rejoin_refresh();return 0;
}

/* Called with NSS admission lock held. Scan callback release is deferred by
 * NSS; no reuse is admitted while ANY old callback is still executing. */
static int njs_rejoin_ready_locked(void){
 if(!njs_native.used||!nsr_native.scan_ready||nsr_native.bad||nsr_native.ptk||nsr_native.gtk||nsr_native.pending||
  !njs_native.scan_returned||nst_native.active||nst_native.state.halt||nst_native.state.error||nst_native.state.callback_active||nst_native.state.wire_busy||nst_native.state.air_pending||!nstx_idle(&nst_native.state.normal)||nst_native.state.submitted!=nst_native.state.dma_completed||nss_native.failed||ncd_native.halt||nwfn_native.halt||nct_native.active||
  njs_native.active||njs_native.retiring||njs_native.cookie||njs_native.completion||!njs_native.cookie_retired||
  nsr_native.closed_generation!=nss_return.origin.generation||!nct_done_receipt(&nct_native.session)||!nst_scan_admissible(&ncb_native.rx.scan))return 0;
 for(unsigned k=0;k<NSS_SLOTS;k++)if(nss_native.slot[k].callback_busy)return 0;
 return nsr_native.closed_generation<255&&nsr_native.next_pid<=NST_PID_MANAGEMENT_MAX-4;
}
static void njs_rejoin_scan_done(void*cookie,uint64_t generation,uint64_t ticket,enum nss_status status){
 struct nss_view v={0};unsigned usable=!nord2_wifi_scan_view(ticket,&v)&&v.scan&&niwj_scan_usable(v.scan);
 IOLockLock(nss_native.lock);
 if(cookie==&nsr_native&&nsr_native.scan_state==2&&generation==nsr_native.closed_generation&&ticket==nsr_native.scan_ticket){
  nsr_native.scan_ready=usable&&(status==NSS_COMPLETE||status==NSS_PARTIAL)&&v.client_generation==generation&&
   v.captured_us>=nsr_native.closed_us&&v.scan->started>=nsr_native.closed_us&&nss_return.consumed&&
   !nct_native.active&&nct_done_receipt(&nct_native.session)&&!ncb_native.rx.active&&!ncb_native.rx.halt&&!ncb_native.rx.error;
  nsr_native.scan_state=!nsr_native.scan_ready&&nsr_native.refresh_after_scan?1:3;nsr_native.refresh_after_scan=0;
 }
 IOLockUnlock(nss_native.lock);(void)nord2_wifi_scan_release(ticket);
}
static void njs_rejoin_refresh(void){
 if(ncd_native.worker!=current_thread()||!nss_available())return;
 IOLockLock(nss_native.lock);
 unsigned run=nss_native.powered&&nsr_native.scan_state==1&&!nss_native.failed&&!njs_native.queued&&!njs_native.active;
 uint64_t generation=nsr_native.closed_generation;
 /* One finite publication attempt; queue-full may retry only inside30s. */
 uint64_t now=njs_uptime_us();
 if(run&&(now<nsr_native.refresh_requested_us||now-nsr_native.refresh_requested_us>=UINT64_C(30000000))){nsr_native.scan_state=3;run=0;}
 IOLockUnlock(nss_native.lock);if(!run)return;
 struct nss_request q={0};uint64_t ticket=0;
 int r=nord2_wifi_scan_enqueue(&q,&nsr_native,generation,njs_rejoin_scan_done,&ticket);
 IOLockLock(nss_native.lock);
 if(!r){nsr_native.scan_ticket=ticket;nsr_native.scan_state=2;}
 else if(r!=2)nsr_native.scan_state=3;
 IOLockUnlock(nss_native.lock);
}
