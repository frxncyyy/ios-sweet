static void full_test_probe(unsigned);
static void nrd_forbid_write(void);
static unsigned long long full_clock_extra(void);
#ifdef FULL_FAULTS
static int full_test_fail(void);
#endif
#include "owner-fixture.inc"
static unsigned mode,delivered,probe_memory,probe_write,delayed_trigger;
static unsigned nrd_in_capture,nrd_calls,nrd_restored,nrd_checked;
static void nrd_forbid_write(void){assert(!nrd_in_capture);}
int nrd_host_probe(const struct ncd_io*io,void*ctx,struct ncr_state*r,struct nrd_snapshot*out,unsigned reason){
unsigned phase=r->phase,current=r->current,reads=mmio_reads,writes=mmio_writes,rx_index=r->init->transport.rx_index;
struct ncr_queue queues[5];memcpy(queues,r->queue,sizeof(queues));
struct nrd_snapshot bad={0};struct ncd_io fake=*io;
assert(nrd_capture(&fake,ctx,r,&bad,reason)<0&&!bad.attempted);
assert(nrd_capture(io,(char*)ctx+1,r,&bad,reason)<0&&!bad.attempted);
assert(nrd_capture(io,ctx,r,&bad,0)<0&&!bad.attempted);
assert(nwfn_read_allowed(0x18024680)&&nwfn_read_allowed(0x18024684)&&nwfn_read_allowed(0x18024688)&&nwfn_read_allowed(0x1802468c));
assert(!nwfn_read_allowed(0x18024690)&&!nwfn_read_allowed(0x18025680));
struct nwfn_native clone=nwfn_native;uint32_t value;assert(nwfn_read(&clone,0x18024680,&value)<0);
assert(reads==mmio_reads&&writes==mmio_writes);nrd_checked++;
nrd_in_capture=1;int result=nrd_capture(io,ctx,r,out,reason);nrd_in_capture=0;nrd_calls++;
assert(r->phase==phase&&r->current==current&&writes==mmio_writes&&r->init->transport.rx_index==rx_index&&!memcmp(queues,r->queue,sizeof(queues)));nrd_restored++;
if(!result){
 assert(out->attempted&&out->complete&&!out->error&&out->valid==3&&out->last>=out->started);
 assert((out->cfg[0]&5)==5&&(out->cfg[1]&5)==5);
 for(unsigned j=0;j<4;j++){
  const struct nrd_queue*q=&out->queue[j];assert(q->valid==63&&q->control[0]==q->expected_descriptors&&q->control[1]==r->queue[q->queue].ring.count&&q->ext_address==0x18024680+4*j&&q->ext==4+0x400000*j);
  assert(q->descriptor==q->expected_descriptors+16*q->next);
 }
 reads=mmio_reads;writes=mmio_writes;assert(nrd_capture(io,ctx,r,out,reason)<0&&reads==mmio_reads&&writes==mmio_writes);
}
return result;
}

static unsigned long long full_clock_extra(void){
 if(mode==4&&!delayed_trigger&&nct_native.active&&nct_native.session.phase==NCT_SERVICE&&ncb_native.rx.done_pending){
  delayed_trigger=1;struct ncr_queue*q=&ncb_native.rx.queue[0];uint8_t*d=pool_memory+(q->ring.descriptors-NDP_POOL_BASE)+16*q->next;put32(d+4,get32(d+4)|0x80000000u);return 14500000000ULL;
 }return 0;
}
static void full_test_probe(unsigned writing){
 struct ncr_state*r=&ncb_native.rx;if(!nct_native.active||!r->active)return;
 const struct ncr_queue*q=&r->queue[r->current];unsigned reads=mmio_reads,writes=mmio_writes;
 if(!writing&&r->phase==NCR_DESC_READ){
  uint64_t a=q->ring.descriptors+16u*q->next;
  assert(ncb_rx_memory(&nwfn_native,a,16,0)&&nwfn_memory_range(&nwfn_native,a,16,0));
  assert(!ncb_rx_memory(&nwfn_native,a+16,16,0)&&!nwfn_memory_range(&nwfn_native,a+16,16,0));
  assert(!ncb_rx_memory(&nwfn_native,a,16,1)&&!ncb_rx_memory(&nwfn_native,a,15,0));
  struct nwfn_native clone=nwfn_native;assert(!ncb_rx_memory(&clone,a,16,0));
  worker_thread=3;assert(!ncb_rx_memory(&nwfn_native,a,16,0));worker_thread=2;
  probe_memory|=1u<<r->current;
 }
 if(writing&&r->phase==NCR_PUBLISH){
  assert(ncb_rx_write(&nwfn_native,q->ring.base+8,q->next));
  assert(!ncb_rx_write(&nwfn_native,q->ring.base+8,(q->next+1)%q->ring.count)&&!ncb_rx_write(&nwfn_native,q->ring.base+12,q->next));
  probe_write|=1u<<r->current;
 }
 assert(mmio_reads==reads&&mmio_writes==writes);
}
static void w16(uint8_t*p,unsigned v){p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);}
static void deliver(unsigned k,const uint8_t*frame,unsigned n,unsigned ddone){
 struct ncr_queue*q=&ncb_native.rx.queue[k];assert(q->ring.count&&n<=3840);
 uint8_t*d=pool_memory+(q->ring.descriptors-NDP_POOL_BASE)+16*q->next;
 uint8_t*b=pool_memory+(q->ring.buffers-NDP_POOL_BASE)+4096*q->next;
 put32(d,0xdeadbeef);put32(d+4,(ddone?0x80000000u:0)|0x40000000u|(n<<16));put32(d+8,0xffffffff);put32(d+12,0x12345678);memcpy(b,frame,n);
 hardware_register(q->ring.base+12,(q->next+1)%q->ring.count);
}
static void beacon(unsigned k,unsigned id,unsigned ddone){uint8_t p[63]={0};w16(p,63);w16(p+2,0x3801);put32(p+8,12u<<8);put32(p+12,6u<<8);p[24]=0x80;p[34]=2;p[40]=2;w16(p+56,100);p[60]=0;p[61]=1;p[62]=(uint8_t)id;deliver(k,p,sizeof(p),ddone);}
static void full_deliver(const unsigned char*cmd){
 if(cmd[36]!=3)return;
 assert(!delivered++&&nct_native.session.cursor==21&&nct_native.session.record[21].publication_attempted&&!nct_native.session.record[21].published);
 beacon(0,'X',mode!=2&&mode!=4);if(mode==4){struct ncr_queue*q=&ncb_native.rx.queue[0];uint8_t*b=pool_memory+(q->ring.buffers-NDP_POOL_BASE)+4096*q->next;w16(b+2,6u<<11);}beacon(2,'Y',1);
 uint8_t p[512]={0};w16(p,32);w16(p+2,6u<<11);deliver(3,p,32,1);w16(p+2,11u<<11);deliver(4,p,32,1);
 if(mode==1)return; /* actual no completion => bounded truthful partial */
 memset(p,0,sizeof(p));w16(p,512);w16(p+2,0x3800);w16(p+24,488);w16(p+26,0x1234);p[28]=mode==3?0xf0:0x0d;p[36]=1;p[40]=(uint8_t)ncb_native.plan.channel_count;p[41]=7;p[42]=2;p[37]=1;p[38]=1;p[39]=6;put32(p+44,13);p[48]=1;p[52]=2;p[56]=6;p[57]=36;w16(p+120,0x1234);w16(p+122,0xabcd);p[248]=2;p[249]=3;p[312]=4;p[313]=5;put32(p+376,8);put32(p+380,5);w16(p+384,0x55aa);w16(p+386,0xa55a);deliver(1,p,sizeof(p),1);
}
static void setup(unsigned which){ready_fixture();memset(&ncb_native,0,sizeof(ncb_native));memset(&nct_native,0,sizeof(nct_native));mode=which;delivered=no_board=probe_memory=probe_write=delayed_trigger=0;normal_enabled=1;scan_flag=1;normal_pubs=normal_no_ddone=normal_wrong_didx=0;basic_flag=1;}
int main(int argc,char**argv){assert(argc==2);FILE*f=fopen(argv[1],"rb");assert(f&&fread(authenticated_board,1,sizeof(authenticated_board),f)==sizeof(authenticated_board));fclose(f);
 setup(0);run_worker();struct nct_session*t=&nct_native.session;struct ncr_state*r=&ncb_native.rx;
 assert(!manual_halts&&ncd_native.parked&&t->phase==NCT_DONE&&t->completed==22&&normal_pubs==22&&ncb_native.prepared);
 assert(r->scan_requested&&r->scan.done&&!r->scan.timed_out&&!r->scan.partial&&r->scan.count==2&&r->scan.frames==2&&r->drain_passes==1&&t->scan_complete);
 assert(r->packets==5&&r->rearmed==5&&r->msdu_reports==1&&r->rx_reports==1&&r->history_count==1&&r->history[0].inner==0x1234&&probe_memory==31&&probe_write==31);
 for(unsigned k=0;k<5;k++)assert(r->queue[k].packets==1&&r->queue[k].rearmed==1&&r->queue[k].raw[0]==0xdeadbeef);
 assert(r->scan.completion.version==2&&r->scan.completion.beacons==13&&r->scan.completion.beacons_2g==8&&r->scan.completion.beacons_5g==5&&r->scan.completion.pno==1&&r->scan.completion.channel_count==2&&r->scan.completion.channels[0]==6&&r->scan.completion.idle[1]==0xabcd&&r->scan.completion.scan_time[1]==0xa55a);
 assert(!nwfn_read_allowed(0x18024680)&&!nwfn_read_allowed(0x1802468c));
 assert(r->readback[0].complete&&r->readback[0].reason==NRD_BASELINE&&r->readback[1].complete&&r->readback[1].reason==NRD_SCAN_DONE);
 assert(t->pse_total==3&&t->basic_spent==1&&t->capacity==2&&t->available==2&&!t->reserved&&t->acquired==22&&t->released==22&&nwfn_native.init.transport.completed==27);
 unsigned reads=mmio_reads,writes=mmio_writes;assert(!nct_native_owner_window(&nwfn_native)&&nct_native_run(0,0)<0&&reads==mmio_reads&&writes==mmio_writes);
 setup(1);run_worker();t=&nct_native.session;r=&ncb_native.rx;assert(!manual_halts&&ncd_native.parked&&t->completed==22&&t->service_timed_out&&!t->scan_complete&&r->scan.timed_out&&r->scan.partial&&!r->scan.done&&r->scan.count==2&&r->readback[1].complete&&r->readback[1].reason==NRD_SCAN_EXPIRED);
 setup(2);run_worker();t=&nct_native.session;r=&ncb_native.rx;assert(!manual_halts&&ncd_native.parked&&t->completed==22&&t->service_timed_out&&!t->scan_complete&&r->done_pending&&!r->scan.done&&r->queue[0].wait_ddone&&!r->queue[0].rearmed);
 setup(4);run_worker();t=&nct_native.session;r=&ncb_native.rx;assert(!manual_halts&&delayed_trigger&&ncd_native.parked&&t->completed==22&&t->service_timed_out&&!t->scan_complete&&r->done_pending&&!r->scan.done&&r->scan_expired&&r->scan.timed_out&&r->scan.partial);
 setup(3);run_worker();t=&nct_native.session;r=&ncb_native.rx;assert(manual_halts==1&&nwm_drain_entered&&drain_blocks==1&&t->halt&&r->halt&&r->error==20&&t->completed==21&&normal_pubs==22&&t->available==2&&!t->reserved);
 setup(0);no_board=1;run_worker();assert(!manual_halts&&ncd_native.parked&&!nct_native.attempted&&!normal_pubs&&ncb_native.refusal==2&&nwfn_native.init.transport.completed==5);
 setup(0);scan_flag=0;run_worker();assert(!manual_halts&&ncd_native.parked&&!nct_native.attempted&&!ncb_native.attempted&&!normal_pubs&&nwfn_native.init.transport.completed==5);
 assert(nrd_calls&&nrd_calls==nrd_restored&&nrd_checked==nrd_calls);
 puts("PASS readonly snapshots/phase restoration/no writes or queue mutation + actual22 sourceconfig/pinnedboard+BSS/scan coordinator, real native lease and five RXrings/opaque descriptors, copiedBSS+matchingdone, softwarecredits/noACK, retained clean timeout/DDONEpending partial, firmwareerror actual30sdrain, missingboard/defaultoff preIO refusal. Synthetic only; no hardware scan claim.");return 0;
}
