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
leave = early.split('nord2_leave_to(uint64_t entry_override)', 1)[1]
assert leave.index('nord2_smp_prepare_return()') < leave.index('nord2_usb_prepare_return()')
assert 'for (;;)' in leave.split('nord2_smp_prepare_return() < 0)', 1)[1].split(
    'nord2_usb_prepare_return()', 1)[0]

prelude = r'''
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
static bool nord2_psci_probe_active;
static uint64_t now;
static unsigned calls, polls, transition_at;
static int32_t state[8];
static volatile uint64_t *mail_state;
static int32_t nord2_psci_call(uint64_t fn, uint64_t target, uint64_t a2, uint64_t a3) {
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
cases = r'''
static void reset(void) {
    nord2_psci_probe_active=true; now=0; calls=polls=transition_at=0;
    mail_state=NULL;
    for (unsigned i=0; i<8; i++) state[i]=1;
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
    puts("PASS: exact PSCI guards; all seven targets; ON/PENDING/errors refuse "
         "return; timeout; late OFF; failed CPU_OFF; empty mailbox; inactive no-op");
}
'''
cfile = out / 'smp-probe-host.c'
cfile.write_text(prelude + code + cases)
exe = out / 'smp-probe-host'
subprocess.run(['cc', '-std=c11', '-Wall', '-Wextra', '-Werror', '-O1', '-g',
                '-fsanitize=address,undefined', str(cfile), '-o', str(exe)], check=True)
subprocess.run([str(exe.resolve())], check=True)
