#include "wifi_rom_powered_adapter.h"

/* Only these six always-on AP/host predicates are read before AP wake. */
static int guard(struct nrpa *a,unsigned st) {
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
 case NRPC_PREWAKE:return a==0x180601a0u;
 case NRPC_AP_ACK:return !write&&a==0x18060184u;
 case NRPC_AP_READY:return a==0x180601a4u||(!write&&a==0x18001000u);
 case NRPC_WF_ACK:return !write&&a==0x18060184u;
 case NRPC_COMMON_READY:return a==0x18000010u||(!write&&(
  a==0x18001000u||a==0x18000000u||a==0x180602ccu||
  a==0x18000014u||a==0x18000008u));
 case NRPC_CPU_HELD:return a==0x18003008u;
 case NRPC_CLOCKS:return a==0x18000000u||(!write&&a==0x180602ccu);
 case NRPC_POWERED:return a==0x18001620u||a==0x18001624u;
 case NRPC_PROTECTION:return a==0x1800e06cu||(!write&&a==0x184b0010u);
 case NRPC_ROM_CONFIG:return (write&&(a==0x18001120u||
  a==0x185001dcu||a==0x185001e0u))||a==0x18000018u;
 case NRPC_CPU_RELEASE:return a==0x18000010u;
 case NRPC_ROM_WAIT:return !write&&a==0x184c1604u;
 case NRPC_RUNNING_CONFIG:return write&&a==0x1800f000u;
 case NRPC_RELEASE:return a==0x18003008u||a==0x180601a4u||
  (write&&a==0x180601a0u);
 default:return 0;
 }
}
static int receipt_ok(const struct nrpa *a) {
 return a->state.rom_placed&&nrpc_receipt_ok(a->rom);
}
static int proof(void *p,unsigned st){return guard(p,st);}
static int read32(void *p,uint32_t addr,uint32_t *v) {
 struct nrpa *a=p;unsigned st=a->state.stage;
 if(!v||!allowed(st,addr,0)||guard(a,st))return -1;
 if(a->hw->read32(a->ctx,addr,v))return -1;
 a->last_addr=addr;a->last_value=*v;a->last_valid=1;return 0;
}
static int write32(void *p,uint32_t addr,uint32_t v) {
 struct nrpa *a=p;unsigned st=a->state.stage;
 if(!allowed(st,addr,1)||guard(a,st))return -1;
 if(addr==0x180601a0u){
  if(!((st==NRPC_PREWAKE&&v==1)||
       (st==NRPC_RELEASE&&v==0&&!a->state.wf_vote&&
        !a->state.clock_vote&&!a->state.sem20&&a->state.rom_ready)))return -1;
 }else if(addr==0x180601a4u){
  if(!a->last_valid||a->last_addr!=addr)return -1;
  if(!((st==NRPC_AP_READY&&v==(a->last_value|1u))||
       (st==NRPC_RELEASE&&v==(a->last_value&~1u)&&
        !a->state.clock_vote&&!a->state.sem20&&a->state.rom_ready)))return -1;
 }else if(addr==0x18000010u){
  if(!a->last_valid||a->last_addr!=addr||
     v!=(st==NRPC_CPU_RELEASE?(a->last_value|1u):(a->last_value&~1u)))return -1;
  if(st==NRPC_CPU_RELEASE&&(!receipt_ok(a)||!a->state.cpu_held))return -1;
 }else if(addr==0x18000018u){
  if(!a->last_valid||a->last_addr!=addr||v!=(a->last_value&~1u))return -1;
 }else if(addr==0x18003008u){
  if(!a->sem20owned||!a->last_valid||a->last_addr!=addr)return -1;
  if(st==NRPC_CPU_HELD){
   if(a->clock_step==0&&v!=(a->last_value|(1u<<21)))return -1;
   if(a->clock_step==1&&v!=(a->last_value|(1u<<20)))return -1;
   if(a->clock_step>1)return -1;
  }else if(st==NRPC_RELEASE){
   if(a->clock_step==2&&v!=(a->last_value&~(1u<<20)))return -1;
   if(a->clock_step==3&&v!=(a->last_value&~(1u<<21)))return -1;
   if(a->clock_step<2||a->clock_step>3)return -1;
  }else return -1;
 }else if(addr==0x18001620u||addr==0x18001624u){
  if(st!=NRPC_POWERED||!a->last_valid||a->last_addr!=addr||
     v!=(a->last_value&~1u))return -1;
 }else if(addr==0x1800e06cu){
  if(!a->last_valid||a->last_addr!=addr||v!=(a->last_value|1u))return -1;
 }else if(addr==0x18000000u){
  if(!a->last_valid||a->last_addr!=addr||
    v!=((a->last_value&0x0000ff7fu)|
      (0x57460080u)))return -1;
 }
 else if(addr==0x18001120u&&v!=0x89040000u)return -1;
 else if((addr==0x185001dcu||addr==0x185001e0u)&&v)return -1;
 else if(addr==0x1800f000u&&(v!=0x32c8001cu||!a->state.rom_ready))return -1;
 if(a->hw->write32(a->ctx,addr,v))return -1;
 if(addr==0x18003008u)a->clock_step++;
 a->last_valid=0;
 if(st==NRPC_RELEASE&&addr==0x180601a0u&&v==0)a->asleep=1;
 return 0;
}
static int delay_us(void *p,unsigned us) {
 struct nrpa *a=p;if(us!=1&&us!=30&&us!=50&&us!=500&&us!=1000)return -1;
 if(guard(a,a->state.stage))return -1;
 return a->hw->delay_us(a->ctx,us);
}
/* Direct selected readability: AP guards above, clock detector with bounded
 * deterministic 1/10us waits, IRQ and HOST ack. No broad debug MMIO. */
static int direct_read(struct nrpa *a,unsigned st,uint32_t addr,uint32_t *v) {
 return guard(a,st)||a->hw->read32(a->ctx,addr,v);
}
static int direct_write(struct nrpa *a,unsigned st,uint32_t addr,uint32_t v) {
 return guard(a,st)||a->hw->write32(a->ctx,addr,v);
}
static int direct_delay(struct nrpa *a,unsigned st,unsigned us) {
 return guard(a,st)||a->hw->delay_us(a->ctx,us);
}
static int readable(void *p) {
 struct nrpa *a=p;unsigned st=a->state.stage,spent=0,i,d=1;
 uint32_t v=0;
 if(st!=NRPC_AP_ACK&&st!=NRPC_WF_ACK&&st!=NRPC_POWERED&&
    st!=NRPC_CPU_HELD&&st!=NRPC_RELEASE)return -1;
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
 struct nrpa *a=p;unsigned st=a->state.stage;uint32_t v;
 if((st!=NRPC_CPU_HELD&&st!=NRPC_RELEASE)||a->sem20owned||guard(a,st))return -1;
 a->sem20pending=1;
 if(a->hw->read32(a->ctx,0x18072050u,&v))return -1;
 if(v&1u){a->sem20owned=1;a->sem20pending=0;return 1;}
 a->sem20pending=0;return 0;
}
static int sem_release(void *p) {
 struct nrpa *a=p;unsigned st=a->state.stage;
 if((st!=NRPC_CPU_HELD&&st!=NRPC_RELEASE)||!a->sem20owned||guard(a,st))return -1;
 a->sem20pending=1;
 if(a->hw->write32(a->ctx,0x18072250u,1))return -1;
 a->sem20pending=0;a->sem20owned=0;return 0;
}
static int native_owner(void *p,enum nrb_owner_stage stage) {
 struct nrpa *a=p;struct nrpc_state *s;
 if(!a||a->blocked||a->asleep||!a->hw||a->hw->owner(a->ctx))return -1;
 s=&a->state;
 if(stage==NRB_COLD_BEFORE_POWER)
  return !(s->stage==NRPC_COMMON_READY&&s->cold_valid==0x1fu&&
   !(s->cold[0]&1u)&&!(s->cold[1]&0x80u)&&!(s->cold[2]&4u)&&
   !(s->cold[3]&1u)&&!(s->cold[4]&0x80u)&&
   s->ap_vote&&s->wf_vote&&!s->cpu_held&&!s->clock_vote&&
   !s->power&&!s->sem20&&!s->cpu_released);
 if(stage==NRB_POWERED_CPU_HELD)
  return !(s->stage==NRPC_ROM_PLACE&&s->local_id&&s->cpu_held&&
   s->ap_vote&&s->wf_vote&&s->clock_vote&&s->power&&
   s->protection&&!s->sem20&&!s->cpu_released&&
   s->wfsys_id==0x20010000u);
 return -1;
}
static int rom_policy(void *p,struct nrb_native *n) {
 struct nrpa *a=p;if(!a||n!=a->rom||native_owner(a,NRB_COLD_BEFORE_POWER))return -1;
 return nrb_xnu_policy(n,native_owner,a);
}
static int rom_place(void *p,struct nrb_native *n) {
 struct nrpa *a=p;if(!a||n!=a->rom||native_owner(a,NRB_POWERED_CPU_HELD))return -1;
 return nrb_xnu_place(n);
}
int nrpa_start(struct nrpa *a,const struct nrpa_hw *h,void *ctx,
 struct nrb_native *prepared,unsigned optin) {
 static const struct nrpc_ops ops={proof,read32,write32,delay_us,readable,
  sem_try,sem_release,rom_policy,rom_place};
 if(!a||!prepared||!optin||optin!=1||a->state.entered)return -1;
 if(!h||!h->owner||!h->read32||!h->write32||!h->delay_us)return -1;
 if(!prepared->state.prepared||prepared->state.policy_attempted||
    prepared->state.halt||prepared->state.phase!=1)return -1;
 a->hw=h;a->ctx=ctx;a->rom=prepared;
 return nrpc_run(&ops,a,1,prepared,&a->state);
}
