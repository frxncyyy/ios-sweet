#!/usr/bin/env python3
"""Exercise the exact Nord2 display gates and bounded command-mode refresh.

This models register completion and timeouts, not the physical panel.
"""
from pathlib import Path
import subprocess
import sys

source, out = map(Path, sys.argv[1:])
out.mkdir(parents=True, exist_ok=True)
early = (source / 'osfmk/arm64/sweet_early.c').read_text()
fb = (source / 'iokit/Kernel/arm/SweetFramebuffer.cpp').read_text()
code = 'static volatile uint32_t *nord2_ovl[4];' + early.split(
    'static volatile uint32_t *nord2_ovl[4];', 1)[1].split('static void\nnord2_flush', 1)[0]
code = code.replace('__asm__ volatile("dsb sy" ::: "memory");', 'barriers++;')
assert 'sweet_early_putc(c[i]);' in early
assert 'nord2_gui_owned && !panic_active()' in early
assert 'if (!nord2_display_begin()) { return; }' in fb
assert fb.index('nord2_display_begin()') < fb.index('int done = nord2_display_refresh()')
assert 'if (step <= 6) { (void)nord2_display_refresh(); }' in fb
model = r'''
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define NORD2_FB_PA 0x76e10000ULL
#define NORD2_FB_WIDTH 1080
#define NORD2_FB_HEIGHT 2400
#define NORD2_FB_PITCH 4352
#define NORD2_DSI_INTSTA 12
#define NORD2_DSI_BUSY (1u << 31)
#define NORD2_DSI_FRAME_DONE (1u << 4)
#define NORD2_DSI_START 0
#define NORD2_MUTEX_EN0 32
static uint32_t dsi[1024], mtx[1024], nord2_gui_owned, nord2_console_owner;
static uintptr_t nord2_va_fb, nord2_va_dsi, nord2_va_mutex;
static int enabled, stop;
static bool panicking, acquire_ok, outer_lock, complete;
static uint64_t ticks, daif, freq;
static unsigned barriers;
static uint32_t ovl[4][1024];
static uintptr_t ml_io_map(uint64_t pa, unsigned size) {
    assert(size == 4096);
    const uint64_t addr[4] = {0x14000000,0x14100000,0x14001000,0x14101000};
    for(unsigned i=0;i<4;i++) if(pa==addr[i]) return (uintptr_t)ovl[i];
    assert(0); return 0;
}
static bool panic_active(void) { return panicking; }
static void PE_parse_boot_argn(const char *name, void *value, unsigned size) {
    assert(size == sizeof(int));
    *(int *)value = strcmp(name, "nord2display") == 0 ? enabled : stop;
}
static uint64_t rsr(const char *reg) {
    if (!strcmp(reg,"DAIF")) return daif;
    if (!strcmp(reg,"CNTFRQ_EL0")) return freq;
    assert(!strcmp(reg,"CNTVCT_EL0"));
    if (complete && dsi[0] == 1) dsi[3] = NORD2_DSI_FRAME_DONE;
    return ++ticks;
}
static void wsr(const char *reg, uint64_t value) {
    if (!strcmp(reg,"DAIFSet")) daif |= value;
    else { assert(!strcmp(reg,"DAIF")); daif = value; }
}
#define __builtin_arm_rsr64(r) rsr(r)
#define __builtin_arm_wsr(r,v) wsr(r,v)
#define __builtin_arm_wsr64(r,v) wsr(r,v)
static unsigned nord2_raw_core(void) { return 3; }
static bool nord2_console_acquire(unsigned owner, bool *outer) {
    assert(owner == 4 && (daif & 15) == 15);
    *outer = outer_lock;
    return acquire_ok;
}
'''
cases = r'''
int main(void) {
    uint64_t phys=0; uint32_t w=0,h=0,pitch=0;
    enabled=1; stop=23; nord2_va_fb=1;
    nord2_va_dsi=(uintptr_t)dsi; nord2_va_mutex=(uintptr_t)mtx;
    assert(nord2_display_info(&phys,&w,&h,&pitch));
    assert(phys==0x76e10000 && w==1080 && h==2400 && pitch==4352);
    for (stop=0;stop<26;stop++) if(stop!=23)
        assert(!nord2_display_info(&phys,&w,&h,&pitch));
    stop=23; enabled=0; assert(!nord2_display_info(&phys,&w,&h,&pitch));
    enabled=1; nord2_va_fb=0; assert(!nord2_display_info(&phys,&w,&h,&pitch));
    nord2_va_fb=1; nord2_va_dsi=0; assert(!nord2_display_info(&phys,&w,&h,&pitch));
    nord2_va_dsi=(uintptr_t)dsi;
    daif=0x80; acquire_ok=false;
    assert(!nord2_display_begin() && !nord2_gui_owned && daif==0x80);
    acquire_ok=true; outer_lock=true; nord2_console_owner=4;
    assert(nord2_display_begin() && nord2_gui_owned && !nord2_console_owner && daif==0x80);
    nord2_gui_owned=0; assert(!nord2_display_refresh() && !ticks && !barriers);
    nord2_gui_owned=1; panicking=true;
    assert(!nord2_display_refresh() && !ticks && !barriers);
    panicking=false; freq=0; assert(!nord2_display_refresh() && !barriers);
    freq=10000; dsi[3]=NORD2_DSI_BUSY;
    assert(!nord2_display_refresh() && !dsi[0] && !mtx[8] && ticks<103 && !barriers);
    ticks=0; dsi[3]=0; complete=false;
    assert(!nord2_display_refresh() && dsi[0]==1 && mtx[8]==1 && ticks<205 && barriers==1);
    ticks=0; dsi[0]=dsi[3]=0; complete=true;
    assert(nord2_display_refresh() && dsi[3]==~NORD2_DSI_FRAME_DONE && barriers==2);
    assert(ticks<10);
    /* Only the exact known composition is eligible; both pipes must match. */
    for(unsigned i=0;i<2;i++) {
        ovl[i][0x2c/4]=2; ovl[i+2][0x2c/4]=1;
        ovl[i][0xf60/4]=0x77806000+i*2160;
        ovl[i+2][0xf40/4]=0x76e10000+i*2160;
        ovl[i][0x64/4]=4320; ovl[i+2][0x44/4]=4352;
        ovl[i][0x58/4]=ovl[i+2][0x38/4]=0x0960021c;
        ovl[i][0xe0/4]=1; ovl[i][0x24/4]=0xabcdef;
    }
    nord2_ovl_checked=false; ovl[1][0xf60/4]++;
    nord2_display_overlay_init();
    assert(!nord2_ovl_saved); nord2_display_overlay_remove();
    assert(ovl[0][0x2c/4]==2 && ovl[1][0x2c/4]==2);
    ovl[1][0xf60/4]--; nord2_ovl_checked=false;
    nord2_display_overlay_init(); assert(nord2_ovl_saved);
    uint32_t saved[4][1024]; memcpy(saved,ovl,sizeof(saved));
    nord2_display_overlay_remove(); assert(nord2_ovl_removed);
    for(unsigned i=0;i<2;i++) {
        assert(ovl[i][0x2c/4]==0 && ovl[i][0xe0/4]==0);
        assert(ovl[i][0x24/4]==(0xabcdef & ~(1<<5)));
        assert(!memcmp(saved[i+2],ovl[i+2],sizeof(ovl[0])));
    }
    nord2_display_overlay_restore();
    assert(!nord2_ovl_removed && !memcmp(saved,ovl,sizeof(saved)));
    puts("PASS: display opt-in, mapping guards, console handoff, panic exclusion,"
         " busy/FRAME_DONE timeouts, W0C, two-pipe overlay guards/removal/restore");
}
'''
cfile, exe = out / 'display-host.c', out / 'display-host'
cfile.write_text(model + code + cases)
subprocess.run(['clang', '-std=gnu11', '-Wall', '-Wextra', '-Werror',
                '-fsanitize=address,undefined', str(cfile), '-o', str(exe)], check=True)
subprocess.run([str(exe.resolve())], check=True)
