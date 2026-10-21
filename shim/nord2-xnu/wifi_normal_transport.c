#include "wifi_normal_transport.h"
static uint16_t u16(const uint8_t*p){return (uint16_t)p[0]|(uint16_t)p[1]<<8;}
static uint32_t u32(const uint8_t*p){return (uint32_t)u16(p)|(uint32_t)u16(p+2)<<16;}
static uint64_t fingerprint(const uint8_t*p,size_t n){uint64_t h=UINT64_C(14695981039346656037);while(n--)h=(h^*p++)*UINT64_C(1099511628211);return h;}
static int refuse(struct nct_session*s,unsigned why){s->refusal=why;return 1;}
static int basic_receipt(const struct nbc_state*b){
 if(!b||!b->cap||!b->init||!b->firmware||!b->attempted||!b->prepared||!b->entered||
    b->phase!=NBC_DONE||b->halt||b->error||b->skipped||!b->completed||!b->tx_done||
    b->submit_attempted!=1||b->publications!=1||!b->tx_observed||!(b->tx_raw[1]&0x80000000u)||
    b->cap->init!=b->init||b->cap->firmware!=b->firmware||!b->cap->entered||b->cap->phase!=5||
    b->cap->halt||b->cap->error||b->cap->completed!=1||b->cap->matched!=1||
    !b->cap->events||b->cap->events>16||b->cap->rearmed!=b->cap->events||
    b->cap->history[b->cap->events-1].kind!=1||b->cap->history[b->cap->events-1].eid!=0xec||
    !b->init->initialized||b->init->halt||b->init->transport.halt||
    b->init->transport.phase!=2||b->init->transport.finish_phase!=2||
    !b->firmware->ready||b->firmware->halt||b->firmware->state.halt||b->firmware->state.error||
    !b->firmware->finish_done||b->firmware->state.completed!=127||
    (b->firmware->ready_raw&3)!=3||!b->firmware->ready_polls||
    b->plan.version!=0x80000001u||b->plan.tc!=4||b->plan.pse_cost!=1||b->plan.ple_cost||
    b->plan.wire_bytes!=76||!b->plan.pse_cmd||b->budget.reserved!=1||
    b->budget.pse_remaining!=b->plan.pse_cmd-1)return -1;
 return 0;
}
static int build(struct nct_session*s,unsigned k){size_t used=0;
 if(k>=s->count||s->build(s->build_context,k,s->wire,sizeof(s->wire),&used))return -1;
 const struct nct_entry*e=&s->entry[k];
 if(used!=e->bytes||used<64||used>NCT_WIRE_LIMIT||u32(s->wire)!=(used|(2u<<23))||
    u16(s->wire+32)!=used-32||s->wire[36]!=e->cid||s->wire[37]!=0xa0||s->wire[38]!=e->set_query)return -1;
 s->wire[39]=(uint8_t)(s->first_sequence+k);return 0;
}
int nct_prepare(const struct nbc_state*b,const struct nct_recipe*r,struct nct_session*s){
 struct ncp_result actual={0};
 if(!s||s->attempted)return -1;
 s->attempted=1;s->basic=b;
 if(basic_receipt(b))return refuse(s,101);
 s->cap=b->cap;s->init=b->init;s->firmware=b->firmware;
 struct nwt_state*t=&s->init->transport;
 if(t->completed!=5||t->submit_attempted||t->tx_done||t->rx_done||t->tx_index!=(b->tx_index+1)%1024||
    b->tx_didx!=t->tx_index||t->next_sequence!=(uint8_t)(b->sequence+1)||t->rx_index>=16)return refuse(s,102);
 if(ncp_parse(s->cap->frame,s->cap->frame_bytes,s->cap->history[s->cap->events-1].sequence,&actual)||
    !actual.resources_supported||actual.resource_version!=0x80000001u||actual.resources[1]!=b->plan.pse_cmd||
    actual.resources[1]<=1)return refuse(s,103);
 if(!r||!r->entry||!r->build||!r->count||r->count>NCT_COMMAND_LIMIT||
    !r->batch_us||r->batch_us>NCT_BATCH_MAX_US||r->service_us>NCT_SERVICE_MAX_US||r->service_us>=r->batch_us)return refuse(s,104);
 s->count=r->count;s->build=r->build;s->build_context=r->context;s->batch_us=r->batch_us;s->service_us=r->service_us;
 s->first_tx=t->tx_index;s->first_sequence=t->next_sequence;s->base_completed=t->completed;
 s->pse_total=actual.resources[1];s->basic_spent=1;s->capacity=s->pse_total-1;s->available=s->capacity;
 for(unsigned k=0;k<s->count;k++){
  s->entry[k]=r->entry[k];
  if(!s->entry[k].cid||s->entry[k].set_query!=1||build(s,k))return refuse(s,105);
  s->fingerprint[k]=fingerprint(s->wire,s->entry[k].bytes);
  s->record[k]=(struct nct_record){.cid=s->entry[k].cid,.sequence=(uint8_t)(s->first_sequence+k),.bytes=s->entry[k].bytes,
   .from=(s->first_tx+k)%1024,.to=(s->first_tx+k+1)%1024,.didx=(s->first_tx+k)%1024};
 }
 s->prepared=1;s->phase=NCT_PREPARED;return 0;
}
int nct_lease_window(const struct nct_session*s,const struct nbc_state*b,const struct ncd_state*c,const struct nwi_state*i,const struct nfp_native*f){
 if(!s||!s->attempted||!s->prepared||!s->entered||s->halt||s->error||s->refusal||
    s->basic!=b||s->cap!=c||s->init!=i||s->firmware!=f||basic_receipt(b)||
    !s->count||s->count>NCT_COMMAND_LIMIT||s->cursor>s->count||s->completed!=s->cursor||s->base_completed!=5||
    s->pse_total!=b->plan.pse_cmd||s->basic_spent!=1||s->capacity!=s->pse_total-1||
    s->available+s->reserved!=s->capacity||s->reserved>1||s->released>s->acquired||
    s->acquired-s->released!=s->reserved||i->transport.completed!=s->base_completed+s->cursor||
    i->transport.tx_index!=(s->first_tx+s->cursor)%1024||
    i->transport.next_sequence!=(uint8_t)(s->first_sequence+s->cursor)||i->transport.rx_index>=16)return 0;
 for(unsigned k=0;k<s->cursor;k++)if(!s->record[k].published||!s->record[k].credit_released||!s->record[k].tx_done)return 0;
 unsigned in_flight=s->phase==NCT_SUBMITTED;
 if(in_flight&&s->cursor>=s->count)return 0;
 if(s->acquired!=s->cursor+in_flight||s->released!=s->cursor+(in_flight&&s->record[s->cursor].published))return 0;
 if(s->phase==NCT_SERVICE)return s->cursor==s->count&&!s->reserved&&!i->transport.submit_attempted;
 if((s->phase!=NCT_COMMAND&&s->phase!=NCT_WRITING&&s->phase!=NCT_SUBMITTED)||s->cursor>=s->count)return 0;
 const struct nct_record*q=&s->record[s->cursor];const struct nct_entry*e=&s->entry[s->cursor];
 if(e->bytes<64||e->bytes>NCT_WIRE_LIMIT||q->cid!=e->cid||q->bytes!=e->bytes||q->sequence!=(uint8_t)(s->first_sequence+s->cursor)||
    q->from!=i->transport.tx_index||q->to!=(q->from+1)%1024||
    s->wire[36]!=e->cid||s->wire[37]!=0xa0||s->wire[38]!=e->set_query||s->wire[39]!=q->sequence||
    fingerprint(s->wire,e->bytes)!=s->fingerprint[s->cursor])return 0;
 if(s->phase==NCT_SUBMITTED)return i->transport.submit_attempted==1&&
   (q->published?(q->credit_released==1&&!s->reserved):s->reserved==1);
 return !i->transport.submit_attempted&&!s->reserved&&!q->publication_attempted;
}
static int fail(struct nct_session*s,unsigned e){if(!s->halt){s->halt=1;s->error=e;s->phase=NCT_HALTED;if(s->init){s->init->halt=1;s->init->transport.halt=1;if(s->mutated)s->init->transport.dirty=1;}}return -1;}
static int access_ok(struct nct_access*a){return a&&a->session&&a==&a->session->access&&a->hardware&&a->hardware->bus;}
static int before(struct nct_access*a){uint64_t now;struct nct_session*s=a->session;
 if(s->halt)return -1;
 if(!nct_lease_window(s,s->basic,s->cap,s->init,s->firmware))return fail(s,1);
 s->callbacks++;if(a->hardware->bus->owner(a->context))return fail(s,1);
 s->callbacks++;if(a->hardware->now_us(a->context,&now))return fail(s,2);
 /* Soft service cutoff is tested only between complete pump passes, so an
  * already started RX descriptor rearm remains atomic under the hard batch
  * bound. Command deadlines remain strict for every callback. */
 uint64_t end=s->phase==NCT_SERVICE?s->deadline:s->command_deadline;
 if(now<s->last||now>=s->deadline||now>=end)return fail(s,3);
 s->last=now;return 0;
}
static int owner(void*c){struct nct_access*a=c;return !access_ok(a)?-1:before(a);}
static int rr(void*c,uint32_t p,uint32_t*v){struct nct_access*a=c;if(!v||!access_ok(a)||before(a))return -1;a->session->callbacks++;return a->hardware->bus->reg_read(a->context,p,v)?fail(a->session,4):0;}
static int rw(void*c,uint32_t p,uint32_t v){struct nct_access*a=c;if(!access_ok(a)||before(a))return -1;struct nct_session*s=a->session;
 if(p==0x18025418){if(s->phase!=NCT_SUBMITTED||s->cursor>=s->count||v!=s->record[s->cursor].to||s->record[s->cursor].publication_attempted)return fail(s,5);s->record[s->cursor].publication_attempted=1;}
 s->mutated=1;s->callbacks++;return a->hardware->bus->reg_write(a->context,p,v)?fail(s,5):0;}
static int mr(void*c,uint64_t p,void*v,uint32_t n){struct nct_access*a=c;if(!v||!access_ok(a)||before(a))return -1;a->session->callbacks++;return a->hardware->bus->mem_read(a->context,p,v,n)?fail(a->session,6):0;}
static int mw(void*c,uint64_t p,const void*v,uint32_t n){struct nct_access*a=c;if(!v||!access_ok(a)||before(a))return -1;struct nct_session*s=a->session;
 if(p==s->init->transport.command&&(s->phase!=NCT_WRITING||s->cursor>=s->count||n!=s->entry[s->cursor].bytes))return fail(s,7);
 if(p==s->init->transport.tx_ring+16u*s->init->transport.tx_index&&(s->phase!=NCT_WRITING||n!=16))return fail(s,7);
 a->session->mutated=1;a->session->callbacks++;return a->hardware->bus->mem_write(a->context,p,v,n)?fail(a->session,7):0;}
static int syncmem(void*c,uint64_t p,uint32_t n,unsigned way){struct nct_access*a=c;if(!access_ok(a)||before(a))return -1;a->session->callbacks++;return a->hardware->bus->sync(a->context,p,n,way)?fail(a->session,8):0;}
static int delay(void*c,unsigned us){struct nct_access*a=c;if(!access_ok(a)||before(a))return -1;a->session->callbacks++;return a->hardware->bus->delay_us(a->context,us)?fail(a->session,9):0;}
static int now(void*c,uint64_t*out){struct nct_access*a=c;if(!out||!access_ok(a)||before(a))return -1;*out=a->session->last;return 0;}
static const struct nwt_io checked_bus={owner,rr,rw,mr,mw,syncmem,delay};
const struct ncd_io nct_checked_io={&checked_bus,now};
static int coherent(struct nct_access*a){uint32_t v;for(unsigned d=0;d<2;d++){if(rr(a,0x18024200u+4096u*d,&v))return -1;if(v&0x00300000u)return fail(a->session,10);}return 0;}
static int pump(const struct nct_hooks*h,struct nct_access*a){struct nct_session*s=a->session;unsigned used=0;
 if(!h||!h->pump)return 0;
 if(before(a))return -1;
 unsigned left=NCT_EVENT_LIMIT-s->events,budget=left<16?left:16;
 if(!budget){s->event_limit_reached=1;return 0;}
 s->pump_calls++;int r=h->pump(h->pump_context,&nct_checked_io,a,budget,&used);
 if(s->halt)return -1;
 if(before(a))return -1;
 if(r<0||used>budget)return fail(s,31);
 s->events+=used;if(s->events==NCT_EVENT_LIMIT)s->event_limit_reached=1;if(r==1)s->scan_complete=1;return 0;
}
static int wait(const struct nct_hooks*h,struct nct_access*a){if(before(a))return -1;
 if(h&&h->sleep){if(h->sleep(h->sleep_context,1000))return fail(a->session,9);return before(a);}
 return delay(a,1000);
}
static int callbacks_ok(const struct ncd_io*o){return o&&o->bus&&o->now_us&&o->bus->owner&&o->bus->reg_read&&o->bus->reg_write&&o->bus->mem_read&&o->bus->mem_write&&o->bus->sync&&o->bus->delay_us;}
int nct_run(const struct ncd_io*o,void*context,const struct nct_hooks*h,struct nct_session*s){uint32_t v;
 if(!callbacks_ok(o)||!s||!s->prepared||s->entered||s->halt||s->refusal||
    (s->service_us&&(!h||!h->pump)))return -1;
 s->access=(struct nct_access){s,o,context};struct nct_access*a=&s->access;
 s->entered=1;s->phase=NCT_COMMAND;
 if(build(s,0)||fingerprint(s->wire,s->entry[0].bytes)!=s->fingerprint[0])return fail(s,32);
 if(!nct_lease_window(s,s->basic,s->cap,s->init,s->firmware))return fail(s,1);
 s->callbacks++;if(o->bus->owner(context))return fail(s,1);
 s->callbacks++;if(o->now_us(context,&s->started))return fail(s,2);
 if(s->started>UINT64_MAX-s->batch_us)return fail(s,3);
 s->last=s->started;s->deadline=s->started+s->batch_us;
 struct nwt_state*t=&s->init->transport;
 while(s->cursor<s->count){
  struct nct_record*q=&s->record[s->cursor];
  s->phase=NCT_COMMAND;
  if(build(s,s->cursor)||fingerprint(s->wire,q->bytes)!=s->fingerprint[s->cursor])return fail(s,32);
  if(s->last>UINT64_MAX-NCT_COMMAND_US)return fail(s,3);
  s->command_deadline=s->last+NCT_COMMAND_US;
  if(rr(a,0x180600f0,&v)||(v&3)!=3)return fail(s,11);
  if(coherent(a)||rr(a,0x18025418,&v)||v!=q->from||rr(a,0x1802541c,&v)||v!=q->from)return fail(s,12);
  uint64_t d=t->tx_ring+16u*q->from;
  if(syncmem(a,d,16,2)||mr(a,d,s->descriptor,16))return -1;
  if(!(u32(s->descriptor+4)&0x80000000u))return fail(s,13);
  /* Cell identity/buffer extent are software-owned. Returned fields from a
   * previous completed command are opaque; overwrite all16B before publish. */
  if(nwt_tx_desc(s->descriptor,t->command,q->bytes))return fail(s,14);
  s->phase=NCT_WRITING;
  if(mw(a,t->command,s->wire,q->bytes)||syncmem(a,t->command,q->bytes,1)||mw(a,d,s->descriptor,16)||syncmem(a,d,16,1))return -1;
  if(!s->available||s->reserved)return fail(s,15);
  s->available--;s->reserved=1;s->acquired++;
  s->phase=NCT_SUBMITTED;t->submit_attempted=1;
  if(rw(a,0x18025418,q->to))return -1;
  /* Exact selected AXI release after successful publication. This returns
   * only this command's software reservation, NEVER the prior BASIC debit,
   * and is not DDONE/firmware acceptance/ACK. Uncertain callbacks never release. */
  q->published=1;s->reserved=0;s->available++;s->released++;q->credit_released=1;
  while(q->polls<NCT_POLL_LIMIT){
   if(coherent(a)||rr(a,0x1802541c,&v))return -1;
   q->didx=v;q->polls++;
   if(v>=1024||(v!=q->from&&v!=q->to))return fail(s,16);
   if(v==q->to){
    if(syncmem(a,d,16,2)||mr(a,d,s->descriptor,16))return -1;
    for(unsigned k=0;k<4;k++)q->raw[k]=u32(s->descriptor+4*k);
    q->observed++;if(q->raw[1]&0x80000000u)q->tx_done=1;
   }
   if(pump(h,a))return -1;
   if(q->tx_done)break;
   if(wait(h,a))return -1;
  }
  if(!q->tx_done)return fail(s,18);
  if(coherent(a))return -1;
  /* No callback in the commit/gap; next lease must name exact next entry. */
  t->tx_index=q->to;t->next_sequence++;t->completed++;t->submit_attempted=t->tx_done=t->rx_done=0;
  s->cursor++;s->completed++;s->phase=NCT_BETWEEN;
 }
 if(s->service_us&&h&&h->pump&&!s->scan_complete){
  if(s->last>UINT64_MAX-s->service_us)return fail(s,3);
  s->service_deadline=s->last+s->service_us;s->phase=NCT_SERVICE;
  while(!s->scan_complete){
   /* Each completed pass leaves RX ownership coherent. These final checks
    * run under the hard wholebatch bound before a clean partial result. */
   if(coherent(a)||before(a))return -1;
   if(s->last>=s->service_deadline){s->service_timed_out=1;break;}
   if(s->event_limit_reached)break;
   if(pump(h,a)||(!s->scan_complete&&!s->event_limit_reached&&wait(h,a)))return -1;
  }
 }
 s->phase=NCT_DONE;return 0;
}
