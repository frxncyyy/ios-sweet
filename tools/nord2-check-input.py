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
# This model exercises input while the radio probe is inactive. The AP/PMIC
# snapshot and active radio restoration have their own exact-C fault models.
radio_start = code.index("static void\nni_wifi_snapshot(")
radio_end = code.index("\nint\nnord2_input_prepare_return(", radio_start)
code = code[:radio_start] + "static bool nw_restore(void) { return true; }\n" + code[radio_end:]
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
            assert(!(regs[0][0x60/4]&regs[0][0x160/4]&(15<<10)));
    } else regs[base][off/4]=v;
    if(base==4 && off==0xc80) {
        assert(v==0x2a); /* only TOPSTATUS READ */
        regs[4][0xca8/4]=(1<<15)|(wrap_complete ? 6<<1 : 2<<1);
        regs[4][0xc94/4]=0xa;
    }
    if(base==4 && off==0xca4) {
        assert(v==1);
        regs[4][0xca8/4]=1<<15;
    }
}
static bool model_pin(unsigned pin) {
    if((pin==204 || pin==202)) return !stuck_clock;
    assert((pin==205 || pin==203) && bit_pos<bit_count);
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
static void queue_read(const uint8_t *data,unsigned len) {
    bit(1); bit(0); bit(0); bit(1); bit(0);
    for(unsigned j=0;j<len;j++) for(unsigned i=0;i<8;i++) bit(data[j]&(0x80>>i));
    bit(1);
}
static void queue_ldo(uint8_t reg,uint8_t value) {
    uint8_t b[2]={value,ni_crc(0xc9,reg,value)};
    queue_read(b,2);
}
static void queue_write(uint8_t reg,uint8_t value) {
    bit(1); for(unsigned i=0;i<5;i++) bit(0); bit(1);
    queue_ldo(reg,value);
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
    assert(!(regs[0][0x60/4]&(15<<10))); /* bus released after STOP */
    reset_bits(); bit(1); bit(1); bit(1); /* address NACK, STOP */
    assert(!ni_read(0xa3,b,1) && bit_pos==bit_count && delays<1000);
    reset_bits(); stuck_clock=true;
    assert(!ni_read(0xa3,b,1) && delays<1000 && !bit_pos);
    assert(!ni_read(0,b,0) && !ni_read(0,b,63));

    uint16_t top=0;
    assert(!ni_keys_read(&top)); /* wrapper not initialized */
    regs[4][0xca8/4]=(1<<15)|(6<<1);
    assert(!ni_keys_read(&top)); /* do not consume somebody else's result */
    regs[4][0xca8/4]=1<<15; wrap_complete=true;
    assert(ni_keys_read(&top) && top==0xa && regs[4][0xca8/4]==1<<15);
    wrap_complete=false; delays=0;
    assert(!ni_keys_read(&top) && delays==1000);
    ni_saved=true; ni_bus_owned=false;
    assert(nord2_input_prepare_return()==-1); /* cannot hand active wrapper to Linux */
    ni_saved=true; regs[4][0xca8/4]=(1<<15)|(6<<1);
    assert(nord2_input_prepare_return()==1 && regs[4][0xca8/4]==1<<15);
    assert(nord2_input_prepare_return()==0);
    /* A partial setup must restore saved fields while retaining neighbours. */
    ni_saved=true; ni_bus_owned=false;
    for(unsigned i=0;i<7;i++) {
        unsigned pin=ni_pins[i].pin;
        ni_pins[i].mux=i+1; ni_pins[i].dir=(pin==20); ni_pins[i].dout=1;
        regs[0][(0x300+pin/8*16)/4]=0xabababab;
    }
    ni_ies_lt=1<<15; ni_ies_tm=1<<12; ni_ies_rt=0; ni_ies_pwr=1<<3;
    regs[1][0x40/4]=regs[2][0x50/4]=regs[3][0x30/4]=regs[2][0x60/4]=0xffffffff;
    assert(nord2_input_prepare_return()==1);
    uint32_t masks[1024]={0};
    for(unsigned i=0;i<7;i++) {
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
    assert(regs[2][0x60/4]==(0xffffffffu&~(1u<<6)));

    /* CRC vectors independently computed by polynomial division (0x107). */
    assert(ni_crc(0xc8,0x15,0xaa)==0x95);
    assert(ni_crc(0xc9,0x11,0x44)==0x2e);
    assert(ni_crc(0xc9,0x15,0xaa)==0xfe);
    regs[0][0x160/4]=0; reset_bits(); ni_ldo_changed=false;
    assert(!ni_ldo_write(0x15,0xaa) && !bit_pos);
    ni_ldo_changed=true; ni_ldo_volt=0xa9; ni_ldo_en=0;
    assert(!ni_ldo_write(0x12,0) && !ni_ldo_write(0x15,0xff));
    assert(!ni_ldo_write(0x11,0x10) && !bit_pos); /* mode changes forbidden */
    queue_write(0x15,0xaa);
    assert(ni_ldo_write(0x15,0xaa) && bit_pos==bit_count);
    reset_bits(); uint8_t wrong_crc[2]={0xaa,0xff}, value=0x77;
    queue_read(wrong_crc,2);
    assert(!ni_ldo_read(0x15,&value) && value==0x77 && bit_pos==bit_count);

    reset_bits(); ni_ldo_changed=false; bit(1); /* PMIC bus initially idle */
    const uint8_t id=0x53;
    queue_read(&id,1); queue_ldo(0x11,0); queue_ldo(0x15,0x8a);
    queue_write(0x11,0); queue_write(0x15,0xaa); queue_write(0x11,0x40);
    queue_ldo(0x11,0x44);
    assert(ni_power_on() && ni_ldo_changed && ni_ldo_volt==0x8a && !ni_ldo_en);
    assert(bit_pos==bit_count);
    reset_bits();
    queue_write(0x11,0); queue_write(0x15,0x8a); queue_write(0x11,0);
    assert(ni_power_restore() && !ni_ldo_changed && bit_pos==bit_count);

    /* An inherited enabled 3.0V rail needs no writes. */
    reset_bits(); bit(1);
    queue_read(&id,1); queue_ldo(0x11,0x44); queue_ldo(0x15,0xad);
    queue_ldo(0x11,0x44);
    assert(ni_power_on() && !ni_ldo_changed && bit_pos==bit_count);

    /* Write transport failure and wrong readback retain pending restoration. */
    ni_ldo_changed=true; ni_ldo_en=0; ni_ldo_volt=0x8a;
    reset_bits(); bit(1); bit(1); bit(1); /* address NACK then STOP */
    assert(!ni_power_restore() && ni_ldo_changed && bit_pos==bit_count);
    reset_bits();
    bit(1); for(unsigned i=0;i<5;i++) bit(0); bit(1);
    queue_ldo(0x11,0x40); /* disable did not stick: no voltage write allowed */
    assert(!ni_power_restore() && ni_ldo_changed && bit_pos==bit_count);
    reset_bits();
    queue_write(0x11,0);
    bit(1); for(unsigned i=0;i<5;i++) bit(0); bit(1);
    queue_ldo(0x15,0xaa); /* saved selector was not restored: do not enable */
    assert(!ni_power_restore() && ni_ldo_changed && bit_pos==bit_count);
    reset_bits();
    queue_write(0x11,0); queue_write(0x15,0x8a); queue_write(0x11,0);
    assert(ni_power_restore() && !ni_ldo_changed && bit_pos==bit_count);

    reset_bits(); bit(1); /* Bad identity cannot cause any power writes. */
    const uint8_t wrong_id=0x20; queue_read(&wrong_id,1); bit(1);
    assert(!ni_power_on() && !ni_ldo_changed && bit_pos==bit_count);
    puts("PASS: x4/multitouch/up/invalid reports, 100k fuzz, HID changes, I2C read/NACK/"
         "stuck-clock/STOP/bounds, PMIF timeout/ownership, masked pin/IES restore,"
         " CRC/identity/write allowlist, 3.0V supply enable/readback/restoration,"
         " inherited-rail preservation and failed-write restoration refusal");
}
'''
cfile, exe = out / "input-host.c", out / "input-host"
cfile.write_text(model + code + cases)
subprocess.run(["clang", "-std=gnu11", "-Wall", "-Wextra", "-Werror",
                "-Wno-unused-function", "-Wno-unused-variable",
                "-fsanitize=address,undefined", str(cfile), "-o", str(exe)], check=True)
subprocess.run([str(exe.resolve())], check=True)
