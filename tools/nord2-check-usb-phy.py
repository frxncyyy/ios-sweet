#!/usr/bin/env python3
"""Check exact TPHY enable/restore code against a bounded register model.

Usage: nord2-check-usb-phy.py XNU_SOURCE_ROOT OUTPUT_DIRECTORY
Verifies access scope, power-on ordering and exact rollback, not analog USB.
"""
from pathlib import Path
import subprocess
import sys

source = Path(sys.argv[1])
out = Path(sys.argv[2])
out.mkdir(parents=True, exist_ok=True)
usb = (source / 'bsd/dev/sweet_usb.c').read_text()
state = usb.split('/* TPHY v2 COM offsets,', 1)[1].split('} nusb_phy;', 1)[0] + '} nusb_phy;\n'
state = state[state.index('static const uint16_t'):]
code = usb.split('static bool\nnusb_phy_snapshot', 1)[1]
code = 'static bool\nnusb_phy_snapshot' + code.split('static void\nnord2_usb_enumerate', 1)[0]
prelude = r'''
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
typedef uintptr_t vm_offset_t;
static bool nusb_smp;
static unsigned core;
#define __builtin_arm_rsr64(s) ((uint64_t)core << 8)
#define NUSB_SOFTCON (1u<<6)
static struct {
    bool owned;
    uint32_t power, lv1ie, epie, commonie, linkie, devconf;
} nusb;
'''
model = r'''
static uint32_t phy_regs[0x100/4], mac_regs[0x2e00/4], original[7];
static uint32_t writes, mac_writes, delays, elapsed, restores;
static uint32_t written_off[32], written_val[32];
static bool map_fail, restoring;
static vm_offset_t ml_io_map(vm_offset_t pa, unsigned size) {
    assert(pa==0x11e40000 && size==0x700);
    return map_fail ? 0 : 0x100000;
}
static unsigned phy_index(uint32_t off) {
    for (unsigned i=0;i<7;i++) if (nusb_phy_offsets[i]==off) return i;
    assert(0); return 0;
}
static uint32_t nusb_phy_rd(uint32_t off) {
    (void)phy_index(off);
    assert(nusb_phy.com==0x100300);
    return phy_regs[off/4];
}
static void nusb_phy_wr(uint32_t off, uint32_t value) {
    unsigned i=phy_index(off);
    assert(nusb_phy.owned);
    if (restoring) {
        /* Exact rollback, after MAC disconnect and before interrupt restore. */
        assert(mac_writes==8 && !(mac_regs[0x2404/4] & NUSB_SOFTCON));
        assert(value==original[i]);
        if (off==0x68) assert(restores==6); /* suspend overrides last */
        restores++;
    } else {
        static const uint32_t change_mask[7]={
            1u<<5, 1u<<11, (1u<<23)|(1u<<20), (1u<<12)|(1u<<9),
            (1u<<9)|(1u<<8),
            0xc4000000u|(1u<<23)|(0x1fu<<17)|(0xfu<<10)|0xfcu,
            (1u<<16)|(1u<<9)|0x36u
        };
        assert(((value ^ original[i]) & ~change_mask[i])==0);
        assert(writes<32);
        written_off[writes]=off; written_val[writes]=value; writes++;
    }
    phy_regs[off/4]=value;
}
static void delay(unsigned us) {
    assert(us==(delays ? 800u:30u));
    if (!delays) {
        assert(writes==3);
        assert(written_off[0]==0x68 && written_off[1]==0x68 && written_off[2]==0x68);
        assert(written_val[0] & (1u<<18));
        assert(!(written_val[1] & (1u<<3)));
        assert((written_val[2]&((1u<<18)|(1u<<3)))==((1u<<18)|(1u<<3)));
    }
    delays++; elapsed+=us;
}
static uint32_t nusb_rd(uint32_t off) {
    assert(off==0x2404); return mac_regs[off/4];
}
static void nusb_wr(uint32_t off, uint32_t val) {
    if (mac_writes++==0) assert(off==0x2404 && !(val&NUSB_SOFTCON));
    switch(off) {
    case 0xc: mac_regs[4/4]&=~val; break;
    case 0x8c: mac_regs[0x84/4]&=~val; break;
    case 8: mac_regs[4/4]|=val; break;
    case 0x88: mac_regs[0x84/4]|=val; break;
    case 0x80: case 0x241c: case 0x854: mac_regs[off/4]&=~val; break;
    case 0x100: case 0x800: case 0x2418: case 0x850: case 0x2404:
        mac_regs[off/4]=val; break;
    default: assert(0);
    }
}
#define printf(...) ((void)0)
'''
cases = r'''
#undef printf
static void reset(unsigned seed) {
    memset(&nusb,0,sizeof(nusb)); memset(&nusb_phy,0,sizeof(nusb_phy));
    writes=mac_writes=delays=elapsed=restores=0;
    nusb_smp=false; core=0;
    map_fail=restoring=false;
    for(unsigned i=0;i<7;i++) {
        seed=seed*1664525u+1013904223u;
        phy_regs[nusb_phy_offsets[i]/4]=original[i]=seed;
    }
}
int main(void) {
    reset(1); map_fail=true;
    assert(!nusb_phy_snapshot() && !writes && !nusb_phy.owned);
    for (unsigned i=0;i<7;i++) {
        reset(i); phy_regs[nusb_phy_offsets[i]/4]=0xffffffffu;
        assert(!nusb_phy_snapshot() && !writes && !nusb_phy.owned);
    }
    for (unsigned seed=1;seed<=1000;seed++) {
        reset(seed);
        assert(nusb_phy_snapshot() && !nusb_phy.owned && !writes);
        for (unsigned i=0;i<7;i++) assert(nusb_phy.saved[i]==original[i]);
        nusb_phy_enable();
        assert(nusb_phy.owned && delays==2 && elapsed==830);
        assert((phy_regs[0x68/4]&0xc4be3cfcu)==0);
        assert(!(phy_regs[0x18/4]&(1u<<23)) && (phy_regs[0x18/4]&(1u<<20)));
        assert(!(phy_regs[0x20/4]&0x300) && !(phy_regs[0x1c/4]&0x1200));
        assert((phy_regs[0x6c/4]&0x10036)==0x26 && (phy_regs[0x6c/4]&0x200));
        nusb.owned=true; nusb.power=0x60a1; nusb.lv1ie=0xff; nusb.epie=0x3000b;
        nusb.commonie=0x3b7; nusb.linkie=1; nusb.devconf=0;
        mac_regs[0x2404/4]=nusb.power|NUSB_SOFTCON;
        restoring=true;
        nusb_smp=true;
        for (core=1;core<8;core++) {
            assert(nord2_usb_prepare_return()==-1 && !mac_writes && !restores &&
                   nusb.owned && nusb_phy.owned);
        }
        core=0;
        assert(nord2_usb_prepare_return()==1);
        assert(!nusb.owned && !nusb_phy.owned && restores==7 && mac_writes==13);
        for (unsigned i=0;i<7;i++) assert(phy_regs[nusb_phy_offsets[i]/4]==original[i]);
        assert(nord2_usb_prepare_return()==0 && mac_writes==13);
        nusb_phy_restore(); assert(restores==7);
    }
    puts("PASS: exact TPHY code; failed mapping/register reads cause no writes;"
         " pulse and delays, seven-register scope, calibration preserved;"
         " MAC disconnect precedes exact PHY rollback; 1000 inherited states");
}
'''
cfile = out / 'usb-phy-host.c'
exe = out / 'usb-phy-host'
cfile.write_text(prelude + state + model + code + cases)
subprocess.run(['clang', '-std=gnu11', '-Wall', '-Wextra', '-Werror',
                '-fsanitize=address,undefined', '-g', str(cfile), '-o', str(exe)],
               check=True)
subprocess.run([str(exe.resolve())], check=True)
