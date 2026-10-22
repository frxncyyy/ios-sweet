static unsigned callbacks,last_status,station_pubs,station_frames,station_commands,station_receipts[20],link_up_calls,link_down_calls,key_deletes,notice_order,defer_data,deferred_data;
static int fixture_rng(void*c,uint8_t*p,size_t n){(void)c;memset(p,0x41,n);return 0;}
static void completed(void*c,uint64_t gen,uint64_t ticket,enum nss_status status){assert(c==&callbacks&&gen==99);callbacks++;last_status=status;assert(!nord2_wifi_scan_release(ticket));}
int nord2_wifi_ifnet_link(uint64_t g,int a){assert(g&&a>=0&&a<=1);if(a){assert(nst_native.state.controlled_port&&nst_native.state.ptk.installed&&nst_native.state.gtk.installed);link_up_calls++;}else{assert(!nst_native.state.controlled_port);link_down_calls++;}return 0;}
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
 if(defer_data&&s->wire_type==3){deferred_data=1;return;}
 put32(d+4,get32(d+4)|0x80000000u);hardware_register(0x1802530c,next);
}
static void station_completion(unsigned next){
 struct nwt_state*t=&nwfn_native.init.transport;struct nst*s=&nst_native.state;
 uint8_t*d=pool_memory+(t->tx_ring-NDP_POOL_BASE)+16*t->tx_index,*p=pool_memory+(t->command-NDP_POOL_BASE);
 assert(station_hardware&&s->wire_busy);station_pubs++;
 if(s->wire_type){station_frames++;/* Synthetic genuine air result only for management; security uses local DDONE. */if(s->wire_type==1)station_packet(1,7,124,1,s->wire_pid,s->wire_wlan);}
 else{station_commands++;assert(p[37]==0xa0&&p[39]==t->next_sequence);
 if(p[36]==0x13&&p[84]==3&&p[115]){station_packet(1,7,44,1,0,0);struct ncr_queue*q=&s->queue[1];uint8_t*e=pool_memory+(q->ring.buffers-NDP_POOL_BASE)+4096*q->next;e[28]=12;e[29]=p[39];memcpy(e+36,s->peer.ap,6);e[42]=s->peer.sta;e[43]=s->peer.bss;}
 if(p[36]==7&&p[64]&&p[66]){station_packet(1,7,44,1,0,0);struct ncr_queue*q=&s->queue[1];uint8_t*e=pool_memory+(q->ring.buffers-NDP_POOL_BASE)+4096*q->next;e[28]=36;e[36]=s->peer.bss;memcpy(e+38,s->peer.ap,6);}
 if(p[36]==7&&!p[64])key_deletes++;
 }
 if(defer_data&&s->wire_type==3){deferred_data=1;return;}
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
 if(notice==NJS_JOINED){assert(!notice_order++&&njs_native.linked&&njs_native.joined&&nst_native.state.controlled_port);return;}
 assert(notice==(notice_order==1?NJS_LEFT:NJS_RETIRED));notice_order++;
 /* Actual njs_notice invokes clients outside NSS lock; both callbacks run
  * before cookie retirement and before scan-return publication. */
 assert(!pthread_mutex_trylock(&nss_native.lock->m));pthread_mutex_unlock(&nss_native.lock->m);
 assert(!nss_return.ready&&!njs_native.scan_returned&&njs_native.retiring&&!njs_native.cookie_retired);
 struct nss_request request={0};uint64_t untouched=123;assert(nord2_wifi_scan_enqueue(&request,&callbacks,99,completed,&untouched)==2&&untouched==123);
}
static struct niwj_target selected;
static void mark_authorized_service(void){
 /* Crypto phase/outcome seam only: all actual native STA/key/readiness/port
  * operations preceding this are checked real-source operations. Existing
  * actual-transport suite separately replays the BearSSL M1/M3 handshake. */
 njs_native.used=njs_native.active=1;njs_native.native_generation=njs_native.ticket=1;njs_native.epoch=7;njs_native.cookie=returned_notices;njs_native.completion=join_completed;
 struct stc*t=&njs_native.driver.owner.transaction;t->phase=STC_CONNECTED;memset(&t->action,0,sizeof t->action);t->outcome=STC_AUTHORIZED;t->authenticated=t->associated=t->ptk_attempted=t->gtk_attempted=t->peer_prepared=1;t->gtk_index=1;njs_native.driver.owner.closed=0;
 assert(nwo_authorized(&njs_native.driver.owner)&&nst_native.state.controlled_port);
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

 assert(!nst_peer_begin(&nst_native.state,10,NST_PREPARE_PEER,&njs_native.profile,0,0));pump_station();assert(station_receipts[NST_PEER_PREPARED]==1&&station_commands==4);
 uint8_t auth[30];size_t used=0;assert(!sw_auth(auth,sizeof auth,&used,&njs_native.profile.assoc.peer));assert(!nst_management(&nst_native.state,11,auth,used));pump_station();
 uint8_t security[18]={0};memcpy(security,nst_native.state.peer.ap,6);memcpy(security+6,nst_native.state.peer.self,6);security[12]=0x88;security[13]=0x8e;security[14]=2;
 assert(!nst_eapol(&nst_native.state,12,security,sizeof security,0));pump_station();
 /* Real native association configuration, key publication, KEY_READY and
  * replay installation: no installed-key/controlled-port test doubles. */
 uint8_t response[33]={0};w16(response,0x10);memcpy(response+4,nst_native.state.peer.self,6);memcpy(response+10,nst_native.state.peer.ap,6);memcpy(response+16,nst_native.state.peer.ap,6);w16(response+24,0x11);w16(response+28,1);response[30]=1;response[31]=1;response[32]=0x82;
 assert(!nst_peer_begin(&nst_native.state,14,NST_CONFIGURE_PEER,0,response,sizeof response));pump_station();assert(nst_native.state.sta_active&&station_receipts[NST_STA_ACTIVE]==1);
 uint8_t key[16]={0},body[64],rsc[8]={0};struct sw_key k={.bss=0,.wlan=5,.pairwise=1,.key_id=0,.key=key,.key_len=16};memcpy(k.ap,nst_native.state.peer.ap,6);
 assert(!sw_key_add(body,sizeof body,&used,&k)&&!nst_command(&nst_native.state,15,7,body,used));pump_station();assert(nst_native.state.ptk_ready&&station_receipts[NST_KEY_READY]==1&&!nst_replay_install(&nst_native.state,1,0,rsc));
 k.wlan=4;k.pairwise=0;k.key_id=1;assert(!sw_key_add(body,sizeof body,&used,&k)&&!nst_command(&nst_native.state,16,7,body,used));pump_station();assert(!nst_replay_install(&nst_native.state,0,1,rsc));assert(!nst_authorize(&nst_native.state));niwj_clear(key,sizeof key);niwj_clear(body,sizeof body);
 mark_authorized_service();assert(njs_owner_work()==1&&link_up_calls==1&&returned_notices[NJS_JOINED]==1&&njs_native.linked&&njs_native.joined);
 uint8_t data[60]={0};memcpy(data,nst_native.state.peer.ap,6);memcpy(data+6,nss_native.info.mac,6);data[12]=8;
 assert(!nord2_wifi_link_send(data,sizeof data,1)&&!nord2_wifi_link_send(data,sizeof data,1));assert(njs_native.tx_count==2);
 unsigned reads=mmio_reads,writes=mmio_writes;assert(nss_return_scans()==1&&reads==mmio_reads&&writes==mmio_writes);
 defer_data=1;assert(njs_owner_work()==1&&deferred_data&&nst_native.state.wire_busy&&nst_native.state.submitted>nst_native.state.dma_completed);
 reads=mmio_reads;writes=mmio_writes;assert(nss_return_scans()==1&&reads==mmio_reads&&writes==mmio_writes);
 defer_data=0;uint8_t*pending_data=pool_memory+(nst_native.state.normal.ring-NDP_POOL_BASE)+16*nst_native.state.normal.next;put32(pending_data+4,0xffffffffu);hardware_register(0x1802530c,(nst_native.state.normal.next+1)%1024);
 for(unsigned tries=0;tries<1000&&njs_native.tx_local!=2;tries++)assert(njs_owner_work()==1);
 assert(njs_native.tx_local==2&&njs_native.tx_accepted==2&&!njs_native.tx_count&&!nst_native.state.wire_busy);
 /* All five real RX queues move; a segmented chain persists across return. */
 for(unsigned q=0;q<5;q++){station_packet(q,0,24,q!=0,0,0);pump_station();}assert(nst_native.state.queue[0].segmented==1);
 /* Queue one extra ordinary frame, then real explicit disconnect must purge
  * it, close native port/BSD link, delete both installed keys, retire peer,
  * deliver LEFT then RETIRED, and only then seal the return origin. */
 assert(!nord2_wifi_link_send(data,sizeof data,1)&&njs_native.tx_count==1);assert(!nord2_wifi_join_disconnect());assert(!nst_native.state.controlled_port&&!njs_native.linked&&!njs_native.tx_count&&link_down_calls==1);
 for(unsigned tries=0;tries<1000&&!nss_return.ready;tries++)assert(njs_owner_work()==1);
 assert(nss_return.ready&&njs_native.joined&&notice_order==3&&returned_notices[NJS_LEFT]==1&&returned_notices[NJS_RETIRED]==1&&!returned_notices[NJS_JOIN_FAILED]);
 assert(station_frames==5&&station_pubs==station_commands+station_frames);
 assert(key_deletes==2&&!nst_native.state.ptk.installed&&!nst_native.state.gtk.installed&&!nst_native.state.sta_active&&nst_native.state.submitted==nst_native.state.dma_completed&&njs_native.queued_drop==1);
 assert(njs_native.driver.owner.transaction.phase==STC_DONE&&njs_native.cookie_retired&&!njs_native.cookie&&!njs_native.completion&&!njs_native.retiring);
 /* Reopen the fixed pure boundary for isolated mutation negatives only. No
  * native operation occurs until every tested closure condition is valid. */
 njs_native.scan_returned=0;nss_return.ready=nss_return.attempted=0;nst_native.active=1;
 reads=mmio_reads;writes=mmio_writes;worker_thread=3;assert(nss_return_scans()==1&&reads==mmio_reads&&writes==mmio_writes);worker_thread=2;
 /* Finite closure failures refuse before backend IO and keep first attempt. */
 unsigned*bad[]={&njs_native.queued,&njs_native.active,&njs_native.retiring,&njs_native.linked,&njs_native.tx_admit,&njs_native.tx_count,&njs_native.rx_count,&njs_native.driver.owner.halted,&njs_native.driver.owner.issuing,&njs_native.driver.owner.in_kind,&nst_native.state.wire_busy,&nst_native.state.air_pending,&nst_native.state.peer_kind,&nst_native.state.activation_pending,&nst_native.state.controlled_port,&nst_native.state.sta_active,&nst_native.state.ptk_ready,&nst_native.state.credit_reserved,&nst_native.state.callback_active,&nst_native.state.ptk.installed,&nst_native.state.gtk.installed};
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
 unsigned cursor[5],seg[5];for(unsigned q=0;q<5;q++){cursor[q]=nst_native.state.queue[q].next;seg[q]=nst_native.state.queue[q].segmented;}
 unsigned used_before=njs_native.used,attempted_before=nst_native.attempted;
 assert(!nss_return_scans()&&nss_return.ready&&!nst_native.active&&njs_native.joined&&njs_native.used==used_before&&nst_native.attempted==attempted_before&&njs_native.arbiter.connection_used==1&&mmio_writes==writes);
 assert(nss_return.origin.completed==nwfn_native.init.transport.completed&&nss_return.origin.tx_index==nwfn_native.init.transport.tx_index&&nss_return.origin.next_sequence==nwfn_native.init.transport.next_sequence);for(unsigned q=0;q<5;q++)assert(nss_return.queue[q].next==cursor[q]&&nss_return.queue[q].segmented==seg[q]);
 reads=mmio_reads;assert(!nss_return_scans()&&reads==mmio_reads&&writes==mmio_writes);assert(!njs_scan_blocked_locked());
 struct nct_session clone=nct_native.session;clone.continuation=2;clone.scan_return=&nss_return.origin;assert(!nss_scan_return_origin_valid(&clone));
 struct nct_scan_return copied_origin=nss_return.origin;clone.scan_return=&copied_origin;assert(!nss_scan_return_origin_valid(&clone));
 nst_native.state.active=1;nst_native.state.phase=NST_TX_READ;assert(!nst_native_owner_window(&nwfn_native));nst_native.state.active=0;nst_native.state.phase=NST_IDLE;
 struct niwj_request join={.ssid_length=4,.security=1};memcpy(join.ssid,"test",4);static const uint8_t rsn[22]={48,20,1,0,0,15,172,4,1,0,0,15,172,4,1,0,0,15,172,2,0,0};memcpy(join.station_rsn,rsn,22);uint64_t denied=123;assert(nord2_wifi_join_enqueue(&join,&callbacks,1,join_completed,&denied)==1&&denied==123);
 station_hardware=0;normal_pubs+=station_commands;/* Command-only fixture oracle excludes actual frame publications. */
 /* First return drain may rearm stale cells then cleanly refuse a pending
  * DDONE. Persist actual RX cursor/segmentation without publishing CID3. */
 unsigned before0=nss_return.queue[0].next;station_packet(0,0,24,1,0,0);
 struct ncr_queue*pending=&nss_return.queue[2];uint8_t*pd=pool_memory+(pending->ring.descriptors-NDP_POOL_BASE)+16*pending->next;put32(pd+4,0);hardware_register(pending->ring.base+12,(pending->next+1)%pending->ring.count);
 unsigned before_refusal=nwfn_native.init.transport.completed;uint8_t scan_sequence_before=nss_native.scan_sequence;
 assert(!nord2_wifi_scan_enqueue(&scan,&callbacks,99,completed,&ticket));drain();assert(last_status==NSS_REFUSED&&!nss_return.consumed&&nwfn_native.init.transport.completed==before_refusal&&nss_native.scan_sequence==scan_sequence_before&&!manual_halts);
 assert(nss_return.queue[0].next==(before0+1)%1024&&!nss_return.queue[0].segmented&&ncb_native.rx.queue[2].wait_ddone>=256);hardware_register(pending->ring.base+12,pending->next);
 /* First new scan drains a late old frame; the segmented q0 head cannot turn
  * into a fresh BSS. Both scans use actual NSS/native owner/callback guards. */
 for(unsigned repeat=0;repeat<2;repeat++){delivered=0;unsigned count_before=nwfn_native.init.transport.completed;assert(!nord2_wifi_scan_enqueue(&scan,&callbacks,99,completed,&ticket));drain();assert(last_status==NSS_COMPLETE&&nwfn_native.init.transport.completed==count_before+1&&!manual_halts&&njs_native.joined&&njs_native.used&&nst_native.attempted&&njs_native.arbiter.connection_used&&!nst_native.active);assert(ncb_native.rx.scan.count>=1);
  assert(nct_native.session.scan_return==&nss_return.origin&&nss_scan_return_origin_valid(&nct_native.session));
  reads=mmio_reads;writes=mmio_writes;
  unsigned sealed=nss_return.origin.sealed;nss_return.origin.sealed=0;assert(!nss_scan_return_origin_valid(&nct_native.session));nss_return.origin.sealed=sealed;
  nss_return.origin.generation++;assert(!nss_scan_return_origin_valid(&nct_native.session));nss_return.origin.generation--;
  njs_native.scan_returned=0;assert(!nss_scan_return_origin_valid(&nct_native.session));njs_native.scan_returned=1;
  nst_native.active=1;assert(!nss_scan_return_origin_valid(&nct_native.session));nst_native.active=0;
  assert(reads==mmio_reads&&writes==mmio_writes);
 }
 assert(nss_return.consumed&&nct_native.session.continuation==1&&nct_native.session.phase==NCT_DONE&&callbacks==4&&!njs_native.linked&&!njs_native.snapshot.data_ready&&njs_native.snapshot.phase==NJS_FAILED);
 assert(nord2_wifi_join_enqueue(&join,&callbacks,1,join_completed,&denied)==1&&denied==123);assert(!nord2_wifi_scan_info(&nss_native.info));
 assert(delivered&&nrd_calls&&nrd_calls==nrd_restored&&nrd_checked==nrd_calls);
 IOLock*notice=njs_native.notice_lock;assert(!pthread_mutex_destroy(&notice->m));free(notice);assert(!pthread_mutex_destroy(&nss_native.lock->m));free(nss_native.lock);
 puts("PASS post-authorized actual owner: native STA/PTK/GTK/replay/port, BSD linkup, two local data retirements, queued purge, real key deletes/peer cleanup, LEFT then RETIRED, closure guards, five RX cursors, two returned scans, history/rejoin preserved; crypto-phase seam, no hardware claim");return 0;}
