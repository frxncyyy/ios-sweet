#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include "wifi_persistent_adapter.h"
#include "wifi_wake_adapter.h"
typedef uintptr_t vm_offset_t;
static bool npa_persistent_lifetime,ni_saved,ncc_fresh_owned,ncc_latched_dirty;
static struct {vm_offset_t spm,infra,gals;bool saved,domain_touched;} nw;
static struct {struct npa state;vm_offset_t rgu,host,cfg,afe,debug,sem;bool outer_retained;} npa_native;
static struct {struct nawa diag;bool active;} nawa_native;
static struct {
 unsigned opt,reads,writes,maps,slept,ap,wf,sem,power,cpu,logs,panic,bad_id;
 uint32_t afe,prot,dma,idle,vdnr,emi,sema_reset;
} model;
static jmp_buf jump;
static int test_printf(const char *fmt,...){(void)fmt;model.logs++;return 0;}
static void panic(const char *fmt,...){(void)fmt;model.panic++;longjmp(jump,1);}
static void PE_parse_boot_argn(const char *name,unsigned *v,size_t n)
{assert(!strcmp(name,"nord2wifiwfsys")&&n==sizeof(*v));*v=model.opt;}
static vm_offset_t ml_io_map(uintptr_t p,size_t n)
{assert((p==0x1800e000||p==0x184b0000)&&n==0x1000);model.maps++;return p;}
static uint32_t ni_rd(vm_offset_t b,unsigned off)
{
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
 case 0x1800f004:return model.idle;
 case 0x18000018:return model.sema_reset;
 case 0x18001c14:return model.emi;
 case 0x18072050:assert(!model.sem);model.sem=1;return 1;
 case 0x184b0010:
  assert(model.ap&&model.wf&&model.power&&!model.cpu&&
         !(model.prot&1)&&!(model.dma&1)&&(model.vdnr&1));
  return model.bad_id?0:0x20010000;
 default:assert(0);return 0;
 }
}
static void ni_wr(vm_offset_t b,unsigned off,uint32_t v)
{
 assert(!model.slept);model.writes++;uint32_t p=(uint32_t)b+off;
 switch(p){
 case 0x180601a0:
  assert(v<=1);
  if(!v){assert(!model.wf&&!model.power&&!model.sem&&!model.cpu&&
                !(model.afe&0x300000));model.slept=1;}
  model.ap=v;break;
 case 0x180601a4:assert(model.ap);model.wf=v;break;
 case 0x18060000:assert((model.ap||model.wf)&&v==0x4007);break;
 case 0x18000010:assert(!(v&1));model.cpu=v;break;
 case 0x18003008:assert(model.sem);model.afe=v;break;
 case 0x18000000:
  assert((v&0xffff0000)==0x57460000&&!model.cpu);
  if(v&0x80)assert(model.wf&&(model.afe&0x300000)==0x300000);
  else assert((model.prot&1)&&(model.dma&1)&&!(model.afe&0x300000)&&!model.sem);
  model.power=!!(v&0x80);break;
 case 0x18001620:assert(model.power);model.prot=v&~8u;break;
 case 0x18001624:assert(model.power);model.dma=v&~8u;break;
 case 0x1800e06c:assert(model.power);model.vdnr=v;break;
 case 0x1800f004:model.idle=v;break;
 case 0x18000018:assert(!(v&1));model.sema_reset=v;break;
 case 0x18001c14:assert(!model.power);model.emi=v;break;
 case 0x18072250:assert(v==1&&model.sem);model.sem=0;break;
 default:assert(0);
 }
}
static void delay(unsigned us)
{assert(!model.slept&&(us==1||us==10||us==30||us==50||us==500||us==1000));}
#define NCC_DEVICE_BARRIER() ((void)0)
#define printf test_printf
#include "wifi_wfsys_bind.inc"
#undef printf
static void reset(void)
{
 model=(__typeof__(model)){.opt=1,.prot=1,.dma=1,.sema_reset=1};
 nw=(__typeof__(nw)){.spm=0x10006000,.infra=0x10001000,.gals=0x1020e000,.saved=true,.domain_touched=true};
 npa_native=(__typeof__(npa_native)){.rgu=0x18000000,.host=0x18060000,.cfg=0x18001000,
  .afe=0x18003000,.debug=0x1800f000,.sem=0x18072000,.outer_retained=true};
 npa_native.state.entered=npa_native.state.retained=1;
 npa_native.state.hw_handoff=2;npa_native.state.result.phase=10;npa_native.state.result.retained=1;
 npa_persistent_lifetime=ni_saved=ncc_fresh_owned=true;ncc_latched_dirty=false;
 nawa_native=(__typeof__(nawa_native)){0};nwsa_native=(__typeof__(nwsa_native)){0};
}
int main(void)
{
 reset();model.opt=0;nwsa_after_persistent();assert(!model.maps&&!model.reads&&!model.writes&&!model.logs);
 reset();npa_native.state.hw_handoff=1;nwsa_after_persistent();assert(!model.maps&&!model.reads&&!model.writes&&model.logs==1);
 reset();nawa_native.diag.result.entered=1;nwsa_after_persistent();assert(!model.maps&&!model.reads&&!model.writes);
 reset();__typeof__(npa_native) old=npa_native;
 nwsa_after_persistent();assert(!memcmp(&old,&npa_native,sizeof(old))&&npa_persistent_lifetime);
 assert(model.maps==2&&model.slept&&!model.panic&&model.logs==5);
 assert(nwsa_native.diag.state.balanced&&nwsa_native.diag.state.cpu_held&&nwsa_native.diag.asleep&&!nwsa_native.active);
 unsigned count=model.maps+model.reads+model.writes;
 if(!setjmp(jump)){nwsa_after_persistent();assert(0);}
 assert(model.panic==1&&count==model.maps+model.reads+model.writes);
 reset();assert(nwsa_xnu_write(0,0x18000010,1)==-1&&!model.writes);
 assert(nwsa_xnu_write(0,0x180602cc,0)==-1&&!model.writes);
 reset();model.bad_id=1;
 if(!setjmp(jump)){nwsa_after_persistent();assert(0);}
 assert(model.panic==1&&model.ap&&model.wf&&model.power&&!model.slept&&
        nwsa_native.diag.state.halt&&npa_persistent_lifetime&&!model.cpu);
 puts("PASS actual WFSYS native binding: default-off and owner/wake-conflict no maps/IO; exact maps and ordered CPU-held cycle; old persistent state unchanged; no IO after AP sleep; reentry and CPU-release rejection; local-ID timeout retains resources");
}
