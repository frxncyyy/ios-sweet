/* Pure transaction engine: no MMIO implementation, mapping, allocation, CPU
 * execution or boot hook. All destination IO is injectable and checked. */
#include "wifi_rom_transaction.h"
#ifdef NWRT_HOST_TEST
#include <string.h>
#else
#include <libkern/libkern.h>
#endif
struct tx { const struct nwrt_ops *o;void *ctx;struct nwrt_result *r; };
static void erase(unsigned char *p,size_t n){volatile unsigned char *v=p;while(n--)*v++=0;}
static int check(struct tx *t){return t->o->check(t->ctx,NWRT_PREREQUISITES);}
static int barrier(struct tx *t){if(check(t))return -1;t->r->barriers++;return t->o->barrier(t->ctx);}
static int read(struct tx *t,uint64_t p,void *d,uint32_t n)
{if(check(t))return -1;t->r->reads++;return t->o->read(t->ctx,p,d,n);}
static int write(struct tx *t,uint64_t p,const void *d,uint32_t n)
{if(check(t))return -1;t->r->write_attempted=t->r->dirty=1;t->r->writes++;return t->o->write(t->ctx,p,d,n);}
static int overlap(const void *a,size_t an,const void *b,size_t bn)
{
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 if(x>UINTPTR_MAX-an || y>UINTPTR_MAX-bn)return 1;
 return x<y+bn && y<x+an;
}
int nwrt_run(const struct nwrt_ops *ops,void *ctx,unsigned prerequisites,
 const unsigned char *mcu,size_t mn,const unsigned char *wifi,size_t wn,
 unsigned char *backup,size_t bn,unsigned char *scratch,size_t sn,struct nwrt_result *r)
{
 if(!r)return -1;
 struct nwrt_result zero={0};*r=zero;
 if(!ops || !ops->check || !ops->read || !ops->write || !ops->barrier ||
    prerequisites!=NWRT_PREREQUISITES || !backup || !scratch ||
    bn<NWRT_BACKUP_BYTES || sn<NWRT_SCRATCH_BYTES ||
    overlap(backup,NWRT_BACKUP_BYTES,scratch,NWRT_SCRATCH_BYTES) ||
    overlap(backup,NWRT_BACKUP_BYTES,mcu,mn) || overlap(backup,NWRT_BACKUP_BYTES,wifi,wn) ||
    overlap(scratch,NWRT_SCRATCH_BYTES,mcu,mn) || overlap(scratch,NWRT_SCRATCH_BYTES,wifi,wn)) {r->error=-1;return -1;}
 struct nwf_rom_plan plan;
 if(nwf_rom_make_plan(mcu,mn,wifi,wn,&plan)){r->error=-2;return -1;}
 const unsigned char *source[2]={mcu,wifi};
 struct tx t={ops,ctx,r};size_t offsets[4],total=0;unsigned touched=0;
 r->phase=1;
 if(check(&t)){r->error=-3;goto done;}
 /* Snapshot ALL exact spans before the first destination write. */
 for(unsigned i=0;i<4;i++) {
  const struct nwf_rom_span *s=&plan.spans[i];offsets[i]=total;
  if(s->bytes>NWRT_BACKUP_BYTES-total){r->error=-1;goto done;}
  for(uint32_t at=0;at<s->bytes;) {
   uint32_t n=s->bytes-at;if(n>NWRT_SCRATCH_BYTES)n=NWRT_SCRATCH_BYTES;
   if(read(&t,s->physical+at,backup+total+at,n)){r->error=-4;goto done;}
   at+=n;
  }
  total+=s->bytes;
 }
 if(total!=NWRT_BACKUP_BYTES){r->error=-1;goto done;}
 r->phase=2;
 for(unsigned i=0;i<4;i++) {
  const struct nwf_rom_span *s=&plan.spans[i];
  for(uint32_t at=0;at<s->bytes;) {
   uint32_t n=s->bytes-at;if(n>NWRT_SCRATCH_BYTES)n=NWRT_SCRATCH_BYTES;
   /* Mark whole span touched before callback: failed writes can be partial. */
   touched=i+1;
   if(write(&t,s->physical+at,source[s->blob]+s->source_offset+at,n)) {r->error=-5;goto restore;}
   at+=n;
  }
 }
 if(barrier(&t)){r->error=-6;goto restore;}
 r->phase=3;
 for(unsigned i=0;i<4;i++) {
  const struct nwf_rom_span *s=&plan.spans[i];
  for(uint32_t at=0;at<s->bytes;) {
   uint32_t n=s->bytes-at;if(n>NWRT_SCRATCH_BYTES)n=NWRT_SCRATCH_BYTES;
   if(read(&t,s->physical+at,scratch,n) || memcmp(scratch,source[s->blob]+s->source_offset+at,n)) {r->error=-7;goto restore;}
   at+=n;
  }
 }
restore:
 if(r->write_attempted) {
  r->restore_attempted=1;r->phase=4;
  for(unsigned i=0;i<touched;i++) {
   const struct nwf_rom_span *s=&plan.spans[i];
   for(uint32_t at=0;at<s->bytes;) {
    uint32_t n=s->bytes-at;if(n>NWRT_SCRATCH_BYTES)n=NWRT_SCRATCH_BYTES;
    if(write(&t,s->physical+at,backup+offsets[i]+at,n))r->restore_error=-8;
    at+=n;
   }
  }
  if(barrier(&t))r->restore_error=-9;
  r->phase=5;
  for(unsigned i=0;i<touched;i++) {
   const struct nwf_rom_span *s=&plan.spans[i];
   for(uint32_t at=0;at<s->bytes;) {
    uint32_t n=s->bytes-at;if(n>NWRT_SCRATCH_BYTES)n=NWRT_SCRATCH_BYTES;
    if(read(&t,s->physical+at,scratch,n) || memcmp(scratch,backup+offsets[i]+at,n))r->restore_error=-10;
    at+=n;
   }
  }
  if(!r->restore_error){r->dirty=0;r->restore_verified=1;}
 }
done:
 if(r->dirty)r->backup_retained=1;
 else erase(backup,NWRT_BACKUP_BYTES);
 erase(scratch,NWRT_SCRATCH_BYTES);
 return r->error || r->restore_error ? -1:0;
}
