#!/usr/bin/env python3
from pathlib import Path
import subprocess,tempfile
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

s=(ROOT/'tools/test-nord2-ufs-two-lane.py').read_text()
start=s.index("c.write_text(r'''")+len("c.write_text(r'''");end=s.index("''')",start)
harness=s[start:end]
harness=harness[:harness.index('static void restored(')]
# Reuse actual callback model; the independent frozen-reference comparison below
# preserves stop equivalence without rerunning the broad legacy suite.
harness=harness.replace('int main(void) {','static int stop_suite(void) {')
# Exact callback trace includes register reads/writes and hardware timestamps.
harness=harness.replace('static int ownership;', 'static uint64_t trace[4000000],saved_trace[4000000];static unsigned events;\nstatic void event(unsigned type,uint32_t a,uint32_t v){assert(events<4000000);trace[events++]=((uint64_t)type<<56)|((uint64_t)a<<32)|v;}\nstatic int ownership;')
harness=harness.replace(' return regs[a/4];',' event(1,a,regs[a/4]);return regs[a/4];',1)
harness=harness.replace(' (void)c;assert(a==0x20', ' (void)c;event(2,a,v);assert(a==0x20',1)
harness=harness.replace('uint64_t old=clockticks;', 'uint64_t old=clockticks;event(3,0,(uint32_t)old);')
harness=harness.replace('assert(n==1);}', 'assert(n==1);event(4,0,n);}',1)
harness=harness.replace('static void barrier(void *c){(void)c;}', 'static void barrier(void *c){(void)c;event(5,0,0);}')
harness=harness.replace('return ownership && (!loss_at || issued<loss_at);', 'int value=ownership && (!loss_at || issued<loss_at);event(6,0,(uint32_t)value);return value;')
harness=harness.replace('assert(lba==1&&n==5&&ownership);', 'assert(lba==1&&n==5&&ownership);event(7,(uint32_t)lba,n);')
harness=harness.replace('pa_mode=pa_reads=pa_error_at=0;', 'pa_mode=pa_reads=pa_error_at=0;events=0;')
extra=r'''
extern int n2l_diagnostic_reference(const struct n2l_ops *,void *,unsigned,struct n2l_result *);
int main(void){
 // Byte-equivalent result and identical callback trace to frozen canonical stop
 // engine, including every command's completed failure and ambiguous timeout.
 for(unsigned fault=0;fault<3;fault++)for(unsigned i=1;i<=115;i++){
  reset();if(fault==1)fail_at=i;if(fault==2)timeout_at=i;
  struct n2l_result reference,actual;int before=n2l_diagnostic_reference(&ops,0,1,&reference);
  unsigned count=events;memcpy(saved_trace,trace,count*sizeof(uint64_t));
  reset();if(fault==1)fail_at=i;if(fault==2)timeout_at=i;
  int after=n2l_diagnostic(&ops,0,1,&actual);
  assert(before==after&&!memcmp(&reference,&actual,sizeof(actual)));
  assert(count==events&&!memcmp(saved_trace,trace,count*sizeof(uint64_t)));
 }
 struct n2l_result r;
 reset();assert(!n2l_live(&ops,0,1,&r));
 assert(r.phase==5&&r.active_verified&&r.root_allowed&&!r.handoff_allowed);
 assert(!r.uncertain&&!r.rollback_attempted&&!r.original_verified);
 assert(r.commands==60&&r.sets==18&&r.reads==2);
 assert(values[0x1580]==2&&values[0x1560]==2&&values[0x15d4]==1);
 reset();assert(n2l_live(&ops,0,0,&r)&&!issued&&!r.root_allowed&&!r.handoff_allowed);
 for(unsigned i=1;i<=60;i++)for(unsigned fault=0;fault<7;fault++){
  reset();switch(fault){case 0:fail_at=i;break;case 1:timeout_at=i;break;
  case 2:loss_at=i;break;case 3:error_at=i;break;case 4:host_error_at=i;break;
  case 5:device_error_at=i;break;case 6:extra_is_at=i;break;}
  assert(n2l_live(&ops,0,1,&r)&&!r.root_allowed&&!r.handoff_allowed&&!r.active_verified);
  if(fault)assert(r.uncertain&&issued==i);
  else if(i<=24)assert(!r.sets&&!r.rollback_attempted);
  else assert(r.rollback_attempted&&r.original_verified&&!r.uncertain);
 }
 for(unsigned i=43;i<=60;i++){
  reset();mismatch_at=i;assert(n2l_live(&ops,0,1,&r)&&r.rollback_attempted&&r.original_verified&&!r.root_allowed&&!r.handoff_allowed);
 }
 for(unsigned read=1;read<=2;read++)for(unsigned fault=0;fault<3;fault++){
  reset();if(fault==0)read_failure=read;if(fault==1)read_corruption=read;if(fault==2)read_uncertain=read;
  assert(n2l_live(&ops,0,1,&r)&&!r.root_allowed&&!r.handoff_allowed&&!r.active_verified);
  if(fault==2)assert(r.uncertain&&!r.rollback_attempted);
  else if(read==2)assert(r.original_verified&&r.rollback_attempted);
  else assert(!r.sets);
 }
 reset();values[0x1581]=1;assert(n2l_live(&ops,0,1,&r)&&!r.sets&&!r.root_allowed);
 reset();timeout_at=25;stalled=1;assert(n2l_live(&ops,0,1,&r)&&r.uncertain&&issued==25&&!r.root_allowed);
 reset();pa_mode=1;assert(!n2l_live(&ops,0,1,&r)&&r.root_allowed&&!r.handoff_allowed);
 return 0;
}
'''
with tempfile.TemporaryDirectory(prefix='nord2-canonical-live-engine-') as td:
 p=Path(td);(p/'test.c').write_text(harness+extra)
 # Frozen 9327714 canonical implementation is independent reference, not the
 # private modified engine renamed into a misleading self-comparison.
 ref=subprocess.run(['git','show','9327714bfe56719881e4c6251243fcd6afc97b24:shim/nord2-xnu/ufs_two_lane.c'],capture_output=True,text=True,check=True).stdout
 ref=ref.replace('int n2l_diagnostic(', 'int n2l_diagnostic_reference(')
 (p/'reference.c').write_text(ref)
 subprocess.run(['clang','-std=c11','-O1','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-I'+str(HERE),str(p/'test.c'),str(p/'reference.c'),str(HERE/'ufs_two_lane.c'),'-o',str(p/'test')],check=True)
 subprocess.run([str(p/'test')],check=True)
 subprocess.run(['clang','--target=aarch64-none-elf','-std=c11','-O2','-Wall','-Wextra','-Werror','-ffreestanding','-c',str(HERE/'ufs_two_lane.c'),'-o',str(p/'engine.o')],check=True)
print('PASS canonical live engine60/18/2; all forward command faults/timeouts/errors/mismatches/digests; no publication/handoff on failures; frozen stop result+trace equivalence; ASan+UBSan+AArch64')
