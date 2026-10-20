#!/usr/bin/env python3
"""Exercise exact XNU SMP shutdown/IRQ/console guards on the host.

Usage: nord2-check-xnu-smp.py XNU_SOURCE_ROOT OUTPUT_DIRECTORY
Hardware startup, coherency, GIC and timer delivery still require a phone test.
"""
from pathlib import Path
import subprocess
import sys

source, out = map(Path, sys.argv[1:])
out.mkdir(parents=True, exist_ok=True)
early = (source / 'osfmk/arm64/sweet_early.c').read_text()
cpu = (source / 'osfmk/arm64/cpu.c').read_text()
irq = (source / 'osfmk/arm64/sleh.c').read_text()
leave = early.split('nord2_leave_to(uint64_t entry_override)', 1)[1]
assert leave.index('nord2_kernel_smp_prepare_return()') < leave.index('nord2_usb_prepare_return()')
assert 'for (;;)' in leave.split('nord2_kernel_smp_prepare_return() < 0)', 1)[1].split(
    'nord2_smp_prepare_return()', 1)[0]
assert cpu.index('nord2_smp_start_prepare((unsigned)cpu);') < cpu.index('(void) PE_cpu_start(')
assert irq.index('nord2_smp_irq_stop(iar);') < irq.index('nord2_irq_seen(intid,')


def function(name):
    start = early.index('\n' + name + '(') + 1
    pos = early.index('{', start)
    depth = 1
    end = pos + 1
    while depth:
        depth += (early[end] == '{') - (early[end] == '}')
        end += 1
    return early[start:end] + '\n'


code = 'static bool\n' + function('nord2_console_acquire')
code += 'static bool\n' + function('nord2_psci_all_off')
code += early.split('/* NORD2 XNU SMP:', 1)[1].split('struct nord2_smp_work {', 1)[0]
# Recover the opening comment omitted by split.
code = code.replace(' bounded scheduler test', '/* bounded scheduler test', 1)
code = code.replace('__asm__ volatile("yield");', 'host_yield();')
code = code.replace('__asm__ volatile("wfe");', 'escape(3);')
assert 'printf(' not in function('nord2_kernel_smp_prepare_return')
prelude = r'''
#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <setjmp.h>
#include <time.h>
#include <pthread.h>
#include <sched.h>
static volatile uint32_t nord2_kernel_smp_active;
static uint32_t nord2_console_owner, nord2_console_dropped;
static unsigned core, requested, stop_point;
static const char *root_arg;
static int write_flags;
static int32_t affinity[8], off_rc;
static unsigned signals[8], signal_count, affinity_calls, clean_count;
static unsigned timer_disabled, masked, acknowledged, off_called, returned;
static uint64_t now;
static unsigned transition;
static bool realtime;
static jmp_buf jump;
#define DSB_SY 15
#define DSB_ISHST 11
#define ISB_SY 15
#define __builtin_arm_dsb(v) ((void)(v))
#define __builtin_arm_isb(v) ((void)(v))
static uint64_t read_reg(const char *s) {
    if (!strcmp(s,"CNTFRQ_EL0")) return realtime ? 1000000000ULL : 1000;
    assert(!strcmp(s,"CNTVCT_EL0"));
    if (!realtime) return now;
    struct timespec ts; clock_gettime(CLOCK_MONOTONIC,&ts);
    return (uint64_t)ts.tv_sec*1000000000+ts.tv_nsec;
}
#define __builtin_arm_rsr64(s) read_reg(s)
static void write_reg(const char *s,uint64_t v) {
    if (!strcmp(s,"DAIFSet")) { assert(v==15); masked++; }
    else if (!strcmp(s,"CNTV_CTL_EL0")) { assert(v==0); timer_disabled++; }
    else if (!strcmp(s,"ICC_EOIR1_EL1")) { assert(v==1 || v==27); acknowledged++; }
    else {
        assert(!strcmp(s,"ICC_SGI1R_EL1"));
        unsigned n=(v>>16)&255;
        assert(n<8 && v==((1ULL<<24)|((uint64_t)n<<16)|1));
        assert(signal_count<8); signals[signal_count++]=n;
    }
}
#define __builtin_arm_wsr(s,v) write_reg(s,v)
#define __builtin_arm_wsr64(s,v) write_reg(s,v)
static void __attribute__((noreturn)) escape(int n) { longjmp(jump,n); }
static unsigned nord2_raw_core(void) { return core; }
static uint64_t nord2_psci_now(void) { return now; }
static void nord2_psci_poll(void) {
    now++;
    if (transition && now==transition) for (unsigned i=1;i<8;i++) affinity[i]=1;
}
static void host_yield(void) { if (realtime) sched_yield(); else now++; }
static void CleanPoC_Dcache(void) { assert(masked && timer_disabled); clean_count++; }
static int32_t nord2_psci_call(uint64_t fn,uint64_t a1,uint64_t a2,uint64_t a3) {
    assert(!a2 && !a3);
    if (fn==0x84000002) {
        assert(!a1 && core>0 && masked && timer_disabled && clean_count);
        off_called++; return off_rc;
    }
    assert(fn==0xc4000004 && a1>=0x100 && a1<=0x700 && !(a1&255));
    affinity_calls++; return affinity[a1>>8];
}
static void sweet_early_string(const char *s) { (void)s; }
static void nord2_hex64(uint64_t n) { (void)n; }
static void PE_parse_boot_argn(const char *s,void *p,size_t len) {
    if (!strcmp(s,"rd")) { assert(len==16); strncpy(p,root_arg,len); return; }
    assert(len==sizeof(unsigned));
    if (!strcmp(s,"cpus")) *(unsigned*)p=requested;
    else if (!strcmp(s,"ufsrw") || !strcmp(s,"rootrw")) *(int*)p=write_flags;
    else { assert(!strcmp(s,"nord2stopat")); *(unsigned*)p=stop_point; }
}
static void __attribute__((noreturn)) panic(const char *fmt,...) { (void)fmt; escape(2); }
static void __attribute__((noreturn)) nord2_return_to_stock(void) { returned++; escape(1); }
static void quiet_printf(const char *fmt,...) { (void)fmt; }
#define printf quiet_printf
'''
cases = r'''
static void reset(void) {
    nord2_kernel_smp_active=0; nord2_smp_stop_requested=0; core=0;
    requested=2; stop_point=20; off_rc=-3; now=0; transition=0;
    root_arg="md0"; write_flags=0; nord2_smp_userland_passed=0;
    nord2_console_owner=nord2_console_dropped=0;
    signal_count=affinity_calls=clean_count=timer_disabled=masked=acknowledged=off_called=returned=0;
    memset(nord2_smp_off_error,0,sizeof(nord2_smp_off_error));
    for (unsigned i=0;i<8;i++) affinity[i]=1;
}
static uint64_t protected_counter;
static void *console_thread(void *arg) {
    unsigned owner=(unsigned)(uintptr_t)arg;
    for (unsigned i=0;i<2000;i++) {
        bool outer,nested;
        assert(nord2_console_acquire(owner,&outer) && outer);
        uint64_t n=protected_counter;
        assert(nord2_console_acquire(owner,&nested) && !nested);
        protected_counter=n+1;
        __atomic_store_n(&nord2_console_owner,0,__ATOMIC_RELEASE);
    }
    return NULL;
}
int main(void) {
    reset(); assert(nord2_kernel_smp_prepare_return()==0 && !affinity_calls && !signal_count);
    for (unsigned count=2;count<=8;count*=2) {
        reset(); requested=count; nord2_smp_start_prepare(1);
        assert(nord2_kernel_smp_active && affinity_calls==7);
        if (count>2) nord2_smp_start_prepare(count-1);
    }
    const unsigned bad_counts[]={0,1,3,5,7,9};
    for (unsigned i=0;i<sizeof(bad_counts)/sizeof(bad_counts[0]);i++) {
        reset(); requested=bad_counts[i];
        int why=setjmp(jump);
        if (!why) { nord2_smp_start_prepare(1); assert(false); }
        assert(why==2 && !nord2_kernel_smp_active);
    }
    reset(); requested=8; stop_point=21; nord2_smp_start_prepare(1);
    assert(nord2_kernel_smp_active && !nord2_smp_userland_ready());
    nord2_smp_userland_passed=1; assert(nord2_smp_userland_ready());
    nord2_smp_stop_requested=1; assert(!nord2_smp_userland_ready());
    for (unsigned mode=0;mode<5;mode++) {
        reset(); requested=8; stop_point=21;
        if (mode==0) requested=4;
        if (mode==1) root_arg="ufsroot0";
        if (mode==2) root_arg="abcdefghijklmnopq";
        if (mode==3) write_flags=1;
        if (mode==4) write_flags=-1;
        if (!setjmp(jump)) { nord2_smp_start_prepare(1); assert(false); }
        assert(!nord2_kernel_smp_active && !nord2_smp_userland_ready());
    }
    reset(); stop_point=19;
    if (!setjmp(jump)) { nord2_smp_start_prepare(1); assert(false); }
    assert(!nord2_kernel_smp_active);
    reset(); affinity[1]=0;
    if (!setjmp(jump)) { nord2_smp_start_prepare(1); assert(false); }
    assert(nord2_kernel_smp_active); /* Unsafe inherited state keeps return guarded. */
    reset(); nord2_kernel_smp_active=1;
    assert(nord2_kernel_smp_prepare_return()==1 && signal_count==7);
    for (unsigned i=0;i<7;i++) assert(signals[i]==i+1);
    const int32_t bad_states[]={0,2,-1,-2,-6};
    for (unsigned c=1;c<8;c++) for (unsigned j=0;j<5;j++) {
        reset(); nord2_kernel_smp_active=1; affinity[c]=bad_states[j];
        assert(nord2_kernel_smp_prepare_return()==-1 && now==5000);
    }
    reset(); nord2_kernel_smp_active=1; affinity[2]=2; transition=3;
    assert(nord2_kernel_smp_prepare_return()==1 && now==3);
    for (unsigned c=1;c<8;c++) {
        reset(); nord2_kernel_smp_active=1; core=c; nord2_console_owner=c+1;
        int why=setjmp(jump);
        if (!why) { (void)nord2_kernel_smp_prepare_return(); assert(false); }
        assert(why==3 && nord2_smp_stop_requested && signal_count==1 && signals[0]==0);
        assert(off_called && !returned && !nord2_console_owner && nord2_smp_off_error[c]==-3);
        reset(); nord2_kernel_smp_active=1; nord2_smp_stop_requested=1; core=c;
        why=setjmp(jump);
        if (!why) { nord2_smp_irq_stop(27); assert(false); }
        assert(why==3 && acknowledged==1 && off_called && !returned);
    }
    reset(); nord2_smp_irq_stop(1); assert(!acknowledged);
    nord2_kernel_smp_active=1; nord2_smp_irq_stop(1); assert(!acknowledged);
    nord2_smp_stop_requested=1;
    if (!setjmp(jump)) { nord2_smp_irq_stop(1); assert(false); }
    assert(returned==1 && acknowledged==1 && !off_called);
    reset(); bool outer; nord2_console_owner=8;
    assert(!nord2_console_acquire(1,&outer) && !outer && now==50);
    assert(nord2_console_dropped==1 && nord2_console_owner==8);
    reset(); realtime=true;
    pthread_t threads[8];
    for (unsigned i=0;i<8;i++) assert(!pthread_create(&threads[i],NULL,console_thread,(void*)(uintptr_t)(i+1)));
    for (unsigned i=0;i<8;i++) assert(!pthread_join(threads[i],NULL));
    assert(protected_counter==16000 && !nord2_console_owner && !nord2_console_dropped);
    puts("PASS: exact XNU SMP startup/return/IRQ guards; all7 affinities; pending/errors/timeouts; "
         "nonboot routes CPU0 then OFF; EOI before shutdown; failed OFF parks; "
         "console timeout/recursion and8-thread protected writes");
}
'''
cfile = out / 'xnu-smp-host.c'
cfile.write_text(prelude + code + cases)
exe = out / 'xnu-smp-host'
subprocess.run(['cc', '-std=c11', '-Wall', '-Wextra', '-Werror', '-Wno-clobbered',
                '-O1', '-g', '-pthread', '-fsanitize=address,undefined',
                str(cfile), '-o', str(exe)], check=True)
subprocess.run([str(exe.resolve())], check=True)
