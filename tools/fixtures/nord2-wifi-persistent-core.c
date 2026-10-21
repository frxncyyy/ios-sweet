#include "wifi_persistent.h"
#include <assert.h>
#include <stdio.h>
struct mock {unsigned calls,fail,failed,after,writes,ctrl,hold,hold19,clear,efuse_stuck,invalid,topword,clockstuck,handed;};
static int tick(struct mock *m){if(m->failed)m->after++;if(++m->calls==m->fail){m->failed=1;return -1;}return 0;}
static int check(void *p,unsigned flags){assert(flags==NPC_REQUIRED);return tick(p);}
static int read(void *p,uint32_t a,uint32_t *v){struct mock *m=p;if(tick(m))return -1;assert(!m->handed||a==0x18001860);
 *v=a==0x18001818?0x1234:0xa5a5a5a5;
 if(a==0x18052830)*v=m->topword;
 if(a==0x18005008)*v=m->clockstuck?(1u<<26):0;
 return 0;}
static int write(void *p,uint32_t a,uint32_t v){struct mock *m=p;if(tick(m))return -1;
 if(m->writes==0)assert(a==0x18052820&&v==0x1234);
 if(a>=0x18050000&&a<0x18054000&&v==0)m->clear++;
 /* Never WFSYS/BGFSYS power/reset, arbitrary DRAM or CPU release. */
 assert(a!=0x18000000&&a!=0x18000008&&a!=0x18000010);
 assert((a>=0x18000000&&a<0x18070000)||a==0x10005054||a==0x10005154||a==0x10005460);
 if(a==0x18052830)m->topword=v;
 if(a==0x18001a00&&!(v&(1u<<23)))m->handed=1;
 m->writes++;return 0;}
static int delay(void *p,unsigned n){assert(n==5||n==30||n==60||n==800);return tick(p);}
static int sem(void *p,unsigned n,int take){struct mock *m=p;assert(n==11||n==19);if(tick(m))return -1;unsigned *held=n==11?&m->hold:&m->hold19;
 if(take){assert(!*held);*held=1;}else{assert(*held);*held=0;}return 0;}
static int spi(void *p,unsigned a,int w,uint32_t *v){struct mock *m=p;assert(m->hold);if(tick(m))return -1;
 if(w){if(a==0x108){assert(!(*v&0xc0));if(*v&0x40000000)m->ctrl=1;}return 0;}
 if(a==0x2c)*v=0x66358a11;
 else if(a==0x108)*v=m->ctrl?(m->efuse_stuck?0x40000000:(m->invalid?0:0x20000000)):0;
 else if(a==0x134)*v=0x8080;
 else *v=0;
 return 0;}
static const struct npc_ops ops={check,read,write,delay,sem,spi};
int main(void){struct npc_result r;struct mock m={0};
 assert(!npc_run(&ops,&m,1,0,&r));assert(r.phase==10&&r.retained&&!r.halt&&!r.sema_held&&!r.sema19_held&&r.efuse_valid);assert(m.clear>=4096&&!m.hold&&!m.hold19&&m.handed);unsigned calls=m.calls;
 for(unsigned i=1;i<=calls;i++){m=(struct mock){.fail=i};assert(npc_run(&ops,&m,1,0,&r));assert(!m.after);if(r.mutated||r.sema_held||r.sema19_held)assert(r.halt);}
 m=(struct mock){.efuse_stuck=1};assert(npc_run(&ops,&m,1,0,&r));assert(r.halt&&r.sema_held&&r.efuse_polls==70);
 m=(struct mock){.clockstuck=1};assert(npc_run(&ops,&m,1,0,&r));assert(r.halt&&r.sema19_held&&r.clock_polls==101);
 m=(struct mock){.invalid=1};assert(!npc_run(&ops,&m,1,0,&r));assert(!r.efuse_valid);
 m=(struct mock){0};assert(npc_run(&ops,&m,0,0,&r)&&!m.calls);
 printf("PASS success, invalid efuse, busy timeout70, clockbusy101, strict gate, %u every-callback fault positions; no callback after failure\n",calls);
}
