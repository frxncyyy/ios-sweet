#include "wifi_main_fw_xnu.h"
#include "wifi_main_phy_xnu.h"
#include "wifi_main_wfdma_native.h"
#include "wifi_main_halt.h"
#include <wifi_rom_xnu.h>
#include <wifi_rom_powered_adapter.h>
#include <sys/errno.h>
#include <sys/systm.h>
#include <pexpert/pexpert.h>
extern vm_offset_t ml_io_map(vm_offset_t,vm_size_t);
extern void delay(int);
struct nfp_native nfp_native;
static int halt(void){
 nfp_native.halt=1;
 return nwm_lifetime_halt(&nwfn_native);
}
static int flags(void){
 unsigned m=0,r=0,d=0;
 return !PE_parse_boot_argn("nord2wifimain",&m,sizeof(m))||m!=1||
        !PE_parse_boot_argn("nord2wifirom",&r,sizeof(r))||r!=1||
        !PE_parse_boot_argn("nord2wifidma",&d,sizeof(d))||d!=1;
}
static int overlaps(uint64_t a,uint32_t n,uint64_t b,uint32_t m){
 return a<b+(uint64_t)m&&b<a+(uint64_t)n;
}
static int spans_disjoint(const struct nrb_native *r){
 const struct nmt_span *e[3]={&nfp_native.layout.patch_date,
  &nfp_native.layout.main[3],&nfp_native.layout.main[4]};
 for(unsigned i=0;i<3;i++){
  for(unsigned j=i+1;j<3;j++)
   if(overlaps(e[i]->host_physical,e[i]->length,
               e[j]->host_physical,e[j]->length))return -1;
  for(unsigned j=0;j<NWF_ROM_SPANS;j++){
   const struct nwf_rom_span *s=&r->state.plan.spans[j];
   if(overlaps(e[i]->host_physical,e[i]->length,s->physical,s->bytes))
    return -1;
  }
 }
 return 0;
}
static int same(void){
 const struct nrb_native *r=nwfn_native.rom;
 const struct npp_state *p=nwfn_native.pool;
 const struct nwi_state *i=&nwfn_native.init;
 return !nfp_native.prepared||nfp_native.halt||!r||!p||!r->enabled||
  !nrpc_receipt_ok(r)||!r->io||
  p!=i->pool||p->stage!=NPP_DONE||!p->mapped||
  !p->verified||!p->zero_complete||!p->balanced||!p->asleep||p->halt||
  !i->initialized||i->phase!=5||i->halt||!i->driver_owned||
  i->transport.phase!=2||i->transport.halt||
  nwfn_native.stage!=NWFN_RETAINED||!nwfn_native.ap_owned||
  !nwfn_native.wf_owned||nwfn_native.asleep||nwfn_native.halt||
  nwfn_native.claim.base!=UINT64_C(0x90000000)||
  nwfn_native.claim.bytes!=UINT64_C(0xf20000);
}
static int owner(void *c){(void)c;return same()||nwfn_owner(&nwfn_native);}
static const struct nmt_span *current(void){
 unsigned st=nfp_native.state.stage;
 if(st==NFP_PATCH_PAYLOAD)return &nfp_native.layout.patch;
 if(st==NFP_PATCH_DATE)return &nfp_native.layout.patch_date;
 if(st>=NFP_N9_0&&st<=NFP_N9_4)return &nfp_native.layout.main[st-NFP_N9_0];
 return 0;
}
static int remap(void *c,uint32_t target){
 (void)c;const struct nmt_span *s=current();
 if(!s||s->space!=NMT_AP2WF_WINDOW||
    s->firmware_target!=target||!nfp_native.cfg_io||owner(c))return -1;
 *(volatile uint32_t *)(nfp_native.cfg_io+0x120)=target;
 __asm__ volatile("dsb sy":: :"memory");return 0;
}
static int address(enum nmt_space space,uint32_t pa){
 const struct nmt_span *s=current();uint32_t start;
 if(!s||s->space!=space||(pa&3))return -1;
 start=space==NMT_AP2WF_WINDOW?UINT32_C(0x18500000):s->host_physical;
 return pa<start||pa-start>s->length-4u;
}
static int write32(void *c,enum nmt_space space,uint32_t pa,uint32_t value){
 (void)c;if(address(space,pa)||!nfp_native.window_io||!nwfn_native.rom->io)return -1;
 uintptr_t base=space==NMT_AP2WF_WINDOW?nfp_native.window_io:nwfn_native.rom->io;
 uint32_t offset=space==NMT_AP2WF_WINDOW?pa-UINT32_C(0x18500000):
  pa-UINT32_C(0x8fb00000);
 *(volatile uint32_t *)(base+offset)=value;return 0;
}
static int read32(void *c,enum nmt_space space,uint32_t pa,uint32_t *value){
 (void)c;if(!value||address(space,pa)||!nfp_native.window_io||!nwfn_native.rom->io)return -1;
 uintptr_t base=space==NMT_AP2WF_WINDOW?nfp_native.window_io:nwfn_native.rom->io;
 uint32_t offset=space==NMT_AP2WF_WINDOW?pa-UINT32_C(0x18500000):
  pa-UINT32_C(0x8fb00000);
 *value=*(volatile const uint32_t *)(base+offset);return 0;
}
static int barrier(void *c){(void)c;__asm__ volatile("dsb sy":: :"memory");return 0;}
static const struct nfp_io io={owner,remap,write32,read32,barrier};
int nfp_xnu_prepare(uintptr_t cfg_io,uintptr_t host_io){
 const struct nrb_native *r=nwfn_native.rom;
 if(flags())return EPERM;
 if(nfp_native.attempted)return EALREADY;
 nfp_native.attempted=1;
 if(!cfg_io||!host_io||!nwfn_native.prepared||!r||!r->enabled||!r->output||
    !nrpc_receipt_ok(r)||!nwfn_native.pool||
    !nwfn_native.pool->verified||!nwfn_native.pool->asleep||
    r->blobs[0].size!=973648u||r->blobs[1].size!=5726u||
    (uintptr_t)r->blobs[0].data<(uintptr_t)r->output||
    (uintptr_t)r->blobs[1].data<(uintptr_t)r->output||
    (uintptr_t)r->blobs[0].data-(uintptr_t)r->output>
      NWF_OUTPUT_BYTES-r->blobs[0].size||
    (uintptr_t)r->blobs[1].data-(uintptr_t)r->output>
      NWF_OUTPUT_BYTES-r->blobs[1].size||
    nmt_parse_fixed_layout(r->blobs[0].data,r->blobs[0].size,
                           r->blobs[1].data,r->blobs[1].size,&nfp_native.layout)||
    spans_disjoint(r))
  return EINVAL;
 nfp_native.cfg_io=cfg_io;nfp_native.host_io=host_io;
 nfp_native.window_io=(uintptr_t)ml_io_map(0x18500000,0x10000);
 if(!nfp_native.window_io)return ENOMEM;
 nfp_native.coex_io=(uintptr_t)ml_io_map(0x10003000,0x1000);
 if(!nfp_native.coex_io)return ENOMEM;
 nfp_native.prepared=1;return 0;
}
int nfp_xnu_span(unsigned index){
 if(same())return halt();
 if(index==0&&!nfp_native.pre_sync_valid){
  if(nwfn_owner(&nwfn_native))return halt();
  nfp_native.pre_sync=*(volatile const uint32_t *)(nfp_native.host_io+0xf0);
  nfp_native.pre_sync_valid=1;
  if(nfp_native.pre_sync&3u)return halt();/* inherited running FW */
 }
 if(index<=1){
  if(nwfn_native.init.transport.completed||
     nwfn_native.init.transport.finish_phase||nfp_native.coex_attempted)return halt();
 }else if(index<=6){
  if(nwfn_native.init.transport.completed!=2||
     nwfn_native.init.transport.finish_phase!=1||
     !nfa_native.exchange.event_valid||
     !nfa_native.exchange.backup_valid||
     !nfa_native.exchange.backup.valid||
     !nfp_native.coex_done)return halt();
 }
 printf("NORD2 WIFI MAIN SPAN ARM: index=%u stage=%u finish=%u events=%u coex=%u\n",
  index,nfp_native.state.stage,nwfn_native.init.transport.finish_phase,
  nwfn_native.init.transport.completed,nfp_native.coex_done);
 int e=nfp_span(&io,&nwfn_native,&nfp_native.layout,&nfp_native.state,index);
 return e?halt():0;
}
int nfp_xnu_coex(void){
 if(same()||nwfn_owner(&nwfn_native)||
    nfp_native.coex_attempted||!nfp_native.coex_io||
    nfp_native.state.completed!=3u||
    nwfn_native.init.transport.finish_phase!=1||
    nwfn_native.init.transport.completed!=2||
    !nfa_native.prepared||nfa_native.board.coant||
    !nfa_native.exchange.event_valid||
    !nfa_native.exchange.backup_valid||
    !nfa_native.exchange.backup.valid)return halt();
 nfp_native.coex_attempted=1;
 printf("NORD2 WIFI MAIN PCCIF ARM: addr=%#x first=0 second=1 cal_emi=%#x/%#x\n",
  0x10003314u,nfa_native.exchange.event.emi_offset,
  nfa_native.exchange.event.emi_bytes);
 *(volatile uint32_t *)(nfp_native.coex_io+0x314)=0;
 __asm__ volatile("dsb sy":: :"memory");
 *(volatile uint32_t *)(nfp_native.coex_io+0x314)=1;
 __asm__ volatile("dsb sy":: :"memory");
 nfp_native.coex_done=1;return 0;
}
int nfp_xnu_finish(void){
 if(same()||nwfn_owner(&nwfn_native)||nfp_native.finish_attempted||
    nfp_native.state.stage!=NFP_COPIED||
    nfp_native.state.completed!=0x7fu||!nfp_native.coex_done||
    nwfn_native.init.transport.finish_phase!=1||
    nwfn_native.init.transport.completed!=2||
    !nfa_native.exchange.backup_valid)return halt();
 nfp_native.finish_attempted=1;
 printf("NORD2 WIFI MAIN FINISH ARM: spans=%#x cal=%u coex=%u cid=%#x\n",
  nfp_native.state.completed,nfa_native.exchange.backup_valid,
  nfp_native.coex_done,0x41u);
 if(nwfn_finish_command(0x41))return halt();
 nfp_native.finish_done=1;
 for(unsigned i=0;i<500;i++){
  if(i)delay(5000);
  if(nwfn_owner(&nwfn_native))return halt();
  nfp_native.ready_raw=*(volatile const uint32_t *)(nfp_native.host_io+0xf0);
  nfp_native.ready_polls++;
  if((nfp_native.ready_raw&3u)==3u){nfp_native.ready=1;return 0;}
 }
 return halt();
}
