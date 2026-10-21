#include "wifi_wfsys_adapter.h"

/* Only these six always-on AP/host predicates are read before AP wake. */
static int guard(struct nwsa *a,unsigned st) {
 static const uint32_t addr[6]={0x10006304u,0x1000616cu,0x10006170u,
  0x1020e504u,0x10001228u,0x10001724u};
 static const uint32_t mask[6]={0x1fu,2u,2u,1u,0x2000u,4u};
 static const uint32_t want[6]={0xdu,2u,2u,1u,0,0};
 uint32_t v;unsigned i;
 if(!a||a->asleep||a->blocked||!a->hw||a->state.stage!=st||
    a->hw->owner(a->ctx)){if(a)a->blocked=1;return -1;}
 for(i=0;i<6;i++){
  if(a->hw->read32(a->ctx,addr[i],&v)||(v&mask[i])!=want[i]){
   a->blocked=1;return -1;
  }
 }
 return 0;
}
static int allowed(unsigned st,uint32_t a,int write) {
 switch(st) {
 case NWS_PREWAKE:return a==0x180601a0u;
 case NWS_AP_ACK:return !write&&a==0x18060184u;
 case NWS_AP_READY:return a==0x180601a4u||(!write&&a==0x18001000u);
 case NWS_WF_ACK:return !write&&a==0x18060184u;
 case NWS_COMMON_READY:return a==0x18000010u||(!write&&(
  a==0x18001000u||a==0x18000000u||a==0x180602ccu||
  a==0x18000014u||a==0x18000008u));
 case NWS_CPU_HELD:return a==0x18003008u;
 case NWS_CLOCKS:return a==0x18000000u||(!write&&a==0x180602ccu);
 case NWS_POWERED:return a==0x18001620u||a==0x18001624u;
 case NWS_PROTECTION:return a==0x1800e06cu||(!write&&a==0x184b0010u);
 case NWS_REVERSE:
  return a==0x1800f004u||a==0x18001620u||a==0x18001624u||
   a==0x18000018u||a==0x18003008u||a==0x18000000u||
   (!write&&a==0x180602ccu)||a==0x18001c14u||
   a==0x180601a4u||a==0x180601a0u;
 default:return 0;
 }
}
static int proof(void *p,unsigned st){return guard(p,st);}
static int read32(void *p,uint32_t addr,uint32_t *v) {
 struct nwsa *a=p;unsigned st=a->state.stage;
 if(!v||!allowed(st,addr,0)||guard(a,st))return -1;
 if(a->hw->read32(a->ctx,addr,v))return -1;
 a->last_addr=addr;a->last_value=*v;a->last_valid=1;return 0;
}
static int write32(void *p,uint32_t addr,uint32_t v) {
 struct nwsa *a=p;unsigned st=a->state.stage;
 if(!allowed(st,addr,1)||guard(a,st))return -1;
 if(addr==0x180601a0u){
  if(!((st==NWS_PREWAKE&&v==1)||
       (st==NWS_REVERSE&&v==0&&!a->state.wf_vote&&!a->state.power&&
        !a->state.clock_vote&&!a->state.sem20)))return -1;
 }else if(addr==0x180601a4u){
  if(!a->last_valid||a->last_addr!=addr)return -1;
  if(!((st==NWS_AP_READY&&v==(a->last_value|1u))||
       (st==NWS_REVERSE&&v==(a->last_value&~1u)&&
        !a->state.power&&!a->state.clock_vote&&!a->state.sem20)))return -1;
 }else if(addr==0x18000010u){
  if(!a->last_valid||a->last_addr!=addr||v!=(a->last_value&~1u))return -1;
 }else if(addr==0x18000018u){
  if(!a->last_valid||a->last_addr!=addr||v!=(a->last_value&~1u))return -1;
 }else if(addr==0x18003008u){
  if(!a->sem20owned||!a->last_valid||a->last_addr!=addr)return -1;
  if(st==NWS_CPU_HELD){
   if(a->clock_step==0&&v!=(a->last_value|(1u<<21)))return -1;
   if(a->clock_step==1&&v!=(a->last_value|(1u<<20)))return -1;
   if(a->clock_step>1)return -1;
  }else{
   if(a->clock_step==2&&v!=(a->last_value&~(1u<<20)))return -1;
   if(a->clock_step==3&&v!=(a->last_value&~(1u<<21)))return -1;
   if(a->clock_step<2||a->clock_step>3)return -1;
  }
 }else if(addr==0x18001620u||addr==0x18001624u){
  if(!a->last_valid||a->last_addr!=addr||
     v!=(st==NWS_POWERED?(a->last_value&~1u):(a->last_value|1u)))return -1;
 }else if(addr==0x1800f004u){
  if(!a->last_valid||a->last_addr!=addr||
     v!=(a->state.power?(a->last_value|1u):(a->last_value&~1u)))return -1;
 }else if(addr==0x1800e06cu){
  if(!a->last_valid||a->last_addr!=addr||v!=(a->last_value|1u))return -1;
 }else if(addr==0x18001c14u){
  if(!a->emi_pending){
   if(!a->last_valid||a->last_addr!=addr||v!=(a->last_value|1u))return -1;
  }else if(v!=(a->emi_original&~1u))return -1;
 }
 else if(addr==0x18000000u){
  if(!a->last_valid||a->last_addr!=addr||
    v!=((a->last_value&0x0000ff7fu)|
      (st==NWS_CLOCKS?0x57460080u:0x57460000u)))return -1;
 }
 if(a->hw->write32(a->ctx,addr,v))return -1;
 if(addr==0x18003008u)a->clock_step++;
 if(addr==0x18001c14u){
  if(!a->emi_pending){a->emi_original=a->last_value;a->emi_pending=1;}
  else a->emi_pending=0;
 }
 a->last_valid=0;
 if(st==NWS_REVERSE&&addr==0x180601a0u&&v==0)a->asleep=1;
 return 0;
}
static int delay_us(void *p,unsigned us) {
 struct nwsa *a=p;if(us!=1&&us!=30&&us!=50&&us!=500&&us!=1000)return -1;
 if(guard(a,a->state.stage))return -1;
 return a->hw->delay_us(a->ctx,us);
}
/* Direct selected readability: AP guards above, clock detector with bounded
 * deterministic 1/10us waits, IRQ and HOST ack. No broad debug MMIO. */
static int direct_read(struct nwsa *a,unsigned st,uint32_t addr,uint32_t *v) {
 return guard(a,st)||a->hw->read32(a->ctx,addr,v);
}
static int direct_write(struct nwsa *a,unsigned st,uint32_t addr,uint32_t v) {
 return guard(a,st)||a->hw->write32(a->ctx,addr,v);
}
static int direct_delay(struct nwsa *a,unsigned st,unsigned us) {
 return guard(a,st)||a->hw->delay_us(a->ctx,us);
}
static int readable(void *p) {
 struct nwsa *a=p;unsigned st=a->state.stage,spent=0,i,d=1;
 uint32_t v=0;
 if(st!=NWS_AP_ACK&&st!=NWS_WF_ACK&&st!=NWS_POWERED&&
    st!=NWS_CPU_HELD&&st!=NWS_REVERSE)return -1;
 for(i=0;i<1600&&spent<16000;i++){
  if(direct_read(a,st,0x18060000u,&v))return -1;
  if(direct_write(a,st,0x18060000u,v|1u))return -1;
  if(direct_delay(a,st,d))return -1;
  spent+=d;
  if(direct_read(a,st,0x18060000u,&v))return -1;
  a->clock_last=v;a->clock_polls++;
  if((v&6u)==6u)break;
  d=10;
 }
 a->clock_spent+=spent;
 if((v&6u)!=6u||i==1600||spent>=16000)return -1;
 if(direct_read(a,st,0x180602d4u,&v))return -1;
 a->irq_last=v;if(!(v&1u))return -1;
 if(direct_read(a,st,0x18060184u,&v))return -1;
 return (v&(1u<<5))?-1:0;
}
/* Source M2 semaphore20: base18070000 + M2_OWN_STA2000/REL2200 +20*4. */
static int sem_try(void *p) {
 struct nwsa *a=p;unsigned st=a->state.stage;uint32_t v;
 if((st!=NWS_CPU_HELD&&st!=NWS_REVERSE)||a->sem20owned||guard(a,st))return -1;
 a->sem20pending=1;
 if(a->hw->read32(a->ctx,0x18072050u,&v))return -1;
 if(v&1u){a->sem20owned=1;a->sem20pending=0;return 1;}
 a->sem20pending=0;return 0;
}
static int sem_release(void *p) {
 struct nwsa *a=p;unsigned st=a->state.stage;
 if((st!=NWS_CPU_HELD&&st!=NWS_REVERSE)||!a->sem20owned||guard(a,st))return -1;
 a->sem20pending=1;
 if(a->hw->write32(a->ctx,0x18072250u,1))return -1;
 a->sem20pending=0;a->sem20owned=0;return 0;
}
int nwsa_start(struct nwsa *a,const struct nwsa_hw *h,void *ctx,unsigned optin) {
 static const struct nws_ops ops={proof,read32,write32,delay_us,readable,sem_try,sem_release};
 if(!a||!optin||optin!=1||a->state.entered)return -1;
 if(!h||!h->owner||!h->read32||!h->write32||!h->delay_us)return -1;
 a->hw=h;a->ctx=ctx;return nws_run(&ops,a,1,&a->state);
}
