#include "wifi_common.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
struct fake {uint32_t regs[40],original[40];unsigned calls,fail,persistent,corrupt,writes,firstwrite;unsigned order[80],nw;};
static int fault(struct fake*f){f->calls++;return f->fail&&(f->calls==f->fail||(f->persistent&&f->calls>=f->fail));}
static unsigned indexof(uint32_t a){unsigned i;for(i=0;i<40;i++)if(ncc_table[i].address==a)return i;assert(0);return 0;}
static int check(void*c,unsigned m){assert(m==NCC_REQUIRED);return fault(c);}
static int readreg(void*c,uint32_t a,uint32_t*v){struct fake*f=c;if(fault(f))return -1;*v=f->regs[indexof(a)];if(f->corrupt==f->calls)*v^=1;return 0;}
static int writereg(void*c,uint32_t a,uint32_t v){struct fake*f=c;unsigned i=indexof(a);f->writes++;if(!f->firstwrite)f->firstwrite=f->calls;assert(f->nw<80);f->order[f->nw++]=i;f->regs[i]=v;return fault(f)?-1:0;}
static int barrier(void*c){return fault(c);}
static const struct ncc_ops ops={check,readreg,writereg,barrier};
static void init(struct fake*f){unsigned i;memset(f,0,sizeof(*f));for(i=0;i<40;i++)f->regs[i]=f->original[i]=0x56781234u+i;}
int main(void){struct fake f;struct ncc_result r;unsigned i,j,total;
 for(j=1;j<=2;j++){init(&f);assert(!ncc_run(&ops,&f,1,j,&r));assert(r.clean&&r.phase==4&&r.snapshots==(j==1?4:40));assert(r.verified==r.snapshots);assert(!memcmp(f.regs,f.original,sizeof(f.regs)));assert(f.firstwrite>=2*r.snapshots);for(i=0;i<r.snapshots;i++){assert(f.order[i]==i);assert(f.order[r.snapshots+i]==r.snapshots-1-i);}}
 total=f.calls;
 for(i=1;i<=total;i++){
  init(&f);f.fail=i;assert(ncc_run(&ops,&f,1,2,&r)==-1);assert(r.error||r.restore_error);assert(r.clean==(r.touched==r.restored));if(r.clean)assert(!memcmp(f.regs,f.original,sizeof(f.regs)));
  init(&f);f.fail=i;f.persistent=1;assert(ncc_run(&ops,&f,1,2,&r)==-1);if(r.touched)assert(!r.clean&&r.restore_error);
 }
 for(i=0;i<4;i++){if(i==1)continue;init(&f);assert(ncc_run(&ops,&f,i,2,&r)==-1);assert(!f.calls&&!f.writes&&r.clean);}
 init(&f);assert(ncc_run(&ops,&f,1,3,&r)==-1&&!f.calls);
 for(i=1;i<=total;i++){init(&f);f.corrupt=i;int rc=ncc_run(&ops,&f,1,2,&r);if(rc==0)assert(r.clean);else assert(r.error||r.restore_error);if(r.clean)assert(!memcmp(f.regs,f.original,sizeof(f.regs)));}
 printf("Exhausted %u callback positions (transient/permanent failures and read corruption)\n",total);
 puts("PASS snapshots/order/masks/reverse restore, all transient+permanent callback faults, partial writes, optout");return 0;}
