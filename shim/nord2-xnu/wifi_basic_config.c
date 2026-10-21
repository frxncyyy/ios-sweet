#include "wifi_basic_config.h"
static uint16_t u16(const uint8_t*p){return (uint16_t)p[0]|(uint16_t)p[1]<<8;}
static uint32_t u32(const uint8_t*p){return (uint32_t)u16(p)|(uint32_t)u16(p+2)<<16;}
static int cap_receipt(const struct ncd_state*c,const struct nwi_state*i,const struct nfp_native*f){
 if(!c||!i||!f||c->init!=i||c->firmware!=f||!c->entered||c->phase!=5||c->halt||c->error||
    c->completed!=1||c->matched!=1||c->submit_attempted!=1||c->tx_done!=1||!c->events||c->events>16||
    c->rearmed!=c->events||c->history[c->events-1].kind!=1||c->history[c->events-1].eid!=0xec||
    !c->capability.decoded||!i->initialized||i->halt||i->transport.halt||i->transport.phase!=2||
    i->transport.finish_phase!=2||i->transport.completed!=4||i->transport.tx_index>=1024||
    i->transport.rx_index>=16||!f->ready||f->halt||f->state.halt||f->state.error||
    f->state.completed!=127||!f->finish_done||(f->ready_raw&3)!=3||!f->ready_polls)return -1;
 return 0;
}
static int skip(struct nbc_state*s,unsigned reason){s->skipped=1;s->skip_reason=reason;s->phase=NBC_SKIPPED;return 1;}
int nbc_prepare(struct ncd_state*c,struct nwi_state*i,const struct nfp_native*f,struct nbc_state*s){
 if(!s||s->attempted)return -1;
 s->attempted=1;s->cap=c;s->init=i;s->firmware=f;
 if(cap_receipt(c,i,f)||i->transport.submit_attempted||i->transport.tx_done||i->transport.rx_done)return skip(s,1);
 if(ncp_parse(c->frame,c->frame_bytes,c->history[c->events-1].sequence,&s->verified_cap))return skip(s,2);
 s->metadata_valid=!nbc_caps_decode(c->frame,c->frame_bytes,&s->caps);
 /* Metadata decode/known MAC is not a BASIC_CONFIG requirement. */
 if(nnr_plan(&s->verified_cap,NNI_BASIC_BYTES,&s->plan)||nnr_begin(&s->plan,&s->budget))return skip(s,3);
 s->sequence=i->transport.next_sequence;s->tx_index=i->transport.tx_index;s->rx_index=i->transport.rx_index;
 if(nni_basic(s->command,sizeof(s->command),s->sequence,0,0))return skip(s,4);
 s->prepared=1;return 0;
}
int nbc_lease_window(const struct nbc_state*s,const struct ncd_state*c,const struct nwi_state*i,const struct nfp_native*f){
 if(!s||!s->attempted||!s->prepared||!s->entered||s->skipped||s->halt||s->error||s->completed||
    s->phase<NBC_LEASED||s->phase>NBC_DRAINING||s->cap!=c||s->init!=i||s->firmware!=f||cap_receipt(c,i,f)||
    i->transport.tx_index!=s->tx_index||i->transport.rx_index!=(s->rx_index+s->rearmed)%16||
    i->transport.next_sequence!=s->sequence||s->command[36]!=2||s->command[37]!=0xa0||s->command[38]!=1||
    s->command[39]!=s->sequence||i->transport.submit_attempted!=s->submit_attempted||
    (s->submit_attempted&&!s->budget.reserved))return 0;
 return 1;
}
static int fail(struct nbc_state*s,unsigned error){if(!s->halt){s->error=error;s->halt=1;if(s->init){s->init->halt=1;s->init->transport.halt=1;if(s->mutated)s->init->transport.dirty=1;}}return -1;}
static int before(const struct ncd_io*o,void*c,struct nbc_state*s){uint64_t now;
 if(s->halt)return -1;
 if(!nbc_lease_window(s,s->cap,s->init,s->firmware))return fail(s,1);
 s->callbacks++;if(o->bus->owner(c))return fail(s,1);
 s->callbacks++;if(o->now_us(c,&now))return fail(s,2);
 if(now<s->last||now>=s->deadline)return fail(s,3);
 s->last=now;return 0;
}
static int rr(const struct ncd_io*o,void*c,struct nbc_state*s,uint32_t a,uint32_t*v){if(before(o,c,s))return -1;s->callbacks++;return o->bus->reg_read(c,a,v)?fail(s,4):0;}
static int rw(const struct ncd_io*o,void*c,struct nbc_state*s,uint32_t a,uint32_t v){if(before(o,c,s))return -1;s->mutated=1;if(a==0x18025418)s->publications++;s->callbacks++;return o->bus->reg_write(c,a,v)?fail(s,5):0;}
static int mr(const struct ncd_io*o,void*c,struct nbc_state*s,uint64_t a,void*p,uint32_t n){if(before(o,c,s))return -1;s->callbacks++;return o->bus->mem_read(c,a,p,n)?fail(s,6):0;}
static int mw(const struct ncd_io*o,void*c,struct nbc_state*s,uint64_t a,const void*p,uint32_t n){if(before(o,c,s))return -1;s->mutated=1;s->callbacks++;return o->bus->mem_write(c,a,p,n)?fail(s,7):0;}
static int sy(const struct ncd_io*o,void*c,struct nbc_state*s,uint64_t a,uint32_t n,unsigned w){if(before(o,c,s))return -1;s->callbacks++;return o->bus->sync(c,a,n,w)?fail(s,8):0;}
static int coherent(const struct ncd_io*o,void*c,struct nbc_state*s){s->irq_valid=0;for(unsigned d=0;d<2;d++){if(rr(o,c,s,0x18024200+0x1000*d,&s->irq[d]))return -1;s->irq_valid|=1u<<d;if(s->irq[d]&0x00300000)return fail(s,10);}return 0;}
/* Observe pending events without waiting for an invented command-result ACK.
 * Only bounded existing ASCII debug metadata may be consumed/rearmed. */
static int drain(const struct ncd_io*o,void*c,struct nbc_state*s){struct nwt_state*t=&s->init->transport;uint32_t v;
 for(;;){
  if(coherent(o,c,s)||rr(o,c,s,0x1802550c,&v))return -1;
  if(v>=16)return fail(s,20);
  if(v==t->rx_index)return 0;
  if(s->debug_events>=NBC_RX_BUDGET)return fail(s,21);
  uint64_t d=t->rx_ring+16u*t->rx_index,b=t->rx_buffers+4096u*t->rx_index;
  if(sy(o,c,s,d,16,2)||mr(o,c,s,d,s->descriptor,16))return -1;
  if(nwt_rx_complete(s->descriptor,b,3840,&s->rx_bytes))return fail(s,22);
  if(sy(o,c,s,b,(uint32_t)s->rx_bytes,2)||mr(o,c,s,b,s->rx,(uint32_t)s->rx_bytes))return -1;
  const uint8_t*p=s->rx;size_t n=s->rx_bytes;
  if(n<36||(u16(p+2)&0x380f)!=0x3800)return fail(s,23);
  struct ncd_event*h=&s->last_debug;*h=(struct ncd_event){0};s->debug_index=t->rx_index;
  h->bytes=(uint32_t)n;h->rxd_bytes=u16(p);h->event_bytes=u16(p+24);h->rx_class=u16(p+2);
  h->eid=p[28];h->sequence=p[29];h->inner_type=u16(p+26);h->header_valid=1;h->kind=4;
  for(unsigned k=0;k<36;k++)h->header[k]=p[k];
  if(n>=48){h->debug_id=u16(p+36);h->debug_type=p[38];h->debug_bytes=u16(p+44);}
  if(n<48||p[26]||p[27]!=0xe0||p[28]!=0x27||p[38]!=3||u16(p+44)>1200||u16(p+44)>n-48)return fail(s,23);
  h->kind=2;s->debug_events++;if(coherent(o,c,s)||nwt_rx_desc(s->descriptor,b,3840)||mw(o,c,s,d,s->descriptor,16)||sy(o,c,s,d,16,1)||rw(o,c,s,0x18025508,t->rx_index))return -1;
  s->rearmed++;t->rx_index=(t->rx_index+1)%16;
 }
}
int nbc_run(const struct ncd_io*o,void*c,struct nbc_state*s){uint32_t v;
 if(!o||!o->bus||!o->now_us||!o->bus->owner||!o->bus->reg_read||!o->bus->reg_write||!o->bus->mem_read||!o->bus->mem_write||!o->bus->sync||!o->bus->delay_us||!s||!s->init||!s->cap||!s->firmware||!s->prepared||s->entered)return -1;
 s->entered=1;s->phase=NBC_LEASED;
 if(!nbc_lease_window(s,s->cap,s->init,s->firmware))return fail(s,2);
 s->callbacks++;if(o->now_us(c,&s->started))return fail(s,2);
 if(s->started>UINT64_MAX-NBC_DEADLINE_US)return fail(s,3);
 s->last=s->started;s->deadline=s->started+NBC_DEADLINE_US;
 struct nwt_state*t=&s->init->transport;
 if(rr(o,c,s,0x180600f0,&v)||(v&3)!=3)return fail(s,11);
 if(coherent(o,c,s)||rr(o,c,s,0x18025418,&v)||v!=t->tx_index||rr(o,c,s,0x1802541c,&v)||v!=t->tx_index)return fail(s,12);
 uint64_t d=t->tx_ring+16u*t->tx_index;
 if(sy(o,c,s,d,16,2)||mr(o,c,s,d,s->descriptor,16))return -1;
 if(!(u32(s->descriptor+4)&0x80000000)||u32(s->descriptor+8)||u32(s->descriptor+12)||(u32(s->descriptor)&&u32(s->descriptor)!=(uint32_t)t->command))return fail(s,13);
 if(nwt_tx_desc(s->descriptor,t->command,sizeof(s->command)))return fail(s,14);
 s->phase=NBC_WRITING;
 if(mw(o,c,s,t->command,s->command,sizeof(s->command))||sy(o,c,s,t->command,sizeof(s->command),1)||mw(o,c,s,d,s->descriptor,16)||sy(o,c,s,d,16,1))return -1;
 unsigned next=(t->tx_index+1)%1024;
 if(nnr_reserve_once(&s->budget))return fail(s,15);
 s->phase=NBC_SUBMITTED;s->submit_attempted=1;t->submit_attempted=1;
 if(rw(o,c,s,0x18025418,next))return -1;
 while(s->polls<NBC_POLL_BUDGET){
  if(coherent(o,c,s)||rr(o,c,s,0x1802541c,&v))return -1;
  s->polls++;s->tx_didx=v;
  if(v>=1024||(v!=s->tx_index&&v!=next))return fail(s,16);
  if(v==next){
   if(sy(o,c,s,d,16,2)||mr(o,c,s,d,s->descriptor,16))return -1;
   uint32_t control=u32(s->descriptor+4);
   if(control&0x80000000){
    if(u32(s->descriptor)!=(uint32_t)t->command||u32(s->descriptor+8)||u32(s->descriptor+12)||(control!=0x80000000&&control!=0xc04c0000))return fail(s,17);
    s->tx_done=1;s->tx_control=control;s->tx_consumed=control==0x80000000;
   }
  }
  if(drain(o,c,s))return -1;
  if(s->tx_done)break;
  if(before(o,c,s))return -1;
  s->callbacks++;if(o->bus->delay_us(c,1000))return fail(s,9);
 }
 if(!s->tx_done)return fail(s,18);
 s->phase=NBC_DRAINING;
 if(coherent(o,c,s))return -1;
 /* No backend callback after completion changes4->5. TX-only is not FW ACK. */
 t->tx_index=next;t->next_sequence++;t->completed++;
 t->submit_attempted=t->tx_done=t->rx_done=0;s->completed=1;s->phase=NBC_DONE;
 return 0;
}
