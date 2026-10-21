#include "wifi_persistent_adapter.h"
#include <assert.h>
#include <stdio.h>
struct reg {uint32_t a,v;};
struct mock {struct npa *owner;uint32_t sram[4096];struct reg reg[180];unsigned n,calls,fail,failed,after,writes,trigger,handed,postreads,postwrites;uint32_t command;unsigned buildword_writes;unsigned efuse,clockfail,busy_after_trigger,delays50,fail_write;uint32_t fail_address;};
static int tick(struct mock *m){if(m->failed)m->after++;if(++m->calls==m->fail){m->failed=1;return -1;}return 0;}
static int ap(void *p){struct mock *m=p;assert(!m->owner->hw_handoff);return tick(m);}
static int proof(void *p){return tick(p);}
static int rd(void *p,uint32_t a,uint32_t *v){struct mock *m=p;if(tick(m))return -1;
 if(m->fail_address==a&&!m->fail_write){m->failed=1;return -1;}
 if(m->owner->hw_handoff){assert(a==0x18001860);m->postreads++;}
 *v=0;
 if(a==0x10006504||a==0x180602d4)*v=1;
 if(a==0x18060000)*v=m->clockfail?0:0x4006;
 if(a==0x1807202c||a==0x1807204c)*v=1;
 if(a==0x18001818)*v=0x1234;
 for(unsigned i=0;i<m->n;i++)if(m->reg[i].a==a)*v=m->reg[i].v;
 if(a>=0x18050000&&a<0x18054000)*v=m->sram[(a-0x18050000)/4];
 if(a==0x18060000)*v=m->clockfail?0:0x4006;
 if(a==0x18004000&&m->busy_after_trigger&&m->trigger)*v=0x20;
 if(a==0x18004058){unsigned r=m->command&0xfff;*v=r==0x2c?0x66358a11:r==0x108?(m->efuse?0x20000000:0):r==0x134?0x8080:0;}
 return 0;
}
static int wr(void *p,uint32_t a,uint32_t v){struct mock *m=p;if(tick(m))return -1;
 if(m->fail_address==a&&m->fail_write){m->failed=1;return -1;}
 if(m->owner->hw_handoff){assert(a==0x18001a00||a==0x18001860);m->postwrites++;}
 assert(a!=0x18000010&&a!=0x18000000&&a!=0x18000008&&a!=0x18000014);
 m->writes++;if(a==0x18052838&&v==3)m->buildword_writes++;
 if(a==0x18004050)m->command=v;
 if(a==0x18004054){m->trigger++;if(m->command==0xa108&&(v&0x40000000))m->efuse=1;}
 if(a>=0x18050000&&a<0x18054000){m->sram[(a-0x18050000)/4]=v;return 0;}
 for(unsigned i=0;i<m->n;i++)if(m->reg[i].a==a){m->reg[i].v=v;goto done;}
 assert(m->n<180);m->reg[m->n++]=(struct reg){a,v};
done:
 return 0;
}
static int delay(void *p,unsigned n){struct mock *m=p;if(n==50)m->delays50++;return tick(p);}
static const struct npa_hw hw={ap,proof,rd,wr,delay};
int main(void){struct npa s={0};struct mock m={.owner=&s};
 assert(npa_start(&s,&hw,&m,0,0x8fb00000,0x8a000000,3)&&!m.calls);
 assert(!npa_start(&s,&hw,&m,1,0x8fb00000,0x8a000000,3));
 assert(s.retained&&!s.fatal&&s.hw_handoff==2&&s.result.phase==10&&!s.sema11&&!s.sema19&&!s.spi_inflight);
 assert(m.postreads==1&&m.postwrites==2);assert(m.buildword_writes==1&&m.sram[0x2838/4]==0);unsigned total=m.calls;assert(npa_start(&s,&hw,&m,1,0x8fb00000,0x8a000000,3));assert(m.calls==total);
 unsigned failures[]={1,2,20,100,1000,10000,total/2,total-3,total-2,total-1,total};
 for(unsigned i=0;i<sizeof(failures)/sizeof(*failures);i++){
  s=(struct npa){0};m=(struct mock){.owner=&s,.fail=failures[i]};
  assert(npa_start(&s,&hw,&m,1,0x8fb00000,0x8a000000,3));assert(!m.after);
  if(s.retained)assert(s.fatal);
 }
 const uint32_t fault_address[]={0x18000010,0x18000014,0x18008040,0x10006504,0x180601c4,0x1807202c,0x1807204c,0x180602d4,0x18004050,0x18004054,0x18004058,0x1807222c,0x1807224c,0x18050000};
 for(unsigned i=0;i<sizeof(fault_address)/sizeof(*fault_address);i++)for(unsigned w=0;w<2;w++){
  s=(struct npa){0};m=(struct mock){.owner=&s,.fail_address=fault_address[i],.fail_write=w};
  int e=npa_start(&s,&hw,&m,1,0x8fb00000,0x8a000000,3);
  if(m.failed){assert(e&&!m.after);if(s.retained)assert(s.fatal);}
 }
 s=(struct npa){0};m=(struct mock){.owner=&s,.busy_after_trigger=1};
 assert(npa_start(&s,&hw,&m,1,0x8fb00000,0x8a000000,3));assert(s.fatal&&s.spi_inflight&&s.sema11&&m.delays50==101);
 s=(struct npa){0};m=(struct mock){.owner=&s,.clockfail=1};assert(npa_start(&s,&hw,&m,1,0x8fb00000,0x8a000000,3));assert(s.fatal&&s.clock_polls==1600);
 s=(struct npa){0};m=(struct mock){.owner=&s};assert(npa_start(&s,&hw,&m,1,0x90000000,0,0)&&!m.calls);
 printf("PASS build3handOff-then-clear native-bound adapter retained/remap/sourcecore, %u backend callbacks; %zu fault samples plus14addressfaultpairs, posttriggerbusy101, clock1600timeout, defaultoff,reentry,badbase\n",total,sizeof(failures)/sizeof(*failures));
}
