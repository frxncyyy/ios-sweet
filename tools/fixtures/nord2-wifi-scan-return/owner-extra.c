static unsigned callbacks,last_status,station_pubs,station_frames,station_commands,station_receipts[20];
static int fixture_rng(void*c,uint8_t*p,size_t n){(void)c;memset(p,0x41,n);return 0;}
static void completed(void*c,uint64_t gen,uint64_t ticket,enum nss_status status){assert(c==&callbacks&&gen==99);callbacks++;last_status=status;assert(!nord2_wifi_scan_release(ticket));}
int nord2_wifi_ifnet_link(uint64_t g,int a){assert(g&&!a);return 0;}
int nord2_wifi_ifnet_receive(uint64_t g,const uint8_t*p,size_t n){assert(g&&p&&n>=14);return 0;}
static void drain(void){worker_thread=2;if(!setjmp(parked_exit))nss_owner_loop();}
static void station_packet(unsigned k,unsigned type,unsigned bytes,unsigned last,unsigned pid,unsigned wlan){
 struct ncr_queue*q=&nst_native.state.queue[k];unsigned at=q->next;
 uint8_t*d=pool_memory+(q->ring.descriptors-NDP_POOL_BASE)+16*at,*p=pool_memory+(q->ring.buffers-NDP_POOL_BASE)+4096*at;memset(p,0,bytes);w16(p,bytes);w16(p+2,type==7?0x3800:type<<11);
 if(type==7){p[28]=0x0f;p[36]=pid;p[40]=wlan;}
 put32(d,(uint32_t)(q->ring.buffers+4096*at));put32(d+4,0x80000000u|(last?0x40000000u:0)|(bytes<<16));put32(d+8,0);put32(d+12,0);hardware_register(q->ring.base+12,(at+1)%q->ring.count);
}
static void station_data_completion(unsigned next){
 struct nst*s=&nst_native.state;uint8_t*d=pool_memory+(s->normal.ring-NDP_POOL_BASE)+16*s->normal.next;
 uint8_t*page=pool_memory+(s->normal.buffer-NDP_POOL_BASE);
 assert(station_hardware&&s->wire_busy&&(s->wire_type==2||s->wire_type==3)&&get32(d)==s->normal.buffer&&((get32(d+4)>>16)&0x3fff)==64);
 assert(get32(page+40)==s->normal.buffer+64&&((get32(page)>>23)&3)==0);station_pubs++;station_frames++;
 station_packet(3,6,12,1,0,0);struct ncr_queue*q=&s->queue[3];uint8_t*r=pool_memory+(q->ring.buffers-NDP_POOL_BASE)+4096*q->next;w16(r+2,(6u<<11)|1);put32(r+4,2u<<16);put32(r+8,s->normal.token);
 put32(d+4,get32(d+4)|0x80000000u);hardware_register(0x1802530c,next);
}
static void station_completion(unsigned next){
 struct nwt_state*t=&nwfn_native.init.transport;struct nst*s=&nst_native.state;
 uint8_t*d=pool_memory+(t->tx_ring-NDP_POOL_BASE)+16*t->tx_index,*p=pool_memory+(t->command-NDP_POOL_BASE);
 assert(station_hardware&&s->wire_busy);station_pubs++;
 if(s->wire_type){station_frames++;/* Synthetic genuine air result only for management; security uses local DDONE. */if(s->wire_type==1)station_packet(1,7,124,1,s->wire_pid,s->wire_wlan);}
 else{station_commands++;assert(p[37]==0xa0&&p[39]==t->next_sequence);}
 put32(d,0xffffffffu);put32(d+4,0xffffffffu);put32(d+8,0xdeadbeefu);put32(d+12,0x12345678u);hardware_register(0x1802541c,next);
}
static int station_event(void*c,const struct nst_event*e){assert(c==&station_pubs&&e->generation==1&&e->kind<20);station_receipts[e->kind]++;return 0;}
static void pump_station(void){for(unsigned tries=0;tries<1000;tries++){
 assert(!nst_step(&nst_native.state,&ncd_io,&ncd_native,16,station_event,&station_pubs));
 if(!nst_native.state.wire_busy&&!nst_native.state.air_pending&&!nst_native.state.peer_kind)return;
 }assert(!"station model did not retire");}
static unsigned returned_notices[5];
static void join_completed(void*c,uint64_t epoch,uint64_t ticket,enum njs_notice notice){
 assert(c==returned_notices&&epoch==7&&ticket==1&&notice<5);returned_notices[notice]++;
 /* Actual njs_notice invokes clients outside NSS lock; both callbacks run
  * before cookie retirement and before scan-return publication. */
 assert(!pthread_mutex_trylock(&nss_native.lock->m));pthread_mutex_unlock(&nss_native.lock->m);
 assert(!nss_return.ready&&!njs_native.scan_returned&&njs_native.retiring&&!njs_native.cookie_retired);
 struct nss_request request={0};uint64_t untouched=123;assert(nord2_wifi_scan_enqueue(&request,&callbacks,99,completed,&untouched)==2&&untouched==123);
}
static struct niwj_target selected;
static void seal_failed_service(void){
 njs_native.used=njs_native.terminal=njs_native.cookie_retired=1;njs_native.native_generation=njs_native.ticket=1;njs_native.snapshot.phase=NJS_FAILED;njs_native.snapshot.result=2;
 njs_native.arbiter.mode=0;njs_native.arbiter.generation=0;njs_native.arbiter.connection_used=1;assert(njs_native.driver.transport==&nst_native.state);njs_native.driver.owner.closed=1;
 struct stc*t=&njs_native.driver.owner.transaction;t->attempted=1;t->generation=1;t->phase=STC_DONE;memset(&t->action,0,sizeof t->action);t->outcome=STC_FAILED;t->arbiter=&njs_native.arbiter;
 /* Service/transaction terminal metadata is a documented seam. All preceding
  * native publications and retirement, worker and memory guards are real. */
}
int main(int argc,char**argv){assert(argc==2);FILE*f=fopen(argv[1],"rb");assert(f&&fread(authenticated_board,1,sizeof authenticated_board,f)==sizeof authenticated_board);fclose(f);
 setup(0);run_worker();assert(nss_native.ready&&!ncd_native.parked&&service_ready_hooks==1&&nct_native.session.completed==26);
 struct nss_request scan={0};uint64_t ticket=0;delivered=0;assert(!nord2_wifi_scan_enqueue(&scan,&callbacks,99,completed,&ticket));drain();assert(callbacks==1&&last_status==NSS_COMPLETE);
 memset(&selected,0,sizeof selected);selected.record=ncb_native.rx.scan.records[0];selected.band=1;selected.channel=selected.record.channel;selected.rate_count=1;selected.rates[0]=0x82;selected.record.interval=100;selected.record.rxv_valid=1;selected.record.group_valid=4;selected.record.rxv[4]=100;selected.record.rxv[5]=255;
 uint64_t now;assert(!ncd_native_now(&ncd_native,&now));assert(!nst_native_prepare(&selected,1,now,now+UINT64_C(60000000),&njs_native.profile));station_hardware=1;assert(!nss_scan_return_origin_valid(&nct_native.session));
 /* Real constructor installs the real native close/submit callbacks. The
  * eventual terminal phase metadata remains the documented fixture seam. */
 static const uint8_t station_rsn[22]={48,20,1,0,0,15,172,4,1,0,0,15,172,4,1,0,0,15,172,2,0,0};uint8_t pmk[32];memset(pmk,0x42,sizeof pmk);
 struct stc_config cfg={.generation=1,.deadline_ms=now/1000+8000,.cleanup_timeout_ms=30000,.assoc=njs_native.profile.assoc,.channel={.bss=0,.token=1,.band=selected.band,.channel=selected.channel,.dbdc_band=4,.interval_ms=4000},.sta_wlan=5,.group_wlan=4,.pmk=pmk,.station_rsn=station_rsn,.ap_rsn=station_rsn,.ap_rsn_bytes=22,.rng=fixture_rng};
 assert(nwn_begin(&njs_native.driver,&nst_native.state,&njs_native.arbiter,&cfg,njs_peer,&njs_native,now/1000)==1);niwj_clear(pmk,sizeof pmk);
 unsigned base_completed=nwfn_native.init.transport.completed,base_tx=nwfn_native.init.transport.tx_index;uint8_t base_sequence=nwfn_native.init.transport.next_sequence;
 assert(!nst_peer_begin(&nst_native.state,10,NST_PREPARE_PEER,&njs_native.profile,0,0));pump_station();assert(station_receipts[NST_PEER_PREPARED]==1&&station_commands==4);
 uint8_t auth[30];size_t used=0;assert(!sw_auth(auth,sizeof auth,&used,&njs_native.profile.assoc.peer));assert(!nst_management(&nst_native.state,11,auth,used));pump_station();
 uint8_t security[18]={0};memcpy(security,nst_native.state.peer.ap,6);memcpy(security+6,nst_native.state.peer.self,6);security[12]=0x88;security[13]=0x8e;security[14]=2;
 assert(!nst_eapol(&nst_native.state,12,security,sizeof security,0));pump_station();
 /* All five real RX queues move. A segmented chain persists across handoff. */
 for(unsigned k=0;k<5;k++){station_packet(k,0,24,k!=0,0,0);pump_station();}
 assert(nst_native.state.queue[0].segmented==1);
 assert(!nst_cleanup_begin(&nst_native.state,nst_native.state.last,nst_native.state.last+UINT64_C(30000000)));
 assert(!nst_peer_begin(&nst_native.state,13,NST_RETIRE_PEER,0,0,0));pump_station();assert(station_receipts[NST_PEER_RETIRED]==1);
 assert(station_pubs==9&&station_frames==2&&station_commands==7&&nst_native.state.submitted==nst_native.state.dma_completed);
 assert(nwfn_native.init.transport.completed==base_completed+8&&nwfn_native.init.transport.tx_index==(base_tx+8)%1024&&nwfn_native.init.transport.next_sequence==(uint8_t)(base_sequence+7));
 seal_failed_service();unsigned reads=mmio_reads,writes=mmio_writes;
 worker_thread=3;assert(nss_return_scans()==1&&reads==mmio_reads&&writes==mmio_writes);worker_thread=2;
 /* Finite closure failures refuse before backend IO and keep first attempt. */
 unsigned*bad[]={&njs_native.queued,&njs_native.active,&njs_native.retiring,&njs_native.linked,&njs_native.tx_admit,&njs_native.tx_count,&njs_native.rx_count,&njs_native.driver.owner.halted,&njs_native.driver.owner.issuing,&njs_native.driver.owner.in_kind,&nst_native.state.wire_busy,&nst_native.state.air_pending,&nst_native.state.peer_kind,&nst_native.state.activation_pending,&nst_native.state.controlled_port,&nst_native.state.sta_active,&nst_native.state.ptk_ready,&nst_native.state.credit_reserved,&nst_native.state.callback_active};
 for(unsigned k=0;k<sizeof bad/sizeof bad[0];k++){unsigned prior=*bad[k];*bad[k]=1;assert(nss_return_scans()==1&&reads==mmio_reads&&writes==mmio_writes&&!nss_return.ready);*bad[k]=prior;}
 njs_native.driver.owner.in_ticket=99;assert(nss_return_scans()==1&&reads==mmio_reads&&writes==mmio_writes);njs_native.driver.owner.in_ticket=0;
 nst_native.state.channel_held=1;assert(nss_return_scans()==1&&reads==mmio_reads&&writes==mmio_writes);nst_native.state.channel_held=0;
 nst_native.state.dma_completed--;assert(nss_return_scans()==1&&reads==mmio_reads&&writes==mmio_writes);nst_native.state.dma_completed++;
 nst_native.state.credit_available--;assert(nss_return_scans()==1&&reads==mmio_reads&&writes==mmio_writes);nst_native.state.credit_available++;

 nst_native.state.normal.published=1;assert(nss_return_scans()==1&&reads==mmio_reads&&writes==mmio_writes);nst_native.state.normal.published=0;
 hardware_register(0x1802530c,(nst_native.state.normal.next+1)%1024);assert(nss_return_scans()==-1&&!nss_return.ready&&mmio_writes==writes);nss_return.attempted=0;hardware_register(0x1802530c,nst_native.state.normal.next);
 uint8_t*nd=pool_memory+(nst_native.state.normal.ring-NDP_POOL_BASE)+16*nst_native.state.normal.next;uint32_t nc=get32(nd+4);put32(nd+4,nc&0x7fffffff);assert(nss_return_scans()==-1&&!nss_return.ready&&mmio_writes==writes);nss_return.attempted=0;put32(nd+4,nc);
 /* Physical preflight failures restore exact phase/active; no writes or
  * return publication. Production caller takes fatal retained-owner path. */
 hardware_register(0x1802541c,(nst_native.state.tx_next+1)%1024);assert(nss_return_scans()==-1&&!nss_return.ready&&nst_native.state.phase==NST_IDLE&&!nst_native.state.active&&mmio_writes==writes);nss_return.attempted=0;hardware_register(0x1802541c,nst_native.state.tx_next);
 uint8_t*next=pool_memory+(nwfn_native.init.transport.tx_ring-NDP_POOL_BASE)+16*nst_native.state.tx_next;uint32_t ctl=get32(next+4);put32(next+4,ctl&0x7fffffff);assert(nss_return_scans()==-1&&!nss_return.ready&&nst_native.state.phase==NST_IDLE&&!nst_native.state.active&&mmio_writes==writes);nss_return.attempted=0;put32(next+4,ctl);
 unsigned cursor[5],seg[5];for(unsigned k=0;k<5;k++){cursor[k]=nst_native.state.queue[k].next;seg[k]=nst_native.state.queue[k].segmented;}
 unsigned used_before=njs_native.used,attempted_before=nst_native.attempted;
 njs_native.active=1;njs_native.terminal=njs_native.cookie_retired=0;njs_native.cookie=returned_notices;njs_native.completion=join_completed;njs_native.epoch=7;njs_native.driver.cleanup_started=1;
 assert(njs_owner_work()==1&&returned_notices[NJS_JOIN_FAILED]==1&&returned_notices[NJS_RETIRED]==1&&njs_native.cookie_retired&&!njs_native.cookie&&!njs_native.completion&&!njs_native.retiring);
 assert(nss_return.ready&&!nss_return.consumed&&njs_native.scan_returned&&!nst_native.active&&njs_native.used==used_before&&nst_native.attempted==attempted_before&&njs_native.arbiter.connection_used==1&&mmio_writes==writes);
 assert(nss_return.origin.completed==base_completed+8&&nss_return.origin.tx_index==(base_tx+8)%1024&&nss_return.origin.next_sequence==(uint8_t)(base_sequence+7));for(unsigned k=0;k<5;k++)assert(nss_return.queue[k].next==cursor[k]&&nss_return.queue[k].segmented==seg[k]);
 reads=mmio_reads;assert(!nss_return_scans()&&reads==mmio_reads&&writes==mmio_writes);assert(!njs_scan_blocked_locked());
 struct nct_session clone=nct_native.session;clone.continuation=2;clone.scan_return=&nss_return.origin;assert(!nss_scan_return_origin_valid(&clone));
 struct nct_scan_return copied_origin=nss_return.origin;clone.scan_return=&copied_origin;assert(!nss_scan_return_origin_valid(&clone));
 nst_native.state.active=1;nst_native.state.phase=NST_TX_READ;assert(!nst_native_owner_window(&nwfn_native));nst_native.state.active=0;nst_native.state.phase=NST_IDLE;
 struct niwj_request join={.ssid_length=4,.security=1};memcpy(join.ssid,"test",4);static const uint8_t rsn[22]={48,20,1,0,0,15,172,4,1,0,0,15,172,4,1,0,0,15,172,2,0,0};memcpy(join.station_rsn,rsn,22);uint64_t denied=123;assert(nord2_wifi_join_enqueue(&join,&callbacks,1,join_completed,&denied)==1&&denied==123);
 station_hardware=0;normal_pubs+=station_commands;/* Source fixture's command-only sequence oracle follows7 commands, not9 frames. */
 /* First return drain may rearm stale cells then cleanly refuse a pending
  * DDONE. Persist actual RX cursor/segmentation without publishing CID3. */
 unsigned before0=nss_return.queue[0].next;station_packet(0,0,24,1,0,0);
 struct ncr_queue*pending=&nss_return.queue[2];uint8_t*pd=pool_memory+(pending->ring.descriptors-NDP_POOL_BASE)+16*pending->next;put32(pd+4,0);hardware_register(pending->ring.base+12,(pending->next+1)%pending->ring.count);
 unsigned before_refusal=nwfn_native.init.transport.completed;uint8_t scan_sequence_before=nss_native.scan_sequence;
 assert(!nord2_wifi_scan_enqueue(&scan,&callbacks,99,completed,&ticket));drain();assert(last_status==NSS_REFUSED&&!nss_return.consumed&&nwfn_native.init.transport.completed==before_refusal&&nss_native.scan_sequence==scan_sequence_before&&!manual_halts);
 assert(nss_return.queue[0].next==(before0+1)%1024&&!nss_return.queue[0].segmented&&ncb_native.rx.queue[2].wait_ddone>=256);hardware_register(pending->ring.base+12,pending->next);
 /* First new scan drains a late old frame; the segmented q0 head cannot turn
  * into a fresh BSS. Both scans use actual NSS/native owner/callback guards. */
 for(unsigned repeat=0;repeat<2;repeat++){delivered=0;unsigned count_before=nwfn_native.init.transport.completed;assert(!nord2_wifi_scan_enqueue(&scan,&callbacks,99,completed,&ticket));drain();assert(last_status==NSS_COMPLETE&&nwfn_native.init.transport.completed==count_before+1&&!manual_halts&&njs_native.used&&nst_native.attempted&&njs_native.arbiter.connection_used&&!nst_native.active);assert(ncb_native.rx.scan.count>=1);
  assert(nct_native.session.scan_return==&nss_return.origin&&nss_scan_return_origin_valid(&nct_native.session));
  reads=mmio_reads;writes=mmio_writes;
  unsigned sealed=nss_return.origin.sealed;nss_return.origin.sealed=0;assert(!nss_scan_return_origin_valid(&nct_native.session));nss_return.origin.sealed=sealed;
  nss_return.origin.generation++;assert(!nss_scan_return_origin_valid(&nct_native.session));nss_return.origin.generation--;
  njs_native.scan_returned=0;assert(!nss_scan_return_origin_valid(&nct_native.session));njs_native.scan_returned=1;
  nst_native.active=1;assert(!nss_scan_return_origin_valid(&nct_native.session));nst_native.active=0;
  assert(reads==mmio_reads&&writes==mmio_writes);
 }
 assert(nss_return.consumed&&nct_native.session.continuation==1&&nct_native.session.phase==NCT_DONE&&callbacks==4&&!njs_native.linked&&!njs_native.snapshot.data_ready&&njs_native.snapshot.phase==NJS_FAILED);
 assert(njs_rejoin_ready_locked()&&njs_native.used&&nst_native.attempted);assert(!nord2_wifi_scan_info(&nss_native.info));
 assert(delivered&&nrd_calls&&nrd_calls==nrd_restored&&nrd_checked==nrd_calls);
 IOLock*notice=njs_native.notice_lock;assert(!pthread_mutex_destroy(&notice->m));free(notice);assert(!pthread_mutex_destroy(&nss_native.lock->m));free(nss_native.lock);
 puts("PASS actual native scan return: boot26/scan then9 mixed station publications7commands/5queue rearm+segmentation/real peer cleanup/closure guards/read-only physical preflight/typed sealed origin/two real scans/fresh-scan rejoin readiness; terminalmetadata seam with actualNJSretire/callback ordering, no RF claim");return 0;}
