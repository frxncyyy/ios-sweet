#include "wifi_station_transport.h"
#include "wifi_station_source_layout.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BASE UINT64_C(0x80000000)
#define MEM_BYTES (UINT64_C(12)*1024*1024)
struct test {struct nst s;struct nwi_state init;struct nct_session prior;struct ncr_state rx;uint8_t*mem;uint64_t now;unsigned data_cidx,data_didx,defer_token;unsigned cidx,didx,rxidx[5],calls,fail_at,defer_dma,defer_air,defer_sta,defer_key;unsigned events[32],last_status,published;uint8_t capture[1600];size_t capture_n;};
static unsigned checks;
#define C(v) do{checks++;assert(v);}while(0)
static uint32_t get32(const uint8_t*p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static void put16(uint8_t*p,unsigned v){p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);}
static void put32(uint8_t*p,uint32_t v){put16(p,v);put16(p+2,v>>16);}
static uint8_t* at(struct test*t,uint64_t a,size_t n){assert(a>=BASE&&a-BASE<=MEM_BYTES&&n<=MEM_BYTES-(a-BASE));return t->mem+(a-BASE);}
static int tick(struct test*t){return ++t->calls==t->fail_at;}
static int owner(void*c){struct test*t=c;return tick(t)?-1:0;}
static int clock_us(void*c,uint64_t*n){struct test*t=c;if(tick(t))return -1;*n=t->now;return 0;}
static void deliver(struct test*t,unsigned k,const uint8_t*p,unsigned n){struct ncr_queue*q=&t->s.queue[k];unsigned slot=t->rxidx[k];uint8_t*d=at(t,q->ring.descriptors+16u*slot,16);put32(d,0xdeadbeef);put32(d+4,0xc0000000u|(n<<16));put32(d+8,0xffffffff);memcpy(at(t,q->ring.buffers+4096u*slot,n),p,n);t->rxidx[k]=(slot+1)%q->ring.count;}
static void event(struct test*t,uint8_t eid,uint8_t seq,const void*body,unsigned n){uint8_t p[256]={0};assert(n<=sizeof p-36);put16(p,n+36);put16(p+2,0x3800);put16(p+24,n+12);p[28]=eid;p[29]=seq;if(n)memcpy(p+36,body,n);deliver(t,1,p,n+36);}
static void txdone(struct test*t,unsigned pid,unsigned wlan,unsigned status){uint8_t b[88]={0};b[0]=(uint8_t)pid;b[1]=(uint8_t)status;b[4]=(uint8_t)wlan;event(t,15,0,b,sizeof b);}
static void consume(struct test*t){unsigned old=t->cidx;t->cidx=(old+1)%1024;if(!t->defer_dma){t->didx=t->cidx;uint8_t*d=at(t,t->init.transport.tx_ring+16u*old,16);put32(d+4,get32(d+4)|0x80000000);}}
static int readreg(void*c,uint32_t a,uint32_t*v){struct test*t=c;if(tick(t))return -1;if(a==0x180600f0){*v=3;return 0;}if(a==0x18024200||a==0x18025200){*v=0;return 0;}if(a==0x18025308){*v=t->data_cidx;return 0;}if(a==0x1802530c){*v=t->data_didx;return 0;}if(a==0x18025418){*v=t->cidx;return 0;}if(a==0x1802541c){*v=t->didx;return 0;}for(unsigned k=0;k<5;k++)if(a==t->s.queue[k].ring.base+12){*v=t->rxidx[k];return 0;}assert(0);return -1;}
static int writereg(void*c,uint32_t a,uint32_t v){struct test*t=c;if(tick(t))return -1;assert(nst_register_write(&t->s,a,v));if(a==0x18025308){C(v==(t->data_cidx+1)%1024);unsigned old=t->data_cidx;uint8_t*d=at(t,t->init.rings[0].descriptors+16u*old,16);C(((get32(d+4)>>16)&0x3fff)==64);uint8_t*h=at(t,get32(d),64);unsigned id=(h[32]|(unsigned)h[33]<<8)&32767u;C((h[33]&128)&&id<NSTX_TOKEN_COUNT);unsigned len=(h[44]|(unsigned)h[45]<<8)&4095u;C(h[45]&128);uint64_t payload=get32(h+40);t->capture_n=len+32;C(t->capture_n<=sizeof t->capture);memcpy(t->capture,h,32);memcpy(t->capture+32,at(t,payload,len),len);C(((get32(h)>>23)&3)==0);t->published++;t->data_cidx=v;if(!t->defer_dma){t->data_didx=v;put32(d+4,get32(d+4)|0x80000000);}if(!t->defer_token){uint8_t b[12]={0};put16(b,sizeof b);put16(b+2,(6u<<11)|1u);put32(b+4,2u<<16);put32(b+8,id|((unsigned)t->s.peer.wlan<<15)|(1u<<25));deliver(t,3,b,sizeof b);}return 0;}if(a==0x18025418){C(v==(t->cidx+1)%1024);t->published++;t->capture_n=t->s.wire_bytes;memcpy(t->capture,at(t,t->init.transport.command,t->capture_n),t->capture_n);consume(t);if(t->s.wire_type){if(!t->defer_air)txdone(t,t->s.wire_pid,t->s.wire_wlan,0);}else{const uint8_t*p=t->capture+64;unsigned cid=t->capture[36];if(cid==0x13&&p[20]==3&&p[51]&&!t->defer_sta){uint8_t b[8];memcpy(b,t->s.peer.ap,6);b[6]=t->s.peer.sta;b[7]=t->s.peer.bss;event(t,12,t->capture[39],b,8);}if(cid==0x1c&&!p[2]){uint8_t b[24]={0};b[0]=p[0];b[1]=p[1];b[3]=p[3];b[5]=p[5];put32(b+12,4000);event(t,16,0,b,24);}if(cid==7&&p[0]&&p[2]&&!t->defer_key){uint8_t b[8]={0};b[0]=p[10];memcpy(b+2,p+4,6);event(t,36,0,b,8);}}return 0;}for(unsigned k=0;k<5;k++)if(a==t->s.queue[k].ring.base+8){C(v==t->s.queue[k].next);return 0;}assert(0);return -1;}
static int mread(void*c,uint64_t a,void*p,uint32_t n){struct test*t=c;if(tick(t))return -1;C(nst_memory_range(&t->s,a,n,0));memcpy(p,at(t,a,n),n);return 0;}
static int mwrite(void*c,uint64_t a,const void*p,uint32_t n){struct test*t=c;if(tick(t))return -1;C(nst_memory_range(&t->s,a,n,1));memcpy(at(t,a,n),p,n);return 0;}
static int syncmem(void*c,uint64_t a,uint32_t n,unsigned way){struct test*t=c;if(tick(t))return -1;C(nst_memory_range(&t->s,a,n,way==1));return 0;}
static int delay(void*c,unsigned n){(void)c;(void)n;return -1;}
static const struct nwt_io bus={owner,readreg,writereg,mread,mwrite,syncmem,delay};static const struct ncd_io io={&bus,clock_us};
static int callback(void*c,const struct nst_event*e){struct test*t=c;C(t->s.phase==NST_IDLE);C(e->generation==17);C(e->kind<32);t->events[e->kind]++;t->last_status=e->status;return 0;}
static unsigned preserve_diag_boot;
static void setup(struct test*t){if(!preserve_diag_boot)nsdd_test_boot_reset();memset(t,0,sizeof *t);t->mem=calloc(1,MEM_BYTES);assert(t->mem);t->now=1000;struct nwi_state*i=&t->init;i->initialized=1;i->claim=(struct ndp_claim){.base=BASE,.bytes=MEM_BYTES,.source=3,.valid=1};i->data_buffer=BASE+MEM_BYTES-4096;i->end=i->data_buffer+4096;i->rings[0]=(struct nwi_ring){.descriptors=BASE+0xb00000,.base=0x18025300,.count=1024};for(unsigned z=0;z<1024;z++)put32(at(t,i->rings[0].descriptors+16u*z+4,4),0x80000000);i->transport.tx_ring=BASE;i->transport.command=BASE+0x4000;i->transport.tx_index=1;i->transport.next_sequence=2;i->transport.completed=6;
 struct nct_session*p=&t->prior;p->init=i;p->attempted=p->prepared=p->entered=1;p->phase=NCT_DONE;p->count=p->cursor=p->completed=1;p->base_completed=5;p->capacity=p->available=4;p->acquired=p->released=1;p->first_sequence=1;p->entry[0]=(struct nct_entry){1252,3,1};p->record[0]=(struct nct_record){.cid=3,.sequence=1,.bytes=1252,.from=0,.to=1,.published=1,.credit_released=1,.tx_done=1};
 static const unsigned counts[5]={1024,16,1024,16,16};static const uint32_t bases[5]={0x18024500,0x18025500,0x18024510,0x18024520,0x18024530};uint64_t off=0x5000;for(unsigned k=0;k<5;k++){struct nwi_ring*r=&i->rings[k+4];r->descriptors=BASE+off;off+=16u*counts[k];r->buffers=BASE+off;off+=4096u*counts[k];r->base=bases[k];r->count=counts[k];r->rx=1;t->rx.queue[k].ring=*r;}assert(off<MEM_BYTES);
 t->rx.init=i;t->rx.session=p;t->rx.scan.entered=t->rx.scan.done=1;t->rx.scan.deadline=30000000;t->rx.scan.completion.state=7;struct ncs_cap cap={.bss=4,.wtbl=32,.bmc=4,.own_mac=1};struct nst_peer peer={.self={2,0,0,0,0,1},.ap={2,0,0,0,0,2},.bss=0,.own_mac=1,.sta=0,.wlan=5,.group_wlan=4,.band=1,.channel=6};struct sw_txd d={.wlan=5,.own_mac=1,.queue=1,.pid=1,.retry_limit=30,.fixed_rate=0,.pse_header_len=8,.lifetime_ms=2000};C(nst_prepare(&t->s,i,p,&t->rx,&cap,&peer,&d,17,t->now,t->now+30000000)==0);t->cidx=t->didx=1;for(unsigned k=0;k<1024;k++)put32(at(t,i->transport.tx_ring+16u*k+4,4),0x80000000);}
static void cleanup(struct test*t){free(t->mem);t->mem=NULL;}
static void step(struct test*t){C(nst_step(&t->s,&io,t,16,callback,t)==0);t->now+=1000;}
static void until(struct test*t,unsigned kind){for(unsigned k=0;k<30&&!t->events[kind];k++)step(t);C(t->events[kind]);}
static void association(uint8_t*p,struct test*t){memset(p,0,38);put16(p,0x10);memcpy(p+4,t->s.peer.self,6);memcpy(p+10,t->s.peer.ap,6);memcpy(p+16,t->s.peer.ap,6);put16(p+24,0x431);put16(p+28,0xc001);p[30]=1;p[31]=6;uint8_t r[6]={0x82,0x84,0x8b,0x96,96,108};memcpy(p+32,r,6);}
static void ethernet_rx(struct test*t,unsigned group,unsigned pn,unsigned error,unsigned plaintext){
 uint8_t p[56+60]={0};put16(p,sizeof p);put16(p+2,2u<<11);put32(p+4,((group?4u:5u))|(9u<<11)|(plaintext?0:(4u<<16))|(error?(1u<<25):0));put32(p+8,1u<<13);put32(p+12,6u<<8);put16(p+24,plaintext?0x0208:0x4208);memcpy(p+26,t->s.peer.ap,6);p[40]=(uint8_t)pn;uint8_t*m=p+56;if(group)memset(m,255,6);else memcpy(m,t->s.peer.self,6);m[6]=2;m[11]=7;m[12]=8;m[13]=0;deliver(t,0,p,sizeof p);
}
static void beacon(struct test*t){uint8_t p[66]={0};put16(p,sizeof p);put16(p+2,2u<<11);put32(p+4,t->s.peer.wlan);put32(p+8,12u<<8);uint8_t*m=p+24;put16(m,0x80);memset(m+4,255,6);memcpy(m+10,t->s.peer.ap,6);memcpy(m+16,t->s.peer.ap,6);m[36]=5;m[37]=4;m[39]=2;deliver(t,0,p,sizeof p);}
#ifndef NST_TEST_NO_MAIN
int main(void){struct test*t=calloc(1,sizeof *t);assert(t);setup(t);uint8_t rates[6]={0x82,0x84,0x8b,0x96,96,108};uint8_t ssid[3]={'l','a','b'};struct nst_profile profile={.assoc={.ssid=ssid,.ssid_len=3,.rates=rates,.rates_len=6,.capability=0x431,.listen_interval=10},.beacon_interval=100,.rcpi=100,.sta_version=1};memcpy(&profile.assoc.peer,&t->s.peer,12);
 uint8_t*short_frame=malloc(1);assert(short_frame);short_frame[0]=0xb0;C(nst_management(&t->s,1,short_frame,0)<0);C(nst_management(&t->s,1,short_frame,1)<0);free(short_frame);
 C(nst_peer_begin(&t->s,1,NST_PREPARE_PEER,&profile,NULL,0)==0);until(t,NST_PEER_PREPARED);C(t->published==4);C(t->capture[36]==0x13&&t->capture[64+SW_OFF_CMD_UPDATE_STA_RECORD_ucRtsPolicy]==3);C(t->s.negotiated_rates==0x300f);C(!t->s.sta_active&&!t->s.ptk_ready);
 uint8_t ch[24];size_t used;struct sw_channel channel={0,1,0,1,6,4,4000};C(!sw_channel(ch,sizeof ch,&used,&channel));C(!nst_command(&t->s,2,28,ch,used));until(t,NST_CHANNEL_GRANT);
 uint8_t auth[30];C(!sw_auth(auth,sizeof auth,&used,&profile.assoc.peer));t->defer_dma=1;C(!nst_management(&t->s,3,auth,used));step(t);C(t->s.air_done&&!t->s.dma_done&&!t->events[NST_FRAME_RESULT]);unsigned old=t->s.tx_next;put32(at(t,t->init.transport.tx_ring+16u*old+4,4),0xc0000000);t->didx=t->cidx;t->defer_dma=0;until(t,NST_FRAME_RESULT);C(!t->last_status);
 uint8_t response[38];association(response,t);t->defer_sta=1;C(!nst_peer_begin(&t->s,4,NST_CONFIGURE_PEER,NULL,response,sizeof response));for(unsigned k=0;k<8;k++)step(t);C(t->s.activation_pending&&!t->events[NST_PEER_CONFIGURED]);uint8_t active[8];memcpy(active,t->s.peer.ap,6);active[6]=0;active[7]=0;event(t,12,(uint8_t)(t->s.activation_sequence+1),active,8);step(t);C(!t->s.sta_active);event(t,12,(uint8_t)t->s.activation_sequence,active,8);until(t,NST_PEER_CONFIGURED);C(t->s.sta_active&&!t->s.channel_held&&t->events[NST_CHANNEL_RELEASED]);C(nst_authorize(&t->s)<0);
 uint8_t key[16]={0},body[64];struct sw_key k={.bss=0,.wlan=5,.pairwise=1,.key_id=0,.key=key,.key_len=16};memcpy(k.ap,t->s.peer.ap,6);C(!sw_key_add(body,sizeof body,&used,&k));C(!nst_command(&t->s,5,7,body,used));unsigned cmds=t->events[NST_COMMAND_DMA];while(t->events[NST_COMMAND_DMA]==cmds)step(t);C(t->s.ptk_ready&&t->events[NST_KEY_READY]);uint8_t rsc[8]={0};C(!nst_replay_install(&t->s,1,0,rsc));C(nst_replay_install(&t->s,1,0,rsc)<0);
 k.wlan=4;k.pairwise=0;k.key_id=0;C(!sw_key_add(body,sizeof body,&used,&k));C(!nst_command(&t->s,6,7,body,used));cmds=t->events[NST_COMMAND_DMA];while(t->events[NST_COMMAND_DMA]==cmds)step(t);rsc[0]=2;C(!nst_replay_install(&t->s,0,0,rsc));C(!nst_authorize(&t->s));for(unsigned x=0;x<128;x++)C(at(t,t->init.transport.command,128)[x]==0);C(t->s.controlled_port);
 /* Source PM connected indication is first-beacon-only, with no fake ACK. */
 unsigned pm_before=t->published;beacon(t);step(t);C(t->s.pm_scheduled&&!t->s.pm_connected&&t->s.wire_busy);
 t->defer_dma=1;step(t);C(t->published==pm_before+1&&!t->s.pm_connected);beacon(t);step(t);C(!t->s.pm_pending&&!t->s.pm_connected&&t->published==pm_before+1);
 unsigned pm_slot=t->s.tx_next;put32(at(t,t->init.transport.tx_ring+16u*pm_slot+4,4),0xc0000000);t->didx=t->cidx;t->defer_dma=0;step(t);C(t->s.pm_connected);
 beacon(t);step(t);step(t);C(!t->s.pm_pending&&t->published==pm_before+1);
 unsigned received=t->events[NST_ETHERNET];ethernet_rx(t,0,1,0,0);step(t);C(t->events[NST_ETHERNET]==++received);ethernet_rx(t,0,1,0,0);step(t);C(t->events[NST_ETHERNET]==received);ethernet_rx(t,0,2,1,0);step(t);C(t->events[NST_ETHERNET]==received);ethernet_rx(t,0,2,0,1);step(t);C(t->events[NST_ETHERNET]==received);ethernet_rx(t,1,2,0,0);step(t);C(t->events[NST_ETHERNET]==received);ethernet_rx(t,1,3,0,0);step(t);C(t->events[NST_ETHERNET]==++received);ethernet_rx(t,1,3,0,0);step(t);C(t->events[NST_ETHERNET]==received);
 /* Link loss on a connection older than initial join deadline must rearm its
  * RX descriptor and deliver closure, without silently disabling deadlines. */
 t->now+=UINT64_C(60000000);step(t);C(t->s.controlled_port);
 uint8_t absence[4]={0,1,0,0};event(t,0x11,0,absence,4);step(t);C(t->s.bss_absent);
 uint8_t paused[60]={0};memcpy(paused+6,t->s.peer.self,6);paused[12]=8;C(nst_data(&t->s,7,paused,sizeof paused)==1);
 absence[1]=0;event(t,0x11,0,absence,4);step(t);C(!t->s.bss_absent);
 uint8_t eth[60]={0};memcpy(eth+6,t->s.peer.self,6);eth[0]=255;eth[12]=8;eth[13]=0;C(!nst_data(&t->s,7,eth,sizeof eth));unsigned sent=t->published;nst_close_port(&t->s);C(!t->s.wire_busy);step(t);C(t->published==sent);C(nst_data(&t->s,8,eth,sizeof eth)==1);
 C(!nst_authorize(&t->s));t->now+=UINT64_C(60000000);
 uint8_t lost[4]={0,2,0,0};event(t,0x13,0,lost,4);step(t);C(t->events[NST_LINK_LOST]==1&&!t->s.controlled_port);
 C(t->s.deadline>t->now);C(nst_authorize(&t->s)<0);
 /* A real long-lived connection must receive a fresh finite cleanup lease. */
 t->now+=UINT64_C(60000000);uint64_t op=t->s.operation_deadline;
 C(nst_cleanup_begin(&t->s,t->now,t->now+UINT64_C(30000001))<0);
 C(!t->s.cleanup_started);C(!nst_cleanup_begin(&t->s,t->now,t->now+UINT64_C(30000000)));
 C(t->s.operation_deadline==op);C(nst_cleanup_begin(&t->s,t->now,t->now+UINT64_C(30000000))<0);
 C(!nst_peer_begin(&t->s,9,NST_RETIRE_PEER,NULL,NULL,0));until(t,NST_PEER_RETIRED);C(!t->s.sta_active&&!t->s.ptk.installed&&!t->s.gtk.installed);cleanup(t);
 /* Late source grant timeout under explicit cleanup is not DMA uncertainty.
  * Unpublished auth is retired as failure; real CID abort still publishes. */
 setup(t);C(!sw_channel(ch,sizeof ch,&used,&channel));C(!nst_command(&t->s,2,28,ch,used));until(t,NST_CHANNEL_GRANT);C(!nst_management(&t->s,3,auth,sizeof auth));
 t->now=t->s.grant_deadline+1000;C(!nst_cleanup_begin(&t->s,t->now,t->now+30000000));unsigned before_drop=t->published;step(t);C(t->published==before_drop&&t->events[NST_FRAME_RESULT]==1&&t->last_status==1);C(nst_management(&t->s,4,auth,sizeof auth)<0);
 channel.abort=1;C(!sw_channel(ch,sizeof ch,&used,&channel));C(!nst_command(&t->s,5,28,ch,used));until(t,NST_CHANNEL_RELEASED);C(!t->s.halt&&!t->s.channel_held);cleanup(t);
 /* Every observed hardware callback boundary is failure-injected separately. */
 setup(t);C(!nst_management(&t->s,1,auth,sizeof auth));step(t);unsigned calls=t->calls;cleanup(t);for(unsigned f=1;f<=calls;f++){setup(t);C(!nst_management(&t->s,1,auth,sizeof auth));t->fail_at=f;C(nst_step(&t->s,&io,t,16,callback,t)<0);C(t->s.halt&&!t->s.controlled_port);C(t->s.credit_available+t->s.credit_reserved==t->s.credit_capacity);C(t->s.credit_acquired-t->s.credit_released==t->s.credit_reserved);cleanup(t);}free(t);printf("PASS %u checks: actual CID/frame DMA publisher, peer recipes, early air/late STA, key readiness/replay, unsent data revoke and callback failure ownership\n",checks);return 0;}

#endif
