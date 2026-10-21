#include "wifi_wfsys_adapter.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

struct ev { char op;uint32_t addr,val; };
struct mock {
 unsigned calls,fault_at,slept,busy,never_ack,never_id,foreign_vote;
 unsigned sem,sem_acquires,sem_releases,writes,reads;
 uint32_t ap,wf,afe,top,power_ack,wfprot,dmaprot,idle,emi,vdnr,cpu,sema_rst;
 uint32_t bg_cpu,bg_top;
 uint32_t host_clock;
 struct ev events[100];unsigned nevents;
};
static int step(struct mock *m) {
 assert(!m->slept);m->calls++;return m->fault_at==m->calls?-1:0;
}
static void event(struct mock *m,char op,uint32_t a,uint32_t v) {
 if(op=='w'&&m->nevents<100)m->events[m->nevents++]=(struct ev){op,a,v};
}
static int owner(void *p){return step(p);}
static int delay(void *p,unsigned us){assert(us==1||us==30||us==50||us==500||us==1000);return step(p);}
static int read32(void *p,uint32_t a,uint32_t *v) {
 struct mock *m=p;if(step(m))return -1;m->reads++;
 switch(a) {
 case 0x10006304:*v=0xdu;break;
 case 0x1000616c:case 0x10006170:*v=2;break;
 case 0x1020e504:*v=1;break;
 case 0x10001228:case 0x10001724:*v=0;break;
 case 0x180601a0:*v=m->ap;break;
 case 0x180601a4:*v=m->wf|m->foreign_vote;break;
 case 0x18060184:*v=m->never_ack?1u<<5:0;break;
 case 0x18060000:*v=m->host_clock|6u;break;
 case 0x180602d4:*v=1;break;
 case 0x18072050:
  if(m->busy){*v=0;break;}
  assert(!m->sem);m->sem=1;m->sem_acquires++;*v=1;break;
 case 0x18001000:*v=0x20010101;break;
 case 0x18000010:*v=m->cpu;break;
 case 0x18000014:*v=m->bg_cpu;break;
 case 0x18000008:*v=m->bg_top;break;
 case 0x18003008:*v=m->afe;break;
 case 0x18000000:*v=m->top;break;
 case 0x180602cc:*v=m->power_ack;break;
 case 0x18001620:*v=m->wfprot;break;
 case 0x18001624:*v=m->dmaprot;break;
 case 0x1800e06c:*v=m->vdnr;break;
 case 0x184b0010:*v=m->never_id?0:0x20010000;break;
 case 0x1800f004:*v=m->idle;break;
 case 0x18000018:*v=m->sema_rst;break;
 case 0x18001c14:*v=m->emi;break;
 default:assert(0);
 }
 event(m,'r',a,*v);return 0;
}
static int write32(void *p,uint32_t a,uint32_t v) {
 struct mock *m=p;if(step(m))return -1;m->writes++;event(m,'w',a,v);
 switch(a) {
 case 0x18060000:m->host_clock=v;break;
 case 0x18072250:assert(v==1&&m->sem);m->sem=0;m->sem_releases++;break;
 case 0x180601a0:m->ap=v;if(v==0)m->slept=1;break;
 case 0x180601a4:m->wf=v;break;
 case 0x18000010:assert(!(v&1));m->cpu=v;break;
 case 0x18003008:assert(m->sem);m->afe=v;break;
 case 0x18000000:m->top=v;m->power_ack=(v&(1u<<7))?4:0;break;
 case 0x18001620:m->wfprot=(v&~8u)|((v&1u)?8u:0);break;
 case 0x18001624:m->dmaprot=(v&~8u)|((v&1u)?8u:0);break;
 case 0x1800e06c:m->vdnr=v;break;
 case 0x1800f004:m->idle=v;break;
 case 0x18000018:m->sema_rst=v;break;
 case 0x18001c14:m->emi=v;break;
 default:assert(0);
 }
 return 0;
}
static const struct nwsa_hw hw={owner,read32,write32,delay};
static unsigned position(const struct mock *m,uint32_t a,uint32_t val) {
 unsigned i;for(i=0;i<m->nevents;i++)if(m->events[i].op=='w'&&m->events[i].addr==a&&m->events[i].val==val)return i;
 assert(0);return 0;
}
int main(void) {
 struct nwsa a={0};struct mock m={0};unsigned i,total;
 assert(nwsa_start(&a,&hw,&m,0)<0&&!a.state.entered&&!m.calls);
 assert(!nwsa_start(&a,&hw,&m,1));
 assert(a.state.stage==NWS_ASLEEP&&a.state.balanced&&a.asleep);
 assert(a.state.local_id&&a.state.wfsys_id==0x20010000&&a.state.cfg_id==0x20010101);
 assert(a.state.cold_valid==0x1f&&a.state.cpu_held&&!(a.state.final_power&4));
 assert(a.clock_polls==6&&a.irq_last==1&&!a.sem20pending&&!a.emi_pending);
 assert(!a.state.ap_vote&&!a.state.wf_vote&&!a.state.clock_vote&&!a.state.power);
 assert(!a.state.sem20&&!m.sem&&m.sem_acquires==2&&m.sem_releases==2);
 assert(m.cpu==0&&m.ap==0&&m.wf==0&&m.afe==0&&!(m.top&(1u<<7)));
 assert(position(&m,0x180601a0,1)<position(&m,0x180601a4,1));
 assert(position(&m,0x18003008,1u<<21)<position(&m,0x18003008,(1u<<20)|(1u<<21)));
 assert(position(&m,0x18000000,0x57460080)<position(&m,0x18001620,0));
 assert(position(&m,0x18001620,1)<position(&m,0x18001624,1));
 assert(position(&m,0x18003008,1u<<21)<position(&m,0x18000000,0x57460000));
 assert(position(&m,0x180601a4,0)<position(&m,0x180601a0,0));
 total=m.calls;assert(nwsa_start(&a,&hw,&m,1)<0&&m.calls==total);

 /* Every callback boundary in the successful path refuses without continuing
  * into another uncertain operation. Pre-write faults may be clean; all later
  * failures retain the old common owner and require manual reset. */
 for(i=1;i<=total;i++) {
  struct nwsa b={0};struct mock q={.fault_at=i};int ret=nwsa_start(&b,&hw,&q,1);
  assert(ret<0&&b.state.entered&&!b.state.balanced&&q.calls<=i);
  if(q.writes||b.state.ap_vote)assert(b.state.halt&&b.state.unverified);
 }
 {struct nwsa b={0};struct mock q={.foreign_vote=1};
  assert(nwsa_start(&b,&hw,&q,1)<0&&b.state.halt&&b.state.ap_vote&&!b.state.wf_vote);
 }
 {struct nwsa b={0};struct mock q={.busy=1};
  assert(nwsa_start(&b,&hw,&q,1)<0&&b.state.sem_tries==1000&&b.state.halt);
 }
 {struct nwsa b={0};struct mock q={.never_ack=1};
  assert(nwsa_start(&b,&hw,&q,1)<0&&b.state.ap_ack_polls==12&&b.state.halt);
 }
 {struct nwsa b={0};struct mock q={.never_id=1};
  assert(nwsa_start(&b,&hw,&q,1)<0&&b.state.id_polls==12&&b.state.halt&&q.cpu==0);
 }
 {struct nwsa b={0};struct mock q={.top=0x80};
  assert(nwsa_start(&b,&hw,&q,1)<0&&b.state.cold_valid==3&&b.state.halt&&
         !b.state.clock_vote&&!b.state.power&&q.afe==0);
 }
 {struct nwsa b={0};struct mock q={.bg_cpu=1};
  assert(nwsa_start(&b,&hw,&q,1)<0&&b.state.cold_valid==15&&b.state.halt&&
         !b.state.clock_vote&&!b.state.power);
 }
 {struct nwsa b={0};struct mock q={.afe=1u<<20};
  assert(nwsa_start(&b,&hw,&q,1)<0&&b.state.sem20&&b.sem20owned&&
         b.state.halt&&!b.state.power);
 }
 printf("wfsys candidate: PASS (%u callback fault positions)\n",total);return 0;
}
