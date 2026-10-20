#!/usr/bin/env python3
"""Run the Nord2 C input code against bounded GPIO/PWRAP and HID models."""
from pathlib import Path
import re
import subprocess
import sys

source, out = map(Path, sys.argv[1:])
out.mkdir(parents=True, exist_ok=True)
text = (source / "bsd/dev/sweet_touch.c").read_text()
code = "enum { NI_SCL" + text.split("enum { NI_SCL", 1)[1].split(
    "static void\nni_thread(", 1)[0]
for name, body in (
    ("ni_rd", "return model_read(base, off);"),
    ("ni_wr", "model_write(base, off, v);"),
    ("ni_level", "return model_pin(pin);"),
    ("ni_wait", "(void)ms;"),
):
    code, count = re.subn(
        rf"({name}\([^{{]+\)\n\{{).*?(\n\}})", rf"\1\n\t{body}\2",
        code, count=1, flags=re.S)
    assert count == 1
model = r'''
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
typedef uintptr_t vm_offset_t;
static uint32_t regs[5][1024];
static bool bits[1024], stuck_clock, wrap_complete;
static unsigned bit_count, bit_pos, delays, events, last_down, last_finger;
static uint32_t model_read(vm_offset_t base,unsigned off) {
    assert(base<5 && off<4096 && !(off%4));
    return regs[base][off/4];
}
static void model_write(vm_offset_t base,unsigned off,uint32_t v) {
    assert(base<5 && off<4096 && !(off%4));
    if(base==0 && ((off&15)==4 || (off&15)==8)) {
        unsigned idx=(off&~15)/4;
        if((off&15)==4) regs[base][idx]|=v; else regs[base][idx]&=~v;
        if((off&~15)==0x60) /* I2C pins must NEVER drive high */
            assert(!(regs[0][0x60/4]&regs[0][0x160/4]&(3<<12)));
    } else regs[base][off/4]=v;
    if(base==4 && off==0xc20) {
        assert(v==((0x2a/2)<<16)); /* only TOPSTATUS READ */
        regs[4][0xc24/4]=(1<<21)|(wrap_complete ? (6<<16)|0xa : 2<<16);
    }
    if(base==4 && off==0xc28) {
        assert(v==1);
        regs[4][0xc24/4]=1<<21;
    }
}
static bool model_pin(unsigned pin) {
    if(pin==204) return !stuck_clock;
    assert(pin==205 && bit_pos<bit_count);
    return bits[bit_pos++];
}
static void delay(unsigned n) { delays+=n; assert(delays<10000000); }
static void sweet_hid_touch(int finger,int down,int x,int y) {
    assert(finger>=0 && finger<10 && x>=0 && x<1080 && y>=0 && y<2400);
    events++; last_down=down; last_finger=finger;
}
static void reset_bits(void) { bit_count=bit_pos=delays=0; stuck_clock=false; }
static void bit(bool b) { bits[bit_count++]=b; }
'''
cases = r'''
static void point(uint8_t b[62],unsigned slot,unsigned id,unsigned event,unsigned x,unsigned y) {
    unsigned o=6*slot;
    b[o+2]=(event<<6)|(x>>10); b[o+3]=x>>2;
    b[o+4]=(id<<4)|(y>>10); b[o+5]=y>>2;
    b[o+6]=((x&3)<<6)|((y&3)<<4); b[o+7]=8;
}
int main(void) {
    uint8_t b[62]; uint32_t active=0; int x[10]={0},y[10]={0};
    memset(b,255,sizeof b); b[1]=0;
    assert(ni_decode(b,&active,x,y) && !active);
    b[1]=2; point(b,0,9,0,4319,9599); point(b,1,0,2,0,0);
    assert(ni_decode(b,&active,x,y) && active==513 && x[9]==1079 && y[9]==2399);
    ni_report(active,x,y); assert(events==2);
    ni_report(active,x,y); assert(events==2);
    ni_report(1,x,y); assert(events==3 && last_finger==9 && !last_down);
    ni_report(0,x,y); assert(events==4 && !ni_active);
    point(b,1,9,0,0,0); assert(!ni_decode(b,&active,x,y)); /* duplicate */
    point(b,1,10,0,0,0); assert(!ni_decode(b,&active,x,y)); /* invalid ID */
    point(b,1,0,3,0,0); assert(!ni_decode(b,&active,x,y)); /* reserved event */
    point(b,1,0,0,4320,0); assert(!ni_decode(b,&active,x,y));
    point(b,1,0,0,0,9600); assert(!ni_decode(b,&active,x,y));
    memset(b,255,sizeof b); b[1]=10;
    for(unsigned i=0;i<10;i++) point(b,i,i,2,i*400,i*900);
    assert(ni_decode(b,&active,x,y) && active==1023);
    b[1]=11; assert(!ni_decode(b,&active,x,y));
    b[1]=0; assert(!ni_decode(b,&active,x,y));
    for(unsigned i=0;i<10;i++) point(b,i,i,1,0,0);
    assert(ni_decode(b,&active,x,y) && !active); /* all fingers UP */
    uint32_t random=17;
    for(unsigned n=0;n<100000;n++) {
        for(unsigned i=0;i<62;i++) { random=random*1664525+1013904223; b[i]=random>>24; }
        (void)ni_decode(b,&active,x,y); /* ASan/UBSan bounds */
    }

    ni_gpio=0; ni_lt=1; ni_tm=2; ni_rt=3; ni_pwrap=4;
    reset_bits(); bit(1); bit(0); bit(0); bit(1); bit(0);
    const uint8_t expected[3]={0x54,0x52,0xa5};
    for(unsigned j=0;j<3;j++) for(unsigned i=0;i<8;i++) bit(expected[j]&(0x80>>i));
    bit(1);
    assert(ni_read(0xa3,b,3) && !memcmp(b,expected,3) && bit_pos==bit_count);
    assert(!(regs[0][0x60/4]&(3<<12))); /* bus released after STOP */
    reset_bits(); bit(1); bit(1); bit(1); /* address NACK, STOP */
    assert(!ni_read(0xa3,b,1) && bit_pos==bit_count && delays<1000);
    reset_bits(); stuck_clock=true;
    assert(!ni_read(0xa3,b,1) && delays<1000 && !bit_pos);
    assert(!ni_read(0,b,0) && !ni_read(0,b,63));

    uint16_t top=0;
    assert(!ni_keys_read(&top)); /* wrapper not initialized */
    regs[4][0xc24/4]=(1<<21)|(6<<16);
    assert(!ni_keys_read(&top)); /* do not consume somebody else's result */
    regs[4][0xc24/4]=1<<21; wrap_complete=true;
    assert(ni_keys_read(&top) && top==0xa && regs[4][0xc24/4]==1<<21);
    wrap_complete=false; delays=0;
    assert(!ni_keys_read(&top) && delays==1000);
    ni_saved=true; ni_bus_owned=false;
    assert(nord2_input_prepare_return()==-1); /* cannot hand active wrapper to Linux */
    ni_saved=true; regs[4][0xc24/4]=(1<<21)|(6<<16);
    assert(nord2_input_prepare_return()==1 && regs[4][0xc24/4]==1<<21);
    assert(nord2_input_prepare_return()==0);
    /* A partial setup must restore saved fields while retaining neighbours. */
    ni_saved=true; ni_bus_owned=false;
    for(unsigned i=0;i<5;i++) {
        unsigned pin=ni_pins[i].pin;
        ni_pins[i].mux=i+1; ni_pins[i].dir=(pin==20); ni_pins[i].dout=1;
        regs[0][(0x300+pin/8*16)/4]=0xabababab;
    }
    ni_ies_lt=1<<15; ni_ies_tm=1<<12; ni_ies_rt=0;
    regs[1][0x40/4]=regs[2][0x50/4]=regs[3][0x30/4]=0xffffffff;
    assert(nord2_input_prepare_return()==1);
    uint32_t masks[1024]={0};
    for(unsigned i=0;i<5;i++) {
        unsigned pin=ni_pins[i].pin, mo=(0x300+pin/8*16)/4, shift=pin%8*4;
        masks[mo]|=15u<<shift;
        assert(((regs[0][mo]>>shift)&15)==ni_pins[i].mux);
        assert(((regs[0][(pin/32*16)/4]>>(pin%32))&1)==ni_pins[i].dir);
        assert((regs[0][(0x100+pin/32*16)/4]>>(pin%32))&1);
    }
    for(unsigned i=0;i<1024;i++) if(masks[i])
        assert((regs[0][i]&~masks[i])==(0xabababab&~masks[i]));
    assert(regs[1][0x40/4]==(0xffffffffu&~(1u<<16)));
    assert(regs[2][0x50/4]==(0xffffffffu&~(1u<<13)));
    assert(regs[3][0x30/4]==(0xffffffffu&~(1u<<24)));
    puts("PASS: x4/multitouch/up/invalid reports, 100k fuzz, HID changes, I2C read/NACK/"
         "stuck-clock/STOP/bounds, PWRAP timeout/ownership, masked pin/IES restoration");
}
'''
cfile, exe = out / "input-host.c", out / "input-host"
cfile.write_text(model + code + cases)
subprocess.run(["clang", "-std=gnu11", "-Wall", "-Wextra", "-Werror",
                "-Wno-unused-function", "-Wno-unused-variable",
                "-fsanitize=address,undefined", str(cfile), "-o", str(exe)], check=True)
subprocess.run([str(exe.resolve())], check=True)
