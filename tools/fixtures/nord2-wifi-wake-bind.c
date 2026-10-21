#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include "wifi_persistent_adapter.h"
typedef uintptr_t vm_offset_t;
static bool npa_persistent_lifetime,ni_saved,ncc_fresh_owned,ncc_latched_dirty;
static struct {vm_offset_t spm,infra,gals;bool saved,domain_touched;} nw;
static struct {struct npa state;vm_offset_t rgu,host,cfg;bool outer_retained;} npa_native;
static struct {unsigned opt,reads,writes,slept,wake,sleep,panic,irqbad,foreign,logs;} model;
static jmp_buf jump;
static int test_printf(const char *fmt,...){(void)fmt;model.logs++;return 0;}
static void panic(const char *fmt,...){(void)fmt;model.panic++;longjmp(jump,1);}
static void PE_parse_boot_argn(const char *name,unsigned *v,size_t n){assert(!strcmp(name,"nord2wifiwake")&&n==sizeof(*v));*v=model.opt;}
static uint32_t ni_rd(vm_offset_t b,unsigned off){assert(!model.slept);model.reads++;
 uint32_t p=(uint32_t)b+off;
 switch(p){
 case 0x10006304:return 0xd;case 0x1000616c:case 0x10006170:return 2;
 case 0x1020e504:return 1;case 0x10001228:case 0x10001724:return 0;
 case 0x180601a0:assert(!model.wake);return model.foreign;
 case 0x18060184:assert(model.wake==1);return 0;
 case 0x18060000:assert(model.wake==1);return 0x4006;
 case 0x180602d4:assert(model.wake==1);return !model.irqbad;
 case 0x18001000:assert(model.wake==1);return 0x20010101;
 case 0x18000010:case 0x18000000:case 0x180602cc:case 0x18000014:case 0x18000008:assert(model.wake==1);return 0;
 default:assert(0);return 0;
 }
}
static void ni_wr(vm_offset_t b,unsigned off,uint32_t v){assert(!model.slept);model.writes++;
 uint32_t p=(uint32_t)b+off;
 if(p==0x180601a0){if(v){assert(v==1&&!model.wake);model.wake++;}else{assert(model.wake==1);model.sleep++;model.slept=1;}}
 else assert(p==0x18060000&&v==0x4007&&model.wake==1);
}
static void delay(unsigned us){assert(!model.slept&&(us==1||us==10||us==1000));}
#define NCC_DEVICE_BARRIER() ((void)0)
#define printf test_printf
#include "wifi_wake_bind.inc"
#undef printf
static void reset(void){
 model=(__typeof__(model)){.opt=1};nw=(__typeof__(nw)){.spm=0x10006000,.infra=0x10001000,.gals=0x1020e000,.saved=true,.domain_touched=true};
 npa_native=(__typeof__(npa_native)){.rgu=0x18000000,.host=0x18060000,.cfg=0x18001000,.outer_retained=true};
 npa_native.state.entered=npa_native.state.retained=1;npa_native.state.hw_handoff=2;
 npa_native.state.result.phase=10;npa_native.state.result.retained=1;
 npa_persistent_lifetime=ni_saved=ncc_fresh_owned=true;ncc_latched_dirty=false;
 nawa_native=(__typeof__(nawa_native)){0};
}
int main(void){
 reset();model.opt=0;nawa_after_persistent();assert(!model.reads&&!model.writes&&!nawa_native.diag.result.entered&&!model.logs);
 reset();npa_native.state.hw_handoff=1;nawa_after_persistent();assert(!model.reads&&!model.writes&&!nawa_native.diag.result.entered&&model.logs==1);
 reset();__typeof__(npa_native) old=npa_native;
 nawa_after_persistent();assert(!memcmp(&old,&npa_native,sizeof(old))&&npa_persistent_lifetime&&ni_saved&&ncc_fresh_owned&&nw.saved&&nw.domain_touched);
 assert(nawa_native.diag.result.balanced&&nawa_native.diag.asleep&&model.wake==1&&model.sleep==1&&!model.panic&&model.logs==3);
 unsigned count=model.reads+model.writes;
 if(!setjmp(jump)){nawa_after_persistent();assert(0);}assert(model.panic==1&&model.reads+model.writes==count);
 reset();model.foreign=1;nawa_after_persistent();assert(!model.writes&&!model.panic&&!nawa_native.diag.result.reference&&!nawa_native.active);
 reset();model.irqbad=1;
 if(!setjmp(jump)){nawa_after_persistent();assert(0);}
 assert(model.panic==1&&model.wake==1&&!model.sleep&&nawa_native.diag.result.halt&&nawa_native.diag.result.reference==1);
 assert(npa_native.state.hw_handoff==2&&npa_native.state.result.phase==10&&npa_persistent_lifetime);
 puts("PASS actual nativebind defaultoff/softwareowner/refusal, exactAP/hostmaps, wakebalanced+noaftersleepMMIO, originalpersistentstatebyteunchanged, reentryzeroIO, IRQfailuremanualhalt/retainedvote");
}
