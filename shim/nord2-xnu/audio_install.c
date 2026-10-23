/* SPDX-License-Identifier: MIT
 * Fixed-path, pre-launchd Nord2 HAL installer. No userspace-pointer syscalls.
 * Single caller, root mounted writable; invoke on EVERY boot before launchd.
 * A return != 0 means refuse launchd until operator chooses recovery policy.
 */
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/errno.h>
#include <sys/vnode.h>
#include <sys/vnode_internal.h>
#include <sys/vnode_if.h>
#include <sys/namei.h>
#include <sys/mount.h>
#include <sys/mount_internal.h>
#include <sys/fcntl.h>
#include <sys/uio.h>
#include <sys/dirent.h>
#include <sys/proc.h>
#include <pexpert/pexpert.h>
#include <pexpert/boot.h>
#include <pexpert/device_tree.h>
#include <libkern/crypto/sha2.h>
#if defined(NORD2)
#include "audio_package.h"
/* flags=0 readdir on bundled HFS emits legacy 32-bit inode dirents. */
_Static_assert(offsetof(struct dirent,d_name)==8,"HFS legacy dirent layout");
_Static_assert(sizeof(((struct dirent *)0)->d_ino)==4,"HFS legacy inode width");
enum path { A,B,S,SD,SE,SP,AD,AE,AP,V,D,J,C,R,P,VP,DP,TUN, PATH_COUNT };
static const char *const paths[PATH_COUNT]={
 "/System/Library/Audio/Plug-Ins/HAL",
 "/System/Library/Audio/Plug-Ins/HAL.nord2-original",
 "/System/Library/Audio/Plug-Ins/HAL.nord2-stage",
 "/System/Library/Audio/Plug-Ins/HAL.nord2-stage/RedwoodAudio.driver",
 "/System/Library/Audio/Plug-Ins/HAL.nord2-stage/RedwoodAudio.driver/RedwoodAudio",
 "/System/Library/Audio/Plug-Ins/HAL.nord2-stage/RedwoodAudio.driver/Info.plist",
 "/System/Library/Audio/Plug-Ins/HAL/RedwoodAudio.driver",
 "/System/Library/Audio/Plug-Ins/HAL/RedwoodAudio.driver/RedwoodAudio",
 "/System/Library/Audio/Plug-Ins/HAL/RedwoodAudio.driver/Info.plist",
 "/Library/Audio/Plug-Ins/HAL/VirtualAudio.plugin",
 "/Library/Audio/Plug-Ins/HAL.disabled/VirtualAudio.plugin",
 "/System/Library/Audio/Plug-Ins/HAL.nord2-journal",
 "/System/Library/Audio/Plug-Ins/HAL.nord2-commit",
 "/System/Library/Audio/Plug-Ins/HAL.nord2-rollback",
 "/System/Library/Audio/Plug-Ins",
 "/Library/Audio/Plug-Ins/HAL",
 "/Library/Audio/Plug-Ins/HAL.disabled",
 "/Library/Audio/Tunings/D22"
};
struct identity { uint64_t ino; uint32_t fsid, type; };
struct journal {
 unsigned char magic[8]; uint32_t version, disabled;
 struct identity original, custom, virtual;
 uint32_t executable_size, plist_size;
 unsigned char executable_hash[32], plist_hash[32], original_hash[32], digest[32];
};
static unsigned char buffer[N2AP_MAX];
static vfs_context_t ctx;
static void hash(const void *p,unsigned n,unsigned char out[32])
{ SHA256_CTX h;SHA256_Init(&h);SHA256_Update(&h,p,n);SHA256_Final(out,&h); }
static int check_hash(const unsigned char *p,unsigned n,const unsigned char expected[32])
{ unsigned char got[32];hash(p,n,got);return memcmp(got,expected,32)?EINVAL:0; }
/* namei's NAMEI_NOFOLLOW_ANY covers intermediate components, unlike lookup NOFOLLOW. */
static int n2_lookup(enum path p,vnode_t *vp)
{
 struct nameidata nd; int e;
 NDINIT(&nd,LOOKUP,OP_LOOKUP,NOFOLLOW,UIO_SYSSPACE,CAST_USER_ADDR_T(paths[p]),ctx);
 nd.ni_flag|=NAMEI_NOFOLLOW_ANY;
 e=namei(&nd);if(e)return e;
 *vp=nd.ni_vp;nameidone(&nd);return 0;
}
static int vnode_identity(vnode_t vp,struct identity *id)
{
 struct vnode_attr va;int e;
 VATTR_INIT(&va);VATTR_WANTED(&va,va_uid);VATTR_WANTED(&va,va_gid);
 VATTR_WANTED(&va,va_mode);VATTR_WANTED(&va,va_fileid);VATTR_WANTED(&va,va_fsid);
 e=vnode_getattr(vp,&va,ctx);
 if(!e && (!VATTR_IS_SUPPORTED(&va,va_uid)||!VATTR_IS_SUPPORTED(&va,va_gid)||
 !VATTR_IS_SUPPORTED(&va,va_mode)||!VATTR_IS_SUPPORTED(&va,va_fileid)||
 !VATTR_IS_SUPPORTED(&va,va_fsid)||va.va_uid||va.va_gid||
 (va.va_mode&0022)||vnode_vtype(vp)==VLNK))e=EINVAL;
 if(!e){id->ino=va.va_fileid;id->fsid=va.va_fsid;id->type=vnode_vtype(vp);}
 return e;
}
static int identity(enum path p,struct identity *id)
{vnode_t vp=NULL;int e=n2_lookup(p,&vp);if(e)return e;e=vnode_identity(vp,id);vnode_put(vp);return e;}
static int same(enum path p,const struct identity *want)
{struct identity got;int e=identity(p,&got);return e?e:(memcmp(&got,want,sizeof(got))?EINVAL:0);}
static int absent(enum path p)
{vnode_t vp=NULL;int e=n2_lookup(p,&vp);if(!e)vnode_put(vp);return e==ENOENT?0:(e?e:EEXIST);}
static int sync_path(enum path p)
{vnode_t vp=NULL;int e=n2_lookup(p,&vp);if(e)return e;e=VNOP_FSYNC(vp,MNT_WAIT,ctx);vnode_put(vp);return e;}
static int directory(enum path p)
{struct identity id;int e=identity(p,&id);return e?e:(id.type==VDIR?0:ENOTDIR);}
static int rw(vnode_t vp,int write,void *bytes,unsigned n)
{int resid=n;int e=vn_rdwr(write?UIO_WRITE:UIO_READ,vp,bytes,n,0,UIO_SYSSPACE,
 write?(IO_SYNC|IO_UNIT):0,vfs_context_ucred(ctx),&resid,vfs_context_proc(ctx));return e?e:(resid?EIO:0);}
static int read_file(enum path p,void *bytes,unsigned n,unsigned mode)
{
 vnode_t vp=NULL;struct vnode_attr va;int e=n2_lookup(p,&vp);if(e)return e;
 VATTR_INIT(&va);VATTR_WANTED(&va,va_data_size);VATTR_WANTED(&va,va_nlink);
 VATTR_WANTED(&va,va_uid);VATTR_WANTED(&va,va_gid);VATTR_WANTED(&va,va_mode);
 e=vnode_getattr(vp,&va,ctx);
 if(!e&&(!VATTR_IS_SUPPORTED(&va,va_data_size)||!VATTR_IS_SUPPORTED(&va,va_nlink)||
 !VATTR_IS_SUPPORTED(&va,va_uid)||!VATTR_IS_SUPPORTED(&va,va_gid)||!VATTR_IS_SUPPORTED(&va,va_mode)||
 vnode_vtype(vp)!=VREG||va.va_data_size!=n||va.va_nlink!=1||va.va_uid||va.va_gid||
 (va.va_mode&07777)!=mode))e=EINVAL;
 if(!e)e=rw(vp,0,bytes,n);vnode_put(vp);return e;
}
/* Exclusive new inode; no truncate or replacement of an existing file. */
static int create_file(enum path p,const void *bytes,unsigned n,unsigned mode)
{
 vnode_t vp=NULL;int flags=FREAD|FWRITE|O_CREAT|O_EXCL|O_NOFOLLOW_ANY;
 int e=vnode_open(paths[p],flags,mode,0,&vp,ctx);if(e)return e;
 struct vnode_attr va;VATTR_INIT(&va);VATTR_SET(&va,va_uid,0);VATTR_SET(&va,va_gid,0);VATTR_SET(&va,va_mode,mode);
 e=vnode_setattr(vp,&va,ctx);if(!e)e=rw(vp,1,(void *)bytes,n);
 if(!e)e=VNOP_FSYNC(vp,MNT_WAIT,ctx);
 int ce=vnode_close(vp,flags,ctx);if(!e)e=ce;
 if(!e)e=read_file(p,buffer,n,mode);
 if(!e&&memcmp(buffer,bytes,n))e=EIO;
 return e;
}
static int mkdir_fixed(enum path p)
{
 struct nameidata nd;struct vnode_attr va;int e;
 NDINIT(&nd,CREATE,OP_MKDIR,WANTPARENT|NOFOLLOW,UIO_SYSSPACE,CAST_USER_ADDR_T(paths[p]),ctx);
 nd.ni_flag|=NAMEI_NOFOLLOW_ANY;e=namei(&nd);if(e)return e;
 if(nd.ni_vp)e=EEXIST;
 else {VATTR_INIT(&va);VATTR_SET(&va,va_type,VDIR);VATTR_SET(&va,va_mode,0755);
 VATTR_SET(&va,va_uid,0);VATTR_SET(&va,va_gid,0);
 e=VNOP_MKDIR(nd.ni_dvp,&nd.ni_vp,&nd.ni_cnd,&va,ctx);
 if(!e)e=VNOP_FSYNC(nd.ni_vp,MNT_WAIT,ctx);
 if(!e)e=VNOP_FSYNC(nd.ni_dvp,MNT_WAIT,ctx);}
 nameidone(&nd);if(nd.ni_vp)vnode_put(nd.ni_vp);if(nd.ni_dvp)vnode_put(nd.ni_dvp);return e;
}
/* Both names are fixed; every rename is non-overwriting and inode-checked.
 * Serialize mount renames BEFORE taking lookup iocounts, as vfs_syscalls does.
 * Single pre-launchd caller; no racing user filesystem operations are permitted.
 */
static int move(enum path from,enum path to,const struct identity *expected)
{
 vnode_t parent=NULL;mount_t mp;struct nameidata f,t;int e,fe=0,te=0;
 e=n2_lookup(P,&parent);if(e)return e;mp=vnode_mount(parent);mount_ref(mp,0);vnode_put(parent);
 mount_lock_renames(mp);
 NDINIT(&f,DELETE,OP_UNLINK,WANTPARENT|NOFOLLOW,UIO_SYSSPACE,CAST_USER_ADDR_T(paths[from]),ctx);
 f.ni_flag|=NAMEI_NOFOLLOW_ANY;e=namei(&f);if(e)goto out;fe=1;
 NDINIT(&t,RENAME,OP_RENAME,WANTPARENT|NOFOLLOW,UIO_SYSSPACE,CAST_USER_ADDR_T(paths[to]),ctx);
 t.ni_flag|=NAMEI_NOFOLLOW_ANY;e=namei(&t);if(e)goto out;te=1;
 if(!f.ni_vp||t.ni_vp){e=EEXIST;goto out;}
 if(vnode_mount(f.ni_vp)!=mp||vnode_mount(t.ni_dvp)!=mp){e=EXDEV;goto out;}
 struct identity actual;e=vnode_identity(f.ni_vp,&actual);if(e)goto out;
 if(memcmp(&actual,expected,sizeof(actual))){e=EINVAL;goto out;}
 /* No other writer exists before launchd; target existence checked above. */
 e=VNOP_RENAMEX(f.ni_dvp,f.ni_vp,&f.ni_cnd,t.ni_dvp,NULL,&t.ni_cnd,VFS_RENAME_EXCL,ctx);
 if(!e)e=VNOP_FSYNC(f.ni_dvp,MNT_WAIT,ctx);
 if(!e&&f.ni_dvp!=t.ni_dvp)e=VNOP_FSYNC(t.ni_dvp,MNT_WAIT,ctx);
 out:
 if(te){nameidone(&t);if(t.ni_vp)vnode_put(t.ni_vp);if(t.ni_dvp)vnode_put(t.ni_dvp);}
 if(fe){nameidone(&f);if(f.ni_vp)vnode_put(f.ni_vp);if(f.ni_dvp)vnode_put(f.ni_dvp);}
 mount_unlock_renames(mp);mount_drop(mp,0);
 if(!e)e=same(to,expected);return e;
}
/* Exact entry allowlist for custom directories, rejecting additions on later boots. */
static int entries_many(enum path p,const char *const *names,unsigned names_count)
{
 vnode_t vp=NULL;uio_t uio=NULL;unsigned char buf[2048];int e=n2_lookup(p,&vp),eof=0,count=0;unsigned seen=0;
 if(e)return e;e=VNOP_OPEN(vp,FREAD,ctx);if(e){vnode_put(vp);return e;}uio=uio_create(1,0,UIO_SYSSPACE,UIO_READ);if(!uio){e=ENOMEM;goto out;}
 for(unsigned pages=0;pages<16&&!eof;pages++){
 off_t off=uio_offset(uio);uio_reset(uio,off,UIO_SYSSPACE,UIO_READ);
 e=uio_addiov(uio,CAST_USER_ADDR_T(buf),sizeof(buf));if(e)goto out;
 e=VNOP_READDIR(vp,uio,0,&eof,&count,ctx);if(e)goto out;
 unsigned n=sizeof(buf)-(unsigned)uio_resid(uio),at=0;
 if(!n&&!eof){e=EIO;goto out;}
 while(at<n){struct dirent *d=(void *)(buf+at);unsigned min=offsetof(struct dirent,d_name);
 if(n-at<min||d->d_reclen<min+1||d->d_reclen>n-at||d->d_namlen>=d->d_reclen-min||d->d_name[d->d_namlen]){e=EINVAL;goto out;}
 if(d->d_ino&&strcmp(d->d_name,".")&&strcmp(d->d_name,"..")){
 unsigned bit=0;for(unsigned k=0;k<names_count;k++)if(!strcmp(d->d_name,names[k]))bit=1u<<k;
 if(!bit||(seen&bit)){e=EINVAL;goto out;}seen|=bit;}
 at+=d->d_reclen;
 }
 }
 if(!eof||seen!=((1u<<names_count)-1))e=EINVAL;
 out:if(uio)uio_free(uio);{int ce=VNOP_CLOSE(vp,FREAD,ctx);if(!e)e=ce;}vnode_put(vp);return e;
}
static int entries(enum path p,const char *one,const char *two)
{const char *names[2]={one,two};return entries_many(p,names,two?2:1);}
/* Journal-free rollback is a read-only no-op ONLY for the verified root-image
 * directory arrangement. Never invent which VirtualAudio placement was original.
 * This checks structure, not every Apple executable's contents. */
static int baseline_original(void)
{
 static const char *const apple[]={"AirPlayHalogen.driver","AppleAOPAudioPlugin.driver",
 "AppleTimeSyncAudioClock.driver","AppleUSBAudio.driver","BTAudioHALPlugin.driver",
 "BasebandVoice.driver","CarPlayHalogen.driver","NetworkUplinkClock.driver","OctaviaHalogen.driver"};
 struct identity virtual;int e;
 if((e=directory(A))||(e=directory(P))||(e=directory(VP))||
 (e=directory(DP))||(e=directory(TUN))||(e=absent(B))||(e=absent(S))||
 (e=absent(C))||(e=absent(R))||(e=absent(V))||(e=identity(D,&virtual)))return e;
 if(virtual.type!=VDIR)return EINVAL;
 return entries_many(A,apple,sizeof(apple)/sizeof(apple[0]));
}
static int custom(const struct journal *j,int active)
{
 int e=same(active?A:S,&j->custom);if(e)return e;
 e=entries(active?A:S,"RedwoodAudio.driver",NULL);if(e)return e;
 e=entries(active?AD:SD,"RedwoodAudio","Info.plist");if(e)return e;
 e=read_file(active?AE:SE,buffer,j->executable_size,0755);if(e)return e;
 e=check_hash(buffer,j->executable_size,j->executable_hash);if(e)return e;
 e=read_file(active?AP:SP,buffer,j->plist_size,0644);if(e)return e;
 return check_hash(buffer,j->plist_size,j->plist_hash);
}
static void seal(struct journal *j)
{
 hash(&j->original,sizeof(j->original)+sizeof(j->custom)+sizeof(j->virtual),j->original_hash);
 hash(j,offsetof(struct journal,digest),j->digest);
}
static int journal_read(struct journal *j)
{
 int e=read_file(J,j,sizeof(*j),0600);if(e)return e;
 if(memcmp(j->magic,"N2AJNL1",8)||j->version!=1||j->disabled>1||
 j->original.type!=VDIR||j->custom.type!=VDIR||j->virtual.type!=VDIR||
 !j->executable_size||!j->plist_size||j->executable_size>N2AP_MAX||
 j->plist_size>N2AP_MAX-j->executable_size)return EINVAL;
 e=check_hash((void *)j,offsetof(struct journal,digest),j->digest);if(e)return e;
 return check_hash((void *)&j->original,sizeof(j->original)+sizeof(j->custom)+sizeof(j->virtual),j->original_hash);
}
static int marker(enum path p,const struct journal *j)
{unsigned char got[32];int e=read_file(p,got,32,0600);return e?e:(memcmp(got,j->digest,32)?EINVAL:0);}
/* No deletions: preserve journal, markers, custom staging and the Apple directory.
 * Rollback intent is durable before first rollback mutation. Recovery recognizes
 * every inode layout and retries moves, including a crash after a successful move.
 */
#include "audio_install_restore.inc"
static int mode(void)
{
 const char *p=PE_boot_args();unsigned seen=0,result=0;
 if(!p)return 0;size_t cap=sizeof(((boot_args *)0)->CommandLine),n=strnlen(p,cap);
 if(n==cap)return -1;
 for(size_t at=0;at<n;){while(at<n&&(p[at]==' '||p[at]=='\t'||p[at]=='\n'||p[at]=='\r'))at++;
 size_t b=at;while(at<n&&p[at]!=' '&&p[at]!='\t'&&p[at]!='\n'&&p[at]!='\r')at++;
 size_t len=at-b;const char key[]="nord2audioinstall";
 if(len>=sizeof(key)-1&&!memcmp(p+b,key,sizeof(key)-1)&&(len==sizeof(key)-1||p[b+sizeof(key)-1]=='=')){
 seen++;if(len==sizeof("nord2audioinstall=1")-1&&!memcmp(p+b,"nord2audioinstall=1",len))result=1;
 else if(len==sizeof("nord2audioinstall=2")-1&&!memcmp(p+b,"nord2audioinstall=2",len))result=2;
 else return -1;}}
 return seen>1?-1:(int)result;
}
#include "audio_install_flow.inc"

#else
int nord2_audio_install(void) { return 0; }
#endif
