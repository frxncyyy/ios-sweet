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

script=(ROOT/'tools/test-nord2-ufs-two-lane-adapter.py').read_text()
start=script.index("c.write_text(r'''")+len("c.write_text(r'''");end=script.index("'''+adapter+r'''",start)
prefix=script[start:end];tail=script[end+len("'''+adapter+r'''"):script.index("''')",end)]
prefix=prefix.replace('static unsigned writes,logs,auto_uic;', 'static unsigned writes,logs,auto_uic,read_error_at,live_logs,stop_logs,other_logs;')
prefix=prefix.replace('logs++;return n;', 'logs++;if(!strncmp(f,"NORD2 UFS X2 LIVE",sizeof("NORD2 UFS X2 LIVE")-1))live_logs++;else if(!strncmp(f,"NORD2 UFS X2 ",sizeof("NORD2 UFS X2 ")-1))stop_logs++;else other_logs++;return n;')
prefix=prefix.replace('reads++;return read_error;', 'reads++;return (unsigned)reads==read_error_at?EIO:read_error;')
prefix=prefix.replace('unsigned lun,bdev,bmajor,cmajor;', 'unsigned lun,bdev;int bmajor,cmajor;')
prefix=prefix.replace('static bool nord2_x2_in_progress,','static bool nord2_live_armed,nord2_live_active;\nstatic bool nord2_x2_in_progress,')
tail=tail.replace('int main(int argc,char **argv){','static int stop_suite(int argc,char **argv){')
tail=tail.replace('nord2_x2_uncertain=nord2_ufs_uic_pending=false;','nord2_live_armed=nord2_live_active=false;held=1;\n nord2_x2_uncertain=nord2_ufs_uic_pending=false;')
tail=tail.replace('assert(logs==20);return 0;','assert(logs==20);testing_halt=0;return 0;')
# Extract exact ready-to-run fixture setup from the old adapter integration suite.
a=tail.index(' reset();counter=0;auto_uic=1;');b=tail.index(' struct n2l_result integrated;',a)
setup=tail[a:b]
# Existing tracked adapter test owns broad stop-mode coverage.
tail=tail[:tail.index('static int stop_suite(')]
source=(ACTIVE/'sweet_ufs.c').read_text()
a=source.index('\tif (nord2_live_armed) {\n\t\tstruct n2l_result result');b=source.index('\tuint32_t x2_optin = 0;',a)
callsite=source[a:b]
a=source.index('live_verified:');b=source.index('#if !defined(NORD2)',a)
publication=source[a:b].replace('#endif','') # source region starts inside #if guard
# Exact production return guard, with downstream restore mocked to count access.
a=source.index('int\nnord2_ufs_prepare_return(void)');b=source.index('\nstatic void\nnord2_ufs_release_root',a)
prepare=source[a:b]
a=source.index('int\nsweet_ufs_rootdev(dev_t *dev)');b=source.index('#endif /* ARM64_BOARD_CONFIG_SWEET */',a)
rootdev=source[a:b]
extras=r'''
static bool nord2_live_invalid;
static void nord2_live_arm(void){}
static void nord2_live_early_halt(int error) __attribute__((noreturn));
static void nord2_live_early_halt(int error){assert(error);longjmp(stop_jump,3);}
static unsigned restores,registrations;static int registration_error;
static struct {uint32_t ie,ba,bau,rsr;} nord2_root_saved;
static bool nord2_ufs_restore(const void *p){(void)p;restores++;return true;}
'''+prepare+r'''
static void lck_mtx_unlock(int *p){assert(p==&sweet_ufs_lock&&held);held=0;}
static int sweet_ufs_bdevsw,sweet_ufs_cdevsw;
static int bdevsw_add(int n,int *p){assert(n==-1&&p==&sweet_ufs_bdevsw);assert(!held&&nord2_live_active&&!su.writable&&!su.ready);registrations++;return registration_error?-1:3;}
static int cdevsw_add_with_bdev(int n,int *p,unsigned major){assert(n==-1&&p==&sweet_ufs_cdevsw&&major==3);assert(!su.writable&&!su.ready);return 4;}
static unsigned makedev(unsigned major,unsigned minor){assert(!minor);return major<<8;}
static void devfs_make_node(unsigned dev,unsigned type,unsigned uid,unsigned gid,unsigned mode,const char *name){(void)dev;(void)type;(void)uid;(void)gid;(void)name;assert(mode==0640&&!su.writable&&!su.ready);}
#define DEVFS_BLOCK 1
#define DEVFS_CHAR 2
#define UID_ROOT 0
#define GID_OPERATOR 0
static int publish(void){
 int error=0;unsigned rw=1;const char *want="userdata";
'''+callsite+publication+r'''
 return 0;
failed:
 assert(nord2_live_armed&&!su.ready&&!su.writable);
 assert(nord2_ufs_prepare_return()==-1&&!restores);longjmp(stop_jump,2);
}
static int init_error;
static int sweet_ufs_init(void){return init_error?init_error:publish();}
#define NORD2 1
'''+rootdev+r'''
static void setup_live(char **argv){
'''+setup+r'''
 nord2_live_armed=true;nord2_live_active=false;nord2_x2_stop=false;
 nord2_live_invalid=false;init_error=0;live_logs=stop_logs=other_logs=0;logs=registrations=restores=read_error_at=0;registration_error=0;testing_halt=0;
}
int main(int argc,char **argv){
 assert(argc==3);
 dev_t root=0xabcdef;
 setup_live(argv);assert(!sweet_ufs_rootdev(&root)&&root==su.bdev&&su.ready&&su.writable);
 assert(live_logs==3&&!stop_logs&&other_logs==1&&logs==4);
 setup_live(argv);root=0xabcdef;init_error=ENOMEM;
 if(!setjmp(stop_jump)){sweet_ufs_rootdev(&root);assert(0);}
 assert(root==0xabcdef&&!registrations&&!su.ready&&!su.writable&&!restores);
 setup_live(argv);assert(publish()==0);
 assert(su.ready&&su.writable&&su.bmajor==3&&su.cmajor==4&&registrations==1&&!held);
 assert(nord2_live_active&&pa[0x1580]==2&&pa[0x1560]==2&&reads==2);
 assert(live_logs==3&&!stop_logs&&other_logs==1&&logs==4);
 assert(nord2_ufs_prepare_return()==-1&&!restores&&nord2_root_owned);
 // Post-verification publication failure cannot release/restore/handoff.
 setup_live(argv);registration_error=1;
 if(!setjmp(stop_jump)){publish();assert(0);}assert(!su.ready&&!su.writable&&nord2_live_active&&!restores);
 assert(live_logs==3&&!stop_logs&&!other_logs&&logs==3);
 // Digest failure forces rollback but never permits root publication or return.
 setup_live(argv);read_error_at=2;testing_halt=1;
 if(!setjmp(stop_jump)){publish();assert(0);}assert(!registrations&&!su.ready&&!su.writable&&!nord2_live_active&&!held);
 assert(nord2_ufs_prepare_return()==-1&&!restores);
 assert(reads==3&&pa[0x1580]==1&&pa[0x1560]==1);
 assert(live_logs==3&&stop_logs==20&&!other_logs&&logs==23);
 // Incoming guard failure also blocks publication with no transition writes.
 setup_live(argv);regs[0x18/4]=1;testing_halt=1;
 if(!setjmp(stop_jump)){publish();assert(0);}assert(!registrations&&!writes&&!su.writable&&!su.ready);
 assert(live_logs==3&&stop_logs==20&&!other_logs&&logs==23);
 assert(nord2_ufs_prepare_return()==-1&&!restores);
 return 0;
}
'''
with tempfile.TemporaryDirectory(prefix='nord2-canonical-live-adapter-') as td:
 p=Path(td);(p/'test.c').write_text(prefix+(HERE/'ufs_two_lane_adapter.inc').read_text()+tail+extras)
 subprocess.run(['clang','-std=c11','-O1','-Wall','-Wextra','-Werror','-Wno-deprecated-declarations','-fsanitize=address,undefined','-I'+str(HERE),str(p/'test.c'),str(HERE/'ufs_two_lane.c'),'-lcrypto','-o',str(p/'test')],check=True)
 subprocess.run([str(p/'test'),str(ROOT/'backup/nord2/ufs-baseline/header.bin'),str(ROOT/'backup/nord2/ufs-baseline/entries.bin')],check=True)
print('PASS canonical adapter+exact driver live callsite/publication+return guard: realGPT hashes; x2verified beforeRW; livefailure/rollback neverpublish; permanentreturnrefusal; boundedlive/failedreportlines; ASan+UBSan')
