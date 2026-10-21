#include "wifi_normal_rx.h"
#include "wifi_async_event.h"
static uint16_t u16(const uint8_t*p){return (uint16_t)p[0]|(uint16_t)p[1]<<8;}
static uint32_t u32(const uint8_t*p){return (uint32_t)u16(p)|(uint32_t)u16(p+2)<<16;}
static void copy(void*d,const void*s,size_t n){uint8_t*a=d;const uint8_t*b=s;while(n--)*a++=*b++;}
static int fail(struct ncr_state*s,unsigned error){if(!s->halt){s->halt=1;s->error=error;}s->active=0;return -1;}
static int same_ring(const struct nwi_ring*a,const struct nwi_ring*b){return a->descriptors==b->descriptors&&a->buffers==b->buffers&&a->base==b->base&&a->count==b->count&&a->ext==b->ext&&a->rx==b->rx;}
static int source(const struct ncr_state*s){
 if(!s||!s->prepared||!s->init||!s->session||s->halt||s->init->halt||s->init->transport.halt||
    s->session->init!=s->init||s->session->halt||s->scan_command>=s->session->count)return 0;
 for(unsigned k=0;k<NCR_QUEUES;k++)if(!same_ring(&s->queue[k].ring,&s->init->rings[k+4])||s->queue[k].next>=s->queue[k].ring.count)return 0;
 return s->queue[1].next==s->init->transport.rx_index;
}
int ncr_prepare(struct ncr_state*s,struct nwi_state*i,struct nct_session*t,unsigned command,uint8_t sequence,uint64_t timeout){
 static const uint32_t base[5]={0x18024500,0x18025500,0x18024510,0x18024520,0x18024530};
 static const unsigned count[5]={1024,16,1024,16,16};
 if(!s||s->prepared||s->entered||!i||!t||!i->initialized||i->halt||i->transport.halt||i->transport.completed!=5||
    i->transport.rx_index>=16||command>=NCT_COMMAND_LIMIT||!timeout||timeout>NCT_SERVICE_MAX_US)return -1;
 for(unsigned k=0;k<5;k++){
  const struct nwi_ring*r=&i->rings[k+4];
  if(r->rx!=1||r->count!=count[k]||r->base!=base[k]||!r->descriptors||!r->buffers||
     r->descriptors<i->claim.base||r->buffers<i->claim.base||
     r->descriptors-i->claim.base>=i->claim.bytes||16u*r->count>i->claim.bytes-(r->descriptors-i->claim.base)||
     r->buffers-i->claim.base>=i->claim.bytes||4096u*r->count>i->claim.bytes-(r->buffers-i->claim.base))return -1;
 }
 s->init=i;s->session=t;s->scan_command=command;s->scan_sequence=sequence;s->scan_timeout=timeout;
 for(unsigned k=0;k<5;k++){s->queue[k].ring=i->rings[k+4];s->queue[k].next=k==1?i->transport.rx_index:0;}
 s->prepared=1;return 0;
}
int ncr_memory_range(const struct ncr_state*s,uint64_t p,uint32_t n,unsigned write){
 if(!source(s)||!s->active||s->current>=5||!n)return 0;
 const struct ncr_queue*q=&s->queue[s->current];
 if(p==q->ring.descriptors+16u*q->next&&n==16)
  return write?s->phase==NCR_DESC_WRITE:s->phase==NCR_DESC_READ;
 return !write&&s->phase==NCR_FRAME_READ&&s->frame_bytes&&s->frame_bytes<=3840&&n==s->frame_bytes&&p==q->ring.buffers+4096u*q->next;
}
int ncr_register_write(const struct ncr_state*s,uint32_t p,uint32_t v){
 if(!source(s)||!s->active||s->current>=5||s->phase!=NCR_PUBLISH)return 0;
 const struct ncr_queue*q=&s->queue[s->current];return p==q->ring.base+8&&v==q->next;
}
/* Pure copied-frame classification. Driver owns all RX buffers; packet data
 * never provides an address or releases a command credit/token. No host data
 * TX token or association exists in this first passive-scan batch. */
static int classify(struct ncr_state*s,uint64_t now){
 const uint8_t*p=s->frame;size_t n=s->frame_bytes;
 if(n<4){s->malformed++;return 0;}
 unsigned type=(u16(p+2)>>11)&31;
 if(type==6){s->msdu_reports++;return 0;} /* no issued host data tokens */
 if(type==11){s->rx_reports++;return 0;} /* no station RSSI state to update */
 if(type!=7){s->dropped++;return 0;}
 unsigned sw=u16(p+2)&0x380f;
 if(sw==0x3801){
  if(s->scan.active){int r=nsb_add(&s->scan,p,n,now);if(r<0)s->malformed++;}
  else s->dropped++;
  return 0;
 }
 if(sw!=0x3800||n<36){s->malformed++;return 0;}
 struct ncr_event h={.bytes=(uint16_t)n,.outer=u16(p+2),.inner=u16(p+26),.eid=p[28],.sequence=p[29]};
 /* Source event dispatch uses the outer SW discriminator and EID, not an
 * equality assertion on the duplicated inner packet-type field. */
 if(h.eid==0x0d){
  struct nsb_done done;h.kind=1;
  if(nsb_decode_done(p+36,n-36,&done)){s->malformed++;}
  else if(!s->scan.active||done.sequence!=s->scan_sequence)s->done_unrelated++;
  else if(s->done_pending)s->done_duplicates++;
  else{copy(s->done_body,p+36,NSB_SCAN_DONE_BYTES);s->done_pending=1;}
 }else if(h.eid==0x27){
  h.kind=2;if(n<48||u16(p+44)>n-48){s->malformed++;}else s->debug++;
 }else if(h.eid==7){
  h.kind=3;uint8_t state;if(nwe_sleepy(p,n,&state)!=1)s->malformed++;else{s->sleepy++;h.status=state;}
  /* Record firmware sleep advice only. Never yield AP/WF ownership. */
 }else if(h.eid==0xf0||(h.eid==0xed&&p[32]==0x23)){
  h.kind=4;
 }else if(h.eid==0xfd){
  /* Source normal path identifies this as unsupported init-command result;
   * no-response commands have no pending success callback to fabricate. */
  h.kind=5;h.status=n>36?p[36]:0xff;
 }else{h.kind=6;s->unknown++;}
 s->events++;if(s->history_count<NCR_HISTORY)s->history[s->history_count++]=h;
 if(h.kind==4)return fail(s,20);
 if(h.kind==5)return fail(s,21);
 return 0;
}
static int one(struct ncr_state*s,const struct ncd_io*o,void*c,unsigned k,uint64_t now){
 const struct nwt_io*b=o->bus;struct ncr_queue*q=&s->queue[k];
 s->current=k;s->phase=NCR_IDLE;
 if(b->reg_read(c,q->ring.base+12,&q->didx))return fail(s,2);
 if(q->didx>=q->ring.count)return fail(s,3);
 if(q->didx==q->next)return 0;
 uint64_t d=q->ring.descriptors+16u*q->next,a=q->ring.buffers+4096u*q->next;
 s->phase=NCR_DESC_READ;
 if(b->sync(c,d,16,2)||b->mem_read(c,d,s->descriptor,16))return fail(s,4);
 for(unsigned j=0;j<4;j++)q->raw[j]=u32(s->descriptor+4*j);
 uint32_t control=q->raw[1];
 if(!(control&0x80000000u)){q->wait_ddone++;s->phase=NCR_IDLE;return 2;}
 /* kalDevPortRead/kalDevReadData + AXI memcpy: address comes from software
 * cell, not descriptor echoes. DDONE, LS0 and bounded SDL0 are relevant. */
 s->frame_bytes=(control>>16)&0x3fff;
 unsigned last=!!(control&0x40000000u),skip=q->segmented||!last;
 q->segmented=!last;
 if(!s->frame_bytes||s->frame_bytes>3840){skip=1;s->malformed++;}
 if(!skip){
  s->phase=NCR_FRAME_READ;
  if(b->sync(c,a,s->frame_bytes,2)||b->mem_read(c,a,s->frame,s->frame_bytes))return fail(s,5);
  if(classify(s,now))return -1;
 }else{q->dropped++;s->dropped++;}
 q->packets++;s->packets++;
 if(nwt_rx_desc(s->descriptor,a,3840))return fail(s,6);
 s->phase=NCR_DESC_WRITE;
 if(b->mem_write(c,d,s->descriptor,16)||b->sync(c,d,16,1))return fail(s,7);
 s->phase=NCR_PUBLISH;
 if(b->reg_write(c,q->ring.base+8,q->next))return fail(s,8);
 q->rearmed++;s->rearmed++;q->next=(q->next+1)%q->ring.count;
 if(k==1)s->init->transport.rx_index=q->next;
 s->phase=NCR_IDLE;return 1;
}
int ncr_pump(void*context,const struct ncd_io*o,void*c,unsigned budget,unsigned*used){
 struct ncr_state*s=context;struct nct_access*a=c;uint64_t now;
 static const unsigned order[5]={0,2,3,4,1}; /* drain data before scan-done */
 if(used)*used=0;
 if(!used||!source(s)||!o||o!=&nct_checked_io||!a||a!=&s->session->access||!budget||budget>16)return -1;
 s->entered=1;s->active=1;
 if(o->now_us(c,&now)||now<s->last)return fail(s,1);
  s->last=now;
 const struct nct_record*request=&s->session->record[s->scan_command];
 if(!s->scan_requested&&request->published){
  if(request->cid!=3||nsb_begin(&s->scan,s->scan_sequence,now,s->scan_timeout))return fail(s,9);
  s->scan_requested=1;
  if(nrd_capture(o,c,s,&s->readback[0],NRD_BASELINE))return fail(s,22);
 }
 while(*used<budget){
  unsigned progress=0,examined=0,pending=0;
  for(unsigned j=0;j<5&&*used<budget;j++){
   if(o->now_us(c,&now)||now<s->last)return fail(s,1);
  s->last=now;
   int r=one(s,o,c,order[j],now);if(r<0)return -1;
   examined++;if(r==1){progress++;(*used)++;}else if(r==2)pending++;
  }
  s->passes++;
  if(!progress&&examined==5){
   if(pending)break;
   /* Scan-done can precede queued management data. Complete only after a
    * subsequent whole-queue drain; keep owned BSS snapshots until then. */
   if(s->scan.active&&now>=s->scan.deadline){nsb_expire(&s->scan,now);s->scan_expired=1;if(!s->readback[1].attempted&&nrd_capture(o,c,s,&s->readback[1],NRD_SCAN_EXPIRED))return fail(s,22);}
   if(s->done_pending&&s->scan.active){
    if(nrd_capture(o,c,s,&s->readback[1],NRD_SCAN_DONE))return fail(s,22);
    if(nsb_complete(&s->scan,s->done_body,NSB_SCAN_DONE_BYTES,now)<0)return fail(s,10);
    s->drain_passes++;s->phase=NCR_IDLE;s->active=0;return 1;
   }
   break;
  }
 }
 if(s->scan.active&&now>=s->scan.deadline){nsb_expire(&s->scan,now);s->scan_expired=1;if(!s->readback[1].attempted&&nrd_capture(o,c,s,&s->readback[1],NRD_SCAN_EXPIRED))return fail(s,22);}
 s->phase=NCR_IDLE;s->active=0;return s->scan.done?1:0;
}
