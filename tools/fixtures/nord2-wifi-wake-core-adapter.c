#include "wifi_wake_adapter.h"
#include <assert.h>
#include <stdio.h>
struct mock {struct nawa *a;unsigned calls,fail,failed,after,wakes,sleeps,clockwrites,slept,ackbusy,clockbad,irqbad,idbad,coldbad,foreign;};
static int tick(struct mock *m){assert(!m->slept);if(m->failed)m->after++;return ++m->calls==m->fail?(m->failed=1,-1):0;}
static int owner(void *p){return tick(p);}
static int read(void *p,uint32_t a,uint32_t *v){struct mock *m=p;if(tick(m))return -1;*v=0;
 switch(a){
 case 0x10006304:*v=0xd;break;case 0x1000616c:case 0x10006170:*v=2;break;case 0x1020e504:*v=1;break;
 case 0x10001228:case 0x10001724:break;
 case 0x180601a0:assert(!m->wakes);*v=m->foreign;break;
 case 0x18060184:assert(m->wakes==1);*v=m->ackbusy?0x20:0;break;
 case 0x18060000:assert(m->wakes==1);*v=m->clockbad?0x4000:0x4006;break;
 case 0x180602d4:assert(m->wakes==1);*v=!m->irqbad;break;
 case 0x18001000:assert(m->a->result.awake);*v=m->idbad?0:0x20010101;break;
 case 0x18000010:case 0x18000000:case 0x180602cc:case 0x18000014:case 0x18000008:
  assert(m->a->result.awake&&m->a->result.chip==0x20010101);*v=m->coldbad?~0u:0;break;
 default:assert(0);
 }
 return 0;
}
static int write(void *p,uint32_t a,uint32_t v){struct mock *m=p;if(tick(m))return -1;
 if(a==0x180601a0){
  if(v){assert(v==1&&!m->wakes);m->wakes++;}
  else{assert(m->a->result.cold_valid==31&&m->wakes==1);m->sleeps++;m->slept=1;}
 }else{assert(a==0x18060000&&v==(m->clockbad?0x4001:0x4007)&&m->wakes==1);m->clockwrites++;}
 return 0;
}
static int delay(void *p,unsigned us){assert(us==1||us==10||us==1000);return tick(p);}
static const struct nawa_hw hw={owner,read,write,delay};
int main(void){struct nawa a={0};struct mock m={.a=&a};
 assert(nawa_start(&a,&hw,&m,0)&&!m.calls&&!a.result.entered);
 assert(!nawa_start(&a,&hw,&m,1));assert(a.result.balanced&&a.result.phase==7&&!a.result.reference&&!a.result.retained&&!a.result.halt&&a.asleep);assert(m.wakes==1&&m.sleeps==1);
 unsigned total=m.calls;assert(nawa_start(&a,&hw,&m,1)&&m.calls==total);
 for(unsigned f=1;f<=total;f++){
  a=(struct nawa){0};m=(struct mock){.a=&a,.fail=f};
  assert(nawa_start(&a,&hw,&m,1));assert(!m.after&&!m.sleeps);
  if(a.result.reference)assert(a.result.halt&&a.result.retained);
 }
 a=(struct nawa){0};m=(struct mock){.a=&a,.foreign=1};assert(nawa_start(&a,&hw,&m,1));assert(!m.wakes&&!a.result.halt);
 a=(struct nawa){0};m=(struct mock){.a=&a,.ackbusy=1};assert(nawa_start(&a,&hw,&m,1));assert(a.result.ack_polls==11&&a.result.halt&&!m.clockwrites&&!m.sleeps);
 a=(struct nawa){0};m=(struct mock){.a=&a,.clockbad=1};assert(nawa_start(&a,&hw,&m,1));assert(a.result.clock_polls==1600&&a.result.halt&&!m.sleeps);
 a=(struct nawa){0};m=(struct mock){.a=&a,.irqbad=1};assert(nawa_start(&a,&hw,&m,1));assert(a.result.halt&&!m.sleeps);
 a=(struct nawa){0};m=(struct mock){.a=&a,.idbad=1};assert(nawa_start(&a,&hw,&m,1));assert(a.result.id_polls==10&&a.result.halt&&!a.result.cold_valid&&!m.sleeps);
 a=(struct nawa){0};m=(struct mock){.a=&a,.coldbad=1};assert(nawa_start(&a,&hw,&m,1));assert(a.result.cold_valid==1&&a.result.halt&&!m.sleeps);
 printf("PASS oneownedAPwake/balancedsleep, nooffdomainprewake/noanycallbackaftersleep, every%ucallbackfault, ACK11/clock1600/ID10 bounds, IRQ/cold/refcount/defaultoff/reentry\n",total);
}
