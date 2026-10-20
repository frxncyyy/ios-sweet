#!/usr/bin/env python3
"""Host checks of the exact native USB request decoder and pmsg reader.

Usage: nord2-check-usb-control.py XNU_SOURCE_ROOT OUTPUT_DIRECTORY
These checks validate request/buffer boundaries, not hardware enumeration.
"""
from pathlib import Path
import subprocess
import sys

source = Path(sys.argv[1])
out = Path(sys.argv[2])
out.mkdir(parents=True, exist_ok=True)
usb = (source / 'bsd/dev/sweet_usb.c').read_text()
decoder = usb.split('static const uint8_t nusb_device[]', 1)[1]
decoder = 'static const uint8_t nusb_device[]' + decoder.split('static uint32_t\nnusb_rd', 1)[0]
early = (source / 'osfmk/arm64/sweet_early.c').read_text()
reader = early.split('uint32_t\nnord2_log_read', 1)[1]
reader = 'uint32_t\nnord2_log_read' + reader.split('static volatile struct nord2_ring *nord2_log', 1)[0]
assert reader.index('ring->size;') < reader.index('__builtin_arm_dmb(DMB_SY)') < reader.index('ring->data[')
writer = early.split('nord2_putc(char c)', 1)[1].split('#else', 1)[0]
assert writer.index('ring()->data[') < writer.index('__builtin_arm_dmb(DMB_SY)') < writer.index('ring()->size =')
prelude = r'''
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DMB_SY 15
#define __builtin_arm_dmb(x) ((void)(x))
#define MIN(a,b) ((a)<(b)?(a):(b))
#define NORD2_PMSG_SIZE 65536u
#define NORD2_RING_HDR 12u
#define NORD2_PMSG_SIG 0x43474244u
struct nord2_ring { uint32_t sig, start, size; uint8_t data[65524]; };
static struct nord2_ring ring;
static volatile struct nord2_ring *nord2_ring(void) { return &ring; }
static struct { uint32_t requests, resets, log_reads, stalls; uint8_t configuration; } nusb;
'''
cases = r'''
static uint8_t data[1024];
static uint32_t length;
static int address, configuration;
static int req(unsigned type, unsigned request, unsigned value, unsigned index, unsigned count) {
    uint8_t s[8] = {type,request,value,value>>8,index,index>>8,count,count>>8};
    int result=nusb_reply(s,data,&length,&address,&configuration);
    assert(result>=0 && result<=2 && length<=sizeof(data) && length<=count);
    assert(address>=-1 && address<=127 && configuration>=-1 && configuration<=1);
    return result;
}
int main(void) {
    ring.sig=NORD2_PMSG_SIG; ring.size=65524; ring.start=0;
    for (unsigned i=0;i<ring.size;i++) ring.data[i]=(uint8_t)i;
    assert(nord2_log_read(65524,data,1024)==0);
    assert(nord2_log_read(UINT32_MAX,data,1024)==0);
    assert(nord2_log_read(65520,data,1024)==4 && data[3]==(uint8_t)65523);
    assert(nord2_log_read(0,data,0)==0);
    ring.size=65525; assert(nord2_log_read(0,data,1024)==0);
    ring.size=65524; ring.sig=0; assert(nord2_log_read(0,data,1024)==0);
    ring.sig=NORD2_PMSG_SIG;
    assert(req(0x80,6,0x100,0,8)==1 && length==8 && data[0]==18);
    assert(req(0x80,6,0x100,0,65535)==1 && length==18 && data[7]==64);
    assert(req(0x80,6,0x200,0,65535)==1 && length==18 && data[13]==0);
    assert(req(0x80,6,0x600,0,65535)==1 && length==10 && data[1]==6);
    assert(req(0x80,6,0x700,0,65535)==1 && length==18 && data[1]==7);
    assert(req(0x80,6,0x300,0,255)==1 && length==4);
    assert(req(0x80,6,0x302,0x409,255)==1 && data[2]=='N' && length==36);
    assert(req(0x80,6,0x304,0x409,255)==0);
    assert(req(0x80,6,0x302,0x410,255)==0);
    assert(req(0x80,6,0x101,0,18)==0);
    assert(req(0,5,127,0,0)==2 && address==127 && configuration==-1);
    assert(req(0,5,128,0,0)==0);
    assert(req(0,5,1,0,1)==0);
    assert(req(0,9,1,0,0)==2 && configuration==1 && address==-1);
    assert(req(0,9,2,0,0)==0);
    nusb.configuration=1;
    assert(req(0x80,8,0,0,1)==1 && data[0]==1);
    assert(req(0x81,10,0,0,1)==1 && data[0]==0);
    assert(req(1,11,0,0,0)==2);
    assert(req(1,11,1,0,0)==0);
    assert(req(0x82,0,0,0x80,2)==1 && data[0]==0 && length==2);
    assert(req(0x82,0,0,0x81,2)==0);
    assert(req(0xc0,0x51,100,0,65535)==1 && length==1024 && data[0]==100);
    assert(req(0xc0,0x51,65520,0,1024)==1 && length==4);
    assert(req(0xc0,0x51,65535,0,1024)==1 && length==0);
    assert(req(0xc0,0x51,0,1,1024)==0);
    assert(req(0x40,0x51,0,0,1024)==0); /* Reject host-to-device data. */
    assert(req(0xc0,0x52,0,0,65535)==1 && length==28 && !memcmp(data,"NUSB",4));
    assert(req(0xc0,0x52,0,0,28)==1 && data[4]==2);
    nord2_stream_init();
    assert(nord2_stream_ready && nord2_stream_size==65524);
    assert(nord2_log_stream_read(65520,data,1024)==4);
    assert(nord2_log_stream_read(UINT32_MAX,data,1024)==0);
    for (uint32_t i=65524;i<NORD2_STREAM_CAPACITY;i++) nord2_stream[i]=(uint8_t)(i*7);
    __atomic_store_n(&nord2_stream_size,NORD2_STREAM_CAPACITY,__ATOMIC_RELEASE);
    assert(req(0xc0,0x53,0,0,65535)==1 && length==8);
    uint32_t cap,size; memcpy(&cap,data,4); memcpy(&size,data+4,4);
    assert(cap==NORD2_STREAM_CAPACITY && size==cap);
    assert(req(0xc0,0x53,1,0,8)==0 && req(0xc0,0x53,0,1,8)==0);
    assert(req(0x40,0x54,0,0,1024)==0);
    assert(req(0xc0,0x54,17,1,65535)==1 && length==1024 && data[0]==119);
    unsigned tail=NORD2_STREAM_CAPACITY-3;
    assert(req(0xc0,0x54,tail&65535,tail>>16,1024)==1 && length==3);
    assert(req(0xc0,0x54,0,64,1024)==1 && length==0);
    assert(req(0xc0,0x54,65535,65535,1024)==1 && length==0);
    uint32_t rng=1;
    for (unsigned i=0;i<100000;i++) {
        uint8_t s[8];
        for (unsigned j=0;j<8;j++) {
            rng=rng*1664525u+1013904223u; s[j]=(uint8_t)(rng>>24);
        }
        int result=nusb_reply(s,data,&length,&address,&configuration);
        assert(result>=0 && result<=2 && length<=1024 && length<=nusb_le16(s+6));
        assert(address>=-1 && address<=127 && configuration>=-1 && configuration<=1);
        if (address>=0) assert(s[0]==0 && s[1]==5 && nusb_le16(s+6)==0);
        if (configuration>=0) assert(s[0]==0 && s[1]==9 && nusb_le16(s+6)==0);
    }
    puts("PASS: native USB descriptors, strict request decoding, read-only log bounds,"
         " invalid pmsg headers, 100000 malformed requests under ASan/UBSan");
}
'''
cfile = out / 'usb-control-host.c'
exe = out / 'usb-control-host'
stream = '#define NORD2_STREAM_CAPACITY' + early.split('#define NORD2_STREAM_CAPACITY', 1)[1].split('/* The GUI reuses', 1)[0]
cfile.write_text(prelude + reader + stream + decoder + cases)
subprocess.run(['clang', '-std=gnu11', '-Wall', '-Wextra', '-Werror',
                '-fsanitize=address,undefined', '-g', str(cfile), '-o', str(exe)],
               check=True)
subprocess.run([str(exe.resolve())], check=True)

cleanup = usb.split('int\nnord2_usb_prepare_return', 1)[1]
cleanup = 'int\nnord2_usb_prepare_return' + cleanup.split('static bool\nnusb_poll_continue', 1)[0]
model = r'''
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
static bool nusb_smp;
static unsigned core;
#define __builtin_arm_rsr64(s) ((uint64_t)core << 8)
#define NUSB_SOFTCON (1u<<6)
static struct {
    bool owned;
    uint32_t power, lv1ie, epie, commonie, linkie, devconf;
} nusb;
static uint32_t regs[0x2e00/4], writes;
static void nusb_phy_restore(void) {} /* Point14 does not take PHY ownership. */
static uint32_t nusb_rd(uint32_t off) { assert(off==0x2404); return regs[off/4]; }
static void nusb_wr(uint32_t off, uint32_t val) {
    if (writes++==0) assert(off==0x2404 && !(val & NUSB_SOFTCON));
    switch(off) {
    case 0xc: regs[4/4]&=~val; break;
    case 0x8c: regs[0x84/4]&=~val; break;
    case 8: regs[4/4]|=val; break;
    case 0x88: regs[0x84/4]|=val; break;
    case 0x80: case 0x241c: case 0x854: regs[off/4]&=~val; break;
    case 0x100: case 0x800: case 0x2418: case 0x850: case 0x2404:
        regs[off/4]=val; break;
    default: assert(0); /* No FIFO, DMA descriptor or unrelated MMIO write. */
    }
}
'''
model_cases = r'''
int main(void) {
    assert(nord2_usb_prepare_return()==0 && writes==0);
    nusb.owned=true; nusb.power=0x60a1; nusb.lv1ie=0xff; nusb.epie=0x3000b;
    nusb.commonie=0x3b7; nusb.linkie=1; nusb.devconf=0;
    regs[0x2404/4]=nusb.power|NUSB_SOFTCON;
    regs[4/4]=0; regs[0x84/4]=0x10001;
    regs[0x800/4]=5u<<24;
    regs[0x80/4]=regs[0x241c/4]=regs[0x854/4]=0xffffffffu;
    nusb_smp=true;
    for (core=1;core<8;core++) assert(nord2_usb_prepare_return()==-1 && !writes && nusb.owned);
    core=0;
    assert(nord2_usb_prepare_return()==1 && !nusb.owned && writes==13);
    assert(regs[0x2404/4]==nusb.power && regs[4/4]==nusb.lv1ie);
    assert(regs[0x84/4]==nusb.epie && regs[0x800/4]==nusb.devconf);
    assert(regs[0x2418/4]==nusb.commonie && regs[0x850/4]==nusb.linkie);
    assert(!regs[0x80/4] && !regs[0x241c/4] && !regs[0x854/4]);
    assert(nord2_usb_prepare_return()==0 && writes==13);
    puts("PASS: exact USB cleanup; inactive no-op, disconnect before restore,"
         " interrupt masks/address restored, no DMA/FIFO writes, idempotence");
}
'''
cfile = out / 'usb-cleanup-host.c'
exe = out / 'usb-cleanup-host'
cfile.write_text(model + cleanup + model_cases)
subprocess.run(['clang', '-std=gnu11', '-Wall', '-Wextra', '-Werror',
                '-fsanitize=address,undefined', '-g', str(cfile), '-o', str(exe)],
               check=True)
subprocess.run([str(exe.resolve())], check=True)
