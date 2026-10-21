/* PRIVATE. Include after pool_bind.inc. No production hook is added here.
 * All source helpers below use fresh state, not sleeping adapter callbacks. */
#include "wifi_main_wfdma_native.h"
struct nwfn_native nwfn_native;
static int nwfn_stop(struct nwfn_native *n,unsigned error)
{
 n->error=error;n->halt=1;n->init.halt=1;n->init.transport.halt=1;return -1;
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
static int nwfn_source(struct nwfn_native *n)
{
 const struct npp_state *p=&nppa_native.probe.state;
 const struct nrpc_state *r=&nrpa_native.diag.state;
 struct ndp_subpool sub;
 if(ndp_subpool_layout(&n->claim,&sub)||n->init.command!=sub.tx_cmd)return -1;
 if(n!=&nwfn_native||!n->prepared||!n->active||n->halt||n->init.halt||n->asleep||
    n->rom!=&nrpa_native.rom||n->pool!=p||n->init.pool!=p||
    n->claim.base!=NDP_POOL_BASE||n->claim.bytes!=NDP_POOL_BYTES||
    n->claim.source!=3||n->claim.valid!=1||
    nppa_native.claim.base!=n->claim.base||nppa_native.claim.bytes!=n->claim.bytes||
    nppa_native.claim.source!=n->claim.source||nppa_native.claim.valid!=n->claim.valid||
    n->pool_va!=(uintptr_t)nppa_native.pool||!n->pool_va||
    !nppa_native.prepared||!nppa_native.attempted||nppa_native.active||
    p->stage!=NPP_DONE||p->error||p->halt||p->uncertain||p->inherited_dma||
    !p->verified||!p->mapped||!p->zero_complete||!p->balanced||!p->asleep||
    p->ap_owned||p->wf_owned||p->set_accepted!=2||p->set_attempted!=3||
    p->raw_valid!=3||p->raw[0][0]||p->raw[1][0]||
    p->words_written!=32768||p->words_read!=32768||
    !nrpc_receipt_ok(&nrpa_native.rom)||nrpa_native.active||
    !r->entered||r->stage!=NRPC_RUNNING||!r->rom_ready||
    !r->cpu_release_attempted||!r->cpu_released||
    !r->power||!r->protection||!r->balanced||r->halt||r->dirty||
    r->unverified||r->ap_vote||r->wf_vote||r->sem20||r->clock_vote||
    r->rom_done!=0x1d1eu||!nrpa_native.diag.asleep||
    !npa_persistent_lifetime||!npa_native.outer_retained||
    !npa_native.state.retained||npa_native.state.fatal||
    npa_native.state.hw_handoff!=2||npa_native.state.result.phase!=10||
    !npa_native.state.result.retained||npa_native.state.result.halt||
    npa_native.state.sema11||npa_native.state.sema19||npa_native.state.spi_inflight||
    !ni_saved||!ncc_fresh_owned||!nw.saved||!nw.domain_touched||ncc_latched_dirty||
    nawa_native.active||nawa_native.diag.result.entered||
    nwsa_native.active||nwsa_native.diag.state.entered||
    !nw.spm||!nw.infra||!nw.gals||!npa_native.host||!npa_native.cfg||
    !npa_native.rgu||!nrpa_native.local||!nrpa_native.rom_done||
    !nppa_native.wfdma||!n->scheduler_va||!n->dummy_va||!n->gic_va)return -1;
 if(n->init.claim.base!=n->claim.base||n->init.claim.bytes!=n->claim.bytes||
    n->init.claim.source!=n->claim.source||n->init.claim.valid!=n->claim.valid)return -1;
 for(unsigned i=0;i<9;i++){
  const struct nwi_ring *a=&n->init.rings[i],*b=&n->expected_rings[i];
  if(a->descriptors!=b->descriptors||a->buffers!=b->buffers||
     a->base!=b->base||a->count!=b->count||a->ext!=b->ext||a->rx!=b->rx)return -1;
 }
 if(n->init.initialized){
  const struct nwt_state *t=&n->init.transport;
  if(n->init.phase!=5||t->phase!=2||t->halt||t->missing_init||
     t->tx_ring!=n->expected_rings[2].descriptors||
     t->rx_ring!=n->expected_rings[5].descriptors||
     t->rx_buffers!=n->expected_rings[5].buffers||t->command!=n->init.command||
     t->tx_index>=1024||t->rx_index>=16)return -1;
 }
 return 0;
}
static int nwfn_guard(struct nwfn_native *n,unsigned full)
{
 static const uint32_t addr[6]={0x10006304,0x1000616c,0x10006170,0x1020e504,0x10001228,0x10001724};
 static const uint32_t mask[6]={0x1f,2,2,1,0x2000,4},want[6]={0xd,2,2,1,0,0};
 if(nwfn_source(n))return nwfn_stop(n,1);
 for(unsigned i=0;i<6;i++)if((nwfn_load(addr[i])&mask[i])!=want[i])return nwfn_stop(n,2);
 /* Saved selected DT: Wi-Fi SPIs375/378 => INTIDs407/410. Read only.
  * No enable/disable or GIC ownership takeover occurs in this candidate. */
 n->gic_enabled=ni_rd(n->gic_va,0x130);
 if(n->gic_enabled&((1u<<23)|(1u<<26)))return nwfn_stop(n,3);
 if(!full)return 0;
 if(!n->ap_owned||!n->wf_owned||nwfn_load(0x180601a0)!=1||
    nwfn_load(0x180601a4)!=(n->wf_initial|1u)||
    (nwfn_load(0x18060184)&0x20)||
    (nwfn_load(0x18060000)&6)!=6||!(nwfn_load(0x180602d4)&1)||
    nwfn_load(0x18001000)!=0x20010101)return nwfn_stop(n,4);
 if((nwfn_load(0x18024204)&0x33f800ffu)||
    (nwfn_load(0x18025204)&0x7d37ffffu))return nwfn_stop(n,5);
 if(n->init.driver_owned&&(nwfn_load(0x18060010)&4))return nwfn_stop(n,6);
 if(n->init.dirty||n->init.initialized){
  struct nwt_state *t=&n->init.transport;t->irq_status_valid=0;
  for(unsigned dma=0;dma<2;dma++){
   t->irq_status[dma]=nwfn_load(0x18024200+0x1000*dma);
   t->irq_status_valid|=1u<<dma;
   if(t->irq_status[dma]&0x00300000u)return nwfn_stop(n,22);
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
          (p==0x18025508&&value==t->rx_index);
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
    n->finish_rx[32]!=0||n->finish_rx[33]!=cid)return nwfn_stop(n,20);
 t->finish_phase++;return 0;
}
