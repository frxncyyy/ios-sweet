#!/usr/bin/env python3
"""Exercise production swap/timer diagnostic paths with a small framebuffer model."""
from pathlib import Path
import subprocess
import tempfile
root=Path(__file__).resolve().parent.parent
s=(root/'research/ci-39158169212/buildtree/xnu/iokit/Kernel/arm/SweetFramebuffer.cpp').read_text()
def extract(start,end):return s[s.index(start):s.index(end,s.index(start))]
struct=extract('struct SweetFBDiagnostics {','class AppleCLCD :')
report=extract('void\nAppleCLCD::reportDiagnostics(', 'void\nAppleCLCD::vsyncFired(')
timer=extract('void\nAppleCLCD::vsyncFired(', 'void\nAppleCLCD::addClient(')
swap=extract('void\nAppleCLCD::swap(', 'void\nAppleCLCD::present(')
a=s.index('\t\tif (!nord2_display_begin()) {')
failed=s[a:s.index('\n#endif',a)]

code=r'''
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#define NORD2 1
#define SWEET_FB_SWAP_REC_SIZE 0x46c
#define SWEET_FB_NOTIFY_VSYNC 5
#define SWEET_FB_VSYNC_US 16667
static unsigned locks, diaglogs, swaplogs, calls, refreshes, allocation_fail, reject;
static uint64_t clockticks, sweet_fb_swap_count;
static uint32_t last_presented_seq;
struct AppleCLCD;static AppleCLCD *active_fb;
static const uint8_t *newer_record;
static bool nord2_display_begin(void);
struct OSObject {};
struct IOTimerEventSource { unsigned arms=0; void setTimeoutUS(unsigned us) { assert(us==16667);arms++; } };
struct IOMobileFramebufferUserClient { void notify(unsigned,const uint64_t *,unsigned) {} };
struct Clients { unsigned getCount(){return 0;} OSObject *getObject(unsigned){abort();} };
static void IOLockLock(void *) { assert(!locks);locks++; }
static void IOLockUnlock(void *) { assert(locks);locks--; }
static uint64_t mach_absolute_time(void) { return ++clockticks; }
static void *IOMallocData(unsigned n){return allocation_fail?nullptr:malloc(n);}
static void IOFreeData(void *p,unsigned){free(p);}
static int nord2_display_refresh(void){refreshes++;return 1;}
static void IOLog(const char *fmt,...) {
 assert(!locks);
 if(!strncmp(fmt,"NORD2 FB DIAG:",14))diaglogs++;
 if(!strncmp(fmt,"NORD2 FB SWAP:",14))swaplogs++;
}
static uint32_t get32(const uint8_t *p,uint32_t off){uint32_t v;memcpy(&v,p+off,4);return v;}
'''+struct+r'''
struct AppleCLCD:OSObject {
 SweetFBDiagnostics fDiag={};uint64_t fVsyncCount=0;Clients clients;Clients *fClients=&clients;
 void *fLock=nullptr;bool fPowerOff=false,fDirty=false,fConsolePaused=false;unsigned fFadeStep=0,fFlushTicks=6;
 uint32_t pixels[1]={0xffffffff};uintptr_t fPixels=(uintptr_t)pixels;
 unsigned fHeight=1,fWidth=1,fRowBytes=4;
 uint8_t fPending[SWEET_FB_SWAP_REC_SIZE]={};
 static void vsyncFired(OSObject *,IOTimerEventSource *);
 void reportDiagnostics(const char *);void swap(const uint8_t *);
 void present(const uint8_t *);
};
static bool nord2_display_begin(void) {
 if(newer_record){active_fb->swap(newer_record);newer_record=nullptr;}
 return !reject;
}
void AppleCLCD::present(const uint8_t *rec) {
 calls++;if(fDiag.enabled)fDiag.presents++;
'''+failed+r'''
 last_presented_seq=get32(rec,0x50);
}
'''+report+timer+swap+r'''
int main(){
 uint8_t rec[SWEET_FB_SWAP_REC_SIZE]={};uint32_t v=3;memcpy(rec+0x104,&v,4);
 AppleCLCD fb;IOTimerEventSource timer;fb.fDiag.enabled=true;
 for(unsigned i=0;i<10000;i++)fb.swap(rec);
 assert(fb.fDiag.swaps==10000 && swaplogs==8);
 for(unsigned i=0;i<10000;i++)AppleCLCD::vsyncFired(&fb,&timer);
 assert(fb.fDiag.vsync==10000 && diaglogs==4 && calls==1 && timer.arms==10000);
 // A single rejected ownership attempt is retried without any new swap.
 fb.swap(rec);reject=1;
 for(unsigned i=0;i<6;i++)AppleCLCD::vsyncFired(&fb,&timer);
 assert(fb.fDirty && fb.fDiag.begin_failed==1);
 unsigned before=calls;reject=0;
 for(unsigned i=0;i<6;i++)AppleCLCD::vsyncFired(&fb,&timer);
 assert(!fb.fDirty && calls==before+1);
 before=calls;
 for(unsigned i=0;i<60;i++)AppleCLCD::vsyncFired(&fb,&timer);
 assert(calls==before);
 // A new swap arriving during failed handoff wins; the old record is not restored.
 uint8_t rec2[SWEET_FB_SWAP_REC_SIZE]={};uint32_t seq=99;memcpy(rec2+0x50,&seq,4);
 fb.swap(rec);active_fb=&fb;newer_record=rec2;reject=1;
 for(unsigned i=0;i<6;i++)AppleCLCD::vsyncFired(&fb,&timer);
 assert(fb.fDirty && !memcmp(fb.fPending,rec2,sizeof(rec2)));
 reject=0;
 for(unsigned i=0;i<6;i++)AppleCLCD::vsyncFired(&fb,&timer);
 assert(!fb.fDirty && last_presented_seq==99);before=calls;
 // Off ticks suppress presents; allocation failure preserves pending dirty frame.
 reject=0;fb.swap(rec);fb.fPowerOff=true;
 for(unsigned i=0;i<60;i++)AppleCLCD::vsyncFired(&fb,&timer);
 assert(fb.fDirty && calls==before && fb.fDiag.off==60 && refreshes==7);
 fb.fPowerOff=false;allocation_fail=1;
 for(unsigned i=0;i<6;i++)AppleCLCD::vsyncFired(&fb,&timer);
 assert(fb.fDirty && fb.fDiag.alloc_failed==1);
 allocation_fail=0;
 for(unsigned i=0;i<6;i++)AppleCLCD::vsyncFired(&fb,&timer);
 assert(!fb.fDirty && calls==before+1);
 // Default behavior produces no new diagnostic counters or logs.
 unsigned dl=diaglogs,sl=swaplogs;AppleCLCD quiet;IOTimerEventSource timer2;
 quiet.swap(rec);
 for(unsigned i=0;i<3600;i++)AppleCLCD::vsyncFired(&quiet,&timer2);
 assert(!quiet.fDiag.swaps && !quiet.fDiag.vsync && dl==diaglogs && sl==swaplogs);
}
'''
with tempfile.TemporaryDirectory(prefix='nord2-fb-diag-') as d:
    c=Path(d)/'test.cpp';exe=Path(d)/'test';c.write_text(code)
    subprocess.run(['clang++','-std=c++17','-O1','-g','-Wall','-Wextra','-Werror',
                    '-fsanitize=address,undefined',str(c),'-o',str(exe)],check=True)
    subprocess.run([str(exe)],check=True)
print('PASS: production swap/timer/report paths; finite8 swap+4 callback logs, power-off/alloc-failure/default behavior and ownership retry/newer-swap preservation')
