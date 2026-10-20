#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <setjmp.h>
#include "wifi_common.h"
#include "wifi_cold_snapshot.h"
typedef uintptr_t vm_offset_t;
static struct {vm_offset_t spm,infra,gals;bool saved,domain_touched;} nw;
static bool ncc_fresh_owned,ncc_latched_dirty;
static unsigned enabled,maps,reads,writes,logs,ap_fail,pred_fail,map_fail,write_fail,drop_after_write,outer_restores;
static bool trace_ap;static unsigned trace_count;static uint32_t trace[8];
static uint32_t regs[40],original[40];
static unsigned order[80];
static jmp_buf panic_target;
static int PE_parse_boot_argn(const char*k,unsigned*v,unsigned n){assert(!strcmp(k,"nord2wificommon")&&n==4);*v=enabled;return 1;}
static vm_offset_t ml_io_map(uint64_t a,unsigned n){assert(n==4096&&(a==0x18000000||a==0x18004000||a==0x18005000||a==0x18008000));maps++;return maps==map_fail?0:a;}
static unsigned table_index(uint32_t a){for(unsigned i=0;i<40;i++)if(ncc_table[i].address==a)return i;assert(0);return 0;}
static uint32_t ni_rd(vm_offset_t,unsigned);
static void ni_wr(vm_offset_t b,unsigned o,uint32_t v){unsigned i=table_index(b+o);assert(writes<80);order[writes++]=i;regs[i]=v^(writes==write_fail?1u:0u);}
static int logline(const char*f,...){char line[256];va_list a;va_start(a,f);int n=vsnprintf(line,sizeof(line),f,a);va_end(a);assert(n>=0&&n<255);logs++;return 0;}
static void panic(const char*s){assert(strstr(s,"dirty restoration"));assert(ncc_latched_dirty&&nw.saved&&nw.domain_touched&&ncc_fresh_owned);longjmp(panic_target,1);}
#define printf logline
#define NCC_DEVICE_BARRIER() ((void)0)
#define NCC_TICKS() UINT64_C(42)
#define NCC_FREQUENCY() UINT64_C(13000000)
#include "wifi_common_adapter.inc"
#undef printf
static uint32_t ni_rd(vm_offset_t b,unsigned o){uint32_t a=b+o;reads++;if(trace_ap){assert(trace_count<8);trace[trace_count++]=a;}
 switch(a){
 case 0x10006304:return ap_fail==1||(drop_after_write&&writes)?0:0xd;
 case 0x1000616c:return ap_fail==2?0:2;
 case 0x10006170:return ap_fail==3?0:2;
 case 0x1020e504:return ap_fail==4?0:1;
 case 0x10001228:return ap_fail==5?0x6000:0;
 case 0x10001724:return ap_fail==6?4:0;
 case 0x18060184:return ap_fail==7?0x20:0;
 }
 for(unsigned i=0;i<14;i++)if(ncc_predicates[i].address==a)return pred_fail==i+1?ncc_predicates[i].mask:0;
 return regs[table_index(a)];
}
static void reset(void){memset(&ncc_native,0,sizeof(ncc_native));nw.spm=0x10006000;nw.infra=0x10001000;nw.gals=0x1020e000;nw.saved=nw.domain_touched=ncc_fresh_owned=true;ncc_latched_dirty=false;enabled=1;maps=reads=writes=logs=ap_fail=pred_fail=map_fail=write_fail=drop_after_write=outer_restores=0;for(unsigned i=0;i<40;i++)regs[i]=original[i]=0x12340000+i;}
/* Outer caller model is tied to exact dirty guards in candidate by Python. */
static int runwindow(void){if(setjmp(panic_target))return -1;ncc_power_window(0x18060000,0x18001000,0x20010101);assert(!ncc_latched_dirty);outer_restores++;return 0;}
int main(void){
 const uint32_t expected_ap[7]={0x10006304,0x1000616c,0x10006170,0x1020e504,0x10001228,0x10001724,0x18060184};
 for(unsigned f=0;f<=7;f++){
  reset();ncc_native.host=0x18060000;ncc_native.id=0x20010101;
  memset(&ncc_ap_failure,0,sizeof(ncc_ap_failure));ap_fail=f;trace_count=0;trace_ap=true;
  assert(ncc_ap_check()==(f?-1:0));trace_ap=false;
  unsigned count=f?f:7;assert(trace_count==count&&!memcmp(trace,expected_ap,count*sizeof(*trace)));
  if(f){assert(ncc_ap_failure.captured&&ncc_ap_failure.owner_ok==0x1ff&&!ncc_ap_failure.owner_fail);
   assert(ncc_ap_failure.valid_mask==((1u<<f)-1)&&ncc_ap_failure.failed==f-1);
   for(unsigned i=f;i<7;i++)assert(!ncc_ap_failure.raw[i]);
   uint32_t saved_raw[7];memcpy(saved_raw,ncc_ap_failure.raw,sizeof(saved_raw));
   unsigned saved=ncc_ap_failure.failed;ap_fail=7;assert(ncc_ap_check()==-1&&ncc_ap_failure.failed==saved);
   assert(!memcmp(saved_raw,ncc_ap_failure.raw,sizeof(saved_raw)));ap_fail=0;assert(!ncc_ap_check());
   assert(ncc_ap_failure.failed==saved&&!memcmp(saved_raw,ncc_ap_failure.raw,sizeof(saved_raw)));
  }else assert(!ncc_ap_failure.captured);
 }
 for(unsigned f=0;f<9;f++){
  reset();ncc_native.host=0x18060000;ncc_native.id=0x20010101;
  memset(&ncc_ap_failure,0,sizeof(ncc_ap_failure));
  switch(f){case 0:ncc_fresh_owned=false;break;case 1:ncc_latched_dirty=true;break;
  case 2:nw.saved=false;break;case 3:nw.domain_touched=false;break;case 4:ncc_native.id=0;break;
  case 5:nw.spm=0;break;case 6:nw.infra=0;break;case 7:nw.gals=0;break;case 8:ncc_native.host=0;break;}
  assert(ncc_ap_check()==-1&&!reads&&ncc_ap_failure.owner_fail==f+1);
  assert(ncc_ap_failure.owner_ok==((1u<<f)-1)&&!ncc_ap_failure.valid_mask&&ncc_ap_failure.failed==7);
 }

 for(unsigned i=0;i<4;i++)if(i!=1){reset();enabled=i;assert(!runwindow());assert(!maps&&!reads&&!writes&&!logs&&outer_restores==1);}
 for(unsigned i=1;i<=7;i++){reset();ap_fail=i;assert(!runwindow());assert(!maps&&!writes&&outer_restores==1);}
 for(unsigned i=1;i<=14;i++){reset();pred_fail=i;assert(!runwindow());assert(!writes&&outer_restores==1);assert(!ncc_latched_dirty);}
 for(unsigned i=1;i<=4;i++){reset();map_fail=i;assert(!runwindow());assert(!writes&&outer_restores==1);}
 reset();ncc_fresh_owned=false;assert(!runwindow()&&!maps&&!reads&&!writes);
 reset();nw.saved=false;assert(!runwindow()&&!maps&&!reads&&!writes);
 reset();nw.domain_touched=false;assert(!runwindow()&&!maps&&!reads&&!writes);
 reset();ncc_power_window(0x18060000,0x18001000,0xbad);assert(!maps&&!reads&&!writes);
 reset();assert(!runwindow());assert(writes==80&&ncc_native.result.clean&&outer_restores==1);assert(!memcmp(regs,original,sizeof(regs)));for(unsigned i=0;i<40;i++){assert(order[i]==i);assert(order[40+i]==39-i);}
 assert(ncc_native_write(0,ncc_table[0].address,0xdeadbeef)==-1&&writes==80);
 ncc_native.result.snapshots=39;assert(ncc_native_write(0,ncc_table[0].address,0)==-1&&writes==80);
 for(unsigned i=1;i<=80;i++){reset();write_fail=i;int rc=runwindow();if(i<=40){assert(!rc&&ncc_native.result.clean&&outer_restores==1);assert(!memcmp(regs,original,sizeof(regs)));}else{assert(rc==-1&&ncc_latched_dirty&&!ncc_native.result.clean&&!outer_restores);assert(ncc_native.result.restore_error);}}
 reset();drop_after_write=1;assert(runwindow()==-1&&ncc_latched_dirty&&!outer_restores&&ncc_native.result.touched==1);
 reset();uint32_t v;ncc_native.result.snapshots=40;assert(ncc_native_read(0,0x18004010,&v)==-1&&!reads);assert(ncc_native_write(0,0x18004010,0)==-1&&!writes);
 puts("PASS reference AP short-circuit traces/9owner gates/valid-unread/firstfailure latch; native adapter optout zeroaccess; each7AP/14predicate/4map failures;40partial apply+40restore corruptions; reverse cleanup; persistent guard loss dirty latch/halt; bounded logs");return 0;
}
