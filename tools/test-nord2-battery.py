#!/usr/bin/env python3
"""Canonical Nord2 battery collector/PIO/presentation tests; never native MMIO."""
from pathlib import Path
import subprocess,tempfile
ROOT=Path(__file__).resolve().parents[1]
P=ROOT/'shim/nord2-xnu'
FIX=ROOT/'tools/fixtures/nord2-battery'
with tempfile.TemporaryDirectory() as d:
 # Other boards must not acquire a fuel symbol or need Nord2/IOKit headers.
 for name in ['nord2_battery.c','mtk_i2c_read.c','nord2_battery_platform.cpp']:
  obj=Path(d)/(name+'.default.o')
  subprocess.run(['cc' if name.endswith('.c') else 'c++','-c',str(P/name),'-o',str(obj)],check=True)
  symbols=subprocess.run(['nm','-g','--defined-only',str(obj)],capture_output=True,text=True,check=True).stdout
  assert not symbols.strip(),name
 for test,src in [('test.c','nord2_battery.c'),('test-i2c.c','mtk_i2c_read.c')]:
  exe=Path(d)/test
  subprocess.run(['cc','-std=c11','-DNORD2=1','-I'+str(P),'-Wall','-Wextra','-Werror','-fsanitize=address,undefined',str(P/src),str(FIX/test),'-o',str(exe)],check=True)
  subprocess.run([str(exe)],check=True)
 text=(P/'SweetBattery.cpp').read_text();method=text[text.index('void\nSweetBattery::pollNord2'):text.index('#endif\n\nvoid\nSweetBattery::poll(void)')].replace('SweetBattery::','Battery::')
 harness=r'''
#include <stdint.h>
#include <string>
#include <map>
#include <cassert>
#include "nord2_battery.h"
using UInt32=uint32_t;
struct OSSymbol{std::string value;static OSSymbol *withCString(const char*s){static OSSymbol v;v.value=s;return &v;}void release() const {}};
static nbg_sample sample;static int rc,calls;
static int nord2_battery_platform_sample(nbg_sample *s){calls++;*s=sample;return rc;}
#define IOLog(...) ((void)0)
class Battery {public:bool fReal=false;int fLastMode=-1;std::map<std::string,int> fields;std::string error;
void pollNord2();void putNumber(const char*k,UInt32 v){fields[k]=(int)v;}void putBool(const char*k,bool v){fields[k]=v;}
void setErrorCondition(OSSymbol*s){error=s->value;}void updateStatus(){}
#define SETTER(name) void set##name(int v){fields[#name]=v;}
SETTER(BatteryInstalled) SETTER(CurrentCapacity) SETTER(MaxCapacity) SETTER(TimeRemaining) SETTER(ExternalConnected) SETTER(ExternalChargeCapable) SETTER(IsCharging) SETTER(Voltage) SETTER(Amperage) SETTER(AtWarnLevel) SETTER(AtCriticalLevel)
};
'''+method+r'''
int main(){Battery b;b.pollNord2();assert(!calls&&b.fields["MaxCapacity"]==0&&!b.fields["IsCharging"]&&!b.fields["ExternalConnected"]&&!b.error.empty());
b.fReal=true;sample={};sample.soc_valid=1;sample.percent=42;sample.voltage_valid=1;sample.pack_mv=7800;sample.charger_valid=1;sample.charging=1;sample.external=1;b.pollNord2();assert(b.fields["CurrentCapacity"]==42&&b.fields["MaxCapacity"]==100&&b.fields["IsCharging"]&&b.fields["Voltage"]==7800&&b.error.empty());
rc=-1;b.pollNord2();assert(b.fields["CurrentCapacity"]==0&&b.fields["MaxCapacity"]==0&&!b.fields["Nord2BatterySOCValid"]&&!b.fields["AtCriticalLevel"]);
}
'''
 file=Path(d)/'presentation.cpp';file.write_text(harness);exe=Path(d)/'presentation'
 subprocess.run(['c++','-std=c++17','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-I'+str(P),str(file),'-o',str(exe)],check=True)
 subprocess.run([str(exe)],check=True)
 # Extract actual software-only report/dedup code; no native MMIO binding.
 native=(P/'nord2_battery_platform.cpp').read_text()
 reports=native[native.index('static unsigned bus_logged'):native.index('static vm_offset_t address')]
 raw=native[native.index(' unsigned valid=s->raw_valid'):native.index(' __sync_lock_release')]
 telemetry=r'''#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <climits>
#include "nord2_battery.h"
#include "mtk_i2c_read.h"
#include "nbg_telemetry.h"
static unsigned lines;
static void IOLog(const char *fmt,...){char line[512];va_list args;va_start(args,fmt);int n=vsnprintf(line,sizeof(line),fmt,args);va_end(args);assert(n>0&&n<255);lines++;}
'''+reports+'static void sample_report(nbg_sample *s,int e){'+raw+'}'+r'''
int main(){nbg_i2c_state s={};report_bus(7,-1,&s);assert(lines==2);report_bus(7,-1,&s);assert(lines==2);s.failure=4;report_bus(7,-1,&s);assert(lines==4);report_bus(11,-1,&s);assert(lines==6);
nbg_sample sample={};sample_report(&sample,-1);assert(lines==7);sample_report(&sample,-1);assert(lines==7);sample.raw_valid=3;sample_report(&sample,-1);assert(lines==8);
IOLog(NBG_I2C_LOG,UINT_MAX,INT_MIN,UINT_MAX,UINT_MAX,UINT_MAX,UINT_MAX,UINT_MAX,UINT_MAX,UINT_MAX,UINT_MAX,UINT_MAX,UINT_MAX);
IOLog(NBG_CLOCK_LOG,UINT_MAX,UINT_MAX,UINT_MAX,UINT_MAX,UINT_MAX,UINT_MAX,UINT_MAX,UINT_MAX);
IOLog(NBG_RAW_LOG,INT_MIN,UINT_MAX,UINT_MAX,UINT_MAX,UINT_MAX,UINT_MAX,UINT_MAX,UINT_MAX,UINT_MAX,UINT_MAX,UINT_MAX,UINT_MAX);
}
'''
 file=Path(d)/'telemetry.cpp';file.write_text(telemetry);exe=Path(d)/'telemetry'
 subprocess.run(['c++','-std=c++17','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-I'+str(P),str(file),'-o',str(exe)],check=True)
 subprocess.run([str(exe)],check=True)
print('PASS: source-derived SOC/charger/invalid samples, actual canonical publishing method, injected PIO success/26M+PLL timing/all callback faults+offline no replay/2000poll bound; telemetry62callbacks unchanged/first-change-dedup/255byte bound; no native hardware')
