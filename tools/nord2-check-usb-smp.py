#!/usr/bin/env python3
"""Check the exact background USB thread's CPU binding, deadlines and guarded exit."""
from pathlib import Path
import subprocess
import sys

source, out = map(Path, sys.argv[1:])
out.mkdir(parents=True, exist_ok=True)
usb = (source / 'bsd/dev/sweet_usb.c').read_text()
early = (source / 'osfmk/arm64/sweet_early.c').read_text()
bsd = (source / 'bsd/kern/bsd_init.c').read_text()
assert bsd.index('nord2_smp_probe();') < bsd.index('nord2_usb_probe();')
assert '__atomic_store_n(&nusb_background_state, 1, __ATOMIC_RELEASE)' in usb
assert '__atomic_load_n(&nusb_background_state, __ATOMIC_ACQUIRE)' in usb
leave = early.split('nord2_leave_to(uint64_t entry_override)', 1)[1]
assert leave.index('nord2_kernel_smp_prepare_return()') < leave.index('nord2_usb_prepare_return()')
assert 'if (usb_restored < 0)' in leave
body = usb.split('static void\nnusb_background_thread(', 1)[1].split(
    '#else /* Qualcomm', 1)[0]
code = 'static bool\nnusb_poll_continue(' + usb.split(
    'static bool\nnusb_poll_continue(', 1)[1].split('static void\nnord2_usb_enumerate(', 1)[0]
code += 'static void\nnusb_background_thread(' + body
assert 'while (nusb_poll_continue(background, nusb_background_stop,' in usb
model = r'''
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <setjmp.h>
typedef uintptr_t vm_offset_t;
typedef unsigned wait_result_t;
#define __unused __attribute__((unused))
#define THREAD_CONTINUE_NULL NULL
static unsigned core, binding, polls, blocks, nusb_background_state;
static unsigned nusb_background_stop;
static bool wrong_migration;
static jmp_buf escape;
static unsigned cpu_to_processor(unsigned n) { assert(n==0); return 17; }
static unsigned thread_bind(unsigned p) { assert(p==17); binding=p; return 0; }
static void thread_block(void *p) {
    assert(!p && binding==17); blocks++;
    if (!wrong_migration) core=0;
}
static unsigned cpu_number(void) { return core; }
static void panic(const char *s) __attribute__((noreturn));
static void panic(const char *s) { (void)s; longjmp(escape,2); }
static void quiet(const char *s,...) { (void)s; }
#define printf quiet
static void nord2_usb_enumerate(vm_offset_t mac,bool phy,bool bg) {
    assert(core==0 && binding==17 && blocks==1 && mac==0x1000 && phy && bg);
    polls++; nusb_background_state=1;
}
static void nord2_stop_at(unsigned stop,const char *label) {
    assert(stop==nusb_background_stop && (stop==16 || stop==21 || stop==22 || stop==23) && label);
    assert(core==0 && nusb_background_state==2 && polls==1);
    longjmp(escape,1);
}
static void *current_thread(void) { return NULL; }
static int thread_terminate(void *p) { (void)p; assert(false); return 0; }
'''
cases = r'''
int main(void) {
    for (unsigned stop=0;stop<25;stop++) for (unsigned bg=0;bg<2;bg++) {
        assert(nusb_poll_continue(bg,stop,44,45));
        assert(nusb_poll_continue(bg,stop,45,45)==(bg && stop==23));
        assert(nusb_poll_continue(bg,stop,UINT64_MAX,45)==(bg && stop==23));
    }
    for(unsigned start=0;start<8;start++) for(unsigned i=0;i<4;i++) {
        core=start; binding=polls=blocks=nusb_background_state=0;
        wrong_migration=false; nusb_background_stop=i?20+i:16;
        int result=setjmp(escape);
        if (!result) nusb_background_thread((void*)0x1000,0);
        assert(result==1 && polls==1);
    }
    core=7; binding=polls=blocks=nusb_background_state=0; wrong_migration=true;
    int result=setjmp(escape);
    if (!result) nusb_background_thread((void*)0x1000,0);
    assert(result==2 && !polls && !nusb_background_state);
    puts("PASS: USB service migrates to CPU0 before MMIO from all8 CPUs;"
         " migration failure refuses MMIO; bounded modes16/21/22; mode23 continuous only in background; setup-failure exit;"
         " SMP shutdown precedes guarded USB cleanup");
}
'''
cfile, exe = out / 'usb-smp-host.c', out / 'usb-smp-host'
cfile.write_text(model + code + cases)
subprocess.run(['cc', '-std=c11', '-Wall', '-Wextra', '-Werror', '-Wno-clobbered',
                '-O1', '-g', '-fsanitize=address,undefined', str(cfile), '-o', str(exe)],
               check=True)
subprocess.run([str(exe.resolve())], check=True)
