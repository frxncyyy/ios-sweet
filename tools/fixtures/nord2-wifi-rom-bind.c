#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include "wifi_persistent_adapter.h"
#include "wifi_wake_adapter.h"
#include "wifi_wfsys_adapter.h"
#include "wifi_rom_powered_adapter.h"
typedef uintptr_t vm_offset_t;
static bool npa_persistent_lifetime,ni_saved,ncc_fresh_owned,ncc_latched_dirty;
static struct {vm_offset_t spm,infra,gals;bool saved,domain_touched;} nw;
static struct {struct npa state;vm_offset_t rgu,host,cfg,afe,debug,sem;bool outer_retained;} npa_native;
static struct {struct nawa diag;bool active;} nawa_native;
static struct {struct nwsa diag;bool active;} nwsa_native;
static struct {
 unsigned opt,reads,writes,maps,slept,ap,wf,sem,power,cpu,logs,panic;
 unsigned prep,policy,place,cpu_arm,debug_write,bad_prep,bad_policy;
 unsigned bad_receipt,bad_done,map_fail;
 uint32_t afe,prot,dma,vdnr,sema_reset;
} model;
static jmp_buf jump;
static int test_printf(const char *fmt,...){
 if(strstr(fmt,"ROM CPU ARM"))model.cpu_arm++;
 model.logs++;return 0;
}
static void panic(const char *fmt,...){(void)fmt;model.panic++;longjmp(jump,1);}
static void PE_parse_boot_argn(const char *name,unsigned *v,size_t n)
{assert(!strcmp(name,"nord2wifirom")&&n==sizeof(*v));*v=model.opt;}
static vm_offset_t ml_io_map(uintptr_t p,size_t n){
 assert((p==0x1800e000||p==0x184b0000||p==0x18500000||p==0x184c1000)&&n==0x1000);
 model.maps++;return model.map_fail&&p==0x18500000?0:p;
}
static uint32_t ni_rd(vm_offset_t b,unsigned off){
 assert(!model.slept);model.reads++;uint32_t p=(uint32_t)b+off;
 switch(p){
 case 0x10006304:return 0xd;
 case 0x1000616c:case 0x10006170:return 2;
 case 0x1020e504:return 1;
 case 0x10001228:case 0x10001724:return 0;
 case 0x180601a0:return model.ap;
 case 0x180601a4:assert(model.ap);return model.wf;
 case 0x18060184:assert(model.ap||model.wf);return 0;
 case 0x18060000:assert(model.ap||model.wf);return 0x4006;
 case 0x180602d4:assert(model.ap||model.wf);return 1;
 case 0x18001000:assert(model.ap);return 0x20010101;
 case 0x18000010:return model.cpu;
 case 0x18000000:return 0x12|(model.power?0x80:0);
 case 0x180602cc:return 0x582fb00|(model.power?4:0);
 case 0x18000014:return 0;
 case 0x18000008:return 0x12;
 case 0x18003008:return model.afe;
 case 0x18001620:return model.prot|(model.prot&1?8:0);
 case 0x18001624:return model.dma|(model.dma&1?8:0);
 case 0x1800e06c:return model.vdnr;
 case 0x18000018:return model.sema_reset;
 case 0x18072050:assert(!model.sem);model.sem=1;return 1;
 case 0x184b0010:
  assert(model.ap&&model.wf&&model.power&&!model.cpu&&
         !(model.prot&1)&&!(model.dma&1)&&(model.vdnr&1));
  return 0x20010000;
 case 0x184c1604:
  assert(model.cpu&&model.place);return model.bad_done?0:0x1d1e;
 default:assert(0);return 0;
 }
}
static void ni_wr(vm_offset_t b,unsigned off,uint32_t v){
 assert(!model.slept);model.writes++;uint32_t p=(uint32_t)b+off;
 switch(p){
 case 0x180601a0:
  assert(v<=1);
  if(!v){assert(!model.wf&&model.power&&!model.sem&&model.cpu&&
                !(model.afe&0x300000));model.slept=1;}
  model.ap=v;break;
 case 0x180601a4:assert(model.ap);model.wf=v;break;
 case 0x18060000:assert((model.ap||model.wf)&&v==0x4007);break;
 case 0x18000010:
  if(v&1u)assert(model.place&&model.cpu_arm==1);
  model.cpu=v;break;
 case 0x18003008:assert(model.sem);model.afe=v;break;
 case 0x18000000:
  assert((v&0xffff0000)==0x57460000&&!model.cpu&&model.wf&&
         (model.afe&0x300000)==0x300000);
  model.power=!!(v&0x80);break;
 case 0x18001620:assert(model.power);model.prot=v&~8u;break;
 case 0x18001624:assert(model.power);model.dma=v&~8u;break;
 case 0x1800e06c:assert(model.power);model.vdnr=v;break;
 case 0x18000018:assert(!(v&1));model.sema_reset=v;break;
 case 0x18072250:assert(v==1&&model.sem);model.sem=0;break;
 case 0x18001120:assert(v==0x89040000&&model.place&&!model.cpu);break;
 case 0x185001dc:case 0x185001e0:assert(!v&&model.place&&!model.cpu);break;
 case 0x1800f000:assert(v==0x32c8001c&&model.cpu);model.debug_write++;break;
 default:assert(0);
 }
}
static void delay(unsigned us)
{assert(!model.slept&&(us==1||us==10||us==30||us==50||us==500||us==1000));}
int nrb_xnu_prepare(struct nrb_native *n){
 assert(!model.reads&&!model.writes&&!model.maps);model.prep++;
 n->attempted=1;if(model.bad_prep)return -5;
 n->enabled=1;n->state.prepared=1;n->state.phase=1;
 n->state.plan.spans[0].bytes=49820;n->state.plan.spans[1].bytes=16;
 n->state.plan.spans[2].bytes=296868;n->state.plan.spans[3].bytes=16;
 n->claim.source=NRC_CLAIM_IOS_FIXED;return 0;
}
int nrb_xnu_policy(struct nrb_native *n,int (*owner)(void *,enum nrb_owner_stage),void *cookie){
 assert(owner(cookie,NRB_COLD_BEFORE_POWER)==0);
 model.policy++;n->fresh_owner=owner;n->owner_cookie=cookie;
 if(model.bad_policy){n->state.policy_attempted=1;n->state.issued=1;
  n->state.accepted=1;n->state.raw_valid=1;n->state.halt=1;return -1;}
 n->state.policy_attempted=1;n->state.issued=2;n->state.accepted=2;
 n->state.raw_valid=3;n->state.raw[0][0]=n->state.raw[1][0]=0;
 n->state.phase=3;return 0;
}
int nrb_xnu_place(struct nrb_native *n){
 assert(n->fresh_owner(n->owner_cookie,NRB_POWERED_CPU_HELD)==0);
 model.place++;n->state.mapped=n->state.backup_complete=n->state.write_attempted=1;
 n->state.writes=88;n->state.reads=176;n->state.barriers=2;
 n->state.placed=1;n->state.phase=model.bad_receipt?6:7;return 0;
}
#define NCC_DEVICE_BARRIER() ((void)0)
#define printf test_printf
#include "wifi_rom_bind.inc"
#undef printf
static void reset(void){
 model=(__typeof__(model)){.opt=1,.prot=1,.dma=1,.sema_reset=1};
 nw=(__typeof__(nw)){.spm=0x10006000,.infra=0x10001000,
  .gals=0x1020e000,.saved=true,.domain_touched=true};
 npa_native=(__typeof__(npa_native)){.rgu=0x18000000,.host=0x18060000,
  .cfg=0x18001000,.afe=0x18003000,.debug=0x1800f000,
  .sem=0x18072000,.outer_retained=true};
 npa_native.state.entered=npa_native.state.retained=1;
 npa_native.state.hw_handoff=2;npa_native.state.result.phase=10;
 npa_native.state.result.retained=1;
 npa_persistent_lifetime=ni_saved=ncc_fresh_owned=true;
 ncc_latched_dirty=false;nawa_native=(__typeof__(nawa_native)){0};
 nwsa_native=(__typeof__(nwsa_native)){0};nrpa_native=(__typeof__(nrpa_native)){0};
}
int main(void){
 reset();model.opt=0;nrpa_prepare_before_power();nrpa_after_persistent();
 assert(!model.prep&&!model.maps&&!model.reads&&!model.writes&&!model.logs);
 reset();model.bad_prep=1;nrpa_prepare_before_power();nrpa_after_persistent();
 assert(model.prep==1&&!model.maps&&!model.reads&&!model.writes&&!model.panic);
 reset();nrpa_prepare_before_power();nwsa_native.diag.state.entered=1;
 nrpa_after_persistent();assert(!model.maps&&!model.reads&&!model.writes);
 reset();nrpa_prepare_before_power();model.map_fail=1;
 nrpa_after_persistent();assert(model.maps==4&&!model.reads&&!model.writes&&
  !model.policy&&!model.panic&&nrpa_native.run_attempted&&!nrpa_native.active);
 reset();nrpa_prepare_before_power();__typeof__(npa_native) old=npa_native;
 nrpa_after_persistent();
 assert(!memcmp(&old,&npa_native,sizeof(old))&&npa_persistent_lifetime);
 assert(model.maps==4&&model.slept&&model.cpu&&model.power&&
  !model.panic&&model.policy==1&&model.place==1&&model.cpu_arm==1&&
  model.debug_write==1&&nrpa_native.diag.state.balanced&&
  nrpa_native.diag.state.stage==NRPC_RUNNING&&!nrpa_native.active);
 unsigned count=model.maps+model.reads+model.writes;
 if(!setjmp(jump)){nrpa_after_persistent();assert(0);}
 assert(model.panic==1&&count==model.maps+model.reads+model.writes);
 reset();nrpa_prepare_before_power();assert(nrpa_xnu_write(0,0x18000010,1)==-1&&!model.writes);
 assert(nrpa_xnu_write(0,0x180602cc,0)==-1&&!model.writes);
 reset();nrpa_prepare_before_power();model.bad_receipt=1;
 if(!setjmp(jump)){nrpa_after_persistent();assert(0);}
 assert(model.panic==1&&model.ap&&model.wf&&model.power&&!model.cpu&&
  nrpa_native.diag.state.halt&&npa_persistent_lifetime);
 reset();nrpa_prepare_before_power();model.bad_policy=1;
 if(!setjmp(jump)){nrpa_after_persistent();assert(0);}
 assert(model.panic==1&&model.ap&&model.wf&&!model.power&&!model.cpu&&
  nrpa_native.diag.state.halt&&npa_persistent_lifetime);
 reset();nrpa_prepare_before_power();model.bad_done=1;
 if(!setjmp(jump)){nrpa_after_persistent();assert(0);}
 assert(model.panic==1&&model.ap&&model.wf&&model.power&&model.cpu&&
  nrpa_native.diag.state.cpu_release_attempted&&
  nrpa_native.diag.state.rom_polls==102);
 puts("PASS actual ROM native binding: opt-in/preparation/conflict no IO; exact four maps; same native object; policy/placement before guarded CPU release; ROM_DONE and final sleep; retained failures");
}
