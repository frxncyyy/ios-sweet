#!/usr/bin/env python3
"""Compile exact UFS observation routines with MMIO/time/lock models under sanitizers."""
from pathlib import Path
import re
import subprocess
import tempfile

root = Path(__file__).resolve().parent.parent
s = (root / 'research/ci-39158169212/buildtree/xnu/bsd/dev/sweet_ufs.c').read_text()
a = s.index('#if defined(NORD2)\n/* Opt-in observation only.')
b = s.index('#endif /* NORD2 */', a) + len('#endif /* NORD2 */')
profile = s[a:b]
a = s.index('static int\nufs_scsi(const ')
b = s.index('/* READ(10)', a)
wrapper = s[a:b]
a = s.index('static bool\nnord2_ufs_restore(')
b = s.index('\nvoid nord2_ufs_probe', a)
restore = s[a:b]
a = s.index('\tuint32_t status = ufs_rd(REG_IS);')
b = s.index('\n#else', a)
ack = s[a:b]
# Integration checks: profile only after successful takeover/GPT under lock,
# restore guard before touching request lists; ordinary disabled path unchanged.
assert s.index('if (!error) { nord2_ufs_profile_link(); }') < s.index('su.bmajor = bdevsw_add')
assert 'if (!error) { nord2_ufs_profile_link(); }\n#endif\n\tlck_mtx_unlock' in s
assert 'nord2_ufs_profile_enabled = (profile == 1);' in s
assert s.count('nord2_ufs_profile_wait(profile_issued)') == 2
assert s.index('profile_issued =') < s.index('ufs_wr(REG_UTRLDBR, 1);')
defines = '\n'.join(re.findall(r'^#define (?:REG_\w+|HCS_\w+|UFS_BLOCK|BOUNCE_SIZE|PRDT_CHUNK|PRDT_MAX|CMD_\w+)\s+.*$', s, re.M))
code = r'''
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#define NORD2 1
#define NSEC_PER_SEC 1000000000ull
#define NSEC_PER_USEC 1000ull
'''+defines+r'''
static struct { uintptr_t regs; bool probe_only; } su;
static int sweet_ufs_lock, held, prints;
static uint64_t ticks, frequency, step;
static uint32_t regs[64], attr_value, result_code, irq_on_complete;
static unsigned writes, submits, status_reads, complete_after, delay_calls;
static bool complete_enabled, ready_enabled, recovering, completion_fired;
static int impl_error;
static uint64_t fake_counter(const char *name) {
 if (!strcmp(name,"CNTFRQ_EL0")) return frequency;
 assert(!strcmp(name,"CNTVCT_EL0")); uint64_t v=ticks; ticks+=step; return v;
}
#define __builtin_arm_rsr64 fake_counter
#define __builtin_arm_dsb(x) do { assert(x==0xf); } while (0)
static void delay(unsigned n) { assert(n==1); delay_calls++; }
static void lck_mtx_lock(int *l) { assert(l==&sweet_ufs_lock && !held); held=1; }
static void lck_mtx_unlock(int *l) { assert(l==&sweet_ufs_lock && held); held=0; }
static int fake_printf(const char *f, ...) { (void)f; assert(!held); prints++; return 0; }
#define printf fake_printf
static uint32_t ufs_rd(uint32_t off) {
 assert(off%4==0 && off/4<64);
 if (off==REG_IS && submits && complete_enabled && !completion_fired && ++status_reads >= complete_after) {
  completion_fired=true; regs[REG_IS/4] |= 0x400 | irq_on_complete;
  if (ready_enabled) regs[REG_HCS/4] |= 8;
  regs[0x98/4]=result_code; regs[0x9c/4]=attr_value;
 }
 return regs[off/4];
}
static void ufs_wr(uint32_t off, uint32_t value) {
 assert(held || recovering); writes++;
 if (off==REG_IS) {
  if (!recovering) assert(value==0x400 || value==1);
  regs[off/4] &= ~value; return; // UFSHCI IS is W1C, unlike display INTSTA
 }
 if (recovering) { regs[off/4]=value; return; }
 assert(off==0x90 || off==0x94 || off==0x98 || off==0x9c);
 if (off==0x94) assert((value&0xffff)==0);
 if (off==0x98 || off==0x9c) assert(value==0);
 if (off==0x90) {
  assert(value==1 && !(regs[REG_IS/4]&0x400)); // local GET only, clean completion
  assert(!regs[REG_UTRLDBR/4] && !regs[0x78/4]);
  assert(regs[REG_HCS/4]&8); submits++; status_reads=0; completion_fired=false;
  regs[REG_HCS/4]&=~8u;
 }
 regs[off/4]=value;
}
'''+profile+r'''
static int ufs_scsi_impl(const uint8_t *cdb, unsigned n, uint32_t len, bool write) {
 (void)cdb; (void)n; (void)len; (void)write; ticks+=30; return impl_error;
}
'''+wrapper+r'''
struct nord2_ufs_saved { uint32_t ie, ba, bau, rsr; };
static bool nord2_ufs_wait(uint32_t off, uint32_t mask, uint32_t value) {
 return (ufs_rd(off)&mask)==value;
}
static void nord2_ufs_write_reg(uint32_t off, uint32_t v) { ufs_wr(off,v); }
'''+restore+r'''
static void scsi_ack(void) {
'''+ack+r'''
}
static void reset(void) {
 memset(regs,0,sizeof(regs)); memset(&nord2_ufs_stats,0,sizeof(nord2_ufs_stats));
 memset(&nord2_ufs_link,0,sizeof(nord2_ufs_link));
 su.regs=1; su.probe_only=false; nord2_root_owned=true;
 nord2_ufs_profile_enabled=true; nord2_ufs_profile_reported=false;
 nord2_ufs_uic_pending=false; prints=writes=submits=status_reads=delay_calls=0;
 ticks=0; step=100; frequency=1000000; recovering=false; completion_fired=false; held=1;
 regs[REG_HCS/4]=HCS_DP|HCS_UTRLRDY|8; regs[REG_HCE/4]=1;
 complete_after=3; complete_enabled=ready_enabled=true;
 attr_value=4; result_code=irq_on_complete=0; impl_error=0;
}
int main(void) {
 uint32_t v,r;
 // Exhaustive address allowlist: only six local PA attributes can dispatch.
 for (unsigned attr=0; attr<65536; attr++) {
  reset(); bool allowed=false;
  for (unsigned i=0;i<6;i++) if(attr==nord2_ufs_attrs[i]) allowed=true;
  int e=nord2_ufs_dme_get(attr,&v,&r);
  assert(e==(allowed?0:EPERM)); assert(submits==(unsigned)allowed);
  if (allowed) assert(v==4 && r==0 && !nord2_ufs_uic_pending);
  else assert(writes==0);
 }
 // Every gate rejects without modifying controller state or active commands.
 for (unsigned gate=0;gate<13;gate++) {
  reset();
  switch(gate) {
   case 0:nord2_ufs_profile_enabled=false;break;
   case 1:nord2_root_owned=false;break;
   case 2:su.probe_only=true;break;
   case 3:su.regs=0;break;
   case 4:regs[REG_HCE/4]=0;break;
   case 5:regs[REG_HCS/4]&=~8u;break;
   case 6:regs[REG_UTRLDBR/4]=2;break;
   case 7:regs[0x78/4]=1;break;
   case 8:regs[REG_IE/4]=0x400;break;
   case 9:regs[REG_IS/4]=0x400;break; // inherited stale completion
   case 10:nord2_ufs_uic_pending=true;break;
   case 11:frequency=0;break;
   case 12:regs[REG_IS/4]=1;break; // pending SCSI completion
  }
  assert(nord2_ufs_dme_get(0x1583,&v,&r)!=0 && writes==0);
 }
 reset(); assert(nord2_ufs_dme_get(0x1583,0,&r)==EPERM && !writes);
 reset(); assert(nord2_ufs_dme_get(0x1583,&v,0)==EPERM && !writes);
 // Result errors are observed and acknowledged, never mistaken for valid values.
 reset(); result_code=9; v=0xfeed;
 assert(nord2_ufs_dme_get(0x1583,&v,&r)==EIO && r==9 && v==0xfeed);
 assert(!nord2_ufs_uic_pending);
 // An unrelated interrupt arriving alongside GET completion must survive ACK.
 reset(); irq_on_complete=2;
 assert(nord2_ufs_dme_get(0x1583,&v,&r)==0 && regs[REG_IS/4]==2);
 unsigned prior=writes;
 assert(nord2_ufs_dme_get(0x1568,&v,&r)==EBUSY && writes==prior);
 // Counter wrap is handled by elapsed subtraction rather than absolute deadlines.
 reset(); ticks=UINT64_MAX-250; assert(nord2_ufs_dme_get(0x1583,&v,&r)==0);
 // Missing completion never retries or resets controller/link, preserves pending.
 reset(); complete_enabled=false;
 assert(nord2_ufs_dme_get(0x1583,&v,&r)==ETIMEDOUT);
 assert(nord2_ufs_uic_pending && writes==4 && ticks<=100300);
 unsigned saved_writes=writes;
 assert(nord2_ufs_dme_get(0x1568,&v,&r)==EBUSY && writes==saved_writes);
 // SCSI acknowledgement preserves late UIC, acknowledging only SCSI status.
 regs[REG_IS/4]=0x401; scsi_ack(); assert(regs[REG_IS/4]==0x400);
 // Recovery refuses pending UIC before changing request lists; late completion drains.
 struct nord2_ufs_saved saved={0x123,0x1000,0,1};
 regs[REG_IS/4]=0; recovering=true; held=0;
 saved_writes=writes; assert(!nord2_ufs_restore(&saved)); assert(writes==saved_writes);
 complete_enabled=true; ready_enabled=true;
 assert(nord2_ufs_restore(&saved) && !nord2_ufs_uic_pending);
 assert(regs[REG_IE/4]==saved.ie && regs[REG_UTRLBA/4]==saved.ba);
 // Stalled counters still have hard poll bound.
 reset(); complete_enabled=false; step=0;
 assert(nord2_ufs_dme_get(0x1583,&v,&r)==ETIMEDOUT && delay_calls==100000);
 // Completion with non-ready UIC must not be handed back or retriggered.
 reset(); ready_enabled=false;
 assert(nord2_ufs_dme_get(0x1583,&v,&r)==ETIMEDOUT && nord2_ufs_uic_pending);
 // Six-field snapshot stops on failure with explicit validity mask.
 reset(); nord2_ufs_profile_link(); assert(nord2_ufs_link.valid_mask==63);
 assert(nord2_ufs_link.completed==6 && submits==6);
 reset(); result_code=2; nord2_ufs_profile_link();
 assert(nord2_ufs_link.valid_mask==0 && nord2_ufs_link.error==EIO && submits==1);
 reset(); nord2_ufs_profile_enabled=false; nord2_ufs_profile_link(); assert(!writes);
 // Counts include attempts; bytes include only successful READ/WRITE payloads.
 reset(); step=0;
 uint8_t read[10]={0x28}, write[10]={0x2a}, sync[10]={0x35};
 assert(ufs_scsi(read,10,4096,false)==0);
 assert(ufs_scsi(write,10,8192,true)==0); assert(ufs_scsi(sync,10,0,false)==0);
 impl_error=ETIMEDOUT; assert(ufs_scsi(read,10,4096,false)==ETIMEDOUT);
 assert(nord2_ufs_stats.commands==4 && nord2_ufs_stats.reads==2);
 assert(nord2_ufs_stats.writes==1 && nord2_ufs_stats.syncs==1);
 assert(nord2_ufs_stats.read_bytes==4096 && nord2_ufs_stats.write_bytes==8192);
 assert(nord2_ufs_stats.errors==1 && nord2_ufs_stats.timeouts==1);
 assert(nord2_ufs_stats.command_ticks==120 && nord2_ufs_stats.command_max_ticks==30);
 ticks=100; nord2_ufs_profile_wait(60); ticks=120; nord2_ufs_profile_wait(110);
 assert(nord2_ufs_stats.doorbell_ticks==50 && nord2_ufs_stats.doorbell_max_ticks==40);
 // Opt-out has no stats/counter dependency and no diagnostic MMIO writes.
 reset(); nord2_ufs_profile_enabled=false; held=0;
 ufs_scsi(read,10,4096,false); nord2_ufs_profile_report();
 assert(!nord2_ufs_stats.commands && !prints && !writes);
 // Once-only report copies under lock then prints exclusively after unlocking.
 reset(); held=0; nord2_ufs_profile_report(); nord2_ufs_profile_report();
 assert(prints==2 && held==0 && !writes);
}
'''
with tempfile.TemporaryDirectory(prefix='nord2-ufs-profile-') as tmp:
    c=Path(tmp)/'test.c'; exe=Path(tmp)/'test'
    c.write_text(code)
    subprocess.run(['clang','-std=c11','-O1','-g','-Wall','-Wextra','-Werror',
                    '-fsanitize=address,undefined',str(c),'-o',str(exe)],check=True)
    subprocess.run([str(exe)],check=True)
print('PASS: production DME_GET/poll/drain/restore/profile/report; 65536 addresses, ownership/active-command gates, timeouts/late completion/stalled clock, stats/default/report-lock')
