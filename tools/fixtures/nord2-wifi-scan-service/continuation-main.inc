
static unsigned callbacks,last_status,defer;static uint64_t last_ticket;
static void completed(void*c,uint64_t gen,uint64_t ticket,enum nss_status status){assert(c==&callbacks&&gen==99);callbacks++;last_status=status;last_ticket=ticket;if(defer)assert(!nord2_wifi_scan_release(ticket));}
static void drain(void){worker_thread=2;if(!setjmp(parked_exit))nss_owner_loop();}
int main(int argc,char**argv){assert(argc==2||argc==3);FILE*f=fopen(argv[1],"rb");assert(f&&fread(authenticated_board,1,sizeof authenticated_board,f)==sizeof authenticated_board);fclose(f);
 if(argc==3){ios_flag=0;setup(0);run_worker();assert(!nss_native.ready&&!service_ready_hooks&&ncd_native.parked&&normal_pubs==26);puts("PASS default off: original boot26/park, no service-ready hook or queue");return 0;}
 setup(0);run_worker();assert(nss_native.ready&&!ncd_native.parked&&service_ready_hooks==1&&nct_native.session.completed==26);unsigned base_pubs=normal_pubs;struct nss_info info;assert(!nord2_wifi_scan_info(&info)&&info.count==22);struct nss_request q={0};uint64_t ticket;struct nss_view view;
 /* Pending DDONE is not quiescence. A stale completed frame may already
  * have been rearmed: preserve that advanced cursor on clean refusal. */
 struct ncr_queue *pending=&ncb_native.rx.queue[2];unsigned before0=ncb_native.rx.queue[0].next;
 beacon(0,'Z',1);hardware_register(pending->ring.base+12,(pending->next+1)%pending->ring.count);
 unsigned pending_pubs=normal_pubs;assert(!nord2_wifi_scan_enqueue(&q,&callbacks,99,completed,&ticket));drain();
 assert(last_status==NSS_REFUSED&&normal_pubs==pending_pubs&&!manual_halts&&nct_native.session.phase==NCT_DONE);
 assert(ncb_native.rx.queue[0].next==(before0+1)%1024&&ncb_native.rx.queue[2].wait_ddone>=256);
 assert(!nord2_wifi_scan_release(ticket));hardware_register(pending->ring.base+12,pending->next);
 callbacks=0;
 unsigned old_data=ncb_native.rx.queue[0].next,old_event=ncb_native.rx.queue[1].next;
 assert(!nord2_wifi_scan_enqueue(&q,&callbacks,99,completed,&ticket));assert(nord2_wifi_scan_view(ticket,&view)==1);delivered=0;drain();assert(callbacks==1&&last_status==NSS_COMPLETE&&normal_pubs==base_pubs+1);assert(nwfn_native.init.transport.completed==32&&nwfn_native.init.transport.next_sequence==33);assert(ncb_native.rx.queue[0].next==(old_data+1)%1024&&ncb_native.rx.queue[1].next==(old_event+1)%16);assert(!nord2_wifi_scan_view(ticket,&view)&&view.scan->sequence==2&&view.scan->count==2);assert(!nord2_wifi_scan_release(ticket));
 for(unsigned n=0;n<3;n++){delivered=0;assert(!nord2_wifi_scan_enqueue(&q,&callbacks,99,completed,&ticket));drain();assert(last_status==NSS_COMPLETE&&!nord2_wifi_scan_view(ticket,&view)&&view.scan->sequence==3+n);assert(!nord2_wifi_scan_release(ticket));}

 /* Persist cross-descriptor segmented state: last fragment is not a fresh BSS. */
 ncb_native.rx.queue[0].segmented=1;delivered=0;assert(!nord2_wifi_scan_enqueue(&q,&callbacks,99,completed,&ticket));drain();assert(!nord2_wifi_scan_view(ticket,&view)&&view.scan->count==1&&!ncb_native.rx.queue[0].segmented);assert(!nord2_wifi_scan_release(ticket));
 /* Exhaust all 4096 stale-frame budget without publishing a scan. Preserve
  * every already-rearmed cursor; a following request drains the remainder. */
 unsigned queued=0;for(unsigned k=0;k<5;k++){struct ncr_queue*z=&ncb_native.rx.queue[k];for(unsigned j=0;j<z->ring.count-1;j++){unsigned at=(z->next+j)%z->ring.count;uint8_t*dd=pool_memory+(z->ring.descriptors-NDP_POOL_BASE)+16*at;put32(dd+4,0xc0000000u);}hardware_register(z->ring.base+12,(z->next+z->ring.count-1)%z->ring.count);queued+=z->ring.count-1;}assert(queued>0);refill_drain=1;
 unsigned budget_pubs=normal_pubs;assert(!nord2_wifi_scan_enqueue(&q,&callbacks,99,completed,&ticket));drain();assert(last_status==NSS_REFUSED&&normal_pubs==budget_pubs&&!manual_halts&&ncb_native.rx.rearmed==4096);refill_drain=0;assert(!nord2_wifi_scan_release(ticket));
 delivered=0;assert(!nord2_wifi_scan_enqueue(&q,&callbacks,99,completed,&ticket));drain();assert(last_status==NSS_COMPLETE&&normal_pubs==budget_pubs+1);assert(!nord2_wifi_scan_release(ticket));
 /* Packet and scan sequence wrap; all five actual ring cursors continue. */
 while(nwfn_native.init.transport.next_sequence!=0){delivered=0;assert(!nord2_wifi_scan_enqueue(&q,&callbacks,99,completed,&ticket));drain();assert(last_status==NSS_COMPLETE);assert(!nord2_wifi_scan_release(ticket));}
 struct nct_session*closed=&nct_native.session;struct nwt_state*tx=&nwfn_native.init.transport;
 tx->tx_index=1023;closed->first_tx=1022;closed->record[0].from=1022;closed->record[0].to=1023;hardware_register(0x1802541c,1023);hardware_register(0x18025418,1023);
 delivered=0;assert(!nord2_wifi_scan_enqueue(&q,&callbacks,99,completed,&ticket));drain();assert(tx->tx_index==0&&tx->next_sequence==1&&nct_native.session.record[0].sequence==0);assert(!nord2_wifi_scan_release(ticket));
 /* Inflight cancel never aborts or replays the already published radio command. */
 delivered=0;assert(!nord2_wifi_scan_enqueue(&q,&callbacks,99,completed,&ticket));cancel_inflight=ticket;defer=1;unsigned completed_before=tx->completed;drain();assert(last_status==NSS_CANCELED&&tx->completed==completed_before+1&&!manual_halts&&nord2_wifi_scan_view(ticket,&view)==1);assert(nord2_wifi_scan_release(ticket)==1);defer=0;
 /* Exhaustion refuses before IO and restores the exact closed session. */
 unsigned saved_base=closed->base_completed,saved_completed=tx->completed;closed->base_completed=UINT32_MAX-1;tx->completed=UINT32_MAX;unsigned exhausted_reads=mmio_reads,exhausted_writes=mmio_writes;
 assert(!nord2_wifi_scan_enqueue(&q,&callbacks,99,completed,&ticket));drain();assert(last_status==NSS_REFUSED&&mmio_reads==exhausted_reads&&mmio_writes==exhausted_writes&&tx->completed==UINT32_MAX);assert(!nord2_wifi_scan_release(ticket));closed->base_completed=saved_base;tx->completed=saved_completed;

 unsigned saved_cost=nbc_native.state.plan.pse_cost;nbc_native.state.plan.pse_cost=0;unsigned refuse_reads=mmio_reads,refuse_writes=mmio_writes;
 assert(!nord2_wifi_scan_enqueue(&q,&callbacks,99,completed,&ticket));drain();assert(last_status==NSS_REFUSED&&mmio_reads==refuse_reads&&mmio_writes==refuse_writes&&nct_native.session.phase==NCT_DONE);assert(!nord2_wifi_scan_release(ticket));nbc_native.state.plan.pse_cost=saved_cost;
 delivered=0;assert(!nord2_wifi_scan_enqueue(&q,&callbacks,99,completed,&ticket));drain();assert(last_status==NSS_COMPLETE);assert(!nord2_wifi_scan_release(ticket));
 unsigned reads=mmio_reads,writes=mmio_writes;assert(!nord2_wifi_scan_enqueue(&q,&callbacks,99,completed,&ticket));assert(!nord2_wifi_scan_cancel(ticket));drain();assert(last_status==NSS_CANCELED&&reads==mmio_reads&&writes==mmio_writes&&nord2_wifi_scan_view(ticket,&view)==1);assert(!nord2_wifi_scan_release(ticket));
 struct nss_request bad={.count=1,.channels={{1,255}}};uint64_t saved=123;assert(nord2_wifi_scan_enqueue(&bad,&callbacks,99,completed,&saved)==-1&&saved==123);
 uint64_t tickets[4];for(unsigned i=0;i<4;i++)assert(!nord2_wifi_scan_enqueue(&q,&callbacks,99,completed,&tickets[i]));assert(nord2_wifi_scan_enqueue(&q,&callbacks,99,completed,&saved)==2&&saved==123);for(unsigned i=0;i<4;i++)assert(!nord2_wifi_scan_cancel(tickets[i]));drain();for(unsigned i=0;i<4;i++)assert(!nord2_wifi_scan_release(tickets[i]));
 mode=1;delivered=0;defer=1;assert(!nord2_wifi_scan_enqueue(&q,&callbacks,99,completed,&ticket));drain();assert(last_status==NSS_PARTIAL&&nord2_wifi_scan_view(ticket,&view)==1&&!manual_halts);defer=0;
 /* A real next command wrong DIDX keeps shared halt/drain, never retries. */
 mode=0;delivered=0;normal_wrong_didx=1;defer=1;assert(!nord2_wifi_scan_enqueue(&q,&callbacks,99,completed,&ticket));drain();assert(last_status==NSS_FATAL&&manual_halts==1&&nss_native.failed&&nwfn_native.halt&&nwfn_native.init.transport.halt&&nwm_drain_entered&&drain_blocks==1);normal_wrong_didx=0;
 assert(nrd_calls&&nrd_calls==nrd_restored&&nrd_checked==nrd_calls&&last_ticket);
 assert(!pthread_mutex_destroy(&nss_native.lock->m));free(nss_native.lock);
 puts("PASS actual owner repeated CID3: boot26 once then250+ scans/packet+TX wrap/segmentedcontinuity/inflightcancel/refusalrestore/fataldrain, sequence/credit/5ring continuity, sourceCID3 realdone/BSS, queuedcancel zeroIO, invalidchannels/fullqueue, deferredrelease, honest timeout; no hardware claim");return 0;}
