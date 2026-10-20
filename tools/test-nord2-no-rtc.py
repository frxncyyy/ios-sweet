#!/usr/bin/env python3
"""Compile actual IOKitInitializeTime with an RTC wait/calendar/counter model."""
from pathlib import Path
import subprocess
import tempfile
root=Path(__file__).resolve().parent.parent
s=(root/'research/ci-39158169212/buildtree/xnu/iokit/Kernel/IOStartIOKit.cpp').read_text()
a=s.index('void\nIOKitInitializeTime( void )');b=s.index('\nvoid\niokit_post_constructor_init(',a)
code=r'''
#include <assert.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#undef __x86_64__
#undef __i386__
struct mach_timespec_t { unsigned tv_sec,tv_nsec; };
static bool arg_present,rtc_completes;
static uint32_t argument;
static unsigned waits,calendars,logs;
static uint64_t ticks=1000,first_begin,first_end;
static std::string calls;
static bool PE_parse_boot_argn(const char *name,void *out,unsigned n) {
 assert(!strcmp(name,"nord2nortc") && n==4);calls+='P';
 if(arg_present)memcpy(out,&argument,4);return arg_present;
}
static uint64_t counter(const char *name) {
 if(!strcmp(name,"CNTFRQ_EL0"))return 13000000;
 assert(!strcmp(name,"CNTVCT_EL0"));calls+='T';return ticks;
}
#define __builtin_arm_rsr64 counter
struct IOService {
 static const char *resourceMatching(const char *s) { assert(!strcmp(s,"IORTC"));calls+='R';return s; }
 static void *waitForService(const char *s,mach_timespec_t *t) {
  assert(!strcmp(s,"IORTC") && t->tv_sec==30 && !t->tv_nsec);
  calls+='W';waits++;ticks+=rtc_completes?50:390000000;return rtc_completes?(void *)1:nullptr;
 }
};
static void clock_initialize_calendar(void) { calendars++;calls+='C'; }
static void IOLog(const char *fmt,...) {
 assert(strstr(fmt,"NORD2 BOOT RTC:") && calendars);calls+='L';logs++;
 va_list a;va_start(a,fmt);int skipped=va_arg(a,int);
 uint64_t begin=va_arg(a,uint64_t),end=va_arg(a,uint64_t),elapsed=va_arg(a,uint64_t),freq=va_arg(a,uint64_t);
 va_end(a);assert(skipped==(arg_present && argument==1));
 assert(end-begin==elapsed && freq==13000000);first_begin=begin;first_end=end;
}
'''+s[a:b]+r'''
int main(int argc,char **argv) {
 assert(argc==4);arg_present=atoi(argv[1]);argument=(uint32_t)strtoull(argv[2],nullptr,0);rtc_completes=atoi(argv[3]);
 bool skip=arg_present && argument==1;
#ifdef BCM2837
 bool actual_wait=false;
#else
#ifdef NORD2
 bool actual_wait=!skip;
#else
 bool actual_wait=true;
#endif
#endif
 IOKitInitializeTime();
 assert(calendars==1 && waits==(unsigned)actual_wait);
#ifdef NORD2
 assert(logs==1 && calls==(actual_wait?"PTRWTCL":"PTTCL"));
 assert(first_end-first_begin==(actual_wait?(rtc_completes?50:390000000):0));
#else
 assert(!logs && calls==(actual_wait?"RWC":"C"));
#endif
 IOKitInitializeTime();
 assert(calendars==2 && waits==2*(unsigned)actual_wait);
#ifdef NORD2
 assert(logs==1); // bounded once, even if calendar initialization is called twice
#else
 assert(!logs);
#endif
 (void)skip;(void)first_begin;(void)first_end;
}
'''
with tempfile.TemporaryDirectory(prefix='nord2-no-rtc-') as d:
    p=Path(d)/'test.cpp';p.write_text(code)
    for variant,flags in [('nord2',['-DNORD2=1']),('other',[]),('bcm',['-DBCM2837=1'])]:
        exe=Path(d)/variant
        subprocess.run(['clang++','-std=c++17','-O1','-g','-Wall','-Wextra','-Werror',
                        '-Wno-unused-function','-Wno-unused-variable','-fsanitize=address,undefined',
                        *flags,str(p),'-o',str(exe)],check=True)
        for present,value in [(0,0),(1,0),(1,1),(1,2),(1,4294967295)]:
            for completed in (0,1):
                subprocess.run([str(exe),str(present),str(value),str(completed)],check=True)
print('PASS: production RTC function, exact1 opt-in/default/other/BCM, wait completion+timeout, call order/calendar retained and once-only timing report')
