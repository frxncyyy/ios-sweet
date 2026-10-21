/* PRIVATE candidate, include after wifi_rom_bind.inc in sweet_touch. The
 * only boot flag is nord2wifidma=1 together with nord2wifirom=1. No old
 * nrpa/nwsa/nawa callback or wake lifetime is reused. */
#include "pool_adapter.h"
#ifndef NPP_NATIVE_HOST_TEST
#include <pexpert/device_tree.h>
#include <pexpert/arm64/boot.h>
#endif
static struct {
 struct nppa probe;struct ndp_claim claim;struct ndp_subpool layout;
 vm_offset_t wfdma,pool;bool active,attempted,prepared;
 bool preflight_arm,map_arm,zero_arm;
 int prepare_status;
} nppa_native;

static int nppa_claim_current(void)
{
 DTEntry e;const void *p=0;unsigned n=0;
 if(SecureDTLookupEntry(NULL,"/chosen",&e)!=kSuccess||
    SecureDTGetProperty(e,"nord2-wifi-dma-claim",&p,&n)!=kSuccess||
    ndp_claim_decode(p,n,&nppa_native.claim)||
    ndp_subpool_layout(&nppa_native.claim,&nppa_native.layout))return -1;
 /* nrb is the exact native object used for the now-complete ROM release.
  * The loader's same-boot FDT proof is carried by this distinct ADT claim. */
 if(!nrpc_receipt_ok(&nrpa_native.rom)||
    nrpa_native.rom.claim.base!=NRC_CONN_BASE||
    nrpa_native.rom.claim.bytes!=NRC_CONN_BYTES||
    nrpa_native.rom.claim.source!=NRC_CLAIM_IOS_FIXED)return -1;
 const boot_args *b=(const boot_args *)PE_state.bootArgs;
 const void *dbp=0,*dsp=0;unsigned dbn=0,dsn=0;uint64_t db=0,ds=0;
 if(!b||b->physBase!=UINT64_C(0x142000000)||
    b->memSize!=UINT64_C(0x3e000000)||b->memSizeActual!=b->memSize||
    SecureDTGetProperty(e,"dram-base",&dbp,&dbn)!=kSuccess||
    SecureDTGetProperty(e,"dram-size",&dsp,&dsn)!=kSuccess||
    dbn!=8||dsn!=8||!dbp||!dsp)return -1;
 memcpy(&db,dbp,8);memcpy(&ds,dsp,8);
 return db!=b->physBase||ds!=b->memSize;
}
static int nppa_source_owner(void)
{
 const struct nrpc_state *r=&nrpa_native.diag.state;
 if(!nppa_native.active||!nppa_native.prepared||!npa_persistent_lifetime||
    !npa_native.outer_retained||!npa_native.state.entered||
    !npa_native.state.retained||npa_native.state.fatal||
    npa_native.state.hw_handoff!=2||npa_native.state.result.phase!=10||
    !npa_native.state.result.retained||npa_native.state.result.halt||
    npa_native.state.sema11||npa_native.state.sema19||
    npa_native.state.spi_inflight||!ni_saved||!ncc_fresh_owned||
    !nw.saved||!nw.domain_touched||ncc_latched_dirty||
    nawa_native.active||nawa_native.diag.result.entered||
    nwsa_native.active||nwsa_native.diag.state.entered||
    nrpa_native.active||!nrpa_native.run_attempted||
    !r->entered||r->stage!=NRPC_RUNNING||!r->rom_ready||
    !r->cpu_release_attempted||!r->cpu_released||
    !r->power||!r->protection||!r->balanced||r->halt||r->dirty||
    r->unverified||r->ap_vote||r->wf_vote||r->sem20||r->clock_vote||
    r->rom_done!=0x1d1eu||!nrpa_native.diag.asleep||
    !nrpc_receipt_ok(&nrpa_native.rom)||
    !nw.spm||!nw.infra||!nw.gals||!npa_native.host||
    !npa_native.cfg||!npa_native.rgu||!nrpa_native.local||
    !nrpa_native.rom_done)return -1;
 return 0;
}
static int nppa_xnu_owner(void *c){(void)c;return !nppa_native.wfdma||nppa_source_owner();}
static vm_offset_t nppa_xnu_base(uint32_t p)
{
 switch(p){
 case 0x10006304:case 0x1000616c:case 0x10006170:return nw.spm;
 case 0x10001228:case 0x10001724:return nw.infra;
 case 0x1020e504:return nw.gals;
 case 0x180601a0:case 0x180601a4:case 0x18060184:
 case 0x18060000:case 0x180602d4:case 0x180602cc:return npa_native.host;
 case 0x18001000:case 0x18001620:case 0x18001624:return npa_native.cfg;
 case 0x18000010:case 0x18000000:return npa_native.rgu;
 case 0x184b0010:return nrpa_native.local;
 case 0x184c1604:return nrpa_native.rom_done;
 default:break;
 }
 if(p>=0x18024000u&&p<0x18026000u)return nppa_native.wfdma;
 return 0;
}
static int nppa_xnu_read(void *c,uint32_t p,uint32_t *v)
{
 (void)c;vm_offset_t base=nppa_xnu_base(p);
 if(!v||!base||nppa_native.probe.asleep)return -1;
 if(p>=0x18024000u&&p<0x18026000u&&!nppa_native.preflight_arm){
  if(nppa_native.probe.state.stage!=NPP_PREFLIGHT)return -1;
  nppa_native.preflight_arm=true;
  printf("NORD2 WIFI DMA ARM: stage=%u action=preflight base=%#x rings=%u\n",
   nppa_native.probe.state.stage,0x18024000u,NWDP_RINGS);
 }
 *v=ni_rd(base,p>=0x18024000u&&p<0x18026000u?(p-0x18024000u):(p&0xfffu));
 return 0;
}
static int nppa_xnu_write(void *c,uint32_t p,uint32_t v)
{
 (void)c;vm_offset_t base=nppa_xnu_base(p);unsigned st=nppa_native.probe.state.stage;
 if(!base||nppa_native.probe.asleep)return -1;
 if(p==0x180601a0u){
  if(!((st==NPP_PREWAKE&&v==1)||
       (st==NPP_RELEASE&&v==0&&!nppa_native.probe.state.wf_owned)))return -1;
 }else if(p==0x180601a4u){
  if(st!=NPP_AP_READY&&st!=NPP_RELEASE)return -1;
 }else if(p==0x18060000u){
  if(st!=NPP_AP_ACK&&st!=NPP_WF_ACK)return -1;
 }else return -1;
 ni_wr(base,p&0xfffu,v);NCC_DEVICE_BARRIER();return 0;
}
static int nppa_xnu_delay(void *c,unsigned us)
{(void)c;if(nppa_native.probe.asleep)return -1;delay(us);return 0;}
static int nppa_xnu_smc(void *c,const uint64_t x[8],uint64_t out[4])
{
 (void)c;struct npp_state *s=&nppa_native.probe.state;
 struct ndp_smc_request expected[2];unsigned g=s->stage==NPP_SET1?0u:1u;
 if((s->stage!=NPP_SET1&&s->stage!=NPP_SET0)||
    s->set_attempted!=(g?3u:1u)||nppa_source_owner()||
    ndp_region18_set_intent(&nppa_native.claim,expected))return -1;
 for(unsigned i=0;i<8;i++)if(x[i]!=expected[g].x[i])return -1;
 /* Pstore-visible captured request before either one-way call. */
 printf("NORD2 WIFI DMA SMC ARM: group=%u base=%#llx bytes=%#llx x0=%#llx x1=%#llx x2=%#llx x3=%#llx x4=%#llx\n",
  g?0u:1u,(unsigned long long)nppa_native.claim.base,
  (unsigned long long)nppa_native.claim.bytes,
  (unsigned long long)x[0],(unsigned long long)x[1],
  (unsigned long long)x[2],(unsigned long long)x[3],
  (unsigned long long)x[4]);
#ifdef NPP_NATIVE_HOST_TEST
 return nppa_host_smc(x,out);
#elif defined(__aarch64__)
 register uint64_t x0 __asm__("x0")=x[0],x1 __asm__("x1")=x[1];
 register uint64_t x2 __asm__("x2")=x[2],x3 __asm__("x3")=x[3];
 register uint64_t x4 __asm__("x4")=x[4],x5 __asm__("x5")=x[5];
 register uint64_t x6 __asm__("x6")=x[6],x7 __asm__("x7")=x[7];
 __asm__ volatile("smc #0" : "+r"(x0),"+r"(x1),"+r"(x2),"+r"(x3),
  "+r"(x4),"+r"(x5),"+r"(x6),"+r"(x7) : :
  "x8","x9","x10","x11","x12","x13","x14","x15","x16","x17","cc","memory");
 out[0]=x0;out[1]=x1;out[2]=x2;out[3]=x3;return 0;
#else
 (void)out;return -1;
#endif
}
static int nppa_xnu_map(void *c,uint64_t base,uint64_t bytes)
{
 (void)c;const struct npp_state *s=&nppa_native.probe.state;
 if(nppa_source_owner()||s->stage!=NPP_MAP||s->set_accepted!=2||
    s->raw_valid!=3||s->inherited_dma||!s->dma.complete||
    !s->dma.observed_empty||!s->dma.stable||nppa_native.pool||
    base!=NDP_POOL_BASE||bytes!=NDP_POOL_BYTES)return -1;
 nppa_native.map_arm=true;
 printf("NORD2 WIFI DMA ARM: stage=%u action=map base=%#llx bytes=%#llx\n",
  s->stage,(unsigned long long)base,(unsigned long long)bytes);
 nppa_native.pool=ml_io_map((vm_offset_t)base,(vm_size_t)bytes);
 return nppa_native.pool?0:-1;
}
static int nppa_xnu_word(void *c,uint32_t off,uint32_t *v,unsigned write)
{
 (void)c;const struct npp_state *s=&nppa_native.probe.state;
 if(nppa_source_owner()||!nppa_native.pool||!v||(off&3u)||
    off>=NDP_SUBPOOL_BYTES||s->set_accepted!=2||s->raw_valid!=3||
    s->stage!=(write?NPP_ZERO:NPP_VERIFY))return -1;
 volatile uint32_t *p=(volatile uint32_t *)(nppa_native.pool+off);
 if(write){
  if(*v)return -1;
  if(!nppa_native.zero_arm){
   nppa_native.zero_arm=true;
   printf("NORD2 WIFI DMA ARM: stage=%u action=zero base=%#llx bytes=%#x\n",
    s->stage,(unsigned long long)nppa_native.claim.base,(unsigned)NDP_SUBPOOL_BYTES);
  }
  *p=0;
 }else *v=*p;
 return 0;
}
static int nppa_xnu_barrier(void *c)
{(void)c;if(nppa_source_owner()||!nppa_native.pool)return -1;
 NCC_DEVICE_BARRIER();return 0;}
static const struct nppa_hw nppa_xnu_hw={nppa_xnu_owner,nppa_xnu_read,
 nppa_xnu_write,nppa_xnu_delay,nppa_xnu_smc,nppa_xnu_map,
 nppa_xnu_word,nppa_xnu_barrier};

/* Call only after nrpa_after_persistent has completed and returned. ROM
 * power/firmware remain running; a new AP/WF wake is owned by this object. */
static void nppa_after_rom(void)
{
 unsigned opt=0;PE_parse_boot_argn("nord2wifidma",&opt,sizeof(opt));
 if(opt!=1)return;
 if(nppa_native.attempted)panic("NORD2 WIFI DMA: reentry; manual reset");
 nppa_native.attempted=true;
 unsigned rom_opt=0;PE_parse_boot_argn("nord2wifirom",&rom_opt,sizeof(rom_opt));
 if(rom_opt!=1||nppa_claim_current()){
  printf("NORD2 WIFI DMA PREP: status=-1 claim=0 reason=rom-or-claim\n");return;
 }
 nppa_native.prepared=true;nppa_native.active=true;
 if(nppa_source_owner()){
  nppa_native.active=false;
  printf("NORD2 WIFI DMA PREP: status=-3 claim=1 reason=owner\n");return;
 }
 nppa_native.wfdma=ml_io_map(0x18024000,0x2000);
 if(!nppa_native.wfdma){
  nppa_native.active=false;
  printf("NORD2 WIFI DMA PREP: status=-2 claim=1 reason=wfdma-map\n");return;
 }
 printf("NORD2 WIFI DMA PREP: status=0 claim=1 base=%#llx bytes=%#llx layout=%#x\n",
  (unsigned long long)nppa_native.claim.base,
  (unsigned long long)nppa_native.claim.bytes,(unsigned)NDP_SUBPOOL_BYTES);
 int status=nppa_start(&nppa_native.probe,&nppa_xnu_hw,0,&nppa_native.claim,1);
 const struct npp_state *s=&nppa_native.probe.state;
 /* Stored observations only. No hardware access after AP sleep. */
 printf("NORD2 WIFI DMA: status=%d stage=%u inherited=%u preflight=%u idle=%u stable=%u reads=%u halt=%u error=%d\n",
  status,s->stage,s->inherited_dma,s->preflight_complete,
  s->dma.observed_empty,s->dma.stable,s->dma.reads,s->halt,s->error);
 printf("NORD2 WIFI DMA SET: attempted=%u accepted=%u raw_valid=%u raw1=%#llx raw0=%#llx mapped=%u zero=%u verified=%u words=%u/%u\n",
  s->set_attempted,s->set_accepted,s->raw_valid,
  (unsigned long long)s->raw[0][0],(unsigned long long)s->raw[1][0],
  s->mapped,s->zero_complete,s->verified,s->words_written,s->words_read);
 for(unsigned g=0;g<2;g++)if(s->raw_valid&(1u<<g))
  printf("NORD2 WIFI DMA RAW: group=%u x=%#llx/%#llx/%#llx/%#llx\n",
   g?0u:1u,(unsigned long long)s->raw[g][0],
   (unsigned long long)s->raw[g][1],(unsigned long long)s->raw[g][2],
   (unsigned long long)s->raw[g][3]);
 printf("NORD2 WIFI DMA OWNER: ap=%u wf=%u asleep=%u balanced=%u possible_lock=%u live=%#x cfg=%#x local=%#x done=%#x first=%u/%#x/%#x/%#x\n",
  s->ap_owned,s->wf_owned,s->asleep,s->balanced,s->possibly_locked,
  s->live_valid,s->cfg_id,s->local_id,s->rom_done,
  s->dma.first_reason,s->dma.first_addr,s->dma.first_raw,s->dma.first_mask);
 printf("NORD2 WIFI DMA LIVE: cpu=%#x top=%#x power=%#x protection=%#x dma_protection=%#x\n",
  s->live[0],s->live[1],s->live[2],s->live[3],s->live[4]);
 if(s->preflight_complete){
  printf("NORD2 WIFI DMA CFG: before=%#x/%#x after=%#x/%#x valid=%#x\n",
   s->dma.cfg[0],s->dma.cfg[1],s->dma.cfg_after[0],
   s->dma.cfg_after[1],s->dma.cfg_valid);
  for(unsigned i=0;i<NWDP_RINGS;i++){
   const struct nwdp_ring *r=&s->dma.rings[i];
   printf("NORD2 WIFI DMA RING: n=%u addr=%#x raw=%#x/%#x/%#x/%#x valid=%#x\n",
    i,r->address,r->raw[0],r->raw[1],r->raw[2],r->raw[3],r->valid);
  }
 }
 if(status<0&&!s->halt&&!s->ap_owned&&!s->wf_owned&&
    !s->set_attempted&&!s->mapped&&!s->zero_attempted){
  nppa_native.active=false;return; /* no new owned state was acquired */
 }
 if(status<0||s->halt||s->uncertain||!s->balanced||!s->asleep||
    s->ap_owned||s->wf_owned)
  panic("NORD2 WIFI DMA: uncertain stage%u; retain resources; manual reset",s->stage);
 nppa_native.active=false;
}
