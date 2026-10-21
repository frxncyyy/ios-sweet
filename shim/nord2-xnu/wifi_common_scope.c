#include "wifi_common_scope.h"
static int ncc_scope_guard(const struct ncc_ops *o,void *c){return o->check(c,NCC_REQUIRED);}
int ncc_scope(const struct ncc_ops *o,void *c,unsigned opt,struct ncc_result *r,int (*inner)(void *,int *),int *inner_clean)
{
 unsigned i,n; uint32_t v,w; int e;
 if(!r)return -1;
 *r=(struct ncc_result){0}; r->clean=1;
 if(inner_clean)*inner_clean=1;
 if(opt!=1||!inner||!inner_clean||!o||!o->check||!o->read||!o->write||!o->barrier){r->error=-1;return -1;}
 *inner_clean=1; n=NCC_COUNT; r->phase=1;
 for(i=0;i<n;i++){
  if(ncc_scope_guard(o,c)||o->read(c,ncc_table[i].address,&r->before[i])){r->error=-2;return -1;}
  r->snapshots++;
 }
 r->phase=2;
 for(i=0;i<n;i++){
  /* Recheck full snapshot before writing: unexpected concurrent changes fail. */
  if(ncc_scope_guard(o,c)||o->read(c,ncc_table[i].address,&v)||v!=r->before[i]){r->error=-3;break;}
  w=(v&~ncc_table[i].mask)|(ncc_table[i].value&ncc_table[i].mask);
  if(ncc_scope_guard(o,c)){r->error=-3;break;}
  r->touched|=UINT64_C(1)<<i; r->clean=0;
  e=o->write(c,ncc_table[i].address,w);
  if(e||ncc_scope_guard(o,c)||o->barrier(c)||ncc_scope_guard(o,c)||o->read(c,ncc_table[i].address,&v)||v!=w){r->error=-4;break;}
  r->verified++;
 }
 if(!r->error){
  *inner_clean=0;
  int ie=inner(c,inner_clean);
  if(!*inner_clean){r->error=-10;return -1;} /* retain ALL common ownership */
  if(ie)r->error=-11;
 }
 r->phase=3;
 /* Best effort for every touched register. Never claim restoration merely
  * because a failing write may actually have reached the device. */
 for(i=n;i>0;){i--;if(!(r->touched&(UINT64_C(1)<<i)))continue;
  if(ncc_scope_guard(o,c)){r->restore_error=-5;continue;}
  e=o->write(c,ncc_table[i].address,r->before[i]);
  if(e||ncc_scope_guard(o,c)||o->barrier(c)||ncc_scope_guard(o,c)||o->read(c,ncc_table[i].address,&v)||v!=r->before[i]){r->restore_error=-6;continue;}
  r->restored|=UINT64_C(1)<<i;
 }
 r->clean=(r->restored==r->touched);
 if(!r->error&&!r->restore_error)r->phase=4;
 return r->error||r->restore_error?-1:0;
}
