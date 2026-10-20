#!/usr/bin/env python3
"""Run the actual Nord2 PMIF/snapshot C with bounded MMIO mocks and sanitizers."""
from pathlib import Path
import argparse
import os
import subprocess
import tempfile

ap = argparse.ArgumentParser(description=__doc__)
ap.add_argument("source", type=Path, nargs="?",
                default=Path("research/ci-39158169212/buildtree/xnu/bsd/dev/sweet_touch.c"))
args = ap.parse_args()
source = args.source.read_text()
start = source.index("static bool\nni_pmic_read(")
end = source.index("/* NORD2 Wi-Fi power probe.", start)
actual = source[start:end]
thread = source[source.index("static void\nni_thread("):]
assert thread.index("ni_saved = true") < thread.index("ni_wifi_snapshot();")
assert thread.index("ni_wifi_snapshot();") < thread.index("ni_gpio_bit(pin, 0, false)")

prefix = r'''
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
typedef uintptr_t vm_offset_t;
static vm_offset_t ni_pwrap=0x10026000, ni_gpio=0x10005000, ni_rt=0x11ea0000;
static bool ni_pwrap_owned, initialized, stalled, map_failed;
static unsigned state, commands, clears, reads, waits, maps, bad_access, address;
static int bootarg;
static const uint16_t allowed[] = {
 0x2a,0x788,0x79e,0x1be2,0x1be6,0x1be8,0x1bee,0x1bf4,
 0x1c08,0x1c0c,0x1c0e,0x1c14,0x1c1a,0x1c1c,0x1c20,0x1c22,
 0x1c28,0x1c2e,0x1c32,0x1c34,0x1c3a,0x1f98,0x1f9c,0x2010,0x202e
};
static bool allowed_addr(unsigned a) {
 for (unsigned i=0;i<sizeof(allowed)/sizeof(allowed[0]);i++)
  if(a==allowed[i])return true;
 return false;
}
static void reset(void) {
 ni_pwrap_owned=false; initialized=true; stalled=false; map_failed=false;
 state=commands=clears=reads=waits=maps=bad_access=address=0;bootarg=0;
}
static void delay(unsigned n) {assert(n==1);waits++;}
static uint32_t ni_rd(vm_offset_t base,unsigned offset) {
 reads++;
 if (base==ni_pwrap) {
  if(offset==0xca8)return (initialized?(1u<<15):0)|(state<<1);
  if(offset==0xc94){assert(state==6);return address^0xa55a;}
  assert(!"unexpected PMIF read");
 }
 const uint64_t permitted[] = {
 0x10006000,0x10006304,0x1000616c,0x10006170,0x10006504,
 0x10001228,0x10001724,0x1020e504,0x10007018,
 0x10005450,0x10005460,0x11ea0000,0x11ea0050,0x11ea0070
 };
 for(unsigned i=0;i<sizeof(permitted)/sizeof(permitted[0]);i++)
  if(base+offset==permitted[i])return 0;
 bad_access++;assert(!"read outside AP-side allowlist");return 0;
}
static void ni_wr(vm_offset_t base,unsigned offset,uint32_t value) {
 assert(base==ni_pwrap);
 if(offset==0xc80) {
  assert(initialized && state==0 && allowed_addr(value));
  assert(!(value&0xffff0000)); /* No PMIC write/data bits. */
  address=value;commands++;state=stalled?1:6;return;
 }
 assert(offset==0xca4 && value==1 && state==6);
 state=0;clears++;
}
static vm_offset_t ml_io_map(vm_offset_t address,size_t length) {
 assert(length==0x1000);
 assert(address==0x10006000 || address==0x10001000 ||
        address==0x1020e000 || address==0x10007000);
 maps++;return map_failed?0:address;
}
static void PE_parse_boot_argn(const char *name,void *value,size_t n) {
 assert(!strcmp(name,"nord2wifi") && n==sizeof(int));*(int*)value=bootarg;
}
static int quiet_printf(const char *format,...) {(void)format;return 0;}
#define printf quiet_printf
'''
suffix = r'''
#undef printf
int main(void) {
 uint16_t value=0;
 for(unsigned a=0;a<65536;a++) {
  reset();bool accepted=ni_pmic_read(a,&value);
  assert(accepted==allowed_addr(a));
  if(accepted) assert(commands==1 && clears==1 && state==0 && value==(uint16_t)(a^0xa55a));
  else assert(reads==0 && commands==0 && clears==0);
 }
 reset();assert(ni_keys_read(&value) && address==0x2a && commands==1);
 reset();initialized=false;assert(!ni_keys_read(&value) && commands==0);
 reset();state=6;assert(!ni_keys_read(&value) && commands==0 && clears==0);
 reset();state=1;assert(!ni_keys_read(&value) && commands==0);
 reset();state=6;ni_pwrap_owned=true;
 assert(ni_keys_read(&value) && commands==1 && clears==2);
 reset();stalled=true;
 assert(!ni_keys_read(&value) && commands==1 && waits==1000 && clears==0);
 for(int disabled=-1;disabled<=3;disabled++) {
  if(disabled==1 || disabled==2)continue;
  reset();bootarg=disabled;ni_wifi_snapshot();assert(maps==0 && reads==0 && commands==0);
 }
 reset();bootarg=1;map_failed=true;ni_wifi_snapshot();assert(maps==4 && reads==0 && commands==0);
 reset();bootarg=1;ni_wifi_snapshot();
 assert(maps==4 && commands==24 && clears==24 && state==0 && bad_access==0);
 reset();bootarg=1;stalled=true;ni_wifi_snapshot();
 assert(commands==1 && waits==1000 && clears==0 && bad_access==0);
 reset();bootarg=1;state=6;ni_wifi_snapshot();assert(commands==0 && clears==0);
 puts("PASS: all 65536 PMIC addresses, key regression, ownership, timeout, AP-only snapshot and fallback order");
}
'''
with tempfile.TemporaryDirectory(prefix="nord2-wifi-test-") as directory:
    c = Path(directory) / "test.c"
    exe = Path(directory) / "test"
    c.write_text(prefix + actual + suffix)
    subprocess.run([os.environ.get("CC", "clang"), "-std=c11", "-O1", "-g",
                    "-fsanitize=address,undefined", "-fno-omit-frame-pointer",
                    "-Wall", "-Wextra", "-Werror", str(c), "-o", str(exe)], check=True)
    subprocess.run([str(exe)], check=True)
