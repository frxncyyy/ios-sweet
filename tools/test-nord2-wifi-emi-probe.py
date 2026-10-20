#!/usr/bin/env python3
"""Exact boot bridge + collector under sanitizers; no native SMC is executed."""
from pathlib import Path
import subprocess,tempfile
ROOT=Path(__file__).resolve().parents[1]
def main():
 with tempfile.TemporaryDirectory() as directory:
  d=Path(directory)
  (d/'wifi_emi_probe_test_platform.h').write_text(r'''
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
typedef int kern_return_t;
typedef int wait_result_t;
typedef unsigned long thread_t;
typedef void (*thread_continue_t)(void *,wait_result_t);
#define KERN_SUCCESS 0
#define os_atomic_cmpxchg(p,a,b,o) __sync_bool_compare_and_swap(p,a,b)
static int present=1,arg=1,calls,fail_at=-1,fail_kind,start_error,starts,deallocs,terminations,logs;
static thread_continue_t continuation;
static uint64_t ticks;
static char last_log[512];
static int PE_parse_boot_argn(const char *name,void *value,int size) {
 assert(!strcmp(name,"nord2wifiemi") && size==sizeof(int));*(int *)value=arg;return present;
}
static void clock_get_uptime(uint64_t *value){*value=++ticks;}
static kern_return_t kernel_thread_start(thread_continue_t fn,void *arg,thread_t *thread) {
 starts++;assert(!arg);if(start_error)return start_error;continuation=fn;*thread=42;return 0;
}
static void thread_deallocate(thread_t thread){assert(thread==42);deallocs++;}
static void thread_terminate_self(void){terminations++;}
static int test_printf(const char *format,...) {
 assert(strstr(format,"NORD2 WIFI EMI"));logs++;
 va_list ap;va_start(ap,format);vsnprintf(last_log,sizeof(last_log),format,ap);va_end(ap);return 0;
}
#define printf test_printf
''')
  (d/'test.c').write_text(r'''
#include "wifi_emi_snapshot.h"
/* Build actual collector/decoder but keep the native SMC entry unreachable. */
#define nwes_native_call nwep_unused_native
#include "wifi_emi_snapshot.c"
#undef nwes_native_call
#include "wifi_emi_probe.c"
int nwes_native_call(void *context,const uint64_t a[8],uint64_t result[4]) {
 assert(!context && calls<8 && a[0]==NWES_SMC_ID && a[1]==2);
 unsigned i=(unsigned)calls/4,f=(unsigned)calls%4;
 assert(a[2]==(f<2?f:2) && a[3]==(i?31:27) && a[4]==(f==3?1:0));
 assert(!a[5] && !a[6] && !a[7]);
 int index=calls++;
 if(index==fail_at && fail_kind==3)return -1;
 result[0]=f==0?(i?0:0x4fb0):f==1?(i?0x1ffff:0x4ff4):0;
 result[1]=0x1122334455667788ull;result[2]=42;result[3]=43;
 if(index==fail_at) {
  if(fail_kind==1)result[0]=UINT64_MAX;
  if(fail_kind==2)result[0]=(uint64_t)-2;
  if(fail_kind==4)result[0]=UINT64_C(1)<<40;
 }
 return 0;
}
static void reset(void) {
 present=arg=1;calls=fail_kind=start_error=starts=deallocs=terminations=logs=0;
 fail_at=-1;nwep_attempted=nwep_start_attempted=0;continuation=NULL;
}
int main(void) {
 for(int value=-2;value<=3;value++)if(value!=1){
  reset();arg=value;assert(nord2_wifi_emi_start()==EPERM);
  assert(nord2_wifi_emi_probe()==EPERM && !calls && !logs && !starts);
 }
 reset();present=0;assert(nord2_wifi_emi_start()==EPERM && !starts);
 reset();assert(!nord2_wifi_emi_probe());assert(calls==8 && logs==12);
 assert(strstr(last_log,"issued=8") && strstr(last_log,"effective=unknown"));
 assert(nord2_wifi_emi_probe()==EALREADY && calls==8);
 for(int kind=1;kind<=4;kind++)for(int index=0;index<8;index++){
  reset();fail_at=index;fail_kind=kind;assert(nord2_wifi_emi_probe()==EIO);
  assert(calls==((kind==1||kind==3)?index+1:8));
  assert(nord2_wifi_emi_probe()==EALREADY);
 }
 reset();start_error=1;assert(nord2_wifi_emi_start()==EAGAIN);
 assert(starts==1 && !deallocs && !calls && !terminations);
 assert(nord2_wifi_emi_start()==EALREADY && starts==1);
 reset();assert(!nord2_wifi_emi_start());assert(starts==1 && deallocs==1 && continuation && !calls);
 continuation(NULL,0);assert(calls==8 && terminations==1);
 assert(nord2_wifi_emi_start()==EALREADY && starts==1);
 reset();fail_at=0;fail_kind=1;assert(!nord2_wifi_emi_start());continuation(NULL,0);
 assert(calls==1 && terminations==1 && deallocs==1);
 return 0;
}
''')
  exe=d/'test'
  subprocess.run(['cc','-std=c11','-O1','-g','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-fno-omit-frame-pointer','-DNWEP_HOST_TEST','-I'+str(d),'-I'+str(ROOT/'shim/nord2-xnu'),str(d/'test.c'),'-o',str(exe)],check=True)
  subprocess.run([str(exe)],check=True)
  print('PASS: exact collector+probe ASan/UBSan;8READ ABI calls; strict gate/once; every unsupported/error/transport/invalid response; spawn/ref/selftermination; no native SMC')
if __name__=='__main__':main()
