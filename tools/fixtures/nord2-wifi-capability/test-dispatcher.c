#include "wifi_capability_dispatcher.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define BASE NDP_POOL_BASE
static uint8_t memory[0x20000];
static struct nwi_state init;
static struct nfp_native firmware;
static struct npp_state pool;
struct mock {unsigned calls,fail,failed,after,writes,txidx,rxidx,events,rxpub,coherent,timeout,mode,synctx,syncdesc,clock_mode,last_way,last_bytes,frame_copied;uint64_t last_address;uint64_t now;};
static int step(struct mock*m){if(m->failed)m->after++;if(++m->calls==m->fail){m->failed=1;return -1;}m->now+=2;return 0;}
static uint8_t*ptr(uint64_t a,unsigned n){assert(a>=BASE&&a-BASE+n<=sizeof(memory));return memory+(size_t)(a-BASE);}
static uint32_t get(const uint8_t*p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static void put(uint8_t*p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(v>>(8*i));}
static int owner(void*c){return step(c);}
static int now(void*c,uint64_t*v){struct mock*m=c;if(step(m))return -1;*v=m->now;if(m->calls>40&&m->clock_mode)*v=m->clock_mode==1?0:m->now+NCD_DEADLINE_US;return 0;}
static int rr(void*c,uint32_t a,uint32_t*v){struct mock*m=c;if(step(m))return -1;*v=0;
 switch(a){case 0x180600f0:*v=3;break;case 0x18025208:*v=5;break;
 case 0x18025418:*v=m->txidx;break;case 0x1802541c:*v=m->timeout?3:m->txidx;break;
 case 0x1802550c:*v=m->rxidx;break;case 0x18024200:case 0x18025200:*v=(m->coherent==1||(m->coherent==2&&m->frame_copied))?0x100000:0;break;default:assert(0);}
 return 0;}
static unsigned diagnostic_fault;
static void event(struct mock*m,unsigned ring,unsigned kind){
 struct nwt_state*t=&init.transport;uint8_t*d=ptr(t->rx_ring+16*ring,16),*p=ptr(t->rx_buffers+4096*ring,64);unsigned bytes=kind?56:48;if(m->mode>=100)bytes=504;if(diagnostic_fault==3)bytes=39;
 memset(p,0,64);p[3]=0x38;p[27]=0xe0;p[28]=kind==0?0x27:0xec;p[29]=t->next_sequence-(kind==2);
 if(kind==0){p[38]=3;p[44]=0;}
 else {p[36]=1;put(p+40,7);put(p+44,8);p[48]=2;p[49]=4;}
 if(m->mode==10)p[2]=1;
 if(m->mode==3)p[28]=7;
 if(m->mode==4)p[28]=0xf0;
 if(m->mode==5){p[28]=0x27;p[38]=3;p[44]=0xff;p[45]=0xff;}
 if(m->mode==7){p[28]=0xec;p[29]=t->next_sequence;p[36]=1;put(p+44,0xffffffffu);}
 if(m->mode>=100){p[0]=504&255;p[1]=504>>8;p[24]=480&255;p[25]=480>>8;p[26]=(m->mode-100)&255;p[27]=(m->mode-100)>>8;}
 if(diagnostic_fault==1)p[2]=1;
 if(diagnostic_fault==2)p[28]=0xed;
 if(diagnostic_fault==4)put(p+44,0xffffffffu);
 if(diagnostic_fault==5){p[36]=2;put(p+56,7);put(p+60,8);}
 if(diagnostic_fault==7){p[28]=0x27;p[38]=2;}
 if(diagnostic_fault==8)p[29]=3;
 put(d+4,(bytes<<16)|0xc0000000u);if(diagnostic_fault==6)put(d+8,1);if(m->mode==8)put(d+8,1);if(m->mode==9)put(d+4,(bytes<<16)|0x80000000u);
}
static int rw(void*c,uint32_t a,uint32_t v){struct mock*m=c;int error=step(m);m->writes++;
 if(a==0x18025418){assert(m->synctx&&m->syncdesc);unsigned previous=m->txidx;m->txidx=v;uint8_t*d=ptr(init.transport.tx_ring+16*previous,16);put(d+4,(get(d+4)|0x80000000u)&(m->mode==6?~0x40000000u:0xffffffffu));
  if(m->mode==11||m->mode==12)put(d+4,0x80000000u);
  if(m->mode==13)put(d+4,0x80010000u);
  if(m->mode==14)put(d+4,0xc0000000u);
  if(m->mode==15){put(d+4,0x80000000u);put(d,0);}
  if(m->mode==16){put(d+4,0x80000000u);put(d+8,1);}
  if(m->mode==17){put(d+4,0x80000000u);put(d+12,1);}
  unsigned count=m->mode>=100?1:m->mode==1?1:m->mode==2?15:3;m->events=count;
  for(unsigned k=0;k<count;k++)event(m,(3+k)%16,m->mode>=100?1:m->mode==1?1:m->mode==2?0:k==0?0:k==1?2:1);
  m->rxidx=(3+count)%16;
 }else {assert(a==0x18025508);m->rxpub++;if(m->mode==2&&m->rxpub==1){event(m,2,0);m->rxidx=3;}assert(v==(3+m->rxpub-1)%16);}
 return error;}
static int mr(void*c,uint64_t a,void*p,uint32_t n){struct mock*m=c;if(step(m))return -1;assert(m->last_address==a&&m->last_bytes==n&&m->last_way==2);memcpy(p,ptr(a,n),n);if(m->mode==12&&m->txidx==4&&a==init.transport.tx_ring+48&&!m->frame_copied){memset(p,0,16);m->frame_copied=2;}if(a>=init.transport.rx_buffers)m->frame_copied=1;m->last_way=0;return 0;}
static int mw(void*c,uint64_t a,const void*p,uint32_t n){struct mock*m=c;int e=step(m);m->writes++;memcpy(ptr(a,n),p,n);return e;}
static int sy(void*c,uint64_t a,uint32_t n,unsigned way){struct mock*m=c;if(step(m))return -1;(void)ptr(a,n);assert(way==1||way==2);m->last_address=a;m->last_bytes=n;m->last_way=way;if(way==1){if(a==init.transport.command)m->synctx=1;if(a==init.transport.tx_ring+16*init.transport.tx_index)m->syncdesc=1;}return 0;}
static int delay(void*c,unsigned us){struct mock*m=c;assert(us==1000);if(step(m))return -1;m->now+=us;return 0;}
static const struct nwt_io bus={owner,rr,rw,mr,mw,sy,delay};
static const struct ncd_io io={&bus,now};
static void fixture(void){memset(memory,0,sizeof(memory));pool=(struct npp_state){.stage=NPP_DONE,.entered=1,.claim_ready=1,.live_valid=31,.preflight_complete=1,.verified=1,.mapped=1,.zero_complete=1,.balanced=1,.asleep=1,.words_written=32768,.words_read=32768,.set_attempted=3,.set_accepted=2,.raw_valid=3};struct ndp_claim claim={BASE,NDP_POOL_BYTES,3,1};struct ndp_subpool layout;assert(!ndp_subpool_layout(&claim,&layout));
 init=(struct nwi_state){.claim=claim,.pool=&pool,.prepared=1,.entered=1,.initialized=1,.phase=5,.dirty=1,.driver_owned=1};init.scheduler.complete=1;init.scheduler.operations=99;init.transport=(struct nwt_state){.entered=1,.dirty=1,.preflight_complete=1,.phase=2,.completed=3,.finish_phase=2,.tx_index=3,.rx_index=3,.next_sequence=4,.tx_ring=layout.tx_desc,.rx_ring=layout.rx_desc,.rx_buffers=layout.rx_events,.command=layout.tx_cmd};
 for(unsigned k=0;k<1024;k++)put(ptr(layout.tx_desc+16*k+4,4),0x80000000u);
 for(unsigned k=0;k<16;k++)assert(!nwt_rx_desc(ptr(layout.rx_desc+16*k,16),layout.rx_events+4096*k,3840));
 firmware=(struct nfp_native){.attempted=1,.prepared=1,.coex_done=1,.finish_attempted=1,.finish_done=1,.ready=1,.ready_raw=3,.ready_polls=1};firmware.state=(struct nfp_state){.entered=1,.dirty=1,.stage=NFP_COPIED,.completed=127,.next=7};}
int main(void){struct ncd_state s={0};struct mock m={.txidx=3,.rxidx=3};fixture();assert(ncd_run(&io,&m,&init,&firmware,&s,0)<0&&!m.calls);assert(!ncd_run(&io,&m,&init,&firmware,&s,1)&&s.completed&&s.events==2&&s.debug_events==1&&!s.sequence_echo&&s.sequence_mismatches==1&&s.rearmed==2&&s.capability.mac_valid&&init.transport.completed==4&&init.transport.finish_phase==2&&init.transport.rx_index==5);unsigned total=m.calls,old=m.calls;assert(ncd_run(&io,&m,&init,&firmware,&s,1)<0&&m.calls==old);
 for(unsigned fault=1;fault<=total;fault++){fixture();s=(struct ncd_state){0};m=(struct mock){.txidx=3,.rxidx=3,.fail=fault};assert(ncd_run(&io,&m,&init,&firmware,&s,1)<0&&s.halt&&init.halt&&init.transport.halt&&!m.after);old=m.calls;assert(ncd_run(&io,&m,&init,&firmware,&s,1)<0&&m.calls==old);}
 fixture();s=(struct ncd_state){0};m=(struct mock){.txidx=3,.rxidx=3,.mode=1};assert(!ncd_run(&io,&m,&init,&firmware,&s,1)&&s.events==1&&s.sequence_echo&&!s.sequence_mismatches);
 /* Unsolicited sleepy hints may be consumed, but cannot satisfy capability. */
 fixture();s=(struct ncd_state){0};m=(struct mock){.txidx=3,.rxidx=3,.mode=3};assert(ncd_run(&io,&m,&init,&firmware,&s,1)<0&&s.halt&&!s.completed&&!s.matched&&s.events==3&&s.rearmed==3&&m.rxpub==3&&init.transport.completed==3);
 for(unsigned k=0;k<s.events;k++)assert(s.history[k].kind==3&&s.history[k].eid==7);
 for(unsigned mode=4;mode<=5;mode++){fixture();s=(struct ncd_state){0};m=(struct mock){.txidx=3,.rxidx=3,.mode=mode};assert(ncd_run(&io,&m,&init,&firmware,&s,1)<0&&s.halt&&!m.rxpub);}
 fixture();s=(struct ncd_state){0};m=(struct mock){.txidx=3,.rxidx=3,.coherent=1};assert(ncd_run(&io,&m,&init,&firmware,&s,1)<0&&!m.writes&&s.error==10);
 for(unsigned mode=6;mode<=10;mode++){fixture();s=(struct ncd_state){0};m=(struct mock){.txidx=3,.rxidx=3,.mode=mode};assert(ncd_run(&io,&m,&init,&firmware,&s,1)<0&&s.halt&&!m.rxpub);}
 fixture();s=(struct ncd_state){0};m=(struct mock){.txidx=3,.rxidx=3,.mode=1,.coherent=2};assert(ncd_run(&io,&m,&init,&firmware,&s,1)<0&&s.error==10&&s.capability.decoded&&!s.completed&&!m.rxpub);
 fixture();s=(struct ncd_state){0};m=(struct mock){.txidx=3,.rxidx=3,.timeout=1};assert(ncd_run(&io,&m,&init,&firmware,&s,1)<0&&s.halt&&s.last<=s.deadline&&s.polls<=3000);
 fixture();s=(struct ncd_state){0};m=(struct mock){.txidx=3,.rxidx=3,.mode=2};assert(ncd_run(&io,&m,&init,&firmware,&s,1)<0&&s.events==16&&s.rearmed==16&&s.error==26);
 for(unsigned cm=1;cm<=2;cm++){fixture();s=(struct ncd_state){0};m=(struct mock){.txidx=3,.rxidx=3,.clock_mode=cm};assert(ncd_run(&io,&m,&init,&firmware,&s,1)<0&&s.error==3&&s.halt);}
 fixture();s=(struct ncd_state){0};firmware.ready=0;m=(struct mock){.txidx=3,.rxidx=3};assert(ncd_run(&io,&m,&init,&firmware,&s,1)<0&&!m.calls);
 for(unsigned mutation=0;mutation<4;mutation++){
  fixture();s=(struct ncd_state){0};m=(struct mock){.txidx=3,.rxidx=3};
  if(mutation==0)pool.raw[1][0]=1;
  if(mutation==1)init.transport.command=BASE+NDP_POOL_BYTES;
  if(mutation==2)init.transport.completed=4;
  if(mutation==3)firmware.state.completed=0x3f;
  assert(ncd_run(&io,&m,&init,&firmware,&s,1)<0&&!m.calls&&!s.entered);
 }
 for(unsigned mode=11;mode<=17;mode++){
  fixture();s=(struct ncd_state){0};m=(struct mock){.txidx=3,.rxidx=3,.mode=mode};
  int rc=ncd_run(&io,&m,&init,&firmware,&s,1);
  if(mode<=12)assert(!rc&&s.completed&&s.tx_done&&init.transport.command==BASE+0x5000);
  else assert(rc<0&&s.error==17&&s.halt&&!m.rxpub&&!s.events);
 }
 printf("PASS capability dispatcher: %u callback fault positions, one submit, debug before EID match, captured sequence echo policy, exact receipt/default-off, memory visibility, no replay/after-error calls, unsupported ownership/assert/malformed events, coherent failure and shared deadline. Synthetic model only.\n",total);
 return 0;
}
