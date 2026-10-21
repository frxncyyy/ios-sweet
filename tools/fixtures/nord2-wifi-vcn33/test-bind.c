#define NVC_HOST_TEST 1
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <setjmp.h>
#include <stdbool.h>
typedef unsigned thread_t;typedef uintptr_t vm_offset_t;
static unsigned thread=7,opt=1,reads,writes,drains,panic_count,read_fail;static uint64_t now=1000;static jmp_buf escape;
static uint16_t regs[65536];
static thread_t current_thread(void){return thread;}
static void clock_get_uptime(uint64_t*t){*t=now;}
static void absolutetime_to_nanoseconds(uint64_t t,uint64_t*n){*n=t*1000;}
static void PE_parse_boot_argn(const char*k,unsigned*v,unsigned n){(void)k;(void)n;*v=opt;}
static bool npa_persistent_lifetime=true,ni_saved=true,ncc_fresh_owned=true,ncc_latched_dirty;
static int proof(void*c){(void)c;return 0;}
static struct {bool outer_retained;struct {unsigned entered,fatal;struct {unsigned phase;}result;}state;int(*proof)(void*);void*proof_cookie;}npa_native={true,{1,0,{10}},proof,0};
static struct {bool saved,domain_touched;vm_offset_t spm;}nw={true,true,1};
static vm_offset_t ni_pwrap=2;static bool ni_pwrap_owned;
static uint16_t pending;
static uint32_t ni_rd(vm_offset_t b,unsigned o){(void)b;assert(o==0x504);return 1;}
static void ni_wr(vm_offset_t b,unsigned o,uint32_t v){(void)b;if(o==0xc84)pending=(uint16_t)v;else{assert(o==0xc80&&(v>>29)==1);regs[v&0xffff]=pending;writes++;}}
static bool ni_pmic_read(uint16_t a,uint16_t*v){reads++;if(read_fail)return false;*v=regs[a];return true;}
static bool nw_wait(vm_offset_t b,unsigned o,unsigned m,unsigned v){(void)b;(void)o;(void)m;(void)v;return true;}
static void panic(const char*s){(void)s;panic_count++;longjmp(escape,1);}
#include "wifi_vcn33_bind.inc"
static void nwm_failure_drain(void){drains++;}
int main(void){
 regs[0x1be8]=regs[0x1c0e]=0x8000;regs[0x1f98]=0xaf05;regs[0x1f9c]=0xbf07;
 opt=0;nvc_after_persistent();assert(!reads&&!writes&&!nvc_native.enabled);opt=1;
 if(setjmp(escape))assert(0);nvc_after_persistent();assert(nvc_native.state.complete&&writes==5&&ni_pwrap_owned);
 unsigned old=reads;thread=8;uint16_t v;assert(nvc_read(&nvc_native,0x1be8,&v)<0&&reads==old);thread=7;
 assert(nvc_write(&nvc_native,0x18b4,0)<0&&writes==5);nvc_observe(2);nvc_mark(3);nvc_observe(3);
 old=reads;now+=100000;nvc_poll();assert(reads==old+13);old=reads;nvc_poll();assert(reads==old);
 read_fail=1;now+=100000;nvc_poll();assert(nvc_native.next==UINT64_MAX&&!nvc_native.halt&&!drains&&!panic_count);old=reads;now+=100000;nvc_poll();assert(reads==old);
 read_fail=0;now=nvc_native.start+30000000;nvc_poll();assert(reads==old);
 if(!setjmp(escape)){nvc_after_persistent();assert(0);}assert(panic_count==1&&drains==1&&writes==5);
 puts("PASS actual VCN33 binder disabled/wrongthread/whitelist/5writes/syncsnap/periodic throttle+readfailuredisable/reentryhalt");
}
