#include "wifi_rx_readback.h"
#include "wifi_normal_rx.h"
static uint32_t word(const uint8_t*p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
int nrd_capture(const struct ncd_io*o,void*c,struct ncr_state*r,struct nrd_snapshot*s,unsigned reason)
{
 static const unsigned queues[NRD_QUEUES]={0,2,3,4};
 unsigned old_phase,old_current;uint8_t raw[16];
 if(!r||!s||s->attempted||o!=&nct_checked_io||!o->bus||!o->now_us||
    !r->prepared||!r->active||r->halt||!r->session||c!=&r->session->access||
    r->phase!=NCR_IDLE||reason<NRD_BASELINE||reason>NRD_SCAN_EXPIRED)return -1;
 s->attempted=1;s->reason=reason;old_phase=r->phase;old_current=r->current;
 /* Preflight existing exact-slot admission before the first callback. */
 for(unsigned j=0;j<NRD_QUEUES;j++){
  const struct ncr_queue*q=&r->queue[queues[j]];r->current=queues[j];r->phase=NCR_DESC_READ;
  if(!ncr_memory_range(r,q->ring.descriptors+16u*q->next,16,0)){s->error=1;goto out;}
 }
 r->phase=old_phase;r->current=old_current;
 if(o->now_us(c,&s->started)){s->error=2;goto out;}s->last=s->started;
 for(unsigned d=0;d<2;d++){
  if(o->bus->reg_read(c,0x18024208u+0x1000u*d,&s->cfg[d])){s->error=3;goto out;}
  s->valid|=1u<<d;
 }
 for(unsigned j=0;j<NRD_QUEUES;j++){
  unsigned k=queues[j];const struct ncr_queue*q=&r->queue[k];struct nrd_queue*v=&s->queue[j];
  v->queue=k;v->next=q->next;v->base=q->ring.base;
  v->expected_descriptors=q->ring.descriptors;v->descriptor=q->ring.descriptors+16u*q->next;
  /* Final physical DMA0 manual-prefetch registers. ring.ext is the selected
   * generic helper's transient DMA1 write address, not this final address. */
  v->ext_address=0x18024680u+4u*j;
  for(unsigned w=0;w<4;w++){
   if(o->bus->reg_read(c,v->base+4u*w,&v->control[w])){s->error=4;goto out;}
   v->valid|=1u<<w;
  }
  if(o->bus->reg_read(c,v->ext_address,&v->ext)){s->error=5;goto out;}v->valid|=1u<<4;
  r->current=k;r->phase=NCR_DESC_READ;
  if(o->bus->sync(c,v->descriptor,16,2)){s->error=6;goto out;}
  if(o->bus->mem_read(c,v->descriptor,raw,16)){s->error=7;goto out;}
  for(unsigned w=0;w<4;w++)v->raw[w]=word(raw+4*w);
  v->valid|=1u<<5;r->phase=old_phase;r->current=old_current;
 }
 if(o->now_us(c,&s->last)){s->error=8;goto out;}
 if(s->last<s->started){s->error=9;goto out;}
 s->complete=1;
out:
 r->phase=old_phase;r->current=old_current;return s->error?-1:0;
}
