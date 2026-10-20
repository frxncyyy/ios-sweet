#!/usr/bin/env python3
"""Exercise the exact MTU3 snapshot C code with read-only mock MMIO.

Usage: nord2-check-usb-probe.py PATH_TO_sweet_usb.c OUTPUT_DIRECTORY
This verifies gates and mapping failures, not actual controller behavior.
"""
from pathlib import Path
import subprocess
import sys

source = Path(sys.argv[1]).read_text()
out = Path(sys.argv[2])
out.mkdir(parents=True, exist_ok=True)
code = source.split("static uint32_t\nnord2_usb_read", 1)[1]
code = "static uint32_t\nnord2_usb_read" + code.split("/* NORD2 USB CONTROL:", 1)[0]
code = code.replace('__asm__ volatile("dmb oshld" ::: "memory");', '')
code = code.replace('*(volatile uint32_t *)(base + offset)', 'checked_read(base, offset)')
prelude = r'''
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
typedef uintptr_t vm_offset_t;
typedef size_t vm_size_t;
typedef int wait_result_t;
typedef uintptr_t thread_t;
static unsigned nusb_background_state;
static bool smp_pass;
static int nord2_smp_userland_ready(void) { return smp_pass; }
static unsigned spawned, freed, sleeps, thread_outcome, cpu_count;
static int create_error;
#define NSEC_PER_MSEC 1000000
#define THREAD_UNINT 0
#define THREAD_CONTINUE_NULL NULL
#define KERN_SUCCESS 0
static void clock_interval_to_deadline(unsigned a, unsigned b, uint64_t *c) {
    assert(a==1 && b==NSEC_PER_MSEC); *c=1;
}
static int assert_wait_deadline(void *a, int b, uint64_t c) {
    assert(a && b==THREAD_UNINT && c==1); return 0;
}
static void thread_block(void *a) {
    assert(a==THREAD_CONTINUE_NULL); sleeps++;
    nusb_background_state=thread_outcome;
}
static int kernel_thread_start(void (*f)(void *,wait_result_t), void *a, thread_t *t) {
    assert(f && a); *t=123; spawned++; return create_error;
}
static void thread_deallocate(thread_t t) { assert(t==123); freed++; }
static void nusb_background_thread(void *a, wait_result_t b) { (void)a; (void)b; assert(0); }
static void nord2_usb_enumerate(vm_offset_t mac, bool phy, bool bg) {
    (void)mac; (void)phy; (void)bg; assert(0);
}
static const uintptr_t pa[4] = {0x10001000, 0x10000000, 0x11203e00, 0x11201000};
static const size_t lengths[4] = {0x1000, 0x1000, 0x100, 0x2e00};
static uint8_t *regions[4];
static unsigned maps[4], reads[4], stopped, selected;
static int failed;
static char output[8192];
static uint32_t checked_read(vm_offset_t base, uint32_t off) {
    unsigned n;
    for (n=0; n<4 && base!=(uintptr_t)regions[n]; n++) {}
    assert(n<4 && off%4==0 && off+4<=lengths[n]);
    assert(n!=3 || off!=0x300); /* FIFO read would consume controller data. */
    reads[n]++;
    return *(volatile uint32_t *)(base+off);
}
static vm_offset_t ml_io_map(vm_offset_t address, vm_size_t len) {
    unsigned n;
    for (n=0; n<4 && address!=pa[n]; n++) {}
    assert(n<4 && len==lengths[n]);
    maps[n]++;
    return failed==(int)n ? 0 : (uintptr_t)regions[n];
}
static void PE_parse_boot_argn(const char *name, void *val, size_t size) {
    assert(size==sizeof(int));
    if (!strcmp(name,"cpus")) *(int *)val=cpu_count;
    else {
        assert(strcmp(name,"nord2stopat")==0);
        *(int *)val=selected;
    }
}
static void nord2_stop_at(unsigned point, const char *label) {
    assert((point==13 && strcmp(label,"after read-only MTU3 snapshot")==0) ||
           ((point==16 || point==21) && strcmp(label,"after background MTU3 USB console")==0));
    stopped++;
}
static int capture(const char *format, ...) {
    va_list ap;
    va_start(ap,format);
    size_t pos=strlen(output);
    int n=vsnprintf(output+pos,sizeof(output)-pos,format,ap);
    va_end(ap);
    assert(n>=0 && (size_t)n<sizeof(output)-pos);
    return n;
}
#define printf capture
'''
cases = r'''
#undef printf
static void reset(void) {
    memset(maps,0,sizeof(maps)); memset(reads,0,sizeof(reads));
    memset(output,0,sizeof(output)); stopped=0; selected=13; failed=-1;
    spawned=freed=sleeps=0; create_error=0; cpu_count=1; thread_outcome=1;
    nusb_background_state=0; smp_pass=false;
    for (unsigned n=0;n<4;n++) {
        assert(mprotect(regions[n],0x4000,PROT_READ|PROT_WRITE)==0);
        memset(regions[n],0,0x4000);
    }
    *(uint32_t *)(regions[2]+0x14)=1;
}
static void run(void) {
    for (unsigned n=0;n<4;n++)
        assert(mprotect(regions[n],0x4000,PROT_READ)==0);
    nord2_usb_probe(); /* Any MMIO write faults, including accidental W1C. */
    if (selected!=16 && selected!=21) assert(stopped==(selected==13));
}
int main(void) {
    for (unsigned n=0;n<4;n++) {
        regions[n]=mmap(NULL,0x4000,PROT_READ|PROT_WRITE,
                       MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
        assert(regions[n]!=MAP_FAILED);
    }
    reset(); selected=12; run();
    for (unsigned n=0;n<4;n++) assert(maps[n]==0 && reads[n]==0);
    for (int n=0;n<4;n++) {
        reset(); failed=n; run();
        assert(strstr(output,"mapping failed"));
        assert(reads[n]==0);
    }
    const unsigned guards[][3]={{0,0xac,1u<<1},{0,0xac,1u<<31},
                               {1,0xa0,1u<<31},{1,0xb0,1u<<7}};
    for (unsigned n=0;n<4;n++) {
        reset();
        *(uint32_t *)(regions[guards[n][0]]+guards[n][1])=guards[n][2];
        run(); assert(!maps[2] && !maps[3] && strstr(output,"clock gated"));
    }
    const unsigned inactive[][2]={{8,1},{0x14,0},{0x50,1},{0x50,2},{0x50,4}};
    for (unsigned n=0;n<5;n++) {
        reset(); *(uint32_t *)(regions[2]+inactive[n][0])=inactive[n][1];
        run(); assert(!maps[3] && strstr(output,"device inactive"));
    }
    reset(); run();
    assert(reads[0]==1 && reads[1]==2 && reads[2]==16 && reads[3]==36);
    assert(strstr(output,"MAC[2460]") && strstr(output,"snapshot end"));
    reset(); selected=16; run();
    assert(!stopped && spawned==1 && freed==1 && sleeps==1 &&
           strstr(output,"continuing BSD boot"));
    reset(); selected=16; cpu_count=2; run();
    assert(stopped==1 && !spawned && !sleeps && strstr(output,"invalid background CPU"));
    reset(); selected=16; create_error=1; run();
    assert(stopped==1 && spawned==1 && !freed && !sleeps);
    reset(); selected=16; thread_outcome=2; run();
    assert(stopped==1 && spawned==1 && freed==1 && sleeps==1);
    reset(); selected=16; thread_outcome=0; run();
    assert(stopped==1 && spawned==1 && freed==1 && sleeps==10000);
    reset(); selected=21; cpu_count=8; run();
    assert(stopped==1 && !spawned && !maps[0] && strstr(output,"all8 SMP pass required"));
    reset(); selected=21; cpu_count=8; smp_pass=true; run();
    assert(!stopped && spawned==1 && nusb_smp && nusb_background_stop==21 &&
           strstr(output,"continuing BSD boot"));
    reset(); selected=21; cpu_count=4; smp_pass=true; run();
    assert(stopped==1 && !spawned && strstr(output,"invalid background CPU"));
    reset(); selected=21; cpu_count=8; smp_pass=true; thread_outcome=0; run();
    assert(stopped==1 && spawned==1 && sleeps==10000);
    for (unsigned n=0;n<4;n++) assert(munmap(regions[n],0x4000)==0);
    puts("PASS: exact MTU3 probe; no MMIO writes; inactive gate, four clock gates,"
         " five device gates, all mapping failures, complete register snapshot;"
         " background ready/failure/timeout, legacy single-CPU and point21 SMP-pass guards");
}
'''
cfile = out / 'usb-probe-host.c'
exe = out / 'usb-probe-host'
cfile.write_text(prelude + code + cases)
subprocess.run(['clang', '-std=gnu11', '-Wall', '-Wextra', '-Werror',
                '-fsanitize=address,undefined', '-g', str(cfile), '-o', str(exe)],
               check=True)
subprocess.run([str(exe.resolve())], check=True)
