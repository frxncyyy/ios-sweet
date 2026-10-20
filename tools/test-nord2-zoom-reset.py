#!/usr/bin/env python3
"""Exact opt-in VFS preference repair, API contracts and failure handling."""
from pathlib import Path
import subprocess,tempfile,plistlib,hashlib,re
root=Path(__file__).resolve().parents[1];s=(root/'shim/nord2-xnu/zoom_reset.c').read_text()
api=(root/'research/ci-39158169212/buildtree/xnu/pexpert/arm/pe_bootargs.c').read_text();header=(root/'research/ci-39158169212/buildtree/xnu/pexpert/pexpert/arm64/boot.h').read_text()
assert 'return (char *)((boot_args *)PE_state.bootArgs)->CommandLine;' in api
assert re.search(r'#define BOOT_LINE_LENGTH\s+608',header) and 'CommandLine[BOOT_LINE_LENGTH]' in header
def pinned(name):
 body=re.search(r'nzr_'+name+r'\[83\]=\{([^}]+)\}',s)[1]
 return bytes(int(v,16) for v in re.findall(r'0x[0-9a-f]+',body))
assert plistlib.loads(pinned('old'))=={'canvas_height':2079,'canvas_width':960}
assert plistlib.loads(pinned('new'))=={'canvas_height':2436,'canvas_width':1125}
assert hashlib.sha256(pinned('old')).hexdigest()=='a162a5b4ce6e5acbce663a3595ce4402effedf3f628e6da0c0c1710d6f92db27'
assert hashlib.sha256(pinned('new')).hexdigest()=='a275c9213db9ef42c86512664684f1d76f0c623cd42b108c516c6b5ac7d2d7a3'
init=(root/'research/ci-39158169212/buildtree/xnu/bsd/kern/bsd_init.c').read_text()
assert init.count('\tnord2_zoom_reset();')==1
assert init.index('vm_init_before_launchd();')<init.index('\tnord2_zoom_reset();')<init.index('\tload_init_program(p);')
mock=r'''
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <errno.h>
#define NZR_HOST_TEST 1
#define FREAD 1
#define FWRITE 2
#define O_NOFOLLOW_ANY 4
#define O_CREAT 8
#define O_EXCL 16
#define UIO_READ 0
#define UIO_WRITE 1
#define UIO_SYSSPACE 0
#define IO_SYNC 1
#define IO_UNIT 2
#define MNT_WAIT 1
#define VREG 1
struct file {unsigned char b[83];unsigned size,uid,gid,mode,type,nlink;int opened;};
typedef struct file* vnode_t;typedef void* vfs_context_t;typedef char* caddr_t;
struct vnode_attr {unsigned va_uid,va_gid,va_mode,va_nlink,va_data_size;};
#define VATTR_INIT(v) memset(v,0,sizeof(*(v)))
#define VATTR_SET(v,k,x) ((v)->k=(x))
#define VATTR_WANTED(v,k) ((void)0)
#define VATTR_IS_SUPPORTED(v,k) 1
static struct file target,backup;static unsigned opt,ops,fail_at,fail_at2,persistent,writes,logs,mutation_op;static const char *args;static int exists,create_error,meta_bad,report_error,reported_restore,reported_verified;
static int fault(void){++ops;return ops==fail_at||ops==fail_at2||(persistent&&ops>=fail_at);}
typedef struct {unsigned char prefix[108];char CommandLine[608];unsigned char suffix[32];} boot_args;
static boot_args fake_boot;
static char *PE_boot_args(void){memset(fake_boot.prefix,0x91,sizeof(fake_boot.prefix));memset(fake_boot.suffix,0xa3,sizeof(fake_boot.suffix));if(args){size_t n=strnlen(args,608);memset(fake_boot.CommandLine,0,608);memcpy(fake_boot.CommandLine,args,n);}else {memset(fake_boot.CommandLine,0,sizeof(fake_boot.CommandLine));strcpy(fake_boot.CommandLine,opt==1?"nord2zoomreset=1":opt==2?"nord2zoomreset=2":"");}return fake_boot.CommandLine;}
static vfs_context_t vfs_context_kernel(void){return (void*)1;}
static void* vfs_context_ucred(void*c){return c;}
static void* vfs_context_proc(void*c){return c;}
static unsigned vnode_vtype(vnode_t v){return v->type;}
static int vnode_getattr(vnode_t v,struct vnode_attr*a,void*c){assert(c);if(fault())return EIO;a->va_uid=v->uid;a->va_gid=v->gid;a->va_mode=v->mode;a->va_nlink=v->nlink;a->va_data_size=v->size;if(meta_bad)a->va_uid=42;return 0;}
static int vnode_setattr(vnode_t v,struct vnode_attr*a,void*c){assert(c&&v==&backup);if(fault())return EIO;v->uid=a->va_uid;v->gid=a->va_gid;v->mode=a->va_mode;return 0;}
static int vnode_open(const char*p,int flags,int mode,int x,vnode_t*out,void*c){assert(c&&!x&&(flags&O_NOFOLLOW_ANY));if(fault())return EIO;struct file*v=strstr(p,"original")?&backup:&target;if(v==&backup){if(flags&O_CREAT){if(exists)return EEXIST;if(create_error)return ELOOP;assert(mode==0600);exists=1;v->gid=501;}else if(!exists)return ENOENT;}*out=v;v->opened++;return 0;}
static int vnode_close(vnode_t v,int flags,void*c){(void)flags;assert(c&&v->opened);v->opened--;return fault()?EIO:0;}
static int vn_rdwr(int dir,vnode_t v,caddr_t p,int n,int off,int space,int flags,void*cred,int*resid,void*proc){assert(n==83&&!off&&!space&&cred&&proc);int bad=fault();if(dir==UIO_WRITE){assert(flags==(IO_SYNC|IO_UNIT));writes++;if(v==&target){mutation_op=ops;assert(exists&&backup.size==83&&!memcmp(backup.b,nzr_old,83));}memcpy(v->b,p,bad?41:83);v->size=83;}else memcpy(p,v->b,83);*resid=bad?42:0;return bad?EIO:0;}
static int VNOP_FSYNC(vnode_t v,int wait,void*c){assert(v&&wait==1&&c);return fault()?EIO:0;}
static int log_report(const char*f,...){assert(!target.opened&&!backup.opened);va_list a;va_start(a,f);(void)va_arg(a,int);report_error=va_arg(a,int);(void)va_arg(a,unsigned);(void)va_arg(a,unsigned);reported_verified=va_arg(a,unsigned);(void)va_arg(a,unsigned);reported_restore=va_arg(a,int);va_end(a);logs++;return 0;}
#define printf log_report
'''
# mock IO references pinned bytes, declare before mock definitions
prefix='static const unsigned char nzr_old[83];\n'
# tentative static declaration and later initialized definition are valid C
with tempfile.TemporaryDirectory() as td:
 c=Path(td)/'test.c';exe=Path(td)/'test'
 c.write_text(prefix+mock+s+r'''
static void reset(void){memset(&target,0,sizeof(target));memset(&backup,0,sizeof(backup));target.uid=target.gid=501;target.mode=backup.mode=0600;target.type=backup.type=1;target.nlink=backup.nlink=1;target.size=83;memcpy(target.b,nzr_old,83);opt=1;args=NULL;ops=fail_at=fail_at2=persistent=mutation_op=writes=logs=0;exists=create_error=meta_bad=report_error=reported_restore=reported_verified=0;}
int main(void){reset();nord2_zoom_reset();unsigned total=ops;assert(reported_verified&&!report_error&&!reported_restore&&!memcmp(target.b,nzr_new,83));assert(!memcmp(backup.b,nzr_old,83));
for(unsigned f=1;f<=total;f++){reset();fail_at=f;nord2_zoom_reset();assert(logs==1);assert(!target.opened&&!backup.opened);if(memcmp(target.b,nzr_old,83)&&memcmp(target.b,nzr_new,83))assert(reported_restore);}
for(unsigned f=1;f<=total;f++){reset();fail_at=f;persistent=1;nord2_zoom_reset();assert(!target.opened&&!backup.opened);if(mutation_op&&!reported_verified)assert(reported_restore);}
reset();nord2_zoom_reset();unsigned first_mutation=mutation_op;
reset();fail_at=first_mutation;fail_at2=first_mutation+1;nord2_zoom_reset();assert(reported_restore&&!reported_verified);
const char *badargs[]={"nord2zoomreset=01","nord2zoomreset=1 nord2zoomreset=1","nord2zoomreset nord2zoomreset=1","nord2zoomreset=0x1","nord2zoomreset=2","nord2zoomreset=1x","x=nord2zoomreset=1"};
for(unsigned i=0;i<sizeof(badargs)/sizeof(*badargs);i++){reset();args=badargs[i];nord2_zoom_reset();assert(!ops&&!writes&&!logs);}
for(unsigned gate=0;gate<6;gate++){reset();switch(gate){case 0:target.uid=0;break;case 1:target.gid=0;break;case 2:target.mode=0644;break;case 3:target.nlink=2;break;case 4:target.size=82;break;case 5:target.type=2;break;}nord2_zoom_reset();assert(!writes&&report_error);}
reset();char longargs[608];memset(longargs,'x',sizeof(longargs));args=longargs;nord2_zoom_reset();assert(!ops&&!writes&&!logs);assert(fake_boot.prefix[107]==0x91&&fake_boot.suffix[0]==0xa3);
reset();opt=0;nord2_zoom_reset();assert(!ops&&!writes&&!logs);
reset();opt=2;nord2_zoom_reset();assert(!ops&&!writes&&!logs);
reset();target.b[0]^=1;nord2_zoom_reset();assert(!writes&&report_error);
reset();meta_bad=1;nord2_zoom_reset();assert(!writes&&report_error);
reset();create_error=1;nord2_zoom_reset();assert(!writes&&report_error);
reset();exists=1;backup.size=83;memcpy(backup.b,nzr_old,83);nord2_zoom_reset();assert(reported_verified&&!memcmp(target.b,nzr_new,83));
reset();exists=1;backup.size=83;nord2_zoom_reset();assert(!writes&&report_error);
reset();memcpy(target.b,nzr_new,83);nord2_zoom_reset();assert(reported_verified&&!writes);
return 0;}
''')
 subprocess.run(['cc','-std=gnu11','-Wall','-Wextra','-Werror','-fsanitize=address,undefined',str(c),'-o',str(exe)],check=True);subprocess.run([str(exe)],check=True)
print('PASS exact source: optin/default/idempotence, pinned plist semantics, metadata/source/backup gates, every operation fault+persistent/double faults, inherited backup GID, literal unique args, independent metadata gates, partial writes/rollback, reports outside open vnodes; ASan/UBSan')
