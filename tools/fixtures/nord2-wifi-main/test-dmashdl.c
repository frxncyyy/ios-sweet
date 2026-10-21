#include "wifi_main_dmashdl.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
struct model{unsigned calls,fail,failed,after,writes,active;uint32_t regs[64];};
static int step(struct model *m){if(m->failed)m->after++;if(++m->calls==m->fail){m->failed=1;return -1;}return 0;}
static int owner(void *c){return step(c);}
static int read32(void *c,uint32_t a,uint32_t *v){struct model*m=c;if(step(m))return -1;if(a>=0x18026000&&a<0x18026100)*v=m->regs[(a-0x18026000)/4];else *v=m->active&&a==0x18024208?1:0;return 0;}
static int write32(void *c,uint32_t a,uint32_t v){struct model*m=c;int e=step(m);assert(a>=0x18026000&&a<0x18026100&&!(a&3));m->regs[(a-0x18026000)/4]=v;m->writes++;return e;}
static const struct nwt_io io={.owner=owner,.reg_read=read32,.reg_write=write32};
int main(void){struct model m={0};struct nwdm_state s={0};assert(nwdm_apply(&io,&m,&s,0)&&!m.calls);for(unsigned i=0;i<64;i++)m.regs[i]=0xf000f000;assert(!nwdm_apply(&io,&m,&s,1)&&s.complete&&s.dirty&&s.operations==99&&m.writes==99);unsigned total=m.calls;assert((m.regs[0x1c/4]&0xfff)==1);assert((m.regs[0x10/4]>>16)==0xffea);assert((m.regs[0x20/4]&0x0fff0fff)==0x015d0010);assert(m.regs[0xd0/4]==0x22220000&&m.regs[0xb0/4]==0x76543210&&m.regs[0xb4/4]==0xfedcba98);assert((m.regs[8/4]&0xffffff)==0x30015);assert(nwdm_apply(&io,&m,&s,1)&&m.calls==total);
 for(unsigned f=1;f<=total;f++){m=(struct model){.fail=f};s=(struct nwdm_state){0};assert(nwdm_apply(&io,&m,&s,1)&&!m.after&&!s.complete&&s.halt);unsigned b=m.calls;assert(nwdm_apply(&io,&m,&s,1)&&m.calls==b);}
 m=(struct model){.active=1};s=(struct nwdm_state){0};assert(nwdm_apply(&io,&m,&s,1)==1&&!m.writes&&!s.halt);
 printf("PASS99sourceRMWs/reservedpreservation/queuepriorityquotas/onewayfailretention/%ucallbackfaults/no replay/activepreflightnomutation; noDMAenable\n",total);}
