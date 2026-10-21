#ifndef NVC_PHASE
#define NVC_PHASE(k) ((void)0)
#endif
/* PRIVATE normal capability adapter. Include after main_bind.inc. Proposed
 * hook is ncd_schedule() after nwfn_after_pool(), never inside its stages. */
#ifndef NCD_NATIVE_HOST_TEST
#include <kern/clock.h>
#include <kern/thread.h>
#include <kern/sched_prim.h>
#endif
#include "wifi_capability_dispatcher.h"
static struct {
 struct ncd_state diag;thread_t worker;
 unsigned attempted,active,parked,halt;int schedule_error;
}ncd_native;
static int ncd_native_owner(void *c)
{
 if(c!=&ncd_native||!ncd_native.active||ncd_native.halt||
    ncd_native.worker!=current_thread()||!nwfn_main.ready||nwfn_main.halt||
    nwfn_native.stage!=NWFN_RETAINED||!nfp_native.ready||nfp_native.halt||
    !nfp_native.finish_done||nfp_native.state.completed!=0x7f||
    ncd_native.diag.init!=&nwfn_native.init||
    ncd_native.diag.firmware!=&nfp_native||
    nfp_native.host_io!=(uintptr_t)npa_native.host)return -1;
 return nwfn_owner(&nwfn_native);
}
static int ncd_native_read(void*c,uint32_t p,uint32_t*v)
{
 if(!v||ncd_native_owner(c))return -1;
 if(p==0x180600f0u){*v=ni_rd(npa_native.host,0xf0);return 0;}
 return nwfn_io.reg_read(&nwfn_native,p,v);
}
static int ncd_native_write(void*c,uint32_t p,uint32_t v)
{if(ncd_native_owner(c))return -1;return nwfn_io.reg_write(&nwfn_native,p,v);}
static int ncd_native_mread(void*c,uint64_t p,void*v,uint32_t n)
{if(ncd_native_owner(c))return -1;return nwfn_io.mem_read(&nwfn_native,p,v,n);}
static int ncd_native_mwrite(void*c,uint64_t p,const void*v,uint32_t n)
{if(ncd_native_owner(c))return -1;return nwfn_io.mem_write(&nwfn_native,p,v,n);}
static int ncd_native_sync(void*c,uint64_t p,uint32_t n,unsigned way)
{if(ncd_native_owner(c))return -1;return nwfn_io.sync(&nwfn_native,p,n,way);}
static int ncd_native_delay(void*c,unsigned us)
{if(ncd_native_owner(c))return -1;return nwfn_io.delay_us(&nwfn_native,us);}
static int ncd_native_now(void*c,uint64_t*out)
{
 uint64_t absolute,ns;
 if(c!=&ncd_native||!out||!ncd_native.active||ncd_native.halt||
    ncd_native.worker!=current_thread())return -1;
 clock_get_uptime(&absolute);absolutetime_to_nanoseconds(absolute,&ns);
 *out=ns/1000;return 0;
}
static const struct nwt_io ncd_bus={ncd_native_owner,ncd_native_read,
 ncd_native_write,ncd_native_mread,ncd_native_mwrite,ncd_native_sync,ncd_native_delay};
static const struct ncd_io ncd_io={&ncd_bus,ncd_native_now};
static uint32_t ncd_header_word(const struct ncd_event*h,unsigned k)
{
 const uint8_t*p=h->header+4*k;
 return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;
}
static void ncd_report(int result)
{
 const struct ncd_state*s=&ncd_native.diag;
 printf("NORD2 WIFI CAP: status=%d phase=%u complete=%u halt=%u error=%u submit=%u txdone=%u events=%u rearmed=%u match=%u polls=%u\n",
  result,s->phase,s->completed,s->halt,s->error,s->submit_attempted,s->tx_done,
  s->events,s->rearmed,s->matched,s->polls);
 printf("NORD2 WIFI CAP RX: bytes=%u irq=%#x/%#x valid=%#x debug=%u echo=%u mismatch=%u elapsed_us=%llu\n",
  (unsigned)s->frame_bytes,s->irq_status[0],s->irq_status[1],s->irq_valid,
  s->debug_events,s->sequence_echo,s->sequence_mismatches,(unsigned long long)(s->last>=s->started?s->last-s->started:0));
 printf("NORD2 WIFI CAP DESCRIPTOR: raw=%#x/%#x/%#x/%#x\n",
  (unsigned)ncd_native.diag.descriptor[0]|(unsigned)ncd_native.diag.descriptor[1]<<8|
  (unsigned)ncd_native.diag.descriptor[2]<<16|(unsigned)ncd_native.diag.descriptor[3]<<24,
  (unsigned)ncd_native.diag.descriptor[4]|(unsigned)ncd_native.diag.descriptor[5]<<8|
  (unsigned)ncd_native.diag.descriptor[6]<<16|(unsigned)ncd_native.diag.descriptor[7]<<24,
  (unsigned)ncd_native.diag.descriptor[8]|(unsigned)ncd_native.diag.descriptor[9]<<8|
  (unsigned)ncd_native.diag.descriptor[10]<<16|(unsigned)ncd_native.diag.descriptor[11]<<24,
  (unsigned)ncd_native.diag.descriptor[12]|(unsigned)ncd_native.diag.descriptor[13]<<8|
  (unsigned)ncd_native.diag.descriptor[14]<<16|(unsigned)ncd_native.diag.descriptor[15]<<24);
 printf("NORD2 WIFI CAP TLV: elements=%u known=%u unknown=%u mac_present=%u mac_valid=%u resources=%u version=%#x product=%#x fw=%#x/%u\n",
  s->capability.elements,s->capability.known,s->capability.unknown,
  s->capability.mac_present,s->capability.mac_valid,s->capability.resources_supported,
  s->capability.resource_version,s->capability.product,s->capability.fw_version,
  s->capability.fw_build);
 printf("NORD2 WIFI CAP OWNER: native_error=%u native_halt=%u init_halt=%u driver=%u dmairq=%#x/%#x valid=%#x parked=%u\n",
  nwfn_native.error,nwfn_native.halt,nwfn_native.init.halt,
  nwfn_native.init.driver_owned,nwfn_native.init.transport.irq_status[0],
  nwfn_native.init.transport.irq_status[1],nwfn_native.init.transport.irq_status_valid,
  ncd_native.parked);
 for(unsigned k=0;k<s->events&&k<NCD_EVENT_BUDGET;k++){
  const struct ncd_event*h=&s->history[k];
  printf("NORD2 WIFI CAP EVENT: n=%u kind=%u type=%#x eid=%#x seq=%u bytes=%u declared=%u/%u debug_type=%u debug_bytes=%u\n",
   k,h->kind,h->rx_class,h->eid,h->sequence,h->bytes,h->rxd_bytes,h->event_bytes,
   h->debug_type,h->debug_bytes);
  if(h->kind==3)printf("NORD2 WIFI CAP SLEEPY: n=%u state=%u\n",k,(unsigned)h->debug_type);
  printf("NORD2 WIFI CAP EVENTHEADER: n=%u valid=%u bytes=%u inner=%#x raw=%#x/%#x/%#x/%#x/%#x/%#x/%#x/%#x/%#x\n",
   k,h->header_valid,h->header_valid?(unsigned)sizeof(h->header):0,h->inner_type,
   ncd_header_word(h,0),ncd_header_word(h,1),ncd_header_word(h,2),
   ncd_header_word(h,3),ncd_header_word(h,4),ncd_header_word(h,5),
   ncd_header_word(h,6),ncd_header_word(h,7),ncd_header_word(h,8));
 }
}
#include "wifi_basic_config_bind.inc"
static void ncd_worker(void*argument,wait_result_t wait_result)
{
 (void)argument;(void)wait_result;
 ncd_native.worker=current_thread();ncd_native.active=1;NVC_PHASE(4);
 printf("NORD2 WIFI CAP ARM: cid=%#x eid=%#x seq=%u deadline_us=%u event_budget=%u\n",
  0x8a,0xec,(unsigned)nwfn_native.init.transport.next_sequence,3000000u,NCD_EVENT_BUDGET);
 int result=ncd_run(&ncd_io,&ncd_native,&nwfn_native.init,
  &nfp_native,&ncd_native.diag,1);
 if(result){
  ncd_native.halt=1;nwfn_native.halt=1;nwfn_native.init.halt=1;
  nwfn_native.init.transport.halt=1;nfp_native.halt=1;
 }
 ncd_report(result); /* captured-only; no live read after error */
 if(result){
  nwm_failure_drain(); /* existing scheduler-only bounded terminal policy */
  panic("NORD2 WIFI CAP: uncertain retained DMA state; manual reset required");
 }
 if(nbc_after_cap()){
  nwm_failure_drain();
  panic("NORD2 WIFI BASIC: uncertain retained DMA state; manual reset required");
 }
 /* Retain the worker and buffers. No interface/carrier is published and this
  * bounded milestone does not claim continuous normal RX service. Future
  * requests need a reviewed queue/consumer; this worker cannot be reused by
  * fabricating another start/readiness flag. */
 ncd_native.parked=1;NVC_PHASE(6);
 printf("NORD2 WIFI CAP WORKER: parking=1 retained=1 callbacks_complete=1\n");
 for(;;){assert_wait((event_t)&ncd_native,THREAD_UNINT);thread_block(THREAD_CONTINUE_NULL);}
}
static void ncd_schedule(void)
{
 unsigned cap=0,main=0;thread_t thread=THREAD_NULL;
 PE_parse_boot_argn("nord2wificap",&cap,sizeof(cap));
 if(cap!=1)return;
 PE_parse_boot_argn("nord2wifimain",&main,sizeof(main));
 if(main!=1||!nwfn_main.ready||nwfn_main.halt||nfp_native.halt||
    !nfp_native.ready||!nfp_native.finish_done||nwfn_native.halt||
    nwfn_native.stage!=NWFN_RETAINED)return;
 if(!__sync_bool_compare_and_swap(&ncd_native.attempted,0,1))return;
 /* The selected caller performs only captured logging/return after this
  * hook; no previous main stage or MMIO callback can execute afterward. */
 kern_return_t kr=kernel_thread_start(ncd_worker,0,&thread);
 ncd_native.schedule_error=kr;
 if(kr!=KERN_SUCCESS){
  printf("NORD2 WIFI CAP: worker_start_error=%d worker_attempted=1 submit=0\n",kr);return;
 }
 thread_deallocate(thread); /* worker has its kernel lifetime; buffers static */
}
