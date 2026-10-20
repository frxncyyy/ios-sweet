#!/usr/bin/env python3
from pathlib import Path
import subprocess,tempfile,json,re
ROOT=Path(__file__).resolve().parents[1]
HERE=ROOT/'shim/nord2-xnu'
ACTIVE=ROOT/'research/ci-39158169212/buildtree/xnu/bsd/dev'
# Test the canonical helpers only after parent regenerates their active copies.
for name in ('ufs_two_lane.c','ufs_two_lane.h','ufs_two_lane_adapter.inc','ufs_two_lane_live.inc'):
 canonical=HERE/name
 assert canonical.exists(), 'integrate canonical helper before running: '+name
 expected=canonical.read_bytes()
 if name=='ufs_two_lane.c':
  expected=b'#include <arm64/sweet_early.h>\n#if defined(NORD2)\n'+expected+b'\n#endif /* NORD2 */\n'
 assert (ACTIVE/name).read_bytes()==expected, 'active/generated parity mismatch: '+name
assert '#include "ufs_two_lane_live.inc"' in (ACTIVE/'sweet_ufs.c').read_text()

# Reuse the exact audited runtime source + actual char*PE_boot_args host contract.
s=(ROOT/'tools/test-nord2-ufs-two-lane-runtime-gate.py').read_text()
a=s.index("with tempfile.TemporaryDirectory")
namespace={'__file__':str(ROOT/'tools/test-nord2-ufs-two-lane-runtime-gate.py')}
exec(compile(s[:a],'existing-runtime-gate-setup','exec'),namespace)
old=namespace['code'].replace('int main(void)', 'int historical_gate_fixture(void)')
# Existing tracked runtime-gate test runs the broad RO/default regression suite.
# Here its exact source/API model is reused for live-specific normal-RW cases.
base=json.loads((ROOT/'research/nord2/ufs-preflight-cost/ios-profile/manifest.json').read_text())['commandline']
bundle=base+' nord2ufs2live=1'
assert len(bundle.encode())<608 and 'nord2ufs2lane=' not in bundle
source=(ACTIVE/'sweet_ufs.c').read_text()
a=source.index('\tnord2_live_arm();',source.index('sweet_ufs_init(void)'))
b=source.index('\n#endif',a)
entry=source[a:b]
extra=r'''
#include <setjmp.h>
#include <errno.h>
static jmp_buf halt_jump;static unsigned starts,logs;
static void delay(unsigned n){assert(n==1000000);longjmp(halt_jump,1);}
static int safe_printf(const char *f,...){(void)f;logs++;return 0;}
#define printf safe_printf
'''+(HERE/'ufs_two_lane_live.inc').read_text()+r'''
static void reset_live(const char *s){
 memset(&ba,0,sizeof(ba));memcpy(ba.CommandLine,s,strlen(s)+1);no_boot_args=false;
 nord2_live_armed=nord2_live_invalid=nord2_live_active=false;starts=logs=0;
}
static void driver_entry(void){
'''+entry+r'''
 starts++;
}
static const char *bundle=BUNDLE_LITERAL;
int main(void){
 reset_live(bundle);
 // Both actual runtime call sites use this same function. Test their stop/cpus
 // condition, not an invented relaxed live-mode SMP exception.
 int cpus=0;PE_parse_boot_argn("cpus",&cpus,sizeof(cpus));
 assert(cpus==8&&nord2_smp_userland_config(23));
 assert(nord2_smp_userland_config(23));
 driver_entry();assert(nord2_live_armed&&!nord2_live_invalid&&!nord2_live_active&&starts==1&&logs==1);
 nord2_live_arm();assert(logs==1); // permanent, idempotent latch
 reset_live(BASE_LITERAL);driver_entry();assert(!nord2_live_armed&&starts==1&&!logs);
 reset_live("nord2ufs2live=0");driver_entry();assert(!nord2_live_armed&&starts==1&&!logs);
 const char *bad[]={
 "nord2ufs2live=1", "nord2ufs2live=2", "nord2ufs2live", "nord2ufs2live=01", "nord2ufs2live=0 nord2ufs2live=1",
 "nord2ufs2live=1 nord2ufs2live=1"};
 for(unsigned i=0;i<sizeof(bad)/sizeof(bad[0]);i++){
  reset_live(bad[i]);if(!setjmp(halt_jump)){driver_entry();assert(0);}
  assert(nord2_live_armed&&nord2_live_invalid&&!starts);
 }
 const char *keys[]={"nord2stopat=23","cpus=8","rd=ufsroot0","ufsroot=userdata","ufsrw=1","rootrw=1","nord2ufsprofile=1","nord2ufs2live=1"};
 for(unsigned i=0;i<8;i++)for(unsigned change=0;change<3;change++){
  char altered[608];strcpy(altered,bundle);char *p=strstr(altered,keys[i]);assert(p);
  if(change==0){ // remove token (live key removal is deliberate optout)
   memmove(p,p+strlen(keys[i]),strlen(p+strlen(keys[i]))+1);
  }else if(change==1){strcat(altered," ");strcat(altered,keys[i]);}
  else{p[strlen(keys[i])-1]='9';}
  reset_live(altered);
  if(i==7&&change==0){driver_entry();assert(!nord2_live_armed);continue;}
  if(!setjmp(halt_jump)){driver_entry();assert(0);}
  assert(nord2_live_armed&&nord2_live_invalid&&!starts);
 }
 for(unsigned count=2;count<=4;count+=2){
  char few[608];strcpy(few,bundle);char *p=strstr(few,"cpus=8");assert(p);p[5]=(char)('0'+count);
  reset_live(few);int actual=0;PE_parse_boot_argn("cpus",&actual,sizeof(actual));
  assert(!(actual==8 && nord2_smp_userland_config(23)));
  if(!setjmp(halt_jump)){driver_entry();assert(0);}assert(nord2_live_invalid&&!starts);
 }
 char conflict[608];strcpy(conflict,bundle);strcat(conflict," nord2ufs2lane=0");reset_live(conflict);
 assert(!nord2_smp_userland_config(23));
 if(!setjmp(halt_jump)){driver_entry();assert(0);}assert(nord2_live_invalid&&!starts);
 // No diagnostic token may coexist, including the actual old RO bundle.
 reset_live("nord2stopat=23 cpus=8 rd=ufsroot0 ufsroot=userdata ufsrw=0 rootrw=0 nord2ufs2live=1 nord2ufs2lane=1");
 if(!setjmp(halt_jump)){driver_entry();assert(0);}assert(nord2_live_invalid&&!starts);
 memset(ba.CommandLine,'x',sizeof(ba.CommandLine));nord2_live_armed=false;nord2_live_invalid=false;
 if(!setjmp(halt_jump)){driver_entry();assert(0);}assert(nord2_live_invalid&&!starts);
 return 0;
}
'''
extra=re.sub(r'\bbundle\b','live_bundle',extra)
extra=extra.replace('BUNDLE_LITERAL',json.dumps(bundle)).replace('BASE_LITERAL',json.dumps(base))
with tempfile.TemporaryDirectory(prefix='nord2-canonical-live-args-') as td:
 p=Path(td);(p/'test.c').write_text(old+extra)
 subprocess.run(['clang','-std=c11','-Wall','-Wextra','-Werror','-fsanitize=address,undefined',str(p/'test.c'),'-o',str(p/'test')],check=True)
 subprocess.run([str(p/'test')],check=True)
# Real source hook ordering: no MMIO/setup before live gate in either path.
for function in ('sweet_ufs_init(void)','nord2_ufs_probe(void)'):
 body=source[source.index(function):]
 assert body.index('nord2_live_arm();')<body.index('ml_io_map(')
assert source.count('nord2_live_armed || nord2_x2_stop || nord2_x2_uncertain')==2
assert source.index('goto live_verified;')<source.index('su.bmajor = bdevsw_add')<source.index('su.writable = (rw != 0);')
print('PASS exact RW bundle in both unchanged SMP startup/completion gates + exact driverentry; no ROtrap; unique/missing/malformed/conflicting args refuse before setup; permanent early returnblock; ASan+UBSan')
