/* Include after wifi_capability_bind.inc. All live operations run in the
 * actual retained CAP/BASIC worker; no service or interface is published. */
#include "wifi_normal_backend.h"
static struct {
 unsigned attempted,enabled,prepared,refusal;
 uint8_t board[6146];
 struct ncb_plan plan;struct ncr_state rx;
}ncb_native;
static int ncb_rx_active(const struct nwfn_native*n)
{return n==&nwfn_native&&ncb_native.prepared&&nct_native.active;}
static int ncb_rx_memory(const struct nwfn_native*n,uint64_t a,uint32_t bytes,unsigned write)
{
 return ncb_rx_active(n)&&nct_native_owner_window(n)&&
  ncb_native.rx.session==&nct_native.session&&ncb_native.rx.init==&nwfn_native.init&&
  ncr_memory_range(&ncb_native.rx,a,bytes,write);
}
static int ncb_rx_write(const struct nwfn_native*n,uint32_t a,uint32_t v)
{
 return ncb_rx_active(n)&&nct_native_owner_window(n)&&
  ncb_native.rx.session==&nct_native.session&&ncb_native.rx.init==&nwfn_native.init&&
  ncr_register_write(&ncb_native.rx,a,v);
}
static int ncb_sleep(void*context,unsigned us)
{
 if(context!=&ncb_native||us!=1000||!ncb_native.prepared||
    ncd_native.worker!=current_thread()||!nct_native.active)return -1;
 uint64_t deadline;clock_interval_to_deadline(us,NSEC_PER_USEC,&deadline);
 assert_wait_deadline((event_t)&ncb_native,THREAD_UNINT,deadline);
 thread_block(THREAD_CONTINUE_NULL);return 0;
}
static void ncb_report(int result)
{
 const struct ncr_state*r=&ncb_native.rx;const struct nsb_scan*s=&r->scan;
 printf("NORD2 WIFI BACKEND: status=%d attempted=%u prepared=%u refusal=%u commands=%u channels=%u nvram=%u interface=0 association=0\n",
  result,ncb_native.attempted,ncb_native.prepared,ncb_native.refusal,ncb_native.plan.prepared?NCB_COMMANDS:0,
  ncb_native.plan.channel_count,ncb_native.plan.config.nvram.fragments);
 printf("NORD2 WIFI RX: entered=%u halt=%u error=%u packets=%u rearmed=%u dropped=%u malformed=%u events=%u debug=%u sleepy=%u unknown=%u msdu=%u reports=%u phase=%u queue=%u\n",
  r->entered,r->halt,r->error,r->packets,r->rearmed,r->dropped,r->malformed,r->events,r->debug,r->sleepy,r->unknown,r->msdu_reports,r->rx_reports,r->phase,r->current);
 printf("NORD2 WIFI SCAN: requested=%u sequence=%u done=%u timed_out=%u partial=%u bss=%u frames=%u dropped=%u state=%u completed_channels=%u pending=%u unrelated=%u drain=%u\n",
  r->scan_requested,s->sequence,s->done,s->timed_out,s->partial,s->count,s->frames,s->dropped,s->completion.state,
  s->completion.completed_channels,r->done_pending,r->done_unrelated,r->drain_passes);
 for(unsigned k=0;k<5;k++){
  const struct ncr_queue*q=&r->queue[k];
  printf("NORD2 WIFI RXQUEUE: n=%u next=%u didx=%u packets=%u rearmed=%u waits=%u segmented=%u dropped=%u raw=%#x/%#x/%#x/%#x\n",
   k,q->next,q->didx,q->packets,q->rearmed,q->wait_ddone,q->segmented,q->dropped,q->raw[0],q->raw[1],q->raw[2],q->raw[3]);
 }
 for(unsigned n=0;n<2;n++){
  const struct nrd_snapshot*d=&r->readback[n];
  printf("NORD2 WIFI RXDMA: n=%u attempted=%u complete=%u error=%u reason=%u valid=%#x cfg=%#x/%#x started=%llu last=%llu\n",
   n,d->attempted,d->complete,d->error,d->reason,d->valid,d->cfg[0],d->cfg[1],(unsigned long long)d->started,(unsigned long long)d->last);
  for(unsigned k=0;k<NRD_QUEUES;k++){
   const struct nrd_queue*q=&d->queue[k];
   printf("NORD2 WIFI RXDMAQUEUE: n=%u slot=%u q=%u next=%u valid=%#x base=%#x expected=%#llx control=%#x/%#x/%#x/%#x ext_addr=%#x ext=%#x descriptor=%#llx raw=%#x/%#x/%#x/%#x\n",
    n,k,q->queue,q->next,q->valid,q->base,(unsigned long long)q->expected_descriptors,q->control[0],q->control[1],q->control[2],q->control[3],q->ext_address,q->ext,(unsigned long long)q->descriptor,q->raw[0],q->raw[1],q->raw[2],q->raw[3]);
  }
 }
 for(unsigned k=0;k<r->history_count;k++){
  const struct ncr_event*h=&r->history[k];
  printf("NORD2 WIFI RXEVENT: n=%u bytes=%u outer=%#x inner=%#x eid=%#x seq=%u kind=%u status=%u\n",
   k,h->bytes,h->outer,h->inner,h->eid,h->sequence,h->kind,h->status);
 }
 if(s->done){
  const struct nsb_done*d=&s->completion;
  printf("NORD2 WIFI SCANRF: version=%u pno=%u beacons=%u split=%u/%u sparse=%u/%u/%u channels=%u\n",
   d->version,d->pno,d->beacons,d->beacons_2g,d->beacons_5g,d->sparse_valid,d->sparse_band,d->sparse_channel,d->channel_count);
  for(unsigned k=0;k<d->channel_count&&k<64;k++)
   printf("NORD2 WIFI SCANRFCHANNEL: n=%u channel=%u beacon_probe=%u mdrdy=%u idle_raw=%u scan_time_raw=%u\n",
    k,d->channels[k],d->beacon_probe_counts[k],d->mdrdy_counts[k],d->idle[k],d->scan_time[k]);
 }
 /* SSIDs/BSSIDs, board MAC and calibration bytes remain in owned memory. */
}
static int ncb_after_basic(void)
{
 unsigned enabled=0;DTEntry chosen;const void*board=0;unsigned bytes=0;
 PE_parse_boot_argn("nord2wifiscan",&enabled,sizeof(enabled));
 if(enabled!=1)return 0;
 if(!__sync_bool_compare_and_swap(&ncb_native.attempted,0,1))return -1;
 ncb_native.enabled=1;
 if(ncd_native.worker!=current_thread()||!ncd_native.active||ncd_native.halt||ncd_native.parked||
    !nbc_native.state.completed||nbc_native.state.halt||nwfn_native.init.transport.completed!=5){ncb_native.refusal=1;ncb_report(1);return 0;}
 if(SecureDTLookupEntry(NULL,"/chosen",&chosen)!=kSuccess||
    SecureDTGetProperty(chosen,"nord2-wifi-calibration",&board,&bytes)!=kSuccess||!board||bytes!=sizeof(ncb_native.board)){
  ncb_native.refusal=2;ncb_report(1);return 0;
 }
 memcpy(ncb_native.board,board,bytes);
 if(ncb_prepare(&ncb_native.plan,ncb_native.board,bytes,ncd_native.diag.frame,ncd_native.diag.frame_bytes,
    ncd_native.diag.history[ncd_native.diag.events-1].sequence,nwfn_native.init.transport.next_sequence)||
    ncr_prepare(&ncb_native.rx,&nwfn_native.init,&nct_native.session,NCB_COMMANDS-1,1,UINT64_C(14000000))){
  ncb_native.refusal=3;ncb_report(1);return 0;
 }
 ncb_native.prepared=1;
 struct nct_recipe recipe={ncb_native.plan.entry,NCB_COMMANDS,ncb_build,&ncb_native.plan,UINT64_C(60000000),UINT64_C(15000000)};
 struct nct_hooks hooks={ncr_pump,&ncb_native.rx,ncb_sleep,&ncb_native};
 printf("NORD2 WIFI BACKEND ARM: commands=%u channels=%u passive=1 nvram=%u\n",NCB_COMMANDS,ncb_native.plan.channel_count,ncb_native.plan.config.nvram.fragments);
 int result=nct_native_run(&recipe,&hooks);
 if(!ncb_native.rx.scan.done&&ncb_native.rx.scan_requested){
  ncb_native.rx.scan.active=0;ncb_native.rx.scan.partial=1;
  if(nct_native.session.service_timed_out)ncb_native.rx.scan.timed_out=1;
 }
 ncb_report(result);return result<0?-1:0;
}
