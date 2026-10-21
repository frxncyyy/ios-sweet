#include "wifi_main_initializer.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned char memory[NDP_POOL_BYTES];
struct mock {unsigned calls,fail,failed,after,writes,memwrites,syncs,inherited,lose,lp,irq,coherent;uint32_t address[2048],value[2048];unsigned registers;};
static int step(struct mock*m){if(m->failed)m->after++;if(++m->calls==m->fail){m->failed=1;return -1;}return 0;}
static int own(void*c){return step(c);}
static int rd(void*c,uint32_t a,uint32_t*v){struct mock*m=c;if(step(m))return -1;*v=0;if(a==0x18024200||a==0x18025200){if(m->coherent==1||(m->coherent==2&&m->writes>100))*v=0x100000;return 0;}if(a==0x18024204||a==0x18025204){*v=m->irq;return 0;}if(a==0x18060010){*v=m->lp;if(m->lose&&m->writes>100)*v=4;return 0;}if(a==0x18024208&&m->inherited){*v=1;return 0;}for(unsigned i=0;i<m->registers;i++)if(m->address[i]==a){*v=m->value[i];break;}return 0;}
static int wr(void*c,uint32_t a,uint32_t v){struct mock*m=c;int e=step(m);m->writes++;assert(a!=0x18024228&&a!=0x18025228);if(a==0x18060010){assert(v==2);m->lp=0;return e;}for(unsigned i=0;i<m->registers;i++)if(m->address[i]==a){m->value[i]=v;return e;}assert(m->registers<2048);m->address[m->registers]=a;m->value[m->registers++]=v;return e;}
static int mw(void*c,uint64_t a,const void*p,uint32_t n){struct mock*m=c;int e=step(m);assert(a>=NDP_POOL_BASE&&a-NDP_POOL_BASE+n<=sizeof(memory)&&n<=4096);m->memwrites++;if(!m->fail)memcpy(memory+a-NDP_POOL_BASE,p,n);return e;}
static int sy(void*c,uint64_t a,uint32_t n,unsigned d){struct mock*m=c;assert(d==1&&n&&a>=NDP_POOL_BASE);m->syncs++;return step(m);}
static int us(void*c,unsigned n){assert(n==1000);return step(c);}
static const struct nwt_io io={.owner=own,.reg_read=rd,.reg_write=wr,.mem_write=mw,.sync=sy,.delay_us=us};
static struct npp_state pool={.entered=1,.stage=NPP_DONE,.claim_ready=1,.live_valid=31,.preflight_complete=1,.verified=1,.zero_complete=1,.balanced=1,.asleep=1,.set_attempted=3,.set_accepted=2,.raw_valid=3,.words_written=32768,.words_read=32768,.mapped=1};
static const struct ndp_claim claim={NDP_POOL_BASE,NDP_POOL_BYTES,3,1};
static struct nwi_state prepare(void){struct nwi_state s={0};assert(!nwi_prepare(&s,&claim,&pool));assert(s.end<=claim.base+claim.bytes);for(unsigned i=0;i<9;i++)for(unsigned j=i+1;j<9;j++)assert(s.rings[i].descriptors+16*s.rings[i].count<=s.rings[j].descriptors||s.rings[j].descriptors+16*s.rings[j].count<=s.rings[i].descriptors);return s;}
int main(void){struct nwi_state s=prepare();struct mock m={.lp=4};assert(!nwi_initialize(&io,&m,&s,1));assert(s.initialized&&s.driver_owned&&s.driver_requested&&s.transport.phase==2&&s.scheduler.operations==99);unsigned total=m.calls;assert(m.memwrites==s.memory_writes&&m.syncs==m.memwrites);for(unsigned r=0;r<9;r++){struct nwi_ring*t=&s.rings[r];unsigned char*d=memory+t->descriptors-NDP_POOL_BASE;if(t->rx){assert(d[7]==15&&d[6]==0);}else assert(d[7]==0x80);}unsigned checked=0;
 /* Every callback fault position, including committed failing writes. The
  * fault run skips copying mock bytes; algorithm and barriers still execute. */
 for(unsigned f=1;f<=total;f++){s=prepare();m=(struct mock){.lp=4,.fail=f};assert(nwi_initialize(&io,&m,&s,1)<0&&!m.after&&s.halt&&!s.initialized);unsigned old=m.calls;assert(nwi_initialize(&io,&m,&s,1)<0&&m.calls==old);checked++;}
 s=prepare();m=(struct mock){.inherited=1};assert(nwi_initialize(&io,&m,&s,1)==1&&!m.writes&&!m.memwrites&&!s.halt);
 s=prepare();m=(struct mock){.lose=1};assert(nwi_initialize(&io,&m,&s,1)<0&&s.halt&&!s.initialized);
 s=prepare();m=(struct mock){.irq=1};assert(nwi_initialize(&io,&m,&s,1)==1&&!m.writes&&!m.memwrites&&!s.halt);
 s=prepare();m=(struct mock){.irq=0x80000000};assert(!nwi_initialize(&io,&m,&s,1)&&s.irq_final[0]==0x80000000);
 s=prepare();m=(struct mock){.coherent=1};assert(nwi_initialize(&io,&m,&s,1)==1&&!m.writes&&!m.memwrites&&!s.halt);
 s=prepare();m=(struct mock){.coherent=2};assert(nwi_initialize(&io,&m,&s,1)<0&&s.halt&&!s.initialized);
 struct npp_state bad=pool;bad.error=-1;s=(struct nwi_state){0};assert(nwi_prepare(&s,&claim,&bad));
 printf("PASS initializer: nine disjoint source rings; driver-own, 99 scheduler operations, memory visibility, %u callback faults, no replay, inherited refusal, ownership-loss halt; private callback model only\n",checked);
}
