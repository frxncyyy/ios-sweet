/* Actual station/transaction/owner/native connector and real IO routines.
 * Existing synthetic radio owns descriptors, never substitutes a port receipt. */
#define main unused_wpa_main
#include "actual-transport-test.c"
#undef main
static void open_phase(struct composed*x,unsigned target){for(unsigned k=0;k<200&&x->n.owner.transaction.phase!=target;k++)advance(x);C(x->n.owner.transaction.phase==target);}
static void raw_open(struct test*h,unsigned group,unsigned wlan,unsigned fault){uint8_t p[84]={0};put16(p,sizeof p);put16(p+2,2u<<11);put32(p+4,wlan);put32(p+8,12u<<8);uint8_t*m=p+24;put16(m,0x0208);if(group)memset(m+4,255,6);else memcpy(m+4,h->s.peer.self,6);memcpy(m+10,h->s.peer.ap,6);m[16]=2;m[21]=9;memcpy(m+24,"\xaa\xaa\3\0\0\0\x08\0",8);m[32]=0x45;
 if(fault==1)m[10]^=4;if(fault==2)m[1]|=64;if(fault==3)m[22]=1;if(fault==4)m[24]=0;if(fault==5)m[4]^=4;if(fault==6){m[30]=0x88;m[31]=0x8e;}if(fault==7)put32(p+4,wlan|(4u<<16));deliver(h,0,p,sizeof p);}
int main(void){for(unsigned scenario=0;scenario<7;scenario++){
 struct test*h=calloc(1,sizeof *h);assert(h);setup(h);h->s.security=SW_SECURITY_OPEN;struct composed x={.h=h};struct stc_arbiter arbiter={0};static const uint8_t rates[]={0x82,0x84,0x8b,0x96,96,108},name[]={1,2,3};
 x.profile=(struct nst_profile){.assoc={.ssid=name,.ssid_len=3,.rates=rates,.rates_len=6,.capability=0x421,.listen_interval=10,.security=SW_SECURITY_OPEN},.beacon_interval=100,.rcpi=100,.sta_version=1};memcpy(&x.profile.assoc.peer,&h->s.peer,12);
 struct stc_config config={.security=SW_SECURITY_OPEN,.generation=17,.deadline_ms=30000,.cleanup_timeout_ms=30000,.assoc=x.profile.assoc,.channel={0,1,0,1,6,4,4000},.sta_wlan=5,.group_wlan=4};
 C(nwn_begin(&x.n,&h->s,&arbiter,&config,native_peer,&x,1)==1);uint8_t eth[60]={0};memset(eth,255,6);memcpy(eth+6,h->s.peer.self,6);eth[12]=8;C(nst_data(&h->s,7,eth,60)==1&&nst_authorize(&h->s)<0);
 open_phase(&x,STC_AUTH_RX);if(scenario==1){C(nwn_cancel(&x.n,17,h->now/1000)>=0);open_phase(&x,STC_DONE);C(!h->s.halt&&!h->s.controlled_port);cleanup(h);free(h);continue;}
 if(scenario==2){h->now=(x.n.owner.transaction.deadline+50)*1000;open_phase(&x,STC_DONE);C(x.n.owner.transaction.outcome==STC_TIMEOUT&&!h->s.halt);cleanup(h);free(h);continue;}
 uint8_t response[38]={0};put16(response,0xb0);memcpy(response+4,h->s.peer.self,6);memcpy(response+10,h->s.peer.ap,6);memcpy(response+16,h->s.peer.ap,6);put16(response+26,2);management(&x,response,30);open_phase(&x,STC_ASSOC_RX);association(response,h);put16(response+24,scenario==3?0x431:0x421);management(&x,response,38);
 if(scenario==3){open_phase(&x,STC_DONE);C(!h->s.halt&&!h->s.controlled_port&&!h->s.sta_active);cleanup(h);free(h);continue;}
 if(scenario==4){h->defer_sta=1;for(unsigned k=0;k<20&&!h->s.activation_pending;k++)advance(&x);C(h->s.activation_pending&&!h->s.sta_active&&!h->s.controlled_port);uint8_t e[8];memcpy(e,h->s.peer.ap,6);e[6]=h->s.peer.sta;e[7]=h->s.peer.bss;event(h,12,h->s.activation_sequence+1,e,8);advance(&x);C(!h->s.sta_active&&!h->s.controlled_port);event(h,12,h->s.activation_sequence,e,8);}
 open_phase(&x,STC_CONNECTED);struct nwp empty={0};C(nwo_authorized(&x.n.owner)&&h->s.controlled_port&&h->s.sta_active&&!h->s.ptk_ready&&!h->s.ptk.installed&&!h->s.gtk.installed&&!h->s.channel_held&&!memcmp(&x.n.owner.transaction.wpa,&empty,sizeof empty));C(!h->events[NST_KEY_READY]&&!h->events[NST_EAPOL]&&!x.n.owner.transaction.ptk_attempted&&!x.n.owner.transaction.gtk_attempted);
 unsigned before=h->events[NST_ETHERNET];ethernet_rx(h,0,1,0,1);advance(&x);C(h->events[NST_ETHERNET]==before+1);ethernet_rx(h,1,1,0,1);advance(&x);C(h->events[NST_ETHERNET]==before+2);
 ethernet_rx(h,0,1,0,0);advance(&x);C(h->events[NST_ETHERNET]==before+2); /* CCMP never becomes OPEN plaintext */
 for(unsigned group=0;group<2;group++){before=h->events[NST_ETHERNET];raw_open(h,group,group?4:5,0);advance(&x);C(h->events[NST_ETHERNET]==before+1&&h->s.rx_untranslated_data==group+1);for(unsigned fault=1;fault<=7;fault++){raw_open(h,0,5,fault);advance(&x);C(h->events[NST_ETHERNET]==before+1);}}
 before=h->events[NST_ETHERNET];raw_open(h,0,4,0);advance(&x);C(h->events[NST_ETHERNET]==before);C(h->s.rx_untranslated==h->s.rx_untranslated_data+h->s.rx_untranslated_eapol+h->s.rx_untranslated_drop);
 uint8_t rsc[8]={0},key[64]={0};C(nst_replay_install(&h->s,1,0,rsc)<0&&nst_replay_install(&h->s,0,0,rsc)<0&&nst_command(&h->s,9,7,key,64)<0);C(nst_eapol(&h->s,8,eth,60,0)<0);C(stc_eapol(&x.n.owner.transaction,17,m1,sizeof m1,h->now/1000)==0&&nwo_authorized(&x.n.owner));
 h->defer_token=scenario==5;h->defer_dma=scenario==6;C(!nst_data(&h->s,UINT64_MAX,eth,60));advance(&x);C(h->capture_n==92&&!(get32(h->capture+12)&2)&&!memcmp(h->capture+32,eth,60));
 if(scenario==6){C(h->s.normal.published&&!h->s.normal.dma_done);h->now=h->s.operation_deadline;C(nst_step(&h->s,&io,h,16,native_event,&x)<0&&h->s.halt&&h->s.normal.published);cleanup(h);free(h);continue;}
 if(scenario==5){C(h->s.normal.dma_done&&!h->s.normal.report_done&&!h->events[NST_DATA_SUBMITTED]);C(nwn_cancel(&x.n,17,h->now/1000)>=0);advance(&x);C(h->s.wire_busy&&!h->s.controlled_port&&!h->s.halt);uint8_t r[12]={0};put16(r,12);put16(r+2,(6u<<11)|1);put32(r+4,2u<<16);put32(r+8,h->s.normal.token);deliver(h,3,r,12);}
 else{C(h->events[NST_DATA_SUBMITTED]==1);C(nwn_cancel(&x.n,17,h->now/1000)>=0);}
 open_phase(&x,STC_DONE);C(!h->s.halt&&!h->s.controlled_port&&!nwo_authorized(&x.n.owner)&&!h->s.ptk.installed&&!h->s.gtk.installed&&!h->s.sta_active&&!h->s.channel_held&&nstx_idle(&h->s.normal));cleanup(h);free(h);
 }
 printf("PASS %u OPEN actual native auth/association/STA/port/plain TX+RX/ownership/failure/teardown checks\n",checks);return 0;}
