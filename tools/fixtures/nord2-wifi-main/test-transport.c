#include "wifi_main_transport.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define BASE 0x90000000u
static unsigned char memory[0x20000];
struct mock {unsigned calls,fail,failed,after,writes,reads,txidx,rxidx,command_sync,desc_sync,event_sync,timeout,badseq,badddone,active,response_bytes,eid,coherent,copied,partial,descriptor_reads,corrupt_word,wrong_didx,control_mode,rx_corrupt,rx_range,rx_wait,rx_bad_done,packet_type,badcid,badstatus;};
static int step(struct mock *m){if(m->failed)m->after++;if(++m->calls==m->fail){m->failed=1;return -1;}return 0;}
static uint32_t get(const unsigned char *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static void put(unsigned char *p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(unsigned char)(v>>(8*i));}
static unsigned char *addr(uint64_t a,unsigned n){assert(a>=BASE&&a-BASE+n<=sizeof(memory));return memory+(size_t)(a-BASE);}
static int owner(void *c){return step(c);}
static int rr(void *c,uint32_t a,uint32_t *v){struct mock *m=c;if(step(m))return -1;m->reads++;*v=0;
 if(a==0x18024200||a==0x18025200){if(m->coherent==1||(m->coherent==2&&m->txidx)||(m->coherent==3&&m->copied))*v=0x200000;}
 else if(a==0x18025208)*v=m->active?5:0;
 else if(a==0x18024400&&m->active==2)*v=1;
 else if(a==0x18025418)*v=m->txidx;
 else if(a==0x1802541c)*v=m->wrong_didx?m->txidx+1:m->timeout?m->txidx-1:m->txidx;
 else if(a==0x1802550c)*v=m->rx_range?16:m->rx_wait?0:m->rxidx;
 return 0;}
static int rw(void *c,uint32_t a,uint32_t v){struct mock *m=c;int e=step(m);m->writes++;
 if(a==0x18025418){assert(m->command_sync&&m->desc_sync);unsigned old=m->txidx;m->txidx=v;
  unsigned char *d=addr(BASE+16*old,16);if(!m->badddone)put(d+4,get(d+4)|(1u<<31));
  unsigned char *rx=addr(BASE+0x4000+16*m->rxidx,16);unsigned bytes=m->response_bytes?m->response_bytes:36;put(rx+4,(bytes<<16)|(3u<<30));
  unsigned char *f=addr(BASE+0x5000+4096*m->rxidx,bytes);memset(f,0,bytes);f[26]=(uint8_t)m->packet_type;f[27]=(uint8_t)(m->packet_type?m->packet_type>>8:0xe0);f[28]=m->eid?m->eid:1;f[29]=addr(BASE+0x15000,72)[39]+m->badseq;if(bytes>33){f[32]=(uint8_t)m->badstatus;f[33]=(uint8_t)(addr(BASE+0x15000,72)[36]+m->badcid);}
  if(m->rx_corrupt)put(rx,get(rx)^4u);
  if(m->rx_bad_done)put(rx+4,get(rx+4)&~(1u<<31));
  m->rxidx=(m->rxidx+1)%16;
 }else assert(a==0x18025508);return e;}
static int mr(void *c,uint64_t a,void *p,uint32_t n){struct mock *m=c;if(step(m))return -1;assert(m->event_sync);memcpy(p,addr(a,n),n);
 if(a<BASE+0x4000&&n==16){
  m->descriptor_reads++;
  if(m->partial&&m->descriptor_reads==1)memset(p,0,16);
  else if(m->control_mode){unsigned char *d=p;uint32_t value=0;
   if(m->control_mode==1)value=0x80000000u;
   if(m->control_mode==2)value=0xc0000000u;
   if(m->control_mode==3)value=0x80008000u;
   if(m->control_mode==4)value=0xc0010000u;
   if(m->control_mode==5)value=0x80440000u;
   if(m->control_mode==6)value=0;
   if(m->control_mode==7)value=0x80000001u;
   if(m->control_mode==8)value=0x80004000u;
   put(d+4,value);
  }
  if(m->corrupt_word){unsigned char *d=p;
   if(m->corrupt_word==1)put(d,get(d)^4u);
   if(m->corrupt_word==2)put(d+4,get(d+4)^(1u<<16));
   if(m->corrupt_word==3)put(d+8,4);
   if(m->corrupt_word==4)put(d+12,1);
  }
 }
 if(a>=BASE+0x5000&&a<BASE+0x15000)m->copied=1;
 return 0;}
static int mw(void *c,uint64_t a,const void *p,uint32_t n){struct mock *m=c;int e=step(m);m->writes++;memcpy(addr(a,n),p,n);return e;}
static int syncmem(void *c,uint64_t a,uint32_t n,unsigned way){struct mock *m=c;if(step(m))return -1;(void)addr(a,n);if(way==1){if(a==BASE+0x15000)m->command_sync=1;else m->desc_sync=1;}else {assert(way==2);m->event_sync=1;}return 0;}
static int us(void *c,unsigned n){assert(n==1000);return step(c);}
static const struct nwt_io io={owner,rr,rw,mr,mw,syncmem,us};
/* Explicit host-only completed-initializer model. Productionpreflight can NOT
 * mintthisstate; tests do NOT claim DMASHDL/nativeinit implementation. */
static struct nwt_state seeded(void){struct nwt_state s={.entered=1,.preflight_complete=1,.phase=2,.tx_ring=BASE,.rx_ring=BASE+0x4000,.rx_buffers=BASE+0x5000,.command=BASE+0x15000,.next_sequence=1};memset(memory,0,sizeof(memory));for(unsigned i=0;i<16;i++)assert(!nwt_rx_desc(addr(s.rx_ring+16*i,16),s.rx_buffers+4096*i,3840));return s;}
int main(void){struct nwt_state s={0};struct mock m={0};assert(nwt_preflight(&io,&m,&s,0)&&!m.calls);assert(nwt_preflight(&io,&m,&s,1)&&s.preflight_complete&&s.missing_init==63&&s.error==20&&!m.writes&&s.ring_reads==108);unsigned total=m.calls;
 for(unsigned f=1;f<=total;f++){s=(struct nwt_state){0};m=(struct mock){.fail=f};assert(nwt_preflight(&io,&m,&s,1)&&!m.after&&!m.writes&&!s.preflight_complete);}
 s=(struct nwt_state){0};m=(struct mock){.active=1};assert(nwt_preflight(&io,&m,&s,1)&&s.error==10&&!m.writes);
 s=(struct nwt_state){0};m=(struct mock){.active=2};assert(nwt_preflight(&io,&m,&s,1)&&!m.writes);
 s=seeded();m=(struct mock){.active=1};assert(!nwt_exchange(&io,&m,&s,0x40)&&s.completed==1&&s.tx_index==1&&s.rx_index==1);unsigned exchange=m.calls;assert(exchange==52);assert(!nwt_exchange(&io,&m,&s,0x41)&&s.completed==2);unsigned before=m.calls;assert(nwt_exchange(&io,&m,&s,0x41)&&m.calls==before);
 for(unsigned f=1;f<=exchange;f++){s=seeded();m=(struct mock){.active=1,.fail=f};assert(nwt_exchange(&io,&m,&s,0x40)&&!m.after);if(s.dirty)assert(s.halt);before=m.calls;if(s.halt)assert(nwt_exchange(&io,&m,&s,0x40)&&m.calls==before);}
 s=seeded();m=(struct mock){.active=1,.timeout=1};assert(nwt_exchange(&io,&m,&s,0x40)&&s.halt&&s.polls==1000);
 s=seeded();m=(struct mock){.active=1,.badddone=1};assert(nwt_exchange(&io,&m,&s,0x40)&&s.halt&&s.polls==1000);
 s=seeded();m=(struct mock){.active=1,.badseq=1};assert(nwt_exchange(&io,&m,&s,0x40)&&s.halt&&!s.completed);
 s=seeded();s.command=s.tx_ring;m=(struct mock){.active=1};assert(nwt_exchange(&io,&m,&s,0x40)&&!m.calls);
 s=seeded();m=(struct mock){.active=1,.response_bytes=60,.eid=5};uint8_t phy[464]={0},event[3840];size_t captured=0;phy[36]=8;phy[37]=0xa0;phy[39]=s.next_sequence;
 assert(!nwt_capture(&io,&m,&s,phy,sizeof(phy),5,event,sizeof(event),&captured)&&captured==60&&s.rx_descriptor_bytes==60&&s.rx_mac_bytes==0&&s.rx_init_bytes==0);
 s=seeded();m=(struct mock){.active=1,.response_bytes=60,.eid=5};phy[39]=s.next_sequence;assert(nwt_capture(&io,&m,&s,phy,sizeof(phy),5,event,36,&captured)&&s.halt&&!captured);
 s=seeded();m=(struct mock){.active=1};assert(!nwt_exchange(&io,&m,&s,0x40));m.response_bytes=60;m.eid=5;phy[39]=s.next_sequence;assert(!nwt_capture(&io,&m,&s,phy,sizeof(phy),5,event,sizeof(event),&captured));m.response_bytes=0;m.eid=0;assert(!nwt_exchange(&io,&m,&s,0x41)&&s.completed==3&&s.finish_phase==2);
 for(unsigned mode=1;mode<=3;mode++){s=seeded();m=(struct mock){.active=1,.coherent=mode};assert(nwt_exchange(&io,&m,&s,0x40)&&s.halt&&!s.completed&&s.error==24);if(mode==1)assert(!m.writes);}
 s=seeded();m=(struct mock){.active=1,.partial=1};assert(!nwt_exchange(&io,&m,&s,0x40)&&s.completed==1&&s.tx_ddone_waits==1&&s.tx_descriptor_valid==3&&!s.tx_mismatch&&s.tx_didx==1&&s.tx_used==68);
 assert(s.tx_submitted[0]==s.command&&s.tx_observed[0]==s.command&&(s.tx_observed[1]&(1u<<31)));
 for(unsigned word=1;word<=4;word++){
  s=seeded();m=(struct mock){.active=1,.corrupt_word=word};assert(nwt_exchange(&io,&m,&s,0x40)&&s.error==16&&s.halt&&!s.completed&&s.tx_mismatch==(1u<<(word-1))&&s.tx_descriptor_valid==3&&!s.rx_descriptor_bytes);
  before=m.calls;assert(nwt_exchange(&io,&m,&s,0x40)&&m.calls==before);
 }
 s=seeded();m=(struct mock){.active=1,.wrong_didx=1};assert(nwt_exchange(&io,&m,&s,0x40)&&s.error==15&&s.halt&&s.tx_descriptor_valid==1&&!m.descriptor_reads);
 s=seeded();m=(struct mock){.active=1,.control_mode=1};assert(!nwt_exchange(&io,&m,&s,0x40)&&s.tx_consumed_zero&&s.tx_used==68&&s.completed==1&&!s.tx_mismatch&&s.tx_observed[1]==0x80000000u&&s.command==BASE+0x15000);
 for(unsigned word=1;word<=4;word++)if(word!=2){
  s=seeded();m=(struct mock){.active=1,.control_mode=1,.corrupt_word=word};assert(nwt_exchange(&io,&m,&s,0x40)&&s.error==16&&s.tx_mismatch==(1u<<(word-1))&&!s.tx_consumed_zero&&!s.rx_descriptor_bytes);
 }
 for(unsigned mode=2;mode<=4;mode++){
  s=seeded();m=(struct mock){.active=1,.control_mode=mode};assert(nwt_exchange(&io,&m,&s,0x40)&&s.error==16&&s.halt&&!s.completed&&!s.tx_consumed_zero&&!s.rx_descriptor_bytes&&s.tx_mismatch==(mode==4?2u:16u));
  before=m.calls;assert(nwt_exchange(&io,&m,&s,0x40)&&m.calls==before);
 }
 for(unsigned mode=7;mode<=8;mode++){
  s=seeded();m=(struct mock){.active=1,.control_mode=mode};assert(nwt_exchange(&io,&m,&s,0x40)&&s.error==16&&s.tx_mismatch==16&&!s.tx_consumed_zero&&!s.rx_descriptor_bytes);
 }
 s=seeded();m=(struct mock){.active=1,.control_mode=1,.badseq=1};assert(nwt_exchange(&io,&m,&s,0x40)&&s.error==21&&s.tx_consumed_zero&&!s.completed&&!s.finish_phase&&s.halt);
 s=seeded();m=(struct mock){.active=1,.control_mode=5};assert(!nwt_exchange(&io,&m,&s,0x40)&&!s.tx_consumed_zero&&s.tx_used==68);/* Original-length branch retains previous flag policy. */
 s=seeded();m=(struct mock){.active=1,.control_mode=6};assert(nwt_exchange(&io,&m,&s,0x40)&&s.error==17&&s.polls==1000&&!s.tx_consumed_zero&&!s.completed&&!s.rx_descriptor_bytes);
 s=seeded();m=(struct mock){.active=1};assert(!nwt_exchange(&io,&m,&s,0x40)&&s.rx_diag_valid==31&&s.rx_failure_reason==0&&s.rx_packet_type==0xe000&&s.rx_expected_eid==1&&s.rx_event_eid==1&&s.rx_event_sequence==1&&s.rx_cid==0x40&&s.rx_status==0&&s.rx_descriptor_raw[0]==s.rx_expected_buffer);
 s=seeded();m=(struct mock){.active=1,.badseq=1};assert(nwt_exchange(&io,&m,&s,0x40)&&s.rx_failure_reason==3&&s.rx_event_sequence==2&&s.rx_diag_valid==31);
 s=seeded();m=(struct mock){.active=1,.response_bytes=32};assert(nwt_exchange(&io,&m,&s,0x40)&&s.rx_diag_valid==15);/* status/CID not copied */
 s=seeded();m=(struct mock){.active=1,.response_bytes=60,.eid=5};phy[39]=s.next_sequence;assert(!nwt_capture(&io,&m,&s,phy,sizeof(phy),5,event,sizeof(event),&captured)&&s.rx_diag_valid==15&&!s.rx_status&&!s.rx_cid);/* PHY bytes never mislabelled CMD status */
 s=seeded();m=(struct mock){.active=1,.rx_corrupt=1};assert(nwt_exchange(&io,&m,&s,0x40)&&s.error==19&&s.rx_failure_reason==2&&s.rx_diag_valid==7&&s.rx_descriptor_raw[0]!=s.rx_expected_buffer&&!s.rx_packet_type);
 s=seeded();m=(struct mock){.active=1,.rx_range=1};assert(nwt_exchange(&io,&m,&s,0x40)&&s.rx_failure_reason==1&&s.rx_didx==16&&s.rx_diag_valid==3);
 s=seeded();m=(struct mock){.active=1,.rx_wait=1};assert(nwt_exchange(&io,&m,&s,0x40)&&s.rx_failure_reason==4&&s.rx_diag_valid==3);
 s=seeded();m=(struct mock){.active=1,.rx_bad_done=1};assert(nwt_exchange(&io,&m,&s,0x40)&&s.rx_failure_reason==4&&s.rx_diag_valid==7&&!(s.rx_descriptor_raw[1]&(1u<<31)));
 s=seeded();m=(struct mock){.active=1,.coherent=3};assert(nwt_exchange(&io,&m,&s,0x40)&&s.rx_failure_reason==5&&s.error==24);
 s=seeded();m=(struct mock){.active=1};assert(nwt_exchange(&io,&m,&s,0x42)==-1&&!m.calls);
 s=seeded();m=(struct mock){.active=1,.packet_type=7,.eid=5};assert(nwt_exchange(&io,&m,&s,0x40)==-1&&s.rx_failure_reason==3&&!s.completed);
 s=seeded();m=(struct mock){.active=1,.packet_type=7,.badcid=1};assert(!nwt_exchange(&io,&m,&s,0x40)&&s.completed==1&&s.finish_phase==1&&s.rx_packet_type==7&&s.rx_diag_valid==31&&s.rx_status==0&&s.rx_cid==0x41);
 s=seeded();m=(struct mock){.active=1,.packet_type=7,.badstatus=1};assert(nwt_exchange(&io,&m,&s,0x40)&&s.halt&&!s.finish_phase&&s.rx_status==1);
 s=seeded();m=(struct mock){.active=1,.packet_type=7,.badseq=1};assert(nwt_exchange(&io,&m,&s,0x40)&&s.error==21&&!s.completed&&s.rx_failure_reason==3);
 s=seeded();m=(struct mock){.active=1,.packet_type=8};assert(nwt_exchange(&io,&m,&s,0x40)&&s.error==21&&!s.completed&&s.rx_failure_reason==3);
 s=seeded();m=(struct mock){.active=1,.packet_type=0xe007};assert(nwt_exchange(&io,&m,&s,0x40)&&s.error==21&&!s.completed);
 puts("PASS INIT format: exact measured0007 and legacye000 only; EID/sequence/status mandatory; CID metadata captured but not an unsupported ACK-echo requirement.");
 puts("PASS RX diagnostics: same52 success callbacks, copied-only descriptor/header metadata, status/CID only EID1+34B, PHY payload excluded, exact failure checkpoint and validity; no extra backend operation.");
 puts("PASS consumed zero: exact observed DDONE-only accepts with software length68 retained; zero+LS0, zero+Burst, other length reject; original-length policy unchanged; DDONE0 never accepts. Synthetic event only.");
 puts("PASS delta: invalid partial descriptor with DDONE0 waits; each DDONE1 pointer/length/DW2/DW3 corruption still fails; wrong DIDX/coherent stop retained; captured raw words and mismatch bits; no replay or RX rearm on error.");
 printf("PASS transport: %u legacy preflight faults; %u exchange callback faults; two finish commands; PHY-sized generic capture; bounded raw counts; no replay; TX DIDX plus DDONE; RX matching event; visibility; 1000-poll timeout. Host callback model only.\n",total,exchange);
 return 0;
}
