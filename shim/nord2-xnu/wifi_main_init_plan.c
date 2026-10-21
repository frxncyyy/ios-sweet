#include "wifi_main_init_plan.h"
int nwt_cfg_plan(unsigned dma,unsigned enable,struct nwt_rmw *o){
 if(!o||dma>1||enable>1)return -1;
 o->physical=0x18024208+0x1000*dma;
 if(!enable){o->clear=(1u<<0)|(1u<<2)|(1u<<15)|(1u<<21)|(1u<<27)|(1u<<28);o->set=0;}
 else {
  /* sourcecontrol(enable), then finaltx/rx enable + selectedaddr_extfalse.
   * SourceDMA0 doesnotassignomit_rx_info; preserveit incombinedRMW. */
  o->clear=(3u<<4)|(1u<<26);
  o->set=(3u<<4)|(1u<<6)|(1u<<12)|(1u<<15)|(1u<<21)|(1u<<28)|(1u<<30)|5u;
  if(dma)o->set|=1u<<27;
 }
 return 0;
}
static void emit(struct nwt_plan_op *p,size_t *n,uint32_t a,uint32_t clr,uint32_t set,unsigned rmw){p[*n]=(struct nwt_plan_op){a,clr,set,rmw};(*n)++;}
int nwt_prefetch_plan(struct nwt_plan_op *p,size_t cap,size_t *used){
 /*2CFG+11RX+6TX+6overwriteTX+6TX8..13+4TX16..19+2priority+4reset+AXI=42*/
 if(!p||!used||cap<42)return -1;
 size_t n=0;uint32_t value=4;
 emit(p,&n,0x18024208,1u<<15,0,1);emit(p,&n,0x18025208,1u<<15,0,1);
 for(unsigned i=0;i<8;i++){emit(p,&n,0x18024680+4*i,0,value,0);value+=0x400000;}
 for(unsigned i=0;i<3;i++){emit(p,&n,0x18025680+4*i,0,value,0);value+=0x400000;}
 uint32_t def=value;
 for(unsigned i=0;i<6;i++){emit(p,&n,0x18025600+4*i,0,value,0);value+=0x400000;}
 /* Actual soc3_0ResetTxDataRingPrefetch overwrites all3data pairs with
  * same def/def+400000, does NOT simply continue increasingvalues. */
 for(unsigned i=0;i<3;i++){emit(p,&n,0x18025600+8*i,0,def,0);emit(p,&n,0x18025604+8*i,0,def+0x400000,0);}
 for(unsigned i=8;i<=13;i++){emit(p,&n,0x18025600+4*i,0,value,0);value+=0x400000;}
 for(unsigned i=16;i<=19;i++){emit(p,&n,0x18025600+4*i,0,value,0);value+=0x400000;}
 emit(p,&n,0x18024298,0,0xf0,1);emit(p,&n,0x18025298,0,2,1);
 emit(p,&n,0x1802420c,0,0xffffffffu,0);emit(p,&n,0x1802520c,0,0xffffffffu,0);
 emit(p,&n,0x18024280,0,0xffffffffu,0);emit(p,&n,0x18025280,0,0xffffffffu,0);
 emit(p,&n,0x18027500,0,0x800,1);
 *used=n;return n==42?0:-2;
}
