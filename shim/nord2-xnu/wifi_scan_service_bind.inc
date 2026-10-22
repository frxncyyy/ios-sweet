/* Include at end of normal backend binder, before actual CAP worker. */
#ifdef NSS_HOST_TEST
#include "nss_host.h"
#else
#include <IOKit/IOLocks.h>
extern void nord2_wifi_ios_backend_ready(void);
#endif
#include "wifi_scan_service.h"
enum {NSS_FREE,NSS_QUEUED,NSS_RUNNING,NSS_DONE};
struct nss_slot {unsigned state,canceled,callback_busy,retire;uint64_t ticket,generation,captured;void*cookie;nss_completion completion;enum nss_status status;struct nss_request request;struct nsb_scan scan;};
static struct {IOLock*lock;unsigned ready,failed;uint64_t serial;uint8_t scan_sequence;struct nss_info info;struct nss_slot slot[NSS_SLOTS];struct nct_session previous;struct ncr_state previous_rx;struct ncs_frames frames;}nss_native;
static int nss_available(void){return __atomic_load_n(&nss_native.ready,__ATOMIC_ACQUIRE)==1;}
static struct nss_slot*nss_find(uint64_t ticket){for(unsigned k=0;k<NSS_SLOTS;k++)if(nss_native.slot[k].state&&nss_native.slot[k].ticket==ticket)return &nss_native.slot[k];return 0;}
int nord2_wifi_scan_info(struct nss_info*out){if(!out)return -1;if(!nss_available())return 1;IOLockLock(nss_native.lock);int r=nss_native.failed?1:0;if(!r){*out=nss_native.info;out->scanning=0;for(unsigned k=0;k<NSS_SLOTS;k++)if(nss_native.slot[k].state==NSS_RUNNING&&!nss_native.slot[k].canceled)out->scanning=1;}IOLockUnlock(nss_native.lock);return r;}
int nord2_wifi_scan_enqueue(const struct nss_request*q,void*cookie,uint64_t generation,nss_completion completion,uint64_t*ticket){
 if(!q||!cookie||!generation||!completion||!ticket||q->count>NSS_CHANNELS)return -1;
 if(!nss_available())return 1;IOLockLock(nss_native.lock);
 if(nss_native.failed||nss_native.serial==UINT64_MAX){IOLockUnlock(nss_native.lock);return 1;}
 for(unsigned i=0;i<q->count;i++){unsigned found=0;for(unsigned j=0;j<nss_native.info.count;j++)if(q->channels[i].band==nss_native.info.channels[j].band&&q->channels[i].channel==nss_native.info.channels[j].channel)found=1;
  for(unsigned j=0;j<i;j++)if(q->channels[i].band==q->channels[j].band&&q->channels[i].channel==q->channels[j].channel)found=0;
  if(!found){IOLockUnlock(nss_native.lock);return -1;}}
 struct nss_slot*s=0;for(unsigned k=0;k<NSS_SLOTS;k++)if(!nss_native.slot[k].state){s=&nss_native.slot[k];break;}
 if(!s){IOLockUnlock(nss_native.lock);return 2;}
 *s=(struct nss_slot){0};s->request=*q;s->cookie=cookie;s->generation=generation;s->completion=completion;s->ticket=++nss_native.serial;s->state=NSS_QUEUED;*ticket=s->ticket;
 IOLockUnlock(nss_native.lock);thread_wakeup((event_t)&nss_native);return 0;
}
int nord2_wifi_scan_cancel(uint64_t ticket){if(!ticket||!nss_available())return 1;IOLockLock(nss_native.lock);struct nss_slot*s=nss_find(ticket);int r=s?0:1;if(s)s->canceled=1;IOLockUnlock(nss_native.lock);if(!r)thread_wakeup((event_t)&nss_native);return r;}
int nord2_wifi_scan_view(uint64_t ticket,struct nss_view*out){if(!out)return -1;if(!nss_available())return 1;IOLockLock(nss_native.lock);struct nss_slot*s=nss_find(ticket);int r=1;if(s&&s->state==NSS_DONE&&!s->canceled&&(s->status==NSS_COMPLETE||s->status==NSS_PARTIAL)){*out=(struct nss_view){&s->scan,s->captured,s->generation,s->status};r=0;}IOLockUnlock(nss_native.lock);return r;}
int nord2_wifi_scan_release(uint64_t ticket){if(!ticket||!nss_available())return 1;IOLockLock(nss_native.lock);struct nss_slot*s=nss_find(ticket);int r=1;if(s&&s->state==NSS_DONE){if(s->callback_busy)s->retire=1;else memset(s,0,sizeof(*s));r=0;}IOLockUnlock(nss_native.lock);return r;}
static int nss_build(void*c,unsigned index,uint8_t*out,size_t capacity,size_t*used){if(c!=&nss_native.frames||index||!out||!used||capacity<NCS_SCAN_BYTES)return -1;memcpy(out,nss_native.frames.scan,NCS_SCAN_BYTES);*used=NCS_SCAN_BYTES;return 0;}
static int nss_start(void){
 unsigned opt=0;PE_parse_boot_argn("nord2wifiios",&opt,sizeof(opt));if(opt!=1)return 1;
 const struct nct_session*s=&nct_native.session;struct nni_address address;struct ncfg_channel channels[39];size_t count=0;
 if(nss_native.lock||ncd_native.worker!=current_thread()||!ncd_native.active||ncd_native.parked||ncd_native.halt||
 !ncb_native.prepared||ncb_native.refusal||!ncb_native.plan.prepared||nct_native.active||!nct_native.attempted||
 !nct_done_receipt(s)||s->phase!=NCT_DONE||s->halt||s->error||s->refusal||s->count!=NCB_COMMANDS||s->completed!=s->count||s->cursor!=s->count||
 s->basic!=&nbc_native.state||s->cap!=&ncd_native.diag||s->init!=&nwfn_native.init||s->firmware!=&nfp_native||
 nbc_native.state.halt||nbc_native.state.error||ncd_native.diag.halt||ncd_native.diag.error||nfp_native.state.halt||nfp_native.state.error||
 nwfn_native.halt||nwfn_native.stage!=NWFN_RETAINED||!nwfn_native.ap_owned||!nwfn_native.wf_owned||nwfn_native.asleep||!nwfn_native.init.initialized||nwfn_native.init.halt||nwfn_native.init.transport.halt||nfp_native.halt||!nfp_native.ready||!nfp_native.finish_done||
 ncb_native.rx.halt||ncb_native.rx.error||ncb_native.rx.active||!ncb_native.rx.scan.done||ncb_native.rx.scan.partial||ncb_native.rx.scan.timed_out||ncb_native.rx.scan.completion.state!=7||
 nwfn_native.init.transport.completed!=5u+NCB_COMMANDS||
 ncfg_address(&ncb_native.plan.input,&ncb_native.plan.config,&address)||address.source!=NNI_ADDRESS_BOARD||
 ncfg_channels(&ncb_native.plan.input,&ncb_native.plan.config,channels,39,&count))return 1;
 for(unsigned k=0;k<6;k++)nss_native.info.mac[k]=address.mac[k];
 const struct nrp_caps*c=&ncb_native.plan.input.caps;nss_native.info.band5=c->band5;nss_native.info.nss=c->nss;nss_native.info.max_bw=c->max_bw;nss_native.info.path=c->path;
 for(unsigned k=0;k<count;k++)if(channels[k].allowed&&!channels[k].dfs)nss_native.info.channels[nss_native.info.count++]=(struct ncs_channel){channels[k].band,channels[k].channel};
 if(!nss_native.info.count||nss_native.info.count!=ncb_native.plan.channel_count)return 1;
 nss_native.scan_sequence=ncb_native.rx.scan.sequence;
 nss_native.lock=IOLockAlloc();if(!nss_native.lock)return 1;
 __atomic_store_n(&nss_native.ready,1,__ATOMIC_RELEASE);
 printf("NORD2 WIFI IOS OWNER: active=1 retained=1 boot_complete=1 ready=1\n");
 nord2_wifi_ios_backend_ready();return 0;
}
static int nss_execute(struct nss_slot*slot){
 struct nct_session*s=&nct_native.session;struct ncr_state*r=&ncb_native.rx;
 if(ncd_native.worker!=current_thread()||!ncd_native.active||ncd_native.halt||nct_native.active||s->phase!=NCT_DONE||s->halt||r->active||r->halt)return 1;
 const struct ncs_channel*channels=slot->request.count?slot->request.channels:nss_native.info.channels;
 size_t count=slot->request.count?slot->request.count:nss_native.info.count;
 uint8_t sequence=(uint8_t)(nss_native.scan_sequence+1u);
 if(ncs_build(ncd_native.diag.frame,ncd_native.diag.frame_bytes,ncd_native.diag.history[ncd_native.diag.events-1].sequence,nss_native.info.mac,channels,count,1,2,sequence,1,0,0,&nss_native.frames))return 1;
 nss_native.previous=*s;nss_native.previous_rx=*r;nss_native.previous_rx.session=&nss_native.previous;
 memset(s,0,sizeof(*s));memset(r,0,sizeof(*r));
 const struct nct_entry entry={NCS_SCAN_BYTES,3,1};const struct nct_recipe recipe={&entry,1,nss_build,&nss_native.frames,UINT64_C(30000000),UINT64_C(15000000)};
 if(nct_continue_prepare(&nbc_native.state,&nss_native.previous,&recipe,s)||ncr_continue_prepare(r,&nwfn_native.init,s,&nss_native.previous_rx,sequence,UINT64_C(14000000))){*s=nss_native.previous;*r=nss_native.previous_rx;r->session=s;return 1;}
 nct_native.active=1;NVC_PHASE(5);
 const struct nct_hooks hooks={ncr_pump,r,ncb_sleep,&ncb_native};
 int result=nct_run(&ncd_io,&ncd_native,&hooks,s);nct_native.active=0;
 if(result==1&&s->drain_refused){/* RX may already have rearmed stale frames: preserve its current cursors. */
  *s=nss_native.previous;r->session=s;r->phase=NCR_IDLE;r->active=0;return 1;
 }
 nss_native.scan_sequence=sequence;
 if(result<0){slot->scan=r->scan;slot->captured=r->scan.last;ncd_native.halt=nwfn_native.halt=nwfn_native.init.halt=nwfn_native.init.transport.halt=nfp_native.halt=1;return -1;}
 if(!r->scan.done&&r->scan_requested){r->scan.active=0;r->scan.partial=1;if(s->service_timed_out)r->scan.timed_out=1;}
 uint64_t now;if(ncd_native_now(&ncd_native,&now)||now<r->scan.last)return -1;
 slot->scan=r->scan;slot->captured=now;return 0;
}
static void nss_deliver(struct nss_slot*s,enum nss_status status){
 IOLockLock(nss_native.lock);s->status=s->canceled?NSS_CANCELED:status;s->state=NSS_DONE;s->callback_busy=1;
 printf("NORD2 WIFI IOS SCAN: ticket=%llu generation=%llu status=%u sequence=%u frames=%u bss=%u done=%u partial=%u canceled=%u error=%u rx_error=%u\n",(unsigned long long)s->ticket,(unsigned long long)s->generation,(unsigned)s->status,s->scan.sequence,s->scan.frames,s->scan.count,s->scan.done,s->scan.partial,s->canceled,nct_native.session.error,ncb_native.rx.error);
 if(s->status==NSS_FATAL)printf("NORD2 WIFI IOS GUARD: reason=%u source=%u stage=%u error=%u valid=%u addr=%#x raw=%#x mask=%#x expected=%#x\n",nwfn_native.guard_reason,nwfn_native.guard_source,nwfn_native.failure_stage,nwfn_native.error,nwfn_native.guard_valid,nwfn_native.guard_address,nwfn_native.guard_raw,nwfn_native.guard_mask,nwfn_native.guard_expected);
 nss_completion fn=s->completion;void*cookie=s->cookie;uint64_t gen=s->generation,ticket=s->ticket;status=s->status;IOLockUnlock(nss_native.lock);
 fn(cookie,gen,ticket,status); /* no lock held; cookie not accessed afterward */
 IOLockLock(nss_native.lock);s->callback_busy=0;if(s->retire)memset(s,0,sizeof(*s));IOLockUnlock(nss_native.lock);
}
static void nss_owner_loop(void){
 for(;;){IOLockLock(nss_native.lock);struct nss_slot*s=0;for(unsigned k=0;k<NSS_SLOTS;k++)if(nss_native.slot[k].state==NSS_QUEUED&&(!s||nss_native.slot[k].ticket<s->ticket))s=&nss_native.slot[k];
  if(!s){NVC_PHASE(6);assert_wait((event_t)&nss_native,THREAD_UNINT);IOLockUnlock(nss_native.lock);thread_block(THREAD_CONTINUE_NULL);continue;}
  s->state=NSS_RUNNING;unsigned canceled=s->canceled;IOLockUnlock(nss_native.lock);
  int result=canceled?1:nss_execute(s);enum nss_status status=canceled?NSS_CANCELED:result<0?NSS_FATAL:result?NSS_REFUSED:(s->scan.done&&!s->scan.partial&&!s->scan.timed_out&&!s->scan.dropped)?NSS_COMPLETE:NSS_PARTIAL;
  if(result<0){ncd_native.halt=nwfn_native.halt=nwfn_native.init.halt=nwfn_native.init.transport.halt=nfp_native.halt=1;IOLockLock(nss_native.lock);nss_native.failed=1;IOLockUnlock(nss_native.lock);}
  nss_deliver(s,status);
  if(result<0){IOLockLock(nss_native.lock);nss_native.failed=1;IOLockUnlock(nss_native.lock);
   for(unsigned k=0;k<NSS_SLOTS;k++){IOLockLock(nss_native.lock);unsigned pending=nss_native.slot[k].state==NSS_QUEUED;IOLockUnlock(nss_native.lock);if(pending)nss_deliver(&nss_native.slot[k],NSS_FATAL);}
   nwm_failure_drain();panic("NORD2 WIFI SERVICE: uncertain retained DMA; manual reset required");}
 }
}
