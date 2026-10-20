#!/usr/bin/env python3
"""Exact production SCSI wait loop; stalled-counter diagnostic DMA guard."""
from pathlib import Path
import subprocess,tempfile
ROOT=Path(__file__).resolve().parents[1]
s=(ROOT/'research/ci-39158169212/buildtree/xnu/bsd/dev/sweet_ufs.c').read_text()
a=s.index('#if defined(NORD2)\n\tunsigned nord2_x2_polls = 0;')
w=s.index('while (ufs_rd(REG_UTRLDBR) & 1)',a)
brace=s.index('{',w);depth=1;b=brace+1
while depth:
 if s[b]=='{':depth+=1
 if s[b]=='}':depth-=1
 b+=1
loop=s[a:b]
assert loop.index('++nord2_x2_polls >= 1000000u')<loop.index('if (now > deadline)')
assert loop.index('return ETIMEDOUT;')<loop.index('ufs_wr(REG_UTRLCLR, ~1u);')
code=r'''
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#define NORD2 1
#define REG_UTRLDBR 0x58u
#define REG_UTRLCLR 0x5cu
#define MIN(a,b) ((a)<(b)?(a):(b))
#define THREAD_UNINT 0
#define TIMEOUT_URGENCY_SYS_CRITICAL 0
#define THREAD_WAITING 1
#define THREAD_CONTINUE_NULL 0
typedef void *event_t;
static struct {bool wait;} su;
static bool nord2_x2_in_progress,nord2_x2_uncertain,nord2_ufs_profile_enabled;
static uint64_t clock_now,clock_step,deadline,spin_until,sleep_interval,profile_issued;
static bool sleepable;
static unsigned polls,complete_after,aborts,delays,profiles,sleeps,prints;
static uint32_t ufs_rd(unsigned off){assert(off==REG_UTRLDBR);polls++;return complete_after&&polls>=complete_after?0:1;}
static void ufs_wr(unsigned off,unsigned value){assert(off==REG_UTRLCLR&&value==~1u);aborts++;}
static uint64_t mach_absolute_time(void){uint64_t n=clock_now;clock_now+=clock_step;return n;}
static void nord2_ufs_profile_wait(uint64_t begin){assert(begin==profile_issued);profiles++;}
static void delay(unsigned n){assert(n==1);delays++;}
static int fake_printf(const char *f,...){(void)f;prints++;return 0;}
#define printf fake_printf
static int assert_wait_deadline_with_leeway(event_t e,int type,int urgency,uint64_t wake,unsigned slack){
 assert(e==&su.wait&&type==THREAD_UNINT&&urgency==TIMEOUT_URGENCY_SYS_CRITICAL&&wake>=clock_now-1&&!slack);return THREAD_WAITING;
}
static void thread_block(int continuation){assert(continuation==THREAD_CONTINUE_NULL);sleeps++;}
static int production_wait(void){uint8_t cdb[1]={0x28};
'''+loop+r'''
 return 0;
}
static void reset(void){
 nord2_x2_in_progress=nord2_ufs_profile_enabled=true;nord2_x2_uncertain=false;
 clock_now=clock_step=profile_issued=0;deadline=5000000;
 polls=complete_after=aborts=delays=profiles=sleeps=prints=0;
 sleepable=false;spin_until=50;sleep_interval=200;
}
int main(void){
 // A stopped timer still stops at exactly one million outstanding observations.
 reset();assert(production_wait()==ETIMEDOUT);
 assert(nord2_x2_uncertain&&polls==1000000&&delays==999999);
 assert(!aborts&&!prints&&profiles==1&&!sleeps);
 // Completion just before the limit is valid; completion at the limit exits
 // through the doorbell condition before interpreting an outstanding poll.
 for(unsigned n=999999;n<=1000000;n++){
  reset();complete_after=n;assert(!production_wait());assert(!nord2_x2_uncertain&&!aborts&&!profiles);
 }
 // Profile disabled does not weaken DMA protection or require counter calls.
 reset();nord2_ufs_profile_enabled=false;assert(production_wait()==ETIMEDOUT);
 assert(nord2_x2_uncertain&&!aborts&&!profiles);
 // Normal boot path retains its existing timer timeout+abort behavior.
 reset();nord2_x2_in_progress=false;clock_step=1;deadline=2;
 assert(production_wait()==ETIMEDOUT&&!nord2_x2_uncertain&&aborts==1&&prints==1&&profiles==1);
 // No diagnostic guard is active on ordinary I/O, even beyond a million polls.
 reset();nord2_x2_in_progress=false;complete_after=1000005;
 assert(!production_wait()&&polls==1000005&&!nord2_x2_uncertain&&!aborts&&!profiles);
 // Existing deadline still protects diagnostic I/O without aborting DMA.
 reset();clock_step=1;deadline=2;assert(production_wait()==ETIMEDOUT);
 assert(nord2_x2_uncertain&&!aborts&&!prints&&profiles==1&&polls==4);
 // Ordinary sleepable polling remains intact.
 reset();nord2_x2_in_progress=false;sleepable=true;clock_step=1;complete_after=100;
 assert(!production_wait()&&sleeps&&!aborts&&!nord2_x2_uncertain);
 return 0;
}
'''
with tempfile.TemporaryDirectory(prefix='nord2-ufs-x2-poll-') as td:
 p=Path(td);c=p/'test.c';exe=p/'test';c.write_text(code)
 subprocess.run(['clang','-std=c11','-O1','-g','-Wall','-Wextra','-Werror','-fsanitize=address,undefined',str(c),'-o',str(exe)],check=True)
 subprocess.run([str(exe)],check=True)
print('PASS: exact production SCSI loop; diagnostic1M bound/stalled timer/preserved DMA/edge completion/profile-off; default timer+abort/sleep behavior unchanged; ASan+UBSan')
