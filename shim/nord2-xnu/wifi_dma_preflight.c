#include "wifi_dma_preflight.h"
static int rd(const struct nwdp_io *o,void *c,struct nwdp_state *s,uint32_t a,uint32_t *v){
 if(o->owner(c)){s->error=1;return -1;}
 if(o->read32(c,a,v)){s->error=2;return -1;}
 s->reads++;return 0;
}
static void mismatch(struct nwdp_state *s,unsigned why,uint32_t a,uint32_t raw,uint32_t mask){
 s->observed_empty=0;
 if(!s->first_reason){s->first_reason=why;s->first_addr=a;s->first_raw=raw;s->first_mask=mask;}
}
int nwdp_capture(const struct nwdp_io *o,void *c,struct nwdp_state *s,unsigned opt){
 if(!o||!o->owner||!o->read32||!s||opt!=1||s->entered)return -1;
 s->entered=1;s->observed_empty=1;s->stable=1;
 for(unsigned dma=0;dma<2;dma++){
  uint32_t a=0x18024208u+dma*0x1000;
  if(rd(o,c,s,a,&s->cfg[dma]))return -1;
  s->cfg_valid|=1u<<dma;
  if(s->cfg[dma]&15)mismatch(s,1,a,s->cfg[dma],15);
 }
 unsigned n=0;
 for(unsigned dma=0;dma<2;dma++)for(unsigned r=0;r<28;r++){
  if((!dma&&r<20)||(dma&&r>=23)||(dma&&r<20&&((r>=6&&r<8)||(r>=14&&r<16))))continue;
  struct nwdp_ring *t=&s->rings[n++];
  t->address=0x18024000u+dma*0x1000+(r<20?0x300+16*r:0x500+16*(r-20));
  /* CODA base31:0; count11:0 +baseextension19:16; indices11:0.
   * Reservedbits are captured but NEVER requiredzero. */
  const uint32_t masks[4]={0xffffffffu,0x000f0fffu,0xfffu,0xfffu};
  for(unsigned w=0;w<4;w++){
   if(rd(o,c,s,t->address+4*w,&t->raw[w]))return -1;
   t->valid|=1u<<w;
   /* Count alone does not program a descriptor pointer. Current retained
    * ROM boot observes 512 on all27 rings, with base/ext/indices zero and
    * both engines disabled. Accept only zero or this observed count; the
    * independent base/ext/index checks remain mandatory. This is not a
    * claim that selected CODA documents a reset default. */
   uint32_t checked=t->raw[w]&masks[w];
   if(w==1&&(checked&0xfffu)==0x200u)checked&=~0xfffu;
   if(checked)mismatch(s,2,t->address+4*w,t->raw[w],masks[w]);
  }
 }
 if(n!=NWDP_RINGS){s->error=3;return -1;}
 for(unsigned dma=0;dma<2;dma++){
  uint32_t a=0x18024208u+dma*0x1000;
  if(rd(o,c,s,a,&s->cfg_after[dma]))return -1;
  s->cfg_valid|=1u<<(dma+2);
  if(s->cfg_after[dma]&15)mismatch(s,1,a,s->cfg_after[dma],15);
  if((s->cfg[dma]^s->cfg_after[dma])&15){s->stable=0;mismatch(s,3,a,s->cfg_after[dma],15);}
 }
 s->complete=1;return s->observed_empty&&s->stable?0:1;
}
