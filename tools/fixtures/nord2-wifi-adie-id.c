#include "wifi_adie_id.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
struct fake {unsigned calls,fail,persistent,owned,triggers,releases,reads,writes,delays,busy_pre,busy_post;uint32_t addr,id;};
static int fault(struct fake*f){return ++f->calls==f->fail||(f->persistent&&f->fail&&f->calls>f->fail);}
static int check(void*c,unsigned mask){assert(mask==NAI_REQUIRED);return fault(c);}
static int acquire(void*c,unsigned n){struct fake*f=c;assert(n==11&&!f->owned);f->owned=1;return fault(f)?-1:0;}
static int release(void*c,unsigned n){struct fake*f=c;assert(n==11&&f->owned);f->releases++;f->owned=0;return fault(f)?-1:0;}
static int readreg(void*c,uint32_t a,uint32_t*v){struct fake*f=c;assert(f->owned);f->reads++;if(fault(f))return -1;switch(a){case 0x18004000:*v=(f->triggers?f->busy_post:f->busy_pre)?0x20:0;return 0;case 0x18004050:*v=f->addr;return 0;case 0x18004058:assert(f->triggers==1&&!f->busy_post);*v=f->id;return 0;default:assert(0);return -1;}}
static int writereg(void*c,uint32_t a,uint32_t v){struct fake*f=c;assert(f->owned);f->writes++;if(a==0x18004050){assert(v==0xb02c||v==0x12345678);f->addr=v;}else{assert(a==0x18004054&&!v&&f->addr==0xb02c);f->triggers++;assert(f->triggers==1);}return fault(f)?-1:0;}
static int barrier(void*c){return fault(c);}
static int delay(void*c,unsigned us){struct fake*f=c;assert(us==50);f->delays++;return fault(f);}
static const struct nai_ops ops={check,acquire,release,readreg,writereg,barrier,delay};
static void init(struct fake*f){memset(f,0,sizeof(*f));f->addr=0x12345678;f->id=0x66358a11;}
int main(void){struct fake f;struct nai_result r;unsigned total;
 init(&f);assert(!nai_read_id(&ops,&f,1,&r));assert(r.id_valid&&r.known_chip&&r.clean&&r.command_complete&&!r.semaphore_owned&&!f.owned&&f.addr==0x12345678&&f.triggers==1&&f.releases==1);total=f.calls;
 for(unsigned i=1;i<=total;i++)for(unsigned permanent=0;permanent<2;permanent++){
  init(&f);f.fail=i;f.persistent=permanent;assert(nai_read_id(&ops,&f,1,&r)==-1);assert(r.error||r.restore_error);assert(f.triggers<=1);if(r.clean)assert(!r.semaphore_owned&&!f.owned&&f.addr==0x12345678);if(r.command_touched&&!r.command_complete)assert(!r.clean&&r.semaphore_owned&&!f.releases);
 }
 for(unsigned i=0;i<4;i++)if(i!=1){init(&f);assert(nai_read_id(&ops,&f,i,&r)==-1&&r.clean&&!f.calls);}
 init(&f);f.busy_pre=1;assert(nai_read_id(&ops,&f,1,&r)==-1&&r.clean&&!f.triggers&&r.polls==100&&f.delays==99);
 init(&f);f.busy_post=1;assert(nai_read_id(&ops,&f,1,&r)==-1&&!r.clean&&f.triggers==1&&!f.releases&&r.polls==101&&f.delays==99);
 init(&f);f.id=0x1234;assert(nai_read_id(&ops,&f,1,&r)==1&&r.clean&&r.id_valid&&!r.known_chip);
 const uint32_t ids[]={0x66358a00,0x66358a10,0x66358a11};for(unsigned i=0;i<3;i++){init(&f);f.id=ids[i];assert(!nai_read_id(&ops,&f,1,&r)&&r.clean);}
 printf("PASS exactTOPread; all%u callback transient/permanent faults; partialtrigger uncertainty retained; bounded pre/post100polls; optout; IDversions; no trigger replay\n",total);return 0;}
