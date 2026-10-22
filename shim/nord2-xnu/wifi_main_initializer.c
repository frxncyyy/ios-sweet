#include "wifi_main_initializer.h"
#define LPCTL 0x18060010u
static int fail(struct nwi_state*s,unsigned e){s->error=e;s->halt=1;return -1;}
static int owner(const struct nwt_io*o,void*c,struct nwi_state*s){uint32_t v;
 if(o->owner(c))return fail(s,1);
 if(s->driver_owned&&(o->reg_read(c,LPCTL,&v)||(v&4)))return fail(s,2);
 return 0;
}
static int rr(const struct nwt_io*o,void*c,struct nwi_state*s,uint32_t a,uint32_t*v){return owner(o,c,s)||o->reg_read(c,a,v)?fail(s,3):0;}
static int rw(const struct nwt_io*o,void*c,struct nwi_state*s,uint32_t a,uint32_t v){if(owner(o,c,s))return -1;s->dirty=1;if(o->reg_write(c,a,v))return fail(s,4);s->reg_writes++;return 0;}
static int rmw(const struct nwt_io*o,void*c,struct nwi_state*s,uint32_t a,uint32_t clr,uint32_t set){uint32_t v;if(rr(o,c,s,a,&v))return -1;return rw(o,c,s,a,(v&~clr)|set);}
static int delay(const struct nwt_io*o,void*c,struct nwi_state*s){return owner(o,c,s)||o->delay_us(c,1000)?fail(s,5):0;}
static int mw(const struct nwt_io*o,void*c,struct nwi_state*s,uint64_t a,const void*p,uint32_t n){
 if(!n||n>4096||a<s->claim.base||a-s->claim.base>s->claim.bytes||n>s->claim.bytes-(a-s->claim.base)||owner(o,c,s))return fail(s,6);
 s->dirty=1;if(o->mem_write(c,a,p,n)||owner(o,c,s)||o->sync(c,a,n,1))return fail(s,7);s->memory_writes++;return 0;
}
static void put(uint8_t*p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(v>>(8*i));}
int nwi_prepare(struct nwi_state*s,const struct ndp_claim*c,const struct npp_state*p){
 struct ndp_subpool sub;if(!s||s->prepared||s->entered||!p||ndp_subpool_layout(c,&sub)||
 p->error||!p->claim_ready||p->live_valid!=0x1f||!p->preflight_complete||p->stage!=NPP_DONE||!p->entered||!p->verified||!p->zero_complete||p->halt||p->uncertain||
 !p->balanced||!p->asleep||p->ap_owned||p->wf_owned||p->inherited_dma||
 p->set_attempted!=3||p->set_accepted!=2||p->raw_valid!=3||p->raw[0][0]||p->raw[1][0]||
 p->words_written!=32768||p->words_read!=32768||!p->mapped)return -1;
 s->claim=*c;s->pool=p;s->command=sub.tx_cmd;uint64_t pos=c->base+0x20000;
 const uint32_t txhw[4]={0,2,17,16};
 for(unsigned i=0;i<4;i++){
  s->rings[i]=(struct nwi_ring){.descriptors=i==2?sub.tx_desc:pos,.base=0x18025300+16*txhw[i],.count=1024,.ext=0x18025600+4*txhw[i]};
  if(i!=2)pos+=0x4000;
 }
 const unsigned rxhw[5]={0,0,1,2,3},rxdma[5]={0,1,0,0,0},count[5]={1024,16,1024,16,16};
 for(unsigned i=0;i<5;i++){
  /* Selected generic ext helper uses DMA1 ext base when wfdma1 supported,
   * even for data rings; preserve that transient source write. Manual
   * prefetch subsequently programs both DMA blocks. */
  s->rings[4+i]=(struct nwi_ring){.descriptors=i==1?sub.rx_desc:pos,.base=0x18024500+0x1000*rxdma[i]+16*rxhw[i],.count=count[i],.ext=0x18025680+4*rxhw[i],.rx=1};
  if(i!=1)pos+=16*count[i];
 }
 pos=(pos+4095)&~UINT64_C(4095);
 for(unsigned i=4;i<9;i++){s->rings[i].buffers=i==5?sub.rx_events:pos;if(i!=5)pos+=4096u*s->rings[i].count;}
 s->data_buffer=pos;pos+=4096;
 if(pos>c->base+c->bytes||pos>UINT32_MAX)return -1;
 s->end=pos;s->prepared=1;return 0;
}
static int idle(const struct nwt_io*o,void*c,struct nwi_state*s,unsigned dma){uint32_t v;
 for(unsigned i=0;i<102;i++){if(i&&delay(o,c,s))return -1;if(rr(o,c,s,0x18024208+dma*0x1000,&v))return -1;s->idle_polls++;if(!(v&10))return 0;}return fail(s,8);
}
struct scheduler_context {const struct nwt_io *io;void *context;struct nwi_state *state;};
static int scheduler_owner(void *v){struct scheduler_context *c=v;return owner(c->io,c->context,c->state);}
static int scheduler_read(void *v,uint32_t a,uint32_t *p){struct scheduler_context *c=v;return c->io->reg_read(c->context,a,p);}
static int scheduler_write(void *v,uint32_t a,uint32_t p){struct scheduler_context *c=v;return c->io->reg_write(c->context,a,p);}
int nwi_initialize(const struct nwt_io*o,void*c,struct nwi_state*s,unsigned opt){
 if(!o||!o->owner||!o->reg_read||!o->reg_write||!o->mem_write||!o->sync||!o->delay_us||!s||opt!=1||!s->prepared||s->entered)return -1;
 s->entered=1;s->phase=1;const struct nwdp_io ro={o->owner,o->reg_read};int pre=nwdp_capture(&ro,c,&s->preflight,1);
 if(pre){if(pre<0)return fail(s,9);s->error=10;return 1;}
 /* Polling derivative: no peripheral interrupt output is enabled. Refuse
  * inherited defined interrupt enables before any mutation. Reserved bits
  * are captured but ignored, using the selected CODA field masks. */
 const uint32_t irq_defined[2]={0x33f800ffu,0x7d37ffffu};
 for(unsigned dma=0;dma<2;dma++){
  if(rr(o,c,s,0x18024204+dma*0x1000,&s->irq_initial[dma]))return -1;
  if(s->irq_initial[dma]&irq_defined[dma]){s->error=16;return 1;}
  if(rr(o,c,s,0x18024200+dma*0x1000,&s->irq_status_initial[dma]))return -1;
  s->irq_status_initial_valid|=1u<<dma;
  if(s->irq_status_initial[dma]&0x00300000u){s->error=18;return 1;}
 }
 uint32_t v;if(rr(o,c,s,LPCTL,&v))return -1;
 if(v&4){s->driver_requested=1;if(rw(o,c,s,LPCTL,2))return -1;
  for(unsigned i=0;i<1000;i++){if(i&&delay(o,c,s))return -1;if(rr(o,c,s,LPCTL,&v))return -1;if(!(v&4))break;if(i==999)return fail(s,11);}
 }
 s->driver_owned=1;s->phase=2;
 /* Source halHifSwInfoInit: DMASHDL before ring allocation/init. */
 struct scheduler_context sc={o,c,s};
 const struct nwt_io scheduler_io={.owner=scheduler_owner,.reg_read=scheduler_read,.reg_write=scheduler_write};
 int d=nwdm_apply(&scheduler_io,&sc,&s->scheduler,1);if(d){if(s->scheduler.dirty)s->dirty=1;return fail(s,12);}s->dirty=1;
 uint8_t buffer[4096]={0};
 for(unsigned r=0;r<9;r++){
  struct nwi_ring*t=&s->rings[r];
  if(t->rx)for(unsigned i=0;i<t->count;i++)if(mw(o,c,s,t->buffers+4096u*i,buffer,3840))return -1;
  for(unsigned first=0;first<t->count;first+=256){unsigned n=t->count-first;if(n>256)n=256;
   for(unsigned j=0;j<n;j++){uint8_t*desc=buffer+16*j;for(unsigned k=0;k<16;k++)desc[k]=0;
    if(t->rx){if(nwt_rx_desc(desc,t->buffers+4096u*(first+j),3840))return fail(s,13);}else put(desc+4,1u<<31);
   }
   if(mw(o,c,s,t->descriptors+16u*first,buffer,16u*n))return -1;
  }
  for(unsigned j=0;j<4096;j++)buffer[j]=0;
 }
 s->phase=3;
 for(unsigned dma=0;dma<2;dma++){
  struct nwt_rmw cfg;if(nwt_cfg_plan(dma,0,&cfg)||rmw(o,c,s,cfg.physical,cfg.clear,cfg.set)||idle(o,c,s,dma)||rw(o,c,s,0x1802420c+dma*0x1000,0xffffffffu)||rw(o,c,s,0x18024280+dma*0x1000,0xffffffffu))return -1;
 }
 /* Selected pdmaSetup(FALSE) clears only its source interrupt masks. */
 const uint32_t irq[2]={0xfu,0x2c200151u};
 for(unsigned dma=0;dma<2;dma++)if(rw(o,c,s,0x1802422c+dma*0x1000,irq[dma]))return -1;
 for(unsigned r=0;r<9;r++){struct nwi_ring*t=&s->rings[r];
  if(rw(o,c,s,t->base,(uint32_t)t->descriptors)||rw(o,c,s,t->base+8,t->rx?t->count-1:0)||rw(o,c,s,t->base+4,t->count)||rw(o,c,s,t->ext,4))return -1;
 }
 struct nwt_plan_op plan[42];size_t n;if(nwt_prefetch_plan(plan,42,&n))return fail(s,14);
 for(size_t i=0;i<n;i++)if(plan[i].rmw?rmw(o,c,s,plan[i].physical,plan[i].clear,plan[i].set):rw(o,c,s,plan[i].physical,plan[i].set))return -1;
 s->phase=4;
 for(unsigned dma=0;dma<2;dma++){
  /* Source control(enable) sets flags before interruptSET and final DMAenable. */
  struct nwt_rmw cfg;if(nwt_cfg_plan(dma,1,&cfg))return fail(s,15);
  if(rmw(o,c,s,cfg.physical,cfg.clear&~(1u<<26),cfg.set&~5u))return -1;
 }
 /* Deliberately omit Linux interrupt SET: XNU has no handler for this
  * polling-only transaction. Require defined peripheral enables remain off. */
 for(unsigned dma=0;dma<2;dma++){
  if(rr(o,c,s,0x18024204+dma*0x1000,&s->irq_final[dma]))return -1;
  if(s->irq_final[dma]&irq_defined[dma])return fail(s,17);
 }
 for(unsigned dma=0;dma<2;dma++)if(rmw(o,c,s,0x18024208+dma*0x1000,1u<<26,5))return -1;
 /* Selected bus setDummyReg is NULL; handshake hook sets MCU DMA0 dummy bit1. */
 if(rmw(o,c,s,0x18402120,0,2))return -1;
 for(unsigned dma=0;dma<2;dma++){
  if(rr(o,c,s,0x18024200+dma*0x1000,&s->irq_status_final[dma]))return -1;
  s->irq_status_final_valid|=1u<<dma;
  if(s->irq_status_final[dma]&0x00300000u)return fail(s,19);
 }
 s->transport=(struct nwt_state){.entered=1,.preflight_complete=1,.phase=2,.tx_ring=s->rings[2].descriptors,.rx_ring=s->rings[5].descriptors,.rx_buffers=s->rings[5].buffers,.command=s->command,.next_sequence=1};
 s->initialized=1;s->phase=5;return 0;
}
