/* PRIVATE. Include after pool_bind.inc. No production hook is added here.
 * All source helpers below use fresh state, not sleeping adapter callbacks. */
#include "wifi_main_wfdma_native.h"
#ifndef NWFN_NORMAL_LEASE
#define NWFN_NORMAL_LEASE(n) 0
#endif
#ifndef NWFN_RX_ACTIVE
#define NWFN_RX_ACTIVE(n) 0
#define NWFN_RX_MEMORY(n,a,b,w) 0
#define NWFN_RX_WRITE(n,a,v) 0
#endif
struct nwfn_native nwfn_native;
static int nwfn_stop(struct nwfn_native *n,unsigned error)
{
 if(!n->error){n->error=error;n->failure_stage=n->stage;}n->halt=1;n->init.halt=1;n->init.transport.halt=1;return -1;
}
static uintptr_t nwfn_base(uint32_t p)
{
 switch(p){
 case 0x10006304:case 0x1000616c:case 0x10006170:return nw.spm;
 case 0x10001228:case 0x10001724:return nw.infra;
 case 0x1020e504:return nw.gals;
 case 0x180601a0:case 0x180601a4:case 0x18060184:
 case 0x18060000:case 0x18060010:case 0x180602d4:case 0x180602cc:return npa_native.host;
 case 0x18001000:case 0x18001620:case 0x18001624:return npa_native.cfg;
 case 0x18000000:case 0x18000010:return npa_native.rgu;
 case 0x184b0010:return nrpa_native.local;
 case 0x184c1604:return nrpa_native.rom_done;
 case 0x18402120:return nwfn_native.dummy_va;
 default:break;
 }
 if(p>=0x18024000u&&p<0x18026000u)return nppa_native.wfdma;
 if(p>=0x18026000u&&p<0x18028000u)return nwfn_native.scheduler_va;
 return 0;
}
static uint32_t nwfn_load(uint32_t p)
{
 uintptr_t base=nwfn_base(p);
 unsigned off=p>=0x18024000u&&p<0x18028000u?(p&0x1fffu):(p&0xfffu);
 return ni_rd(base,off);
}
static void nwfn_store(uint32_t p,uint32_t v)
{
 uintptr_t base=nwfn_base(p);
 unsigned off=p>=0x18024000u&&p<0x18028000u?(p&0x1fffu):(p&0xfffu);
 ni_wr(base,off,v);
}
/* Capture only a failed existing read/predicate, never sample again. */
static int nwfn_guard_failure(struct nwfn_native *n,unsigned reason,unsigned source,
 uint32_t address,uint32_t raw,uint32_t mask,uint32_t expected,unsigned valid)
{
 if(!n->error&&!n->guard_reason){n->guard_reason=reason;n->guard_source=source;
  n->guard_address=address;n->guard_raw=raw;n->guard_mask=mask;
  n->guard_expected=expected;n->guard_valid=valid;}
 return -1;
}
static int nwfn_source(struct nwfn_native *n)
{
 const struct npp_state *p=&nppa_native.probe.state;
 const struct nrpc_state *r=&nrpa_native.diag.state;
 struct ndp_subpool sub;
 if(ndp_subpool_layout(&n->claim,&sub))return nwfn_guard_failure(n,1,1,0,1,1,0,0);
 if(n->init.command!=sub.tx_cmd)return nwfn_guard_failure(n,1,2,0,1,1,0,0);

 if(n!=&nwfn_native)return nwfn_guard_failure(n,1,3,0,1,1,0,0);
 if(!n->prepared)return nwfn_guard_failure(n,1,4,0,1,1,0,0);
 if(!n->active)return nwfn_guard_failure(n,1,5,0,1,1,0,0);
 if(n->halt)return nwfn_guard_failure(n,1,6,0,1,1,0,0);
 if(n->init.halt)return nwfn_guard_failure(n,1,7,0,1,1,0,0);
 if(n->asleep)return nwfn_guard_failure(n,1,8,0,1,1,0,0);
 if(n->rom!=&nrpa_native.rom)return nwfn_guard_failure(n,1,9,0,1,1,0,0);
 if(n->pool!=p)return nwfn_guard_failure(n,1,10,0,1,1,0,0);
 if(n->init.pool!=p)return nwfn_guard_failure(n,1,11,0,1,1,0,0);
 if(n->claim.base!=NDP_POOL_BASE)return nwfn_guard_failure(n,1,12,0,1,1,0,0);
 if(n->claim.bytes!=NDP_POOL_BYTES)return nwfn_guard_failure(n,1,13,0,1,1,0,0);
 if(n->claim.source!=3)return nwfn_guard_failure(n,1,14,0,1,1,0,0);
 if(n->claim.valid!=1)return nwfn_guard_failure(n,1,15,0,1,1,0,0);
 if(nppa_native.claim.base!=n->claim.base)return nwfn_guard_failure(n,1,16,0,1,1,0,0);
 if(nppa_native.claim.bytes!=n->claim.bytes)return nwfn_guard_failure(n,1,17,0,1,1,0,0);
 if(nppa_native.claim.source!=n->claim.source)return nwfn_guard_failure(n,1,18,0,1,1,0,0);
 if(nppa_native.claim.valid!=n->claim.valid)return nwfn_guard_failure(n,1,19,0,1,1,0,0);
 if(n->pool_va!=(uintptr_t)nppa_native.pool)return nwfn_guard_failure(n,1,20,0,1,1,0,0);
 if(!n->pool_va)return nwfn_guard_failure(n,1,21,0,1,1,0,0);
 if(!nppa_native.prepared)return nwfn_guard_failure(n,1,22,0,1,1,0,0);
 if(!nppa_native.attempted)return nwfn_guard_failure(n,1,23,0,1,1,0,0);
 if(nppa_native.active)return nwfn_guard_failure(n,1,24,0,1,1,0,0);
 if(p->stage!=NPP_DONE)return nwfn_guard_failure(n,1,25,0,1,1,0,0);
 if(p->error)return nwfn_guard_failure(n,1,26,0,1,1,0,0);
 if(p->halt)return nwfn_guard_failure(n,1,27,0,1,1,0,0);
 if(p->uncertain)return nwfn_guard_failure(n,1,28,0,1,1,0,0);
 if(p->inherited_dma)return nwfn_guard_failure(n,1,29,0,1,1,0,0);
 if(!p->verified)return nwfn_guard_failure(n,1,30,0,1,1,0,0);
 if(!p->mapped)return nwfn_guard_failure(n,1,31,0,1,1,0,0);
 if(!p->zero_complete)return nwfn_guard_failure(n,1,32,0,1,1,0,0);
 if(!p->balanced)return nwfn_guard_failure(n,1,33,0,1,1,0,0);
 if(!p->asleep)return nwfn_guard_failure(n,1,34,0,1,1,0,0);
 if(p->ap_owned)return nwfn_guard_failure(n,1,35,0,1,1,0,0);
 if(p->wf_owned)return nwfn_guard_failure(n,1,36,0,1,1,0,0);
 if(p->set_accepted!=2)return nwfn_guard_failure(n,1,37,0,1,1,0,0);
 if(p->set_attempted!=3)return nwfn_guard_failure(n,1,38,0,1,1,0,0);
 if(p->raw_valid!=3)return nwfn_guard_failure(n,1,39,0,1,1,0,0);
 if(p->raw[0][0])return nwfn_guard_failure(n,1,40,0,1,1,0,0);
 if(p->raw[1][0])return nwfn_guard_failure(n,1,41,0,1,1,0,0);
 if(p->words_written!=32768)return nwfn_guard_failure(n,1,42,0,1,1,0,0);
 if(p->words_read!=32768)return nwfn_guard_failure(n,1,43,0,1,1,0,0);
 if(!nrpc_receipt_ok(&nrpa_native.rom))return nwfn_guard_failure(n,1,44,0,1,1,0,0);
 if(nrpa_native.active)return nwfn_guard_failure(n,1,45,0,1,1,0,0);
 if(!r->entered)return nwfn_guard_failure(n,1,46,0,1,1,0,0);
 if(r->stage!=NRPC_RUNNING)return nwfn_guard_failure(n,1,47,0,1,1,0,0);
 if(!r->rom_ready)return nwfn_guard_failure(n,1,48,0,1,1,0,0);
 if(!r->cpu_release_attempted)return nwfn_guard_failure(n,1,49,0,1,1,0,0);
 if(!r->cpu_released)return nwfn_guard_failure(n,1,50,0,1,1,0,0);
 if(!r->power)return nwfn_guard_failure(n,1,51,0,1,1,0,0);
 if(!r->protection)return nwfn_guard_failure(n,1,52,0,1,1,0,0);
 if(!r->balanced)return nwfn_guard_failure(n,1,53,0,1,1,0,0);
 if(r->halt)return nwfn_guard_failure(n,1,54,0,1,1,0,0);
 if(r->dirty)return nwfn_guard_failure(n,1,55,0,1,1,0,0);
 if(r->unverified)return nwfn_guard_failure(n,1,56,0,1,1,0,0);
 if(r->ap_vote)return nwfn_guard_failure(n,1,57,0,1,1,0,0);
 if(r->wf_vote)return nwfn_guard_failure(n,1,58,0,1,1,0,0);
 if(r->sem20)return nwfn_guard_failure(n,1,59,0,1,1,0,0);
 if(r->clock_vote)return nwfn_guard_failure(n,1,60,0,1,1,0,0);
 if(r->rom_done!=0x1d1eu)return nwfn_guard_failure(n,1,61,0,1,1,0,0);
 if(!nrpa_native.diag.asleep)return nwfn_guard_failure(n,1,62,0,1,1,0,0);
 if(!npa_persistent_lifetime)return nwfn_guard_failure(n,1,63,0,1,1,0,0);
 if(!npa_native.outer_retained)return nwfn_guard_failure(n,1,64,0,1,1,0,0);
 if(!npa_native.state.retained)return nwfn_guard_failure(n,1,65,0,1,1,0,0);
 if(npa_native.state.fatal)return nwfn_guard_failure(n,1,66,0,1,1,0,0);
 if(npa_native.state.hw_handoff!=2)return nwfn_guard_failure(n,1,67,0,1,1,0,0);
 if(npa_native.state.result.phase!=10)return nwfn_guard_failure(n,1,68,0,1,1,0,0);
 if(!npa_native.state.result.retained)return nwfn_guard_failure(n,1,69,0,1,1,0,0);
 if(npa_native.state.result.halt)return nwfn_guard_failure(n,1,70,0,1,1,0,0);
 if(npa_native.state.sema11)return nwfn_guard_failure(n,1,71,0,1,1,0,0);
 if(npa_native.state.sema19)return nwfn_guard_failure(n,1,72,0,1,1,0,0);
 if(npa_native.state.spi_inflight)return nwfn_guard_failure(n,1,73,0,1,1,0,0);
 if(!ni_saved)return nwfn_guard_failure(n,1,74,0,1,1,0,0);
 if(!ncc_fresh_owned)return nwfn_guard_failure(n,1,75,0,1,1,0,0);
 if(!nw.saved)return nwfn_guard_failure(n,1,76,0,1,1,0,0);
 if(!nw.domain_touched)return nwfn_guard_failure(n,1,77,0,1,1,0,0);
 if(ncc_latched_dirty)return nwfn_guard_failure(n,1,78,0,1,1,0,0);
 if(nawa_native.active)return nwfn_guard_failure(n,1,79,0,1,1,0,0);
 if(nawa_native.diag.result.entered)return nwfn_guard_failure(n,1,80,0,1,1,0,0);
 if(nwsa_native.active)return nwfn_guard_failure(n,1,81,0,1,1,0,0);
 if(nwsa_native.diag.state.entered)return nwfn_guard_failure(n,1,82,0,1,1,0,0);
 if(!nw.spm)return nwfn_guard_failure(n,1,83,0,1,1,0,0);
 if(!nw.infra)return nwfn_guard_failure(n,1,84,0,1,1,0,0);
 if(!nw.gals)return nwfn_guard_failure(n,1,85,0,1,1,0,0);
 if(!npa_native.host)return nwfn_guard_failure(n,1,86,0,1,1,0,0);
 if(!npa_native.cfg)return nwfn_guard_failure(n,1,87,0,1,1,0,0);
 if(!npa_native.rgu)return nwfn_guard_failure(n,1,88,0,1,1,0,0);
 if(!nrpa_native.local)return nwfn_guard_failure(n,1,89,0,1,1,0,0);
 if(!nrpa_native.rom_done)return nwfn_guard_failure(n,1,90,0,1,1,0,0);
 if(!nppa_native.wfdma)return nwfn_guard_failure(n,1,91,0,1,1,0,0);
 if(!n->scheduler_va)return nwfn_guard_failure(n,1,92,0,1,1,0,0);
 if(!n->dummy_va)return nwfn_guard_failure(n,1,93,0,1,1,0,0);
 if(!n->gic_va)return nwfn_guard_failure(n,1,94,0,1,1,0,0);

 if(n->init.claim.base!=n->claim.base)return nwfn_guard_failure(n,1,95,0,1,1,0,0);
 if(n->init.claim.bytes!=n->claim.bytes)return nwfn_guard_failure(n,1,96,0,1,1,0,0);
 if(n->init.claim.source!=n->claim.source)return nwfn_guard_failure(n,1,97,0,1,1,0,0);
 if(n->init.claim.valid!=n->claim.valid)return nwfn_guard_failure(n,1,98,0,1,1,0,0);

 for(unsigned i=0;i<9;i++){
  const struct nwi_ring *a=&n->init.rings[i],*b=&n->expected_rings[i];
  if(a->descriptors!=b->descriptors)return nwfn_guard_failure(n,1,99,0,1,1,0,0);
 if(a->buffers!=b->buffers)return nwfn_guard_failure(n,1,100,0,1,1,0,0);
 if(a->base!=b->base)return nwfn_guard_failure(n,1,101,0,1,1,0,0);
 if(a->count!=b->count)return nwfn_guard_failure(n,1,102,0,1,1,0,0);
 if(a->ext!=b->ext)return nwfn_guard_failure(n,1,103,0,1,1,0,0);
 if(a->rx!=b->rx)return nwfn_guard_failure(n,1,104,0,1,1,0,0);

 }
 if(n->init.initialized){
  const struct nwt_state *t=&n->init.transport;
  if(n->init.phase!=5)return nwfn_guard_failure(n,1,105,0,1,1,0,0);
 if(t->phase!=2)return nwfn_guard_failure(n,1,106,0,1,1,0,0);
 if(t->halt)return nwfn_guard_failure(n,1,107,0,1,1,0,0);
 if(t->missing_init)return nwfn_guard_failure(n,1,108,0,1,1,0,0);
 if(t->tx_ring!=n->expected_rings[2].descriptors)return nwfn_guard_failure(n,1,109,0,1,1,0,0);
 if(t->rx_ring!=n->expected_rings[5].descriptors)return nwfn_guard_failure(n,1,110,0,1,1,0,0);
 if(t->rx_buffers!=n->expected_rings[5].buffers)return nwfn_guard_failure(n,1,111,0,1,1,0,0);
 if(t->command!=n->init.command)return nwfn_guard_failure(n,1,112,0,1,1,0,0);
 if(t->tx_index>=1024)return nwfn_guard_failure(n,1,113,0,1,1,0,0);
 if(t->rx_index>=16)return nwfn_guard_failure(n,1,114,0,1,1,0,0);

 }
 return 0;
}
static int nwfn_guard(struct nwfn_native *n,unsigned full)
{
 static const uint32_t addr[6]={0x10006304,0x1000616c,0x10006170,0x1020e504,0x10001228,0x10001724};
 static const uint32_t mask[6]={0x1f,2,2,1,0x2000,4},want[6]={0xd,2,2,1,0,0};
 uint32_t raw;
 if(nwfn_source(n))return nwfn_stop(n,1);
 for(unsigned i=0;i<6;i++){
  raw=nwfn_load(addr[i]);if((raw&mask[i])!=want[i]){
   nwfn_guard_failure(n,2,i+1,addr[i],raw,mask[i],want[i],1);return nwfn_stop(n,2);}
 }
 n->gic_enabled=ni_rd(n->gic_va,0x130);
 if(n->gic_enabled&((1u<<23)|(1u<<26))){
  nwfn_guard_failure(n,3,0,0x0c000130,n->gic_enabled,(1u<<23)|(1u<<26),0,1);return nwfn_stop(n,3);}
 if(!full)return 0;
 if(!n->ap_owned||!n->wf_owned){
  nwfn_guard_failure(n,4,!n->ap_owned?1:2,0,0,1,1,0);return nwfn_stop(n,4);}
 /* Exactly the previous short-circuit read order, with captured failures. */
 static const uint32_t live_addr[6]={0x180601a0,0x180601a4,0x18060184,0x18060000,0x180602d4,0x18001000};
 const uint32_t live_mask[6]={0xffffffffu,0xffffffffu,0x20,6,1,0xffffffffu};
 const uint32_t live_want[6]={1,n->wf_initial|1u,0,6,1,0x20010101};
 for(unsigned i=0;i<6;i++){
  raw=nwfn_load(live_addr[i]);if((raw&live_mask[i])!=live_want[i]){
   nwfn_guard_failure(n,4,i+3,live_addr[i],raw,live_mask[i],live_want[i],1);return nwfn_stop(n,4);}
 }
 for(unsigned i=0;i<2;i++){
  uint32_t address=0x18024204+0x1000*i,mask=i?0x7d37ffffu:0x33f800ffu;
  /* Measured late N9-start transition: only MCU software-event enable.
   * Delivery remains GIC-masked; a pending software event is fatal below. */
  if(i==1&&n->stage==NWFN_RETAINED&&n->finish_tx[36]==0x41&&
     n->finish_tx[37]==0xa0&&
     ((n->init.transport.finish_phase==1&&n->init.transport.completed==2&&
       n->init.transport.submit_attempted)||
      (n->init.transport.finish_phase==2&&
       (n->init.transport.completed==3||
        NWFN_NORMAL_LEASE(n)))))
   mask&=~0x20000000u;
  raw=nwfn_load(address);
  if(raw&mask){
   nwfn_guard_failure(n,5,i+1,address,raw,mask,0,1);return nwfn_stop(n,5);}
 }
 if(n->init.driver_owned){raw=nwfn_load(0x18060010);if(raw&4){
  nwfn_guard_failure(n,6,0,0x18060010,raw,4,0,1);return nwfn_stop(n,6);}}
 if(n->init.dirty||n->init.initialized){
  struct nwt_state *t=&n->init.transport;t->irq_status_valid=0;
  for(unsigned dma=0;dma<2;dma++){
   t->irq_status[dma]=nwfn_load(0x18024200+0x1000*dma);
   t->irq_status_valid|=1u<<dma;
   if(t->irq_status[dma]&0x00300000u){
    nwfn_guard_failure(n,22,dma+1,0x18024200+0x1000*dma,t->irq_status[dma],0x00300000,0,1);return nwfn_stop(n,22);}
   if(dma==1&&(t->irq_status[dma]&0x20000000u)){
    /* No software-event handler/recovery exists. Capture the existing raw
     * summary and stop; no subordinate read, W1C or retry. */
    nwfn_guard_failure(n,23,2,0x18025200,t->irq_status[dma],0x20000000,0,1);
    return nwfn_stop(n,23);
   }
  }
 }
 return 0;
}
int nwfn_owner(void *c)
{
 struct nwfn_native *n=c;
 if(!n||n!=&nwfn_native)return -1;
 return nwfn_guard(n,n->stage>=NWFN_LIVE&&n->stage<NWFN_RELEASE);
}
static int nwfn_read_allowed(uint32_t p)
{
 /* Read-only selected DMA0 RX prefetch diagnostics under the existing
  * retained normal lease. No new write or ownership path is admitted. */
 if(NWFN_RX_ACTIVE(&nwfn_native)&&p>=0x18024680u&&p<=0x1802468cu&&!(p&3))return 1;
 if(p==0x18060010||p==0x18402120)return 1;
 for(unsigned d=0;d<2;d++){
  uint32_t b=0x18024000+0x1000*d;
  if(p==b+0x200||p==b+0x204||p==b+0x208||p==b+0x298)return 1;
 }
 struct nwt_plan_op plan[99];size_t count=0;
 if(nwdm_plan(plan,99,&count))return 0;
 for(size_t i=0;i<count;i++)if(p==plan[i].physical)return 1;
 for(unsigned d=0;d<2;d++)for(unsigned r=0;r<28;r++){
  if((!d&&r<20)||(d&&r>=23)||(d&&r<20&&((r>=6&&r<8)||(r>=14&&r<16))))continue;
  uint32_t a=0x18024000+0x1000*d+(r<20?0x300+16*r:0x500+16*(r-20));
  if(p>=a&&p<=a+12&&!(p&3))return 1;
 }
 return p==0x18027500;
}
static int nwfn_read(void *c,uint32_t p,uint32_t *v)
{
 struct nwfn_native *n=c;
 if(!v||nwfn_owner(c)||!nwfn_read_allowed(p))return -1;
 *v=nwfn_load(p);
 /* Internal driver-own reads must not replace a pending RMW snapshot. */
 if(p!=0x18060010){n->last_address=p;n->last_value=*v;n->last_valid=1;}
 return 0;
}
static int nwfn_write(void *c,uint32_t p,uint32_t value)
{
 struct nwfn_native *n=c;unsigned allowed=0;
 if(nwfn_owner(c))return -1;
 if(n->stage==NWFN_RETAINED){
  struct nwt_state *t=&n->init.transport;
  allowed=(p==0x18025418&&t->submit_attempted&&value==(t->tx_index+1)%1024)||
          (!NWFN_RX_ACTIVE(n)&&p==0x18025508&&value==t->rx_index)||
          NWFN_RX_WRITE(n,p,value);
 }else if(n->stage==NWFN_INITIALIZING){
  if(n->init.phase==1)allowed=p==0x18060010&&value==2&&n->init.driver_requested;
  if(n->init.phase==2){
   struct nwt_plan_op plan[99];size_t count=0;
   if(nwdm_plan(plan,99,&count))return -1;
   unsigned k=n->init.scheduler.operations;
   if(k<count&&p==plan[k].physical&&n->last_valid&&n->last_address==p)
    allowed=value==((n->last_value&~plan[k].clear)|plan[k].set);
  }
  if(n->init.phase==3){
   for(unsigned d=0;d<2;d++){
    uint32_t b=0x18024000+0x1000*d;
    if((p==b+0x20c||p==b+0x280)&&value==0xffffffffu)allowed=1;
    if(p==b+0x22c&&value==(d?0x2c200151u:0xfu))allowed=1;
    if(p==b+0x208&&!(value&5))allowed=1;
   }
   for(unsigned i=0;i<9;i++){
    const struct nwi_ring *r=&n->expected_rings[i];
    if(p==r->base&&value==r->descriptors)allowed=1;
    if(p==r->base+4&&value==r->count)allowed=1;
    if(p==r->base+8&&value==(r->rx?r->count-1:0))allowed=1;
    if(p==r->ext&&value==4)allowed=1;
   }
   struct nwt_plan_op plan[42];size_t count=0;
   if(nwt_prefetch_plan(plan,42,&count))return -1;
   for(size_t i=0;i<count;i++)if(p==plan[i].physical){
    if(!plan[i].rmw&&value==plan[i].set)allowed=1;
    if(plan[i].rmw&&n->last_valid&&n->last_address==p&&
       value==((n->last_value&~plan[i].clear)|plan[i].set))allowed=1;
   }
  }
  if(n->init.phase==4){
   if((p==0x18024208||p==0x18025208)&&n->last_valid&&n->last_address==p){
    struct nwt_rmw cfg;
    if(nwt_cfg_plan(p==0x18025208,1,&cfg))return -1;
    allowed=value==((n->last_value&~(cfg.clear&~(1u<<26)))|(cfg.set&~5u))||
            value==((n->last_value&~(1u<<26))|5u);
   }
   if(p==0x18402120&&n->last_valid&&n->last_address==p)
    allowed=value==(n->last_value|2u);
  }
 }
 if(!allowed||!nwfn_base(p))return nwfn_stop(n,7);
 nwfn_store(p,value);n->last_valid=0;return 0;
}
static int nwfn_memory_range(struct nwfn_native *n,uint64_t a,uint32_t bytes,unsigned write)
{
 if(!bytes||a<n->claim.base||a-n->claim.base>=n->claim.bytes||bytes>n->claim.bytes-(a-n->claim.base))return 0;
 if(n->stage==NWFN_RETAINED){
  const struct nwt_state *t=&n->init.transport;
  if(a==t->command&&write&&bytes<=4096)return 1;
  if(a==t->tx_ring+16u*t->tx_index&&bytes==16)return 1;
  if(NWFN_RX_ACTIVE(n))return NWFN_RX_MEMORY(n,a,bytes,write);
  if(a==t->rx_ring+16u*t->rx_index&&bytes==16)return 1;
  if(!write&&a==t->rx_buffers+4096u*t->rx_index&&bytes<=3840)return 1;
  return 0;
 }
 if(n->stage!=NWFN_INITIALIZING||n->init.phase!=2||!n->init.scheduler.complete||!write)return 0;
 for(unsigned i=0;i<9;i++){
  const struct nwi_ring *r=&n->expected_rings[i];
  if(a>=r->descriptors&&a-r->descriptors<16u*r->count&&
     bytes<=16u*r->count-(a-r->descriptors))return 1;
  if(r->rx&&a>=r->buffers&&a-r->buffers<4096u*r->count&&
     !((a-r->buffers)&4095u)&&bytes==3840)return 1;
 }
 return 0;
}
static int nwfn_memory_read(void *c,uint64_t a,void *output,uint32_t bytes)
{
 struct nwfn_native *n=c;uint8_t *p=output;
 if(!p||nwfn_owner(c)||!nwfn_memory_range(n,a,bytes,0))return -1;
 const volatile uint8_t *source=(const volatile uint8_t *)(n->pool_va+(uintptr_t)(a-n->claim.base));
 for(uint32_t i=0;i<bytes;i++)p[i]=source[i];
 return 0;
}
static int nwfn_memory_write(void *c,uint64_t a,const void *input,uint32_t bytes)
{
 struct nwfn_native *n=c;const uint8_t *p=input;
 if(!p||nwfn_owner(c)||!nwfn_memory_range(n,a,bytes,1))return -1;
 volatile uint8_t *dest=(volatile uint8_t *)(n->pool_va+(uintptr_t)(a-n->claim.base));
 for(uint32_t i=0;i<bytes;i++)dest[i]=p[i];
 return 0;
}
static int nwfn_sync(void *c,uint64_t a,uint32_t bytes,unsigned way)
{
 struct nwfn_native *n=c;
 if((way!=1&&way!=2)||nwfn_owner(c)||
    (!nwfn_memory_range(n,a,bytes,way==1)&&!nwfn_memory_range(n,a,bytes,way!=1)))return -1;
 /* Retained low pool uses ml_io_map outside managed DRAM, matching the
  * accepted pool mapping. Device mapping: no cached CPU alias is introduced. */
#ifndef NWFN_NATIVE_HOST_TEST
 __asm__ volatile("dsb sy" ::: "memory");
#else
 nwfn_test_barrier();
#endif
 return 0;
}
static int nwfn_delay(void *c,unsigned us)
{
 if(us!=1000||nwfn_owner(c))return -1;
 delay(us);return 0;
}
const struct nwt_io nwfn_io={nwfn_owner,nwfn_read,nwfn_write,
 nwfn_memory_read,nwfn_memory_write,nwfn_sync,nwfn_delay};
static int nwfn_fread(struct nwfn_native *n,uint32_t p,uint32_t *value)
{
 if(!value||!nwfn_base(p)||nwfn_guard(n,0))return -1;
 *value=nwfn_load(p);return 0;
}
static int nwfn_fwrite(struct nwfn_native *n,uint32_t p,uint32_t value)
{
 if(nwfn_guard(n,0))return -1;
 if(!((p==0x180601a0&&((n->stage==NWFN_PREPARED&&value==1)||
      (n->stage==NWFN_RELEASE&&value==0&&!n->wf_owned)))||
      (p==0x180601a4&&((n->stage==NWFN_AP_READY&&value==(n->wf_initial|1u))||
      (n->stage==NWFN_RELEASE&&value==n->wf_initial)))||
      (p==0x18060000&&(n->stage==NWFN_AP_ACK||n->stage==NWFN_WF_ACK))))return nwfn_stop(n,8);
 nwfn_store(p,value);return 0;
}
static int nwfn_pause(struct nwfn_native *n,unsigned us)
{
 if(nwfn_guard(n,0))return -1;
 delay(us);return 0;
}
static int nwfn_ack(struct nwfn_native *n,unsigned *count)
{
 uint32_t value;
 for(unsigned i=0;i<12;i++){
  if(i&&nwfn_pause(n,1000))return -1;
  if(nwfn_fread(n,0x18060184,&value))return -1;
  ++*count;if(!(value&0x20))return 0;
 }
 return nwfn_stop(n,9);
}
static int nwfn_readable(struct nwfn_native *n)
{
 unsigned spent=0,us=1;uint32_t value=0;
 for(unsigned i=0;i<1600&&spent<16000;i++){
  if(nwfn_fread(n,0x18060000,&value)||nwfn_fwrite(n,0x18060000,value|1)||
     nwfn_pause(n,us))return -1;
  spent+=us;if(nwfn_fread(n,0x18060000,&value))return -1;
  n->clock_raw=value;n->clock_polls++;
  if((value&6)==6)break;
  us=10;
 }
 n->clock_spent+=spent;
 if((value&6)!=6||spent>=16000)return nwfn_stop(n,10);
 if(nwfn_fread(n,0x180602d4,&n->irq_raw)||!(n->irq_raw&1)||
    nwfn_fread(n,0x18060184,&value)||(value&0x20))return nwfn_stop(n,11);
 return 0;
}
static int nwfn_version(struct nwfn_native *n,uint32_t p,uint32_t want,
 unsigned bound,unsigned us,uint32_t *last)
{
 for(unsigned i=0;i<bound;i++){
  if(i&&us&&nwfn_pause(n,us))return -1;
  if(nwfn_fread(n,p,last))return -1;
  if(*last==want)return 0;
 }
 return nwfn_stop(n,12);
}
int nwfn_prepare(void)
{
 struct nwfn_native *n=&nwfn_native;unsigned main=0,rom=0,dma=0;
 PE_parse_boot_argn("nord2wifimain",&main,sizeof(main));
 if(main!=1)return 1;
 PE_parse_boot_argn("nord2wifirom",&rom,sizeof(rom));
 PE_parse_boot_argn("nord2wifidma",&dma,sizeof(dma));
 if(rom!=1||dma!=1||n->attempted||n->prepared||
    !nppa_native.prepared||!nppa_native.pool||!nppa_native.wfdma||
    nwi_prepare(&n->init,&nppa_native.claim,&nppa_native.probe.state))return -1;
 n->rom=&nrpa_native.rom;n->pool=&nppa_native.probe.state;
 n->claim=nppa_native.claim;n->pool_va=nppa_native.pool;
 memcpy(n->expected_rings,n->init.rings,sizeof(n->expected_rings));
 /* Sleepable mapping only. No register or pool memory is accessed here. */
 n->scheduler_va=ml_io_map(0x18026000,0x2000);
 n->dummy_va=ml_io_map(0x18402000,0x1000);
 n->gic_va=ml_io_map(0x0c000000,0x1000);
 if(!n->scheduler_va||!n->dummy_va||!n->gic_va)return -1;
 n->prepared=1;n->stage=NWFN_PREPARED;return 0;
}
static int nwfn_sleep_refusal(struct nwfn_native *n)
{
 uint32_t value;
 if(n->init.dirty||n->init.initialized||n->init.halt)return nwfn_stop(n,13);
 n->stage=NWFN_RELEASE;
 if(nwfn_fread(n,0x180601a4,&value)||value!=(n->wf_initial|1u)||
    nwfn_fwrite(n,0x180601a4,n->wf_initial))return nwfn_stop(n,14);
 n->wf_owned=0;
 /* AP literal zero is final hardware access. */
 if(nwfn_fwrite(n,0x180601a0,0))return nwfn_stop(n,14);
 n->ap_owned=0;n->asleep=1;n->balanced=1;n->active=0;n->stage=NWFN_ASLEEP;return 1;
}
int nwfn_start(void)
{
 struct nwfn_native *n=&nwfn_native;uint32_t value;
 if(!n->prepared||n->attempted||n->halt)return -1;
 n->attempted=1;n->active=1;n->stage=NWFN_PREPARED;
 if(nwfn_fread(n,0x180601a0,&value)||value)return nwfn_stop(n,15);
 n->ap_owned=1;
 if(nwfn_fwrite(n,0x180601a0,1))return -1;
 n->stage=NWFN_AP_ACK;
 if(nwfn_ack(n,&n->ap_polls)||nwfn_readable(n))return -1;
 n->stage=NWFN_AP_READY;
 if(nwfn_version(n,0x18001000,0x20010101,10,0,&n->cfg_id)||
    nwfn_fread(n,0x180601a4,&value)||(value&1))return nwfn_stop(n,16);
 n->wf_initial=value;n->wf_owned=1;
 if(nwfn_fwrite(n,0x180601a4,value|1))return -1;
 n->stage=NWFN_WF_ACK;
 if(nwfn_ack(n,&n->wf_polls)||nwfn_readable(n)||
    nwfn_version(n,0x18001000,0x20010101,12,1000,&n->cfg_id))return -1;
 const uint32_t addr[5]={0x18000010,0x18000000,0x180602cc,0x18001620,0x18001624};
 for(unsigned i=0;i<5;i++){
  if(nwfn_fread(n,addr[i],&n->live[i]))return -1;
  n->live_valid|=1u<<i;
 }
 if(!(n->live[0]&1)||!(n->live[1]&0x80)||!(n->live[2]&4)||
    (n->live[3]&9)||(n->live[4]&9)||
    nwfn_version(n,0x184b0010,0x20010000,12,500,&n->local_id)||
    nwfn_version(n,0x184c1604,0x1d1e,12,1000,&n->rom_done))return nwfn_stop(n,17);
 n->stage=NWFN_LIVE;
 if(nwfn_owner(n))return -1;
 n->stage=NWFN_INITIALIZING;
 int result=nwi_initialize(&nwfn_io,n,&n->init,1);
 if(result==1)return nwfn_sleep_refusal(n);
 if(result||!n->init.initialized||n->init.phase!=5||!n->init.driver_owned)
  return nwfn_stop(n,18);
 n->stage=NWFN_RETAINED;return nwfn_owner(n);
}

/* Native wrapper parity with nwt_exchange, using retained event storage. */
int nwfn_finish_command(uint8_t cid)
{
 struct nwfn_native *n=&nwfn_native;struct nwt_state *t=&n->init.transport;
 size_t used=0;
 if(n->stage!=NWFN_RETAINED||nwfn_owner(n)||
    t->finish_phase>1||cid!=(t->finish_phase?0x41:0x40)||
    nwt_finish(n->finish_tx,sizeof(n->finish_tx),cid,t->next_sequence,&used))return nwfn_stop(n,19);
 if(nwt_capture(&nwfn_io,n,t,n->finish_tx,used,1,n->finish_rx,
    sizeof(n->finish_rx),&n->finish_received)||n->finish_received<34||
    n->finish_rx[32]!=0)return nwfn_stop(n,20);
 t->finish_phase++;return 0;
}
