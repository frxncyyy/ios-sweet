#!/usr/bin/env python3
"""Exercise the exact Nord2 power probe with MMIO/PMIF fault injection."""
from pathlib import Path
import os
import subprocess
import tempfile

canonical = Path(__file__).resolve().parents[1] / "shim/nord2-xnu"
source = Path("research/ci-39158169212/buildtree/xnu/bsd/dev/sweet_touch.c").read_text()
pmic = source[source.index("static bool\nni_pmic_read("):
              source.index("static void\nni_wifi_snapshot(")]
actual = source[source.index("/* NORD2 Wi-Fi power probe."):
                source.index("\nint\nnord2_input_prepare_return(")]
assert source.index("if (!nw_restore()) { return -1; }") < source.index(
    "if (!ni_saved) { return 0; }")
thread = source[source.index("static void\nni_thread("):]
assert thread.index("ni_saved = true") < thread.index("ni_wifi_power_probe();")

prefix = r'''
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <stdarg.h>
#include "wifi_common.h"
#include "wifi_cold_snapshot.h"
#define NCC_DEVICE_BARRIER() ((void)0)
#define NCC_TICKS() UINT64_C(42)
#define NCC_FREQUENCY() UINT64_C(13000000)
typedef uintptr_t vm_offset_t;
static vm_offset_t ni_pwrap=0x10026000, ni_gpio=0x10005000, ni_rt=0x11ea0000;
static bool ni_pwrap_owned;
static uint16_t pm[65536], pm_original[65536];
static unsigned fsm, addr, wdata, commands, pmwrites, apwrites, connreads;
static unsigned fail_write, drop_prot, settling13, settling18, wait_us;
static int bootarg, fault;
static bool reported_success, reported_restore, panicked;
static jmp_buf panic_env;
enum { F_NONE, F_RAIL, F_PWR1, F_PWR2, F_CLEAR, F_SLEEP, F_ID,
       F_CONNMAP, F_SET, F_OFF, F_PMSTUCK };
static const uintptr_t addresses[] = {
 0x10006000,0x10006304,0x1000616c,0x10006170,0x10006504,
 0x10001228,0x10001724,0x1020e504,
 0x10005450,0x10005460,0x11ea0000,0x11ea0050,0x11ea0070,
 0x18060184,0x18001000,0x18001004
};
static uint32_t regs[16], original[16];
static uint32_t *reg(uintptr_t a) {
 for(unsigned i=0;i<16;i++)if(addresses[i]==a)return &regs[i];
 assert(!"unexpected AP/CONN address");return NULL;
}
#define R(a) (*reg(a))
static void delay(unsigned n) { assert(n<=6000);wait_us+=n; }
static void require_conn_access(void) {
 assert((R(0x10006304)&31)==13 && (R(0x1000616c)&2) && (R(0x10006170)&2));
 assert(!(R(0x10001228)&0x6000) && !(R(0x10001724)&4) && (R(0x1020e504)&1));
 assert((pm[0x1c20]&3)==3 && (pm[0x1c32]&3)==3);
}
static uint32_t ni_rd(vm_offset_t b,unsigned off) {
 if(b==ni_pwrap) {
  if(off==0xca8)return 0x8000|(fsm<<1);
  assert(off==0xc94 && fsm==6);
  if(addr==0x1c20 && settling13 && !--settling13)pm[addr]=0;
  if(addr==0x1c32 && settling18 && !--settling18)pm[addr]=0;
  return pm[addr];
 }
 uintptr_t a=b+off;
 if(a>=0x18000000) {
  require_conn_access();connreads++;
  if(a==0x18060184 && fault==F_SLEEP)return 0x20;
  if(a==0x18001000 && fault==F_ID)return 0xdeadfeed;
 }
 return R(a);
}
static void ni_wr(vm_offset_t b,unsigned off,uint32_t v) {
 if(b==ni_pwrap) {
  if(off==0xca4){assert(v==1 && fsm==6);fsm=0;return;}
  if(off==0xc84){assert(fsm==0 && !(v&0xffff0000));wdata=v;return;}
  assert(off==0xc80 && fsm==0 && !(v&~0x2000ffffu));
  addr=v&0xffff;commands++;
  if(!(v&(1u<<29))){fsm=6;return;}
  pmwrites++;
  assert(addr==0x1c34 || addr==0x1c3a || addr==0x1c2e ||
         addr==0x1c22 || addr==0x1c28 || addr==0x1c1c);
  unsigned mask=(addr==0x1c2e || addr==0x1c1c)?3:0xf0;
  assert(!((pm[addr]^wdata)&~mask));
  if(fault==F_PMSTUCK){fsm=1;return;}
  if(fail_write==pmwrites)return;
  pm[addr]=wdata;
  if(addr==0x1c1c || addr==0x1c2e) {
   unsigned mon=addr==0x1c1c?0x1c20:0x1c32;
   if(wdata&1)pm[mon]=fault==F_RAIL?0:3;
   else if(pm[mon]) {
    if(mon==0x1c20)settling13=3;else settling18=3;
   }
  }
  return;
 }
 apwrites++;
 uintptr_t a=b+off;
 assert(a<0x18000000); /* Probe never writes into CONN/firmware/DMA. */
 if(a==0x100012a0 || a==0x100012a4) {
  assert(!(v&~0x6000u));
  if(a==0x100012a0) {
   if(fault==F_SET && connreads)return;
   R(0x10001228)|=v;
  } else {
   if(fault==F_CLEAR && !drop_prot++){return;}
   R(0x10001228)&=~v;
  }
  return;
 }
 if(a==0x10001714 || a==0x10001718) {
  assert(!(v&~4u));
  if(a==0x10001714)R(0x10001724)|=v;else R(0x10001724)&=~v;
  return;
 }
 if(a==0x10006000) {
  assert((v>>16)==0xb16);R(a)=v&0xffff;return;
 }
 if(a==0x10006304) {
  assert(!((R(a)^v)&~31u));
  if((R(a)&12)&~v) {
   assert((R(0x10001228)&0x6000)==0x6000 && (R(0x10001724)&4));
   assert((v&0x13)==0x12);
  }
  R(a)=v;
  if(v&4) {
   assert((R(0x1020e504)&1) && (pm[0x1c20]&3)==3 && (pm[0x1c32]&3)==3);
   if(fault!=F_PWR1)R(0x1000616c)|=2;
  } else if(fault!=F_OFF)R(0x1000616c)&=~2u;
  if(v&8){if(fault!=F_PWR2)R(0x10006170)|=2;}
  else if(fault!=F_OFF)R(0x10006170)&=~2u;
  return;
 }
 unsigned mask=0;
 switch(a) {
 case 0x1020e504:mask=1;break;
 case 0x10005450:mask=0xffff0000;break;
 case 0x10005460:mask=0x0fffffff;break;
 case 0x11ea0000:mask=0x3fff8;break;
 case 0x11ea0050:case 0x11ea0070:mask=0x4800;break;
 default:assert(!"unexpected AP write");
 }
 assert(!((R(a)^v)&~mask));R(a)=v;
}
static void ni_mask(vm_offset_t b,unsigned off,uint32_t m,uint32_t v) {
 ni_wr(b,off,(ni_rd(b,off)&~m)|(v&m));
}
static vm_offset_t ml_io_map(vm_offset_t a,size_t n) {
 assert(n==4096);
 assert(a==0x10006000 || a==0x10001000 || a==0x1020e000 ||
        a==0x18060000 || a==0x18001000);
 if(a>=0x18000000) {
  require_conn_access();
  if(fault==F_CONNMAP)return 0;
 }
 return a;
}
static void PE_parse_boot_argn(const char *n,void *v,size_t s) {
 if(!strcmp(n,"nord2wificommon")){assert(s==sizeof(unsigned));*(unsigned*)v=0;return;}
 assert(!strcmp(n,"nord2wifi") && s==sizeof(int));*(int*)v=bootarg;
}
static int quiet_printf(const char *fmt,...) {
 va_list ap;va_start(ap,fmt);
 if(strstr(fmt,"power probe stage=")) {
  (void)va_arg(ap,unsigned);(void)va_arg(ap,unsigned);(void)va_arg(ap,unsigned);
  reported_success=va_arg(ap,int);
 }
 if(strstr(fmt,"power probe restored="))reported_restore=va_arg(ap,int);
 va_end(ap);return 0;
}
static void panic(const char *fmt,...) {(void)fmt;panicked=true;longjmp(panic_env,1);}
#define printf quiet_printf
'''
suffix = r'''
#undef printf
static void reset(void) {
 memset(&nw,0,sizeof(nw));memset(pm,0,sizeof(pm));memset(regs,0,sizeof(regs));
 ni_pwrap_owned=false;fsm=addr=wdata=commands=pmwrites=apwrites=connreads=0;
 fail_write=drop_prot=settling13=settling18=wait_us=0;
 fault=F_NONE;bootarg=2;reported_success=reported_restore=panicked=false;
 R(0x10006000)=1;R(0x10006304)=0x12;
 R(0x1000616c)=R(0x10006170)=0x10580e01;R(0x10006504)=0x401425;
 R(0x10001228)=0xe70c500e;R(0x10001724)=0xe7;
 R(0x10005450)=0x6666;R(0x10005460)=0x10000000;
 R(0x11ea0050)=0x87ffc3f;R(0x11ea0070)=0x18003c0;
 R(0x18001000)=0x20010101;R(0x18001004)=3;
 pm[0x1c34]=pm[0x1c22]=0x8000;pm[0x202e]=0x400;pm[0x2a]=14;
 memcpy(original,regs,sizeof(regs));memcpy(pm_original,pm,sizeof(pm));
}
static void check_restored(void) {
 assert(!nw.saved && !nw.domain_touched && !panicked && fsm==0);
 assert(!memcmp(original,regs,sizeof(regs)) && !memcmp(pm_original,pm,sizeof(pm)));
 assert(wait_us<500000);
}
static void run(void) {if(!setjmp(panic_env))ni_wifi_power_probe();}
int main(void) {
 reset();uint16_t key=0;assert(ni_keys_read(&key) && key==14);
 run();assert(reported_success && reported_restore && connreads==3);check_restored();
 for(int b=-1;b<5;b++)if(b!=2) {
  reset();bootarg=b;run();assert(!commands && !apwrites && !nw.saved);
 }
 for(unsigned which=0;which<11;which++) {
  reset();
  switch(which) {
  case 0:R(0x10006304)=13;break;
  case 1:R(0x1000616c)|=2;break;
  case 2:R(0x10006170)|=2;break;
  case 3:R(0x10006504)&=~1u;break;
  case 4:pm[0x202e]=0x500;break;
  case 5:pm[0x1c20]=3;break;
  case 6:pm[0x1c32]=3;break;
  case 7:R(0x10005450)|=0x10000;break;
  case 8:pm[0x1c34]|=16;break;
  case 9:R(0x10001228)&=~0x4000u;break;
  case 10:R(0x10001724)&=~4u;break;
  }
  run();assert(!apwrites && !pmwrites && !connreads && !nw.saved);
 }
 for(unsigned fail=1;fail<=4;fail++) {
  reset();fail_write=fail;run();
  assert(!reported_success && reported_restore && !connreads);check_restored();
 }
 for(int f=F_RAIL;f<=F_CONNMAP;f++) {
  reset();fault=f;run();
  assert(!reported_success && reported_restore);check_restored();
 }
 for(int f=F_SET;f<=F_PMSTUCK;f++) {
  reset();fault=f;run();
  assert(panicked && nw.saved && !reported_restore);
  if(f!=F_PMSTUCK)assert((pm[0x1c20]&1) && (pm[0x1c32]&1));
  fault=F_NONE;fsm=0;panicked=false;
  assert(nw_restore());check_restored();
 }
 reset();assert(!nw_pmic_write(0,false));nw.saved=true;
 assert(!nw_pmic_write(6,false) && !nw_pmic_write(65535,true) && !pmwrites);
 puts("PASS: exact power C, cold guards, write bounds, gated CONN reads, partial-on/PMIF/rail/bus faults, restoration and refusal to return on failure");
}
'''
with tempfile.TemporaryDirectory() as tmp:
    c = Path(tmp) / "wifi-power.c"
    exe = Path(tmp) / "wifi-power"
    c.write_text(prefix + pmic + actual + suffix)
    subprocess.run([os.environ.get("CC", "cc"), "-std=c11", "-O1", "-g",
                    "-Wall", "-Wextra", "-Werror", "-fsanitize=address,undefined",
                    "-fno-omit-frame-pointer", "-I" + str(canonical), str(c),
                    str(canonical / "wifi_common.c"),
                    str(canonical / "wifi_cold_snapshot.c"), "-o", str(exe)], check=True)
    subprocess.run([str(exe)], check=True)
