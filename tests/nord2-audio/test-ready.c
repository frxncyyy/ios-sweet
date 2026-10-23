#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "audio_hw.h"
#include "audio_native.h"
typedef uintptr_t vm_offset_t;
typedef uintptr_t thread_t;
typedef size_t vm_size_t;
#define PAGE_SIZE 0x4000u
static thread_t caller=1;
static thread_t current_thread(void) { return caller; }
static vm_offset_t ni_gpio,ni_lt;
static struct { vm_offset_t infra; } nw;
static unsigned maps,fail_map,barriers,delays,commands_seen,pauses,publish_at;
static uint64_t fake_us;
static struct nau_hw hardware;
static _Alignas(16384) unsigned char memory[9][16384];
static void barrier(void) { barriers++;__atomic_thread_fence(__ATOMIC_SEQ_CST); }
#define NAH_NATIVE_BARRIER() barrier()
static void delay(unsigned us) { (void)us;delays++; }
int naf_write_allowed(uint32_t pa) { return pa==NAF_AFE_DAC_CON0; }
#define NAH_NATIVE_DEFINE_PREPARE 1
#include "audio_hw_native.inc"
vm_offset_t ml_io_map(vm_offset_t pa,vm_size_t bytes)
{
 assert(nah_native_dependency_state()==NAH_NATIVE_READY);
 assert(bytes==0x4000);
 if (++maps==fail_map) return 0;
 for(unsigned i=0;i<9;i++)if(pa==nah_native_bases[i])return (vm_offset_t)memory[i];
 assert(0);return 0;
}
static void shared_maps(void)
{
 nw.infra=(vm_offset_t)memory[0]+0x1000;
 ni_gpio=(vm_offset_t)memory[1]+0x1000;ni_lt=(vm_offset_t)memory[8];
}
static uint64_t now_us(void) { return fake_us; }
static void commands(void)
{ assert(hardware.status.error==NAH_NATIVE_WAIT);commands_seen++; }
static void native_wait_pause(void)
{
 assert(!nah_native_attempted && maps==0 && !barriers && !delays);
 fake_us+=100000;
 if(++pauses==publish_at) {
  thread_t saved=caller;caller=2; /* NI producer publishes to audio worker1. */
  shared_maps();assert(nah_native_publish_ready()==NAH_NATIVE_READY);caller=saved;
 }
}
#include "ready-worker-helper.inc"
int main(int argc,char **argv)
{
 assert(argc==2);
 struct nah_io out={0};void *cookie=(void*)0x1234;
 if(!strcmp(argv[1],"wait-publish")) {
  for(unsigned i=0;i<25;i++)assert(nah_native_prepare(&out,&cookie)==NAH_NATIVE_WAIT);
  assert(!nah_native_attempted && !maps && cookie==(void*)0x1234 && !out.read);
  shared_maps(); /* Existence alone is not publication / completion. */
  assert(nah_native_prepare(&out,&cookie)==NAH_NATIVE_WAIT && !maps);
  assert(nah_native_publish_ready()==NAH_NATIVE_READY);
  nw.infra=ni_gpio=ni_lt=0; /* Consumer uses immutable published copies. */
  assert(nah_native_prepare(&out,&cookie)==NAH_NATIVE_READY && maps==6);
  assert(nah_native_valid(cookie));
  assert(nah_native_prepare(&out,&cookie)==NAH_NATIVE_E_REENTRY && maps==6);
  caller=3;
  uint32_t value=0;
  assert(out.worker(cookie)==0);
  assert(out.read(cookie,NAF_AFE_DAC_CON0,4,&value)==-1);
  assert(out.write(cookie,NAF_AFE_DAC_CON0,4,0)==-1);
 } else if(!strcmp(argv[1],"publish-failed")) {
  ni_gpio=(vm_offset_t)memory[1]+0x1000;
  assert(nah_native_publish_ready()==NAH_NATIVE_E_DEPENDENCIES);
  shared_maps();assert(nah_native_publish_ready()==NAH_NATIVE_E_DEPENDENCIES);
  assert(nah_native_prepare(&out,&cookie)==NAH_NATIVE_E_DEPENDENCIES && !maps);
 } else if(!strcmp(argv[1],"publish-unaligned")) {
  shared_maps();ni_gpio+=4;
  assert(nah_native_publish_ready()==NAH_NATIVE_E_WINDOW);
  assert(nah_native_prepare(&out,&cookie)==NAH_NATIVE_E_WINDOW && !maps);
 } else if(!strcmp(argv[1],"map-once")) {
  shared_maps();assert(nah_native_publish_ready()==0);fail_map=2;
  assert(nah_native_prepare(&out,&cookie)==NAH_NATIVE_E_MAP && maps==2);
  fail_map=0;
  assert(nah_native_prepare(&out,&cookie)==NAH_NATIVE_E_REENTRY && maps==2);
 } else if(!strcmp(argv[1],"no-context")) {
  shared_maps();assert(nah_native_publish_ready()==0);caller=0;
  assert(nah_native_prepare(&out,&cookie)==NAH_NATIVE_E_CONTEXT && !maps && !nah_native_attempted);
 } else if(!strcmp(argv[1],"worker-ready")) {
  publish_at=3;
  assert(prepare_native_deferred(&out,&cookie)==0);
  assert(pauses==3 && commands_seen==3 && fake_us==300000 && maps==6);
 } else if(!strcmp(argv[1],"worker-timeout")) {
  assert(prepare_native_deferred(&out,&cookie)==NAH_NATIVE_E_TIMEOUT);
  assert(pauses==600 && commands_seen==601 && fake_us==60000000);
  assert(!maps && !nah_native_attempted);
 } else if(!strcmp(argv[1],"worker-failed")) {
  assert(nah_native_publish_ready()==NAH_NATIVE_E_DEPENDENCIES);
  assert(prepare_native_deferred(&out,&cookie)==NAH_NATIVE_E_DEPENDENCIES);
  assert(!maps && !pauses && !commands_seen);
 } else if(!strcmp(argv[1],"worker-map-once")) {
  shared_maps();assert(nah_native_publish_ready()==0);fail_map=1;
  assert(prepare_native_deferred(&out,&cookie)==NAH_NATIVE_E_MAP);
  fail_map=0;
  assert(prepare_native_deferred(&out,&cookie)==NAH_NATIVE_E_REENTRY);
  assert(maps==1 && !pauses && !commands_seen);
 } else assert(0);
 assert(!barriers && !delays); /* No MMIO callback or hardware delay ran. */
 puts("PASS actual native bridge publication / bounded worker dependency wait");
}
