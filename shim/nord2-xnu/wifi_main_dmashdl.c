#include "wifi_main_dmashdl.h"
static void add(struct nwt_plan_op *p,size_t *n,uint32_t a,uint32_t mask,uint32_t v){p[*n]=(struct nwt_plan_op){a,mask,v,1};(*n)++;}
int nwdm_plan(struct nwt_plan_op *p,size_t capacity,size_t *used){
 if(!p||!used||capacity<99)return -1;
 size_t n=0;static const uint8_t q[32]={0,0,0,0,2,2,2,2,0,0,0,0,2,2,2,2,0,0,0,2,2,2,2,2,0,0,0,2,2,2,2,2};
 add(p,&n,0x1802601c,0xfff,1);
 for(unsigned i=0;i<16;i++){
  unsigned en=i==0||i==2||i==4;uint32_t mask=1u<<(16+i);
  add(p,&n,0x18026010,mask,en?0:mask);
  add(p,&n,0x18026020+4*i,0x0fff0000,en?(0x15du<<16):0);
  add(p,&n,0x18026020+4*i,0x00000fff,en?0x10:0);
 }
 for(unsigned i=0;i<32;i++)add(p,&n,0x180260d0+4*(i/8),0xfu<<(4*(i%8)),(uint32_t)q[i]<<(4*(i%8)));
 for(unsigned i=0;i<16;i++)add(p,&n,0x180260b0+4*(i/8),0xfu<<(4*(i%8)),i<<(4*(i%8)));
 add(p,&n,0x1802600c,0x10000,0);
 add(p,&n,0x18026008,0xffffff,0x30015);
 *used=n;return n==99?0:-1;
}
static int fail(struct nwdm_state *s,unsigned e){s->error=e;s->halt=1;return -1;}
int nwdm_apply(const struct nwt_io *o,void *c,struct nwdm_state *s,unsigned opt){
 struct nwt_plan_op plan[99];size_t n=0;uint32_t v;
 if(!o||!o->owner||!o->reg_read||!o->reg_write||!s||opt!=1||s->entered)return -1;
 s->entered=1;
 if(nwdm_plan(plan,99,&n))return fail(s,1);
 const struct nwdp_io ro={o->owner,o->reg_read};int pre=nwdp_capture(&ro,c,&s->snapshot,1);
 if(pre){s->error=pre==1?2:3;s->halt=pre<0;return pre==1?1:-1;}
 for(size_t i=0;i<n;i++){
  if(o->owner(c)||o->reg_read(c,plan[i].physical,&v))return fail(s,4);
  v=(v&~plan[i].clear)|plan[i].set;
  if(o->owner(c))return fail(s,5);
  s->dirty=1;
  if(o->reg_write(c,plan[i].physical,v))return fail(s,6);
  s->operations++;
 }
 s->complete=1;return 0;
}
