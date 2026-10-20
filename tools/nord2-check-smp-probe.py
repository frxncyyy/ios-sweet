#!/usr/bin/env python3
"""Check exact PSCI probe wait/return guards with firmware failure injection.

Usage: nord2-check-smp-probe.py XNU_SOURCE_ROOT OUTPUT_DIRECTORY
Host tests cannot prove CPU_ON/OFF or the secondary assembly on real hardware.
"""
from pathlib import Path
import subprocess
import sys

source, out = map(Path, sys.argv[1:])
out.mkdir(parents=True, exist_ok=True)
early = (source / 'osfmk/arm64/sweet_early.c').read_text()
code = early.split('static bool\nnord2_psci_all_off(void)', 1)[1]
code = 'static bool\nnord2_psci_all_off(void)' + code.split(
    '\nvoid\nnord2_smp_probe(void)', 1)[0]
code = code.replace('__builtin_arm_rsr64("CNTFRQ_EL0")', '1')
code = code.replace('__asm__ volatile("dmb oshld" ::: "memory");', '')
probe_loop = early.split(
    'nord2_psci_probe_active = true; /* Also protects every panic return. */', 1)[1]
probe_loop = probe_loop.split('\n\tif (nord2_smp_prepare_return() == 1)', 1)[0]
probe_loop = probe_loop.replace('__builtin_arm_rsr64("CNTFRQ_EL0")', '1')
probe_loop = probe_loop.replace(
    '__asm__ volatile("dsb sy; ic ialluis; dsb sy; isb" ::: "memory");', '')
leave = early.split('nord2_leave_to(uint64_t entry_override)', 1)[1]
assert leave.index('nord2_smp_prepare_return()') < leave.index('nord2_usb_prepare_return()')
assert 'for (;;)' in leave.split('nord2_smp_prepare_return() < 0)', 1)[1].split(
    'nord2_usb_prepare_return()', 1)[0]

prelude = r'''
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#define NORD2_PSCI_LOW_SIZE 0x8000
#define DSB_SY 15
#define __builtin_arm_dsb(v) ((void)(v))
static bool nord2_psci_probe_active;
static volatile uint8_t *nord2_psci_low_alias;
static uint8_t scratch[NORD2_PSCI_LOW_SIZE];
static uint64_t now;
static unsigned calls, polls, transition_at;
static int32_t state[8];
static volatile uint64_t *mail_state;
static int32_t emulate_on(uint64_t target, uint64_t entry, uint64_t context);
static int32_t nord2_psci_call(uint64_t fn, uint64_t target, uint64_t a2, uint64_t a3) {
    if (fn==0xc4000003) return emulate_on(target,a2,a3);
    assert(fn==0xc4000004 && !a2 && !a3);
    assert(target>=0x100 && target<=0x700 && !(target&255));
    calls++;
    return state[target>>8];
}
static uint64_t nord2_psci_now(void) { return now; }
static void nord2_psci_poll(void) {
    now++; polls++;
    if (transition_at && now==transition_at) {
        for (unsigned i=1; i<8; i++) state[i]=1;
        if (mail_state) *mail_state=1;
    }
}
'''
loop_prelude = r'''
static struct nord2_psci_mailbox loop_boxes[8];
static unsigned launches, bad_launch, fail_on_launch, pending_launch, summaries;
static unsigned summary_pass[2], summary_count[2], cleans;
static uint8_t stub[188];
#define nord2_psci_low_probe_entry stub
#define nord2_psci_probe_end (stub+sizeof(stub))
typedef uintptr_t vm_offset_t;
static uint64_t ml_static_vtop(vm_offset_t va) {
    assert(va==(uintptr_t)stub); return 0x1434900ccULL;
}
static void CleanPoC_DcacheRegion_Force(vm_offset_t va, size_t len) {
    assert(va==(uintptr_t)stub && len==sizeof(stub)); cleans++;
}
static int32_t emulate_on(uint64_t target, uint64_t entry, uint64_t context) {
    unsigned core=target>>8;
    assert(core>=1 && core<=7 && context==0x40184000+core*128);
    assert(entry==(launches<7 ? 0x40180000ULL : 0x1434900ccULL));
    /* High pass must clear prior successful mailboxes before reusing them. */
    assert(loop_boxes[core].state==0 && loop_boxes[core].magic==0);
    launches++;
    if (launches==fail_on_launch) return -3;
    loop_boxes[core]=(struct nord2_psci_mailbox){
        .magic=0x4e32534d,.state=1,.el=4,.mpidr=0x81000000+(core<<8),
        .midr=(uint64_t)(core<4?0xd05:0xd41)<<4,.sctlr=0x30d00800,
        .frequency=1,.context=0
    };
    if (launches==bad_launch) loop_boxes[core].midr=0;
    if (launches==pending_launch) state[core]=2;
    return 0;
}
static void loop_log(const char *fmt, ...) {
    if (strncmp(fmt,"NORD2 SMP: pass=%u firmware probe",32)) return;
    assert(summaries<2);
    va_list ap; va_start(ap,fmt);
    summary_pass[summaries]=va_arg(ap,unsigned);
    summary_count[summaries++]=va_arg(ap,unsigned);
    va_end(ap);
}
#define printf loop_log
static void run_probe_loop(void) {
    unsigned stop=19;
    uint64_t pa=0x40184000,entry=0x40180000;
    volatile struct nord2_psci_mailbox *boxes=loop_boxes;
'''
cases = r'''
static void reset(void) {
    nord2_psci_probe_active=true; now=0; calls=polls=transition_at=0;
    mail_state=NULL; nord2_psci_low_alias=NULL;
    for (unsigned i=0; i<8; i++) state[i]=1;
}
static void reset_loop(void) {
    reset(); memset(loop_boxes,0,sizeof(loop_boxes));
    launches=bad_launch=fail_on_launch=pending_launch=summaries=cleans=0;
}
int main(void) {
    reset(); nord2_psci_probe_active=false;
    assert(nord2_smp_prepare_return()==0 && !calls);
    reset();
    assert(nord2_smp_prepare_return()==1 && !nord2_psci_probe_active && calls==7);
    /* Any live, pending, or unknown affinity prevents Linux chain-back. */
    for (unsigned core=1; core<8; core++) {
        const int32_t bad[]={0,2,-1,-2,-6};
        for (unsigned j=0;j<sizeof(bad)/sizeof(bad[0]);j++) {
            reset(); state[core]=bad[j];
            assert(nord2_smp_prepare_return()==-1 && nord2_psci_probe_active && now==5);
        }
    }
    reset(); state[7]=2; transition_at=3;
    assert(nord2_smp_prepare_return()==1 && now==3 && !nord2_psci_probe_active);
    reset(); memset(scratch,0xa5,sizeof(scratch)); nord2_psci_low_alias=scratch;
    state[2]=0;
    assert(nord2_smp_prepare_return()==-1 && nord2_psci_low_alias==scratch);
    for (unsigned i=0;i<sizeof(scratch);i++) assert(scratch[i]==0xa5);
    state[2]=1;
    assert(nord2_smp_prepare_return()==1 && !nord2_psci_low_alias);
    for (unsigned i=0;i<sizeof(scratch);i++) assert(scratch[i]==0);
    struct nord2_psci_mailbox box={0};
    reset(); box.state=1;
    assert(nord2_psci_wait_off(&box,3,5) && !polls);
    /* OFF alone is not a successful entry; no stale/empty mailbox accepted. */
    reset(); box.state=0;
    assert(!nord2_psci_wait_off(&box,3,5) && now==5);
    reset(); box.state=1; state[3]=0;
    assert(!nord2_psci_wait_off(&box,3,5) && now==5);
    reset(); box.state=2; state[3]=0;
    assert(!nord2_psci_wait_off(&box,3,5) && !polls);
    reset(); box.state=1; state[3]=-1;
    assert(!nord2_psci_wait_off(&box,3,5) && !polls);
    reset(); box.state=0; state[3]=2; transition_at=2; mail_state=&box.state;
    assert(nord2_psci_wait_off(&box,3,5) && now==2 && box.state==1);
    /* Observed firmware context=0 is accepted ONLY for the fixed-slot mode.
     * Every other completion, identity, MMU and shutdown check still applies. */
    for (unsigned core=1; core<8; core++) {
        const uint64_t context=0x40184000+core*128;
        struct nord2_psci_mailbox good={
            .magic=0x4e32534d, .state=1, .el=4, .mpidr=0x81000000+(core<<8),
            .midr=(uint64_t)(core<4?0xd05:0xd41)<<4,
            .sctlr=0x30d00800, .frequency=13000000
        };
        assert(nord2_psci_result_valid(&good,core,context,19,13000000,true));
        assert(!nord2_psci_result_valid(&good,core,context,18,13000000,true));
        assert(!nord2_psci_result_valid(&good,core,context,17,13000000,true));
        good.context=context;
        assert(nord2_psci_result_valid(&good,core,context,17,13000000,true));
        assert(nord2_psci_result_valid(&good,core,context,18,13000000,true));
        good.context=0xdeadbeef; /* Arbitrary x0 is never used as a pointer. */
        assert(nord2_psci_result_valid(&good,core,context,19,13000000,true));
        assert(!nord2_psci_result_valid(&good,core,context,19,13000000,false));
        const unsigned offsets[]={0,1,2,3,4,5,8,9};
        for (unsigned j=0;j<sizeof(offsets)/sizeof(offsets[0]);j++) {
            box=good;
            uint64_t *fields=(uint64_t *)&box;
            fields[offsets[j]]^=(offsets[j]==3 ? 16 : 1);
            assert(!nord2_psci_result_valid(&box,core,context,19,13000000,true));
        }
        box=good; box.mpidr|=1ULL<<32;
        assert(!nord2_psci_result_valid(&box,core,context,19,13000000,true));
    }
    reset_loop(); run_probe_loop();
    assert(launches==14 && cleans==1 && summaries==2);
    assert(summary_pass[0]==0 && summary_pass[1]==1);
    assert(summary_count[0]==7 && summary_count[1]==7);
    assert(nord2_smp_prepare_return()==1);
    /* Any failed core aborts the remaining low/high sequence. */
    for (unsigned n=1;n<=14;n++) {
        reset_loop(); bad_launch=n; run_probe_loop();
        assert(launches==n && summaries==(n<=7?1:2));
        assert(summary_count[summaries-1]==(n-1)%7);
        assert(nord2_smp_prepare_return()==1);
        reset_loop(); fail_on_launch=n; run_probe_loop();
        assert(launches==n && summary_count[summaries-1]==(n-1)%7);
        assert(nord2_smp_prepare_return()==1);
        reset_loop(); pending_launch=n; run_probe_loop();
        assert(launches==n && summary_count[summaries-1]==(n-1)%7);
        assert(nord2_smp_prepare_return()==-1);
    }
    puts("PASS: exact PSCI guards; all seven targets; ON/PENDING/errors refuse "
         "return; timeout; late OFF; failed CPU_OFF; empty mailbox; inactive no-op; "
         "low scratch retained until all cores OFF, then fully cleared; "
         "fixed-mailbox validation tolerates lost context only in mode19; "
         "low/high passes clear mailboxes and abort on each of 14 failures");
}
'''
cfile = out / 'smp-probe-host.c'
cfile.write_text(prelude + code + loop_prelude + probe_loop +
                 '\n}\n#undef printf\n' + cases)
exe = out / 'smp-probe-host'
subprocess.run(['cc', '-std=c11', '-Wall', '-Wextra', '-Werror', '-O1', '-g',
                '-fsanitize=address,undefined', str(cfile), '-o', str(exe)], check=True)
subprocess.run([str(exe.resolve())], check=True)
