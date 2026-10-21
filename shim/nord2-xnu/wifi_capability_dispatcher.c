#include "wifi_capability_dispatcher.h"
#include "wifi_async_event.h"
#define TX_CIDX 0x18025418u
#define TX_DIDX 0x1802541cu
#define RX_CIDX 0x18025508u
#define RX_DIDX 0x1802550cu
static uint16_t u16(const uint8_t*p){return (uint16_t)p[0]|(uint16_t)p[1]<<8;}
static uint32_t u32(const uint8_t*p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static int fail(struct ncd_state*s,unsigned error){
 if(s->halt)return -1;
 s->error=error;s->halt=1;
 if(s->init){s->init->halt=1;s->init->transport.halt=1;
  if(s->mutated)s->init->transport.dirty=1;}
 return -1;
}
static int before(const struct ncd_io*o,void*c,struct ncd_state*s){uint64_t now;
 if(s->halt)return -1;
 if(o->bus->owner(c))return fail(s,1);
 if(o->now_us(c,&now))return fail(s,2);
 if(now<s->last||now>=s->deadline)return fail(s,3);
 s->last=now;return 0;
}
static int rr(const struct ncd_io*o,void*c,struct ncd_state*s,uint32_t a,uint32_t*v){if(before(o,c,s))return -1;return o->bus->reg_read(c,a,v)?fail(s,4):0;}
static int rw(const struct ncd_io*o,void*c,struct ncd_state*s,uint32_t a,uint32_t v){if(before(o,c,s))return -1;s->mutated=1;return o->bus->reg_write(c,a,v)?fail(s,5):0;}
static int mr(const struct ncd_io*o,void*c,struct ncd_state*s,uint64_t a,void*p,uint32_t n){if(before(o,c,s))return -1;return o->bus->mem_read(c,a,p,n)?fail(s,6):0;}
static int mw(const struct ncd_io*o,void*c,struct ncd_state*s,uint64_t a,const void*p,uint32_t n){if(before(o,c,s))return -1;s->mutated=1;return o->bus->mem_write(c,a,p,n)?fail(s,7):0;}
static int syncmem(const struct ncd_io*o,void*c,struct ncd_state*s,uint64_t a,uint32_t n,unsigned way){if(before(o,c,s))return -1;return o->bus->sync(c,a,n,way)?fail(s,8):0;}
static int wait(const struct ncd_io*o,void*c,struct ncd_state*s){if(before(o,c,s))return -1;return o->bus->delay_us(c,1000)?fail(s,9):0;}
static int coherent(const struct ncd_io*o,void*c,struct ncd_state*s){
 s->irq_valid=0;
 for(unsigned d=0;d<2;d++){
  if(rr(o,c,s,0x18024200+0x1000*d,&s->irq_status[d]))return -1;
  s->irq_valid|=1u<<d;if(s->irq_status[d]&0x00300000u)return fail(s,10);
 }
 return 0;
}
static int receipt(struct nwi_state*i,const struct nfp_native*f){
 struct ndp_subpool sub;
 if(!i||!f||ndp_subpool_layout(&i->claim,&sub)||!i->pool||
    i->pool->stage!=NPP_DONE||!i->pool->entered||i->pool->error||
    !i->pool->claim_ready||i->pool->live_valid!=0x1f||!i->pool->preflight_complete||
    !i->pool->verified||!i->pool->mapped||!i->pool->zero_complete||
    !i->pool->balanced||!i->pool->asleep||i->pool->ap_owned||i->pool->wf_owned||
    i->pool->words_written!=32768||i->pool->words_read!=32768||
    i->pool->set_attempted!=3||i->pool->inherited_dma||i->pool->halt||i->pool->uncertain||
    i->pool->set_accepted!=2||i->pool->raw_valid!=3||i->pool->raw[0][0]||i->pool->raw[1][0]||
    !i->prepared||!i->entered||!i->initialized||i->phase!=5||i->halt||
    !i->dirty||!i->driver_owned||!i->scheduler.complete||i->scheduler.halt||i->scheduler.error||i->scheduler.operations!=99||
    !i->transport.entered||!i->transport.dirty||i->transport.phase!=2||i->transport.halt||i->transport.missing_init||
    !i->transport.preflight_complete||i->transport.submit_attempted||
    i->transport.completed!=3||i->transport.finish_phase!=2||
    i->transport.tx_index>=1024||i->transport.rx_index>=16||
    i->transport.tx_ring!=sub.tx_desc||i->transport.rx_ring!=sub.rx_desc||
    i->transport.rx_buffers!=sub.rx_events||i->transport.command!=sub.tx_cmd||
    !f->attempted||!f->prepared||f->halt||f->state.halt||f->state.error||
    !f->state.entered||!f->state.dirty||f->state.stage!=NFP_COPIED||f->state.completed!=0x7f||f->state.next!=7||
    !f->coex_done||!f->finish_attempted||!f->finish_done||!f->ready||
    (f->ready_raw&3)!=3||!f->ready_polls)return -1;
 return 0;
}
static int event_kind(struct ncd_state*s,struct ncd_event*h){
 const uint8_t*p=s->frame;size_t n=s->frame_bytes;
 if(n<36)return fail(s,20);
 h->bytes=(uint32_t)n;h->rxd_bytes=u16(p);h->event_bytes=u16(p+24);
 h->eid=p[28];h->sequence=p[29];h->rx_class=u16(p+2);h->kind=4;
 /* Actual selected RXD software packet discriminator, not the inner event
  * comment: NIC_RX_GET_U2_SW_PKT_TYPE & 0x380f must equal0x3800. */
 if((h->rx_class&0x380fu)!=0x3800u)return fail(s,20);
 /* Capture only the already-owned, copied RXD24 + event12 header. Never
  * include byte36 onward (capability/debug body and possible MAC TLV).
  * The inner type is opaque for exact EIDec capability and bounded EID27
  * ASCII debug and unsolicited sleepy metadata; unsupported EIDs retain strict inner-type fail-stop. */
 h->inner_type=u16(p+26);
 for(unsigned k=0;k<sizeof(h->header);k++)h->header[k]=p[k];
 h->header_valid=1;
 if(h->eid==0xec){
  /* Selected first capability wait matches EID only. Sequence is captured,
   * not invented as an additional readiness/correlation requirement. */
  h->kind=1;s->sequence_echo=h->sequence==s->sequence;
  if(!s->sequence_echo)s->sequence_mismatches++;
  if(ncp_parse(p,n,h->sequence,&s->capability))return fail(s,21);
  return 0;
 }
 /* Selected normal event dispatch is outer/EID based. EID27 reaches only
  * the unchanged bounded ASCII parser; unknown event policy stays below. */
 if(h->eid==0x27){
  /* Selected EVENT_DEBUG_MSG header12; bounded ASCII log only. No payload
   * logging, memory dereference, QA command or resource-state transition. */
  if(n<48||p[38]!=3)return fail(s,22);
  h->debug_id=u16(p+36);h->debug_type=p[38];h->debug_bytes=u16(p+44);
  if(h->debug_bytes>1200||h->debug_bytes>n-48)return fail(s,22);
  h->kind=2;s->debug_events++;return 0;
 }
 if(h->eid==7){
  uint8_t state;
  if(nwe_sleepy(p,n,&state)!=1)return fail(s,22);
  h->kind=3;h->debug_type=state;return 0;
 }
 if(p[26]||p[27]!=0xe0)return fail(s,20);
 /* TXdone/ownership/failure/unknown events need their actual handler.
  * Capture metadata but never silently discard, acknowledge or rearm them. */
 h->kind=4;return fail(s,23);
}
int ncd_run(const struct ncd_io*o,void*c,struct nwi_state*i,
 const struct nfp_native*f,struct ncd_state*s,unsigned opt){
 if(!o||!o->bus||!o->now_us||!o->bus->owner||!o->bus->reg_read||
    !o->bus->reg_write||!o->bus->mem_read||!o->bus->mem_write||
    !o->bus->sync||!o->bus->delay_us||!s||opt!=1||s->entered||receipt(i,f))return -1;
 s->init=i;s->firmware=f;s->entered=1;s->phase=1;s->sequence=i->transport.next_sequence;
 struct nwt_state*t=&i->transport;uint32_t v;
 if(o->bus->owner(c))return fail(s,1);
 if(o->now_us(c,&s->started))return fail(s,2);
 if(s->started>UINT64_MAX-NCD_DEADLINE_US)return fail(s,3);
 s->last=s->started;s->deadline=s->started+NCD_DEADLINE_US;
 if(rr(o,c,s,0x180600f0,&s->sw_sync)||(s->sw_sync&3)!=3)return fail(s,11);
 if(coherent(o,c,s)||rr(o,c,s,0x18025208,&v))return -1;
 if((v&5)!=5||(v&(1u<<26)))return fail(s,12);
 if(rr(o,c,s,TX_CIDX,&v)||v!=t->tx_index)return fail(s,13);
 if(rr(o,c,s,TX_DIDX,&v)||v!=t->tx_index)return fail(s,13);
 uint64_t txd=t->tx_ring+16u*t->tx_index;
 if(syncmem(o,c,s,txd,16,2)||mr(o,c,s,txd,s->descriptor,16))return -1;
 if(!(u32(s->descriptor+4)&0x80000000u)||u32(s->descriptor+8)||u32(s->descriptor+12)||
    (u32(s->descriptor)&&u32(s->descriptor)!=(uint32_t)t->command))return fail(s,14);
 if(ncp_query(s->command,sizeof(s->command),s->sequence)||
    nwt_tx_desc(s->descriptor,t->command,sizeof(s->command)))return fail(s,15);
 s->phase=2;
 if(mw(o,c,s,t->command,s->command,64)||syncmem(o,c,s,t->command,64,1)||
    mw(o,c,s,txd,s->descriptor,16)||syncmem(o,c,s,txd,16,1))return -1;
 unsigned next=(t->tx_index+1)%1024;s->submit_attempted=1;t->submit_attempted=1;
 if(rw(o,c,s,TX_CIDX,next))return -1;
 s->phase=3;
 while(s->polls<NCD_POLL_BUDGET){
  if(coherent(o,c,s)||rr(o,c,s,TX_DIDX,&v))return -1;
  s->polls++;
  if(v>=1024||(v!=t->tx_index&&v!=next))return fail(s,16);
  if(v==next){
   if(syncmem(o,c,s,txd,16,2)||mr(o,c,s,txd,s->descriptor,16))return -1;
   /* Partial writeback is not a completed descriptor. The exact consumed
    * control word is measured on this hardware; retain software length64. */
   uint32_t control=u32(s->descriptor+4);
   if(control&0x80000000u){
    if(u32(s->descriptor)!=(uint32_t)t->command||u32(s->descriptor+8)||u32(s->descriptor+12)||
       (control!=0x80000000u&&
        (control&~0x80000000u)!=((64u<<16)|(1u<<30))))return fail(s,17);
    s->tx_done=1;t->tx_done=1;break;
   }
  }
  if(wait(o,c,s))return -1;
 }
 if(!s->tx_done)return fail(s,18);
 s->phase=4;
 while(s->polls<NCD_POLL_BUDGET&&s->events<NCD_EVENT_BUDGET){
  if(coherent(o,c,s)||rr(o,c,s,RX_DIDX,&v))return -1;
  s->polls++;
  if(v>=16)return fail(s,24);
  if(v==t->rx_index){if(wait(o,c,s))return -1;continue;}
  uint64_t rxd=t->rx_ring+16u*t->rx_index,buffer=t->rx_buffers+4096u*t->rx_index;
  if(syncmem(o,c,s,rxd,16,2)||mr(o,c,s,rxd,s->descriptor,16))return -1;
  if(!(u32(s->descriptor+4)&0x80000000u)){if(wait(o,c,s))return -1;continue;}
  if(nwt_rx_complete(s->descriptor,buffer,3840,&s->frame_bytes)||s->frame_bytes<36)return fail(s,25);
  if(syncmem(o,c,s,buffer,(uint32_t)s->frame_bytes,2)||mr(o,c,s,buffer,s->frame,(uint32_t)s->frame_bytes))return -1;
  struct ncd_event*h=&s->history[s->events++];
  if(event_kind(s,h)||coherent(o,c,s))return -1;
  if(nwt_rx_desc(s->descriptor,buffer,3840)||mw(o,c,s,rxd,s->descriptor,16)||
     syncmem(o,c,s,rxd,16,1)||rw(o,c,s,RX_CIDX,t->rx_index))return -1;
  s->rearmed++;t->rx_index=(t->rx_index+1)%16;
  if(h->kind==1){
   t->tx_index=next;t->next_sequence++;t->completed++;
   t->submit_attempted=t->tx_done=t->rx_done=0;
   t->rx_descriptor_bytes=(uint32_t)s->frame_bytes;t->rx_mac_bytes=h->rxd_bytes;t->rx_init_bytes=h->event_bytes;
   s->matched=1;s->completed=1;s->phase=5;return 0;
  }
 }
 return fail(s,26);
}
