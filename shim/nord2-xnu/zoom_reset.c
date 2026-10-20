/* Opt-in Nord2 exact-file repair. Call once immediately before load_init_program.
 * No raw IO, filesystem mount/repair, arbitrary paths or generic plist parser. */
#ifndef NZR_HOST_TEST
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/vnode.h>
#include <sys/vnode_internal.h>
#include <sys/vnode_if.h>
#include <sys/fcntl.h>
#include <sys/uio.h>
#include <sys/mount.h>
#include <sys/proc.h>
#include <pexpert/pexpert.h>
#include <pexpert/boot.h>
#endif
#define NZR_BYTES 83
static const unsigned char nzr_old[83]={0x62,0x70,0x6c,0x69,0x73,0x74,0x30,0x30,0xd2,0x01,0x02,0x03,0x04,0x5d,0x63,0x61,0x6e,0x76,0x61,0x73,0x5f,0x68,0x65,0x69,0x67,0x68,0x74,0x5c,0x63,0x61,0x6e,0x76,0x61,0x73,0x5f,0x77,0x69,0x64,0x74,0x68,0x11,0x08,0x1f,0x11,0x03,0xc0,0x08,0x0d,0x1b,0x28,0x2b,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2e};
static const unsigned char nzr_new[83]={0x62,0x70,0x6c,0x69,0x73,0x74,0x30,0x30,0xd2,0x01,0x02,0x03,0x04,0x5d,0x63,0x61,0x6e,0x76,0x61,0x73,0x5f,0x68,0x65,0x69,0x67,0x68,0x74,0x5c,0x63,0x61,0x6e,0x76,0x61,0x73,0x5f,0x77,0x69,0x64,0x74,0x68,0x11,0x09,0x84,0x11,0x04,0x65,0x08,0x0d,0x1b,0x28,0x2b,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2e};
static const char nzr_path[]="/private/var/mobile/Library/Preferences/com.apple.iokit.IOMobileGraphicsFamily.plist";
static const char nzr_backup[]="/private/var/mobile/Library/Preferences/com.apple.iokit.IOMobileGraphicsFamily.plist.nord2-zoomreset-original";
static int nzr_meta(vnode_t vp,vfs_context_t ctx,unsigned uid,unsigned gid,unsigned bytes)
{
 struct vnode_attr va;VATTR_INIT(&va);
 VATTR_WANTED(&va,va_uid);VATTR_WANTED(&va,va_gid);VATTR_WANTED(&va,va_mode);
 VATTR_WANTED(&va,va_nlink);VATTR_WANTED(&va,va_data_size);
 int e=vnode_getattr(vp,&va,ctx);if(e)return e;
 if(!VATTR_IS_SUPPORTED(&va,va_uid)||!VATTR_IS_SUPPORTED(&va,va_gid)||
  !VATTR_IS_SUPPORTED(&va,va_mode)||!VATTR_IS_SUPPORTED(&va,va_nlink)||
  !VATTR_IS_SUPPORTED(&va,va_data_size)||vnode_vtype(vp)!=VREG||va.va_uid!=uid||(gid!=(unsigned)-1 && va.va_gid!=gid)||
  (va.va_mode&07777)!=0600||va.va_nlink!=1||va.va_data_size!=bytes)return EINVAL;
 return 0;
}
static int nzr_io(vnode_t vp,vfs_context_t ctx,int direction,unsigned char *b)
{
 int resid=83;int e=vn_rdwr(direction,vp,(caddr_t)b,83,0,UIO_SYSSPACE,
  direction==UIO_WRITE?(IO_SYNC|IO_UNIT):0,vfs_context_ucred(ctx),&resid,vfs_context_proc(ctx));
 return e?e:(resid?EIO:0);
}
/* Require one literal token; reject duplicates, numeric aliases and bare keys.
 * ARM boot command line is bounded; no dependency on numeric parser aliases. */
static bool nzr_enabled(void)
{
 const char *p=PE_boot_args();if(!p)return false;
 size_t cap=sizeof(((boot_args *)0)->CommandLine);
 size_t len=strnlen(p,cap);if(len==cap)return false;
 unsigned seen=0,valid=0;
 for(size_t at=0;at<len;){
  while(at<len && (p[at]==' '||p[at]=='\t'||p[at]=='\n'||p[at]=='\r'))at++;
  size_t begin=at;while(at<len && p[at]!=' '&&p[at]!='\t'&&p[at]!='\n'&&p[at]!='\r')at++;
  size_t n=at-begin;static const char key[]="nord2zoomreset";
  if(n>=sizeof(key)-1 && !memcmp(p+begin,key,sizeof(key)-1) &&
    (n==sizeof(key)-1 || p[begin+sizeof(key)-1]=='=')){
   seen++;if(n==sizeof("nord2zoomreset=1")-1 && !memcmp(p+begin,"nord2zoomreset=1",n))valid++;
  }
 }
 return seen==1 && valid==1;
}
/* All reports after closing vnodes; no console/UFS lock nesting. */
void nord2_zoom_reset(void)
{
 if(!nzr_enabled())return;
 vfs_context_t ctx=vfs_context_kernel();vnode_t vp=NULL,bp=NULL;unsigned char data[83];
 int error=0,restore=0,close_error=0,stage=1;bool attempted=false,verified=false,backup_ok=false,already=false;
 int target_flags=FREAD|FWRITE|O_NOFOLLOW_ANY,backup_flags=FREAD|FWRITE|O_CREAT|O_EXCL|O_NOFOLLOW_ANY;
 error=vnode_open(nzr_path,target_flags,0,0,&vp,ctx);if(error)goto out;
 error=nzr_meta(vp,ctx,501,501,83);if(error)goto out;
 error=nzr_io(vp,ctx,UIO_READ,data);if(error)goto out;
 if(!memcmp(data,nzr_new,83)){already=true;verified=true;goto out;}
 if(memcmp(data,nzr_old,83)){error=EINVAL;goto out;}
 stage=2;error=vnode_open(nzr_backup,backup_flags,0600,0,&bp,ctx);
 if(error==EEXIST){backup_flags=FREAD|O_NOFOLLOW_ANY;error=vnode_open(nzr_backup,backup_flags,0,0,&bp,ctx);}
 else if(!error){
  /* Only the exclusively new inode may be assigned backup ownership. */
  error=nzr_meta(bp,ctx,0,(unsigned)-1,0);if(error)goto out;
  struct vnode_attr va;VATTR_INIT(&va);VATTR_SET(&va,va_uid,0);VATTR_SET(&va,va_gid,0);VATTR_SET(&va,va_mode,0600);
  error=vnode_setattr(bp,&va,ctx);if(error)goto out;
  error=nzr_meta(bp,ctx,0,0,0);if(error)goto out;
  error=nzr_io(bp,ctx,UIO_WRITE,(unsigned char *)nzr_old);if(error)goto out;
 }
 if(error)goto out;
 error=nzr_meta(bp,ctx,0,0,83);if(error)goto out;
 error=nzr_io(bp,ctx,UIO_READ,data);if(error)goto out;
 if(memcmp(data,nzr_old,83)){error=EINVAL;goto out;}
 error=VNOP_FSYNC(bp,MNT_WAIT,ctx);if(error)goto out;
 backup_ok=true;
 close_error=vnode_close(bp,backup_flags,ctx);bp=NULL;if(close_error){error=close_error;goto out;}
 stage=3;error=nzr_meta(vp,ctx,501,501,83);if(error)goto out;
 error=nzr_io(vp,ctx,UIO_READ,data);if(error)goto out;
 if(memcmp(data,nzr_old,83)){error=EINVAL;goto out;}
 attempted=true;error=nzr_io(vp,ctx,UIO_WRITE,(unsigned char *)nzr_new);
 if(!error)error=VNOP_FSYNC(vp,MNT_WAIT,ctx);
 if(!error)error=nzr_io(vp,ctx,UIO_READ,data);
 if(!error && memcmp(data,nzr_new,83))error=EIO;
 if(!error)error=nzr_meta(vp,ctx,501,501,83);
 if(!error){verified=true;goto out;}
 stage=4;restore=nzr_io(vp,ctx,UIO_WRITE,(unsigned char *)nzr_old);
 if(!restore)restore=VNOP_FSYNC(vp,MNT_WAIT,ctx);
 if(!restore)restore=nzr_io(vp,ctx,UIO_READ,data);
 if(!restore && memcmp(data,nzr_old,83))restore=EIO;
 if(!restore)restore=nzr_meta(vp,ctx,501,501,83);
 out:
 if(bp){close_error=vnode_close(bp,backup_flags,ctx);if(!error)error=close_error;}
 if(vp){close_error=vnode_close(vp,target_flags,ctx);if(!error)error=close_error;}
 printf("NORD2 ZOOM RESET: stage=%d error=%d attempted=%u backup=%u verified=%u already=%u restore_error=%d\n",
 stage,error,attempted,backup_ok,verified,already,restore);
}
