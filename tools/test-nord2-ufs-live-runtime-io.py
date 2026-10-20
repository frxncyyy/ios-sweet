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

s=(ROOT/'tools/test-nord2-ufs-x2-poll.py').read_text()
s=s.replace("s=(ROOT/'research/ci-39158169212/buildtree/xnu/bsd/dev/sweet_ufs.c').read_text()",f"s=Path({str(ACTIVE/'sweet_ufs.c')!r}).read_text()")
ns={'__file__':str(ROOT/'tools/test-nord2-ufs-x2-poll.py')};exec(compile(s[:s.index('with tempfile.TemporaryDirectory')],'existing-poll-setup','exec'),ns)
code=ns['code'].replace('int main(void){','static int old_poll_suite(void){')
code=code.replace('static bool nord2_x2_in_progress,','static bool nord2_live_armed,nord2_live_active;\nstatic bool nord2_x2_in_progress,')
code=code.replace('nord2_x2_in_progress=nord2_ufs_profile_enabled=true;','nord2_live_armed=nord2_live_active=false;nord2_x2_in_progress=nord2_ufs_profile_enabled=true;')
code=code.replace('#define REG_UTRLDBR','#define REG_IS 0x20u\n#define N2_UIC_COMPLETE 0x400u\n#define REG_UTRLDBR')
code=code.replace('static unsigned polls,','static unsigned irq_ack;static uint32_t irq_status;static bool nord2_ufs_uic_pending;\nstatic unsigned polls,')
code=code.replace('assert(off==REG_UTRLDBR);polls++;','if(off==REG_IS)return irq_status;assert(off==REG_UTRLDBR);polls++;')
code=code.replace('assert(off==REG_UTRLCLR&&value==~1u);aborts++;','if(off==REG_IS){irq_ack++;return;}assert(off==REG_UTRLCLR&&value==~1u);aborts++;')
source=(ACTIVE/'sweet_ufs.c').read_text()
a=source.index('static struct nord2_ufs_profile_stats {');b=source.index('} nord2_ufs_stats;',a)+len('} nord2_ufs_stats;');stats=source[a:b]
a=source.index('static int\nufs_scsi(const');b=source.index('/* READ(10)',a);wrapper=source[a:b]
a=source.index('\tuint32_t status = ufs_rd(REG_IS);',source.index('ufs_scsi_impl('));b=source.index('#else',a);status=source[a:b]
# Extract the exact root-init TUR retry consumer, not a handwritten equivalent.
a=source.index('for (int i = 0; i < 3; i++)',source.index('sweet_ufs_init(void)'))
brace=source.index('{',a);depth=1;b=brace+1
while depth:
 if source[b]=='{':depth+=1
 if source[b]=='}':depth-=1
 b+=1
tur=source[a:b]
extra=r'''
#include <setjmp.h>
static jmp_buf panic_jump;static unsigned panics,impl_calls;static int impl_error,impl_wait,impl_status,impl_eio_once;
static uint64_t nord2_ufs_counter(void){return 1;}
static void test_panic(const char *message) __attribute__((noreturn));
static void test_panic(const char *message){assert(message);panics++;longjmp(panic_jump,1);}
#define panic test_panic
'''+stats+r'''
static int production_status(void){
'''+status+r'''
 return 0;
}
static int ufs_scsi_impl(const uint8_t *c,unsigned cn,uint32_t len,bool write){(void)c;(void)cn;(void)len;(void)write;
 impl_calls++;if(impl_eio_once)return impl_calls==1?EIO:0;
 return impl_wait?production_wait():impl_status?production_status():impl_error;}
'''+wrapper+r'''
static int production_tur(void){int error=0;static const uint8_t tur[6]={0};
'''+tur+r'''
 return error;
}
int main(void){
 old_poll_suite();
 reset();nord2_x2_in_progress=false;nord2_live_armed=true;
 assert(production_wait()==ETIMEDOUT&&nord2_x2_uncertain&&polls==1000000&&!aborts);
 reset();nord2_x2_in_progress=false;nord2_live_armed=true;clock_step=1;deadline=2;
 assert(production_wait()==ETIMEDOUT&&nord2_x2_uncertain&&!aborts);
 reset();nord2_x2_in_progress=false;nord2_live_armed=true;complete_after=5;
 assert(!production_wait()&&!nord2_x2_uncertain&&!aborts);
 for(unsigned flag=0;flag<2;flag++){
  reset();nord2_x2_in_progress=false;nord2_live_armed=true;nord2_live_active=true;nord2_ufs_profile_enabled=flag;
  irq_status=0x401;irq_ack=0;impl_status=1;impl_wait=0;panics=0;
  if(!setjmp(panic_jump)){ufs_scsi((uint8_t[]){0x28},1,4096,false);assert(0);}
  assert(panics==1&&nord2_x2_uncertain&&!irq_ack&&!aborts);
  // One full call through exact wait loop + exact wrapper, stopped timer.
  reset();nord2_x2_in_progress=false;nord2_live_armed=nord2_live_active=true;nord2_ufs_profile_enabled=flag;
  impl_status=0;impl_wait=1;panics=0;
  if(!setjmp(panic_jump)){ufs_scsi((uint8_t[]){0x28},1,4096,false);assert(0);}
  assert(panics==1&&polls==1000000&&!aborts&&nord2_x2_uncertain);
  // Even completed protocol errors stop active live I/O before next request.
  reset();nord2_x2_in_progress=false;nord2_live_armed=nord2_live_active=true;nord2_ufs_profile_enabled=flag;
  impl_wait=0;impl_error=EIO;panics=0;
  if(!setjmp(panic_jump)){ufs_scsi((uint8_t[]){0x28},1,4096,false);assert(0);}
  assert(panics==1&&nord2_x2_uncertain&&!aborts);
 }
 // Pre-root armed-but-inactive path must never let actual TUR retry an
 // ambiguous first command; test real status and timeout flag producers.
 for(unsigned flag=0;flag<2;flag++)for(unsigned fault=0;fault<2;fault++){
  reset();nord2_x2_in_progress=false;nord2_live_armed=true;nord2_live_active=false;nord2_ufs_profile_enabled=flag;
  impl_calls=panics=irq_ack=0;impl_eio_once=0;impl_status=fault==0;impl_wait=fault==1;irq_status=0x401;
  clock_step=1;deadline=2;
  if(!setjmp(panic_jump)){production_tur();assert(0);}
  assert(panics==1&&impl_calls==1&&nord2_x2_uncertain&&!irq_ack&&!aborts);
 }
 for(unsigned flag=0;flag<2;flag++){
  // Entry guard is before any descriptor/command callback, even with no error
  // available yet. A latched ambiguous state must not be "retried".
  reset();nord2_live_armed=true;nord2_live_active=false;nord2_x2_uncertain=true;nord2_ufs_profile_enabled=flag;
  impl_calls=panics=0;impl_wait=impl_status=impl_eio_once=0;impl_error=0;
  if(!setjmp(panic_jump)){production_tur();assert(0);}
  assert(panics==1&&!impl_calls&&!aborts);
  // Known-completed unit attention retains the real bounded retry behavior.
  reset();nord2_x2_in_progress=false;nord2_live_armed=true;nord2_ufs_profile_enabled=flag;
  impl_calls=panics=0;impl_eio_once=1;
  assert(!production_tur()&&impl_calls==2&&!panics&&!nord2_x2_uncertain);
  // A cleanly completed EIO may still be returned to the negotiation engine
  // for its existing rollback contract, in either profile mode.
  reset();nord2_live_armed=true;nord2_x2_in_progress=true;nord2_ufs_profile_enabled=flag;
  impl_calls=panics=0;impl_eio_once=0;impl_error=EIO;
  assert(ufs_scsi((uint8_t[]){0x28},1,4096,false)==EIO&&impl_calls==1&&!panics&&!nord2_x2_uncertain);
 }
 reset();nord2_live_armed=true;nord2_live_active=false;impl_error=EIO;
 assert(ufs_scsi((uint8_t[]){0x28},1,4096,false)==EIO); // engine can still negotiate known rollback
 reset();nord2_x2_in_progress=false;nord2_ufs_profile_enabled=false;
 assert(ufs_scsi((uint8_t[]){0x28},1,4096,false)==EIO); // unchanged default error return
 return 0;}
'''
with tempfile.TemporaryDirectory(prefix='nord2-canonical-live-runtime-io-') as td:
 p=Path(td);(p/'test.c').write_text(code+extra)
 subprocess.run(['clang','-std=c11','-O1','-Wall','-Wextra','-Werror','-fsanitize=address,undefined',str(p/'test.c'),'-o',str(p/'test')],check=True)
 subprocess.run([str(p/'test')],check=True)
print('PASS exact production live runtimeI/O guards:1M/timertimeout noabort/DMApreserved, unknownIRQ noACK; exactwrapper panics before nextrequest bothprofilemodes; armed-pre-root actualTUR ambiguity no-retry/entryguard, knowncompleted retry/rollback + default/stop retained; ASan+UBSan')
