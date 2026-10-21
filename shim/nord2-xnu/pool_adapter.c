#include "pool_adapter.h"
#define AP 0x180601a0u
#define WF 0x180601a4u
#define ACK 0x18060184u
#define CLOCK 0x18060000u
#define IRQ 0x180602d4u

/* Same six always-on guards and clock/readability bound as accepted ROM
 * adapter. Never touch a sleeping WFSYS register before fresh AP wake. */
static int guard(struct nppa *a,unsigned st){
 static const uint32_t addr[6]={0x10006304u,0x1000616cu,0x10006170u,
  0x1020e504u,0x10001228u,0x10001724u};
 static const uint32_t mask[6]={0x1fu,2u,2u,1u,0x2000u,4u};
 static const uint32_t want[6]={0xdu,2u,2u,1u,0,0};
 uint32_t v;
 if(!a||a->asleep||a->blocked||!a->hw||a->state.stage!=st||a->hw->owner(a->ctx)){
  if(a)a->blocked=1;
  return -1;
 }
 /* Device-word loops are bounded to 128 KiB; their native software owner
  * remains checked per word. AP guard registers are rechecked at release. */
 if(st==NPP_ZERO||st==NPP_VERIFY)return 0;
 for(unsigned i=0;i<6;i++)if(a->hw->read32(a->ctx,addr[i],&v)||(v&mask[i])!=want[i]){
  a->blocked=1;return -1;
 }
 return 0;
}
static int allowed(unsigned st,uint32_t addr,int write){
 switch(st){
 case NPP_PREWAKE:return addr==AP;
 case NPP_AP_ACK:case NPP_WF_ACK:return !write&&addr==ACK;
 case NPP_AP_READY:return addr==WF||(!write&&addr==0x18001000u);
 case NPP_LIVE:return !write&&(addr==0x18001000u||addr==0x18000010u||
  addr==0x18000000u||addr==0x180602ccu||addr==0x18001620u||
  addr==0x18001624u||addr==0x184b0010u||addr==0x184c1604u);
 case NPP_RELEASE:return addr==WF||(write&&addr==AP);
 default:return 0;
 }
}
static int proof(void *p,unsigned st){return guard(p,st);}
static int ready(void *p,const struct ndp_claim *claim){
 struct nppa *a=p;struct ndp_smc_request req[2];
 return !a||!claim||ndp_region18_set_intent(claim,req)||
  !a->hw||a->hw->owner(a->ctx);
}
static int rd(void *p,uint32_t addr,uint32_t *v){
 struct nppa *a=p;unsigned st=a->state.stage;
 if(!v||!allowed(st,addr,0)||guard(a,st)||a->hw->read32(a->ctx,addr,v))return -1;
 a->last_addr=addr;a->last_value=*v;a->last_valid=1;return 0;
}
static int wr(void *p,uint32_t addr,uint32_t v){
 struct nppa *a=p;unsigned st=a->state.stage;
 if(!allowed(st,addr,1)||guard(a,st))return -1;
 if(addr==AP){
  if(!((st==NPP_PREWAKE&&v==1&&a->last_valid&&a->last_addr==AP&&a->last_value==0)||
       (st==NPP_RELEASE&&v==0&&!a->state.wf_owned)))return -1;
 }else if(addr==WF){
  if(!a->last_valid||a->last_addr!=WF||
   !((st==NPP_AP_READY&&v==(a->last_value|1u)&&!(a->last_value&1u))||
     (st==NPP_RELEASE&&v==(a->last_value&~1u)&&a->state.wf_owned)))return -1;
 }else return -1;
 if(a->hw->write32(a->ctx,addr,v))return -1;
 a->last_valid=0;
 if(addr==AP&&st==NPP_RELEASE)a->asleep=1;
 return 0;
}
static int delay_us(void *p,unsigned us){struct nppa *a=p;
 if(us!=1&&us!=10&&us!=500&&us!=1000)return -1;
 return guard(a,a->state.stage)||a->hw->delay_us(a->ctx,us);
}
static int direct_read(struct nppa *a,unsigned st,uint32_t addr,uint32_t *v){return guard(a,st)||a->hw->read32(a->ctx,addr,v);}
static int direct_write(struct nppa *a,unsigned st,uint32_t addr,uint32_t v){return guard(a,st)||a->hw->write32(a->ctx,addr,v);}
static int direct_delay(struct nppa *a,unsigned st,unsigned us){return guard(a,st)||a->hw->delay_us(a->ctx,us);}
static int readable(void *p){
 struct nppa *a=p;unsigned st=a->state.stage,spent=0,d=1;uint32_t v=0;
 if(st!=NPP_AP_ACK&&st!=NPP_WF_ACK)return -1;
 for(unsigned i=0;i<1600&&spent<16000;i++){
  if(direct_read(a,st,CLOCK,&v)||direct_write(a,st,CLOCK,v|1u)||
     direct_delay(a,st,d))return -1;
  spent+=d;if(direct_read(a,st,CLOCK,&v))return -1;
  a->clock_last=v;a->clock_polls++;if((v&6u)==6u)break;d=10;
 }
 a->clock_spent+=spent;if((v&6u)!=6u||spent>=16000)return -1;
 if(direct_read(a,st,IRQ,&v))return -1;
 a->irq_last=v;
 if(!(v&1u))return -1;
 if(direct_read(a,st,ACK,&v))return -1;
 return (v&(1u<<5))?-1:0;
}
static int dma_owner(void *p){struct nppa *a=p;return guard(a,NPP_PREFLIGHT);}
static int dma_addr(uint32_t addr){
 for(unsigned dma=0;dma<2;dma++){
  uint32_t base=0x18024000u+dma*0x1000u;
  if(addr==base+0x208u)return 1;
  for(unsigned r=0;r<28;r++){
   if((!dma&&r<20)||(dma&&r>=23)||
      (dma&&r<20&&((r>=6&&r<8)||(r>=14&&r<16))))continue;
   uint32_t ring=base+(r<20?0x300u+16u*r:0x500u+16u*(r-20u));
   if(addr==ring||addr==ring+4u||addr==ring+8u||addr==ring+12u)return 1;
  }
 }
 return 0;
}
static int dma_read(void *p,uint32_t addr,uint32_t *v){
 struct nppa *a=p;
 if(a->state.stage!=NPP_PREFLIGHT||!v||!dma_addr(addr)||
    guard(a,NPP_PREFLIGHT))return -1;
 return a->hw->read32(a->ctx,addr,v);
}
static int preflight(void *p,struct nwdp_state *out){
 static const struct nwdp_io io={dma_owner,dma_read};
 struct nppa *a=p;if(a->state.stage!=NPP_PREFLIGHT||guard(a,NPP_PREFLIGHT))return -1;
 return nwdp_capture(&io,a,out,1);
}
static int smc(void *p,const uint64_t x[8],uint64_t out[4]){
 struct nppa *a=p;if((a->state.stage!=NPP_SET1&&a->state.stage!=NPP_SET0)||
  guard(a,a->state.stage))return -1;
 return a->hw->smc(a->ctx,x,out);
}
static int map(void *p,uint64_t base,uint64_t size){
 struct nppa *a=p;if(a->state.stage!=NPP_MAP||guard(a,NPP_MAP))return -1;
 return a->hw->map(a->ctx,base,size);
}
static int word(void *p,uint32_t off,uint32_t *v,unsigned write){
 struct nppa *a=p;if((a->state.stage!=(write?NPP_ZERO:NPP_VERIFY))||
  !v||off>=NDP_SUBPOOL_BYTES||(off&3u)||guard(a,a->state.stage))return -1;
 return a->hw->word(a->ctx,off,v,write);
}
static int barrier(void *p){struct nppa *a=p;
 return guard(a,a->state.stage)||a->hw->barrier(a->ctx);
}
int nppa_start(struct nppa *a,const struct nppa_hw *h,void *c,
 const struct ndp_claim *claim,unsigned opt){
 struct npp_ops x={0};
 if(!a||!h||!claim||opt!=1||a->state.entered||!h->owner||!h->read32||
   !h->write32||!h->delay_us||!h->smc||!h->map||!h->word||!h->barrier)return -1;
 a->hw=h;a->ctx=c;
 /* Native owner/claim is checked before the first hardware read. */
 if(h->owner(c))return -1;
 x.ready=ready;
 x.proof=proof;x.read=rd;x.write=wr;x.delay_us=delay_us;x.readable=readable;
 x.preflight=preflight;x.smc=smc;x.map=map;x.word=word;x.barrier=barrier;
 return npp_run(&x,a,claim,&a->state,opt);
}
