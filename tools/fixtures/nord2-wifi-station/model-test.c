#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "wifi_station_transaction.h"
#include "vectors.h"
static unsigned checks,rng_count;
static int rng(void*x,uint8_t*p,size_t n){(void)x;assert(n==32);memcpy(p,sn,n);rng_count++;return 0;}
static struct stc_config cfg(void){static const uint8_t name[]={1,2,3},rates[]={0x82,0x84,11,22};struct stc_config c={0};c.generation=42;c.deadline_ms=10000;c.cleanup_timeout_ms=30000;c.assoc.peer=(struct sw_peer){{2,0,0,0,0,1},{2,0,0,0,0,2}};c.assoc.ssid=name;c.assoc.ssid_len=3;c.assoc.rates=rates;c.assoc.rates_len=4;c.assoc.capability=0x11;c.assoc.listen_interval=1;c.channel=(struct sw_channel){.bss=0,.token=1,.band=1,.channel=1,.interval_ms=10000};c.sta_wlan=1;c.group_wlan=2;c.pmk=pmk_vector;c.station_rsn=rsn;c.ap_rsn=rsn;c.rng=rng;return c;}
static void ack(struct stc*s){const struct stc_action*a=stc_next(s);assert(a);uint64_t t=a->ticket,now=s->last_ms;assert(stc_receipt(s,41,t,1,now)==0&&s->action.ticket==t);(void)stc_receipt(s,42,t,1,now);assert(stc_receipt(s,42,t,1,now)==0);checks+=3;}
static void response(struct stc*s,int assoc){uint8_t p[30]={0};p[0]=assoc?0x10:0xb0;memcpy(p+4,own,6);memcpy(p+10,peer,6);memcpy(p+16,peer,6);if(assoc){p[28]=1;p[29]=0xc0;}else p[26]=2;assert(stc_management(s,41,p,30,1)==0);assert(stc_management(s,42,p,30,1)>=0);checks+=2;}
static void prepare(struct stc*s,struct stc_arbiter*a){struct stc_config c=cfg();assert(stc_begin(s,a,&c,0)==1);assert(s->phase==STC_PREPARE&&a->mode==2);ack(s);assert(s->phase==STC_CHANNEL);assert(stc_grant(s,42,0,1,1,1,1,1)==1);ack(s);assert(s->phase==STC_AUTH_TX);response(s,0);ack(s);assert(s->phase==STC_ASSOC_TX);response(s,1);ack(s);assert(s->phase==STC_CONFIGURE&&s->aid==1);ack(s);assert(s->phase==STC_EAPOL);checks+=7;}
static void keys(struct stc*s){assert(stc_eapol(s,42,m1,sizeof m1,1)==1);assert(s->action.bytes==sizeof m2_oracle&&!memcmp(s->action.payload,m2_oracle,sizeof m2_oracle));/* M3 can arrive before M2 EID0f. */assert(stc_eapol(s,42,m3,sizeof m3,1)==1);ack(s);assert(s->phase==STC_EAPOL_TX&&s->action.bytes==sizeof m4_oracle&&!memcmp(s->action.payload,m4_oracle,sizeof m4_oracle));ack(s);assert(s->phase==STC_PTK);uint8_t e[8]={0,0};memcpy(e+2,peer,6);assert(stc_key_event(s,41,e,8,1)==0);e[0]=1;assert(stc_key_event(s,42,e,8,1)==0);e[0]=0;e[1]=0xa5;assert(stc_key_event(s,42,e,8,1)==1&&s->phase==STC_PTK);ack(s);assert(s->phase==STC_GTK);assert(stc_key_event(s,42,e,8,1)==0);ack(s);assert(s->phase==STC_DATA);ack(s);assert(s->phase==STC_CONNECTED&&s->outcome==STC_AUTHORIZED);checks+=12;}
static void cleanup(struct stc*s){unsigned steps=0;while(stc_next(s)&&steps++<8)ack(s);assert(steps<=8&&s->phase==STC_DONE&&s->arbiter->mode==0);struct nwp z={0};assert(!memcmp(&s->wpa,&z,sizeof z));checks+=2;}
static void reach(struct stc*s,struct stc_arbiter*a,unsigned kind){
 struct stc_config c=cfg();assert(stc_begin(s,a,&c,0)==1);
 for(unsigned steps=0;steps<24;steps++){
  if(s->action.kind==kind)return;
  switch(s->phase){
   case STC_PREPARE:ack(s);break;
   case STC_CHANNEL:ack(s);assert(stc_grant(s,42,0,1,1,1,1,1)==1);break;
   case STC_AUTH_TX:response(s,0);ack(s);break;
   case STC_ASSOC_TX:response(s,1);ack(s);break;
   case STC_CONFIGURE:ack(s);break;
   case STC_EAPOL:assert(stc_eapol(s,42,s->wpa.state==NWP_M1?m1:m3,s->wpa.state==NWP_M1?sizeof m1:sizeof m3,1)==1);break;
   case STC_EAPOL_TX:ack(s);break;
   case STC_PTK:ack(s);{uint8_t e[8]={0,0};memcpy(e+2,peer,6);assert(stc_key_event(s,42,e,8,1)==1);}break;
   case STC_GTK:ack(s);break;
   default:assert(0);
  }
 }
 assert(0);
}
int main(void){struct stc s={0};struct stc_arbiter a={0};struct stc_config c=cfg();assert(!stc_scan_acquire(&a,1));assert(stc_begin(&s,&a,&c,0)<0&&!rng_count);assert(stc_scan_release(&a,2)<0);assert(!stc_scan_release(&a,1));checks+=4;prepare(&s,&a);assert(stc_scan_acquire(&a,9)<0);keys(&s);
/* M3 retransmission after authorization only re-answers; never reinstalls. */assert(stc_eapol(&s,42,m3,sizeof m3,20000)==1);assert(s.action.kind==STC_TX_EAPOL);(void)stc_receipt(&s,42,s.action.ticket,1,20000);assert(s.phase==STC_CONNECTED&&s.wpa.installed==1);checks+=3;
assert(stc_cancel(&s,41,20000)==0);assert(stc_cancel(&s,42,20000)==1&&s.phase==STC_DEAUTH&&s.action.bytes==26&&s.action.payload[24]==3);cleanup(&s);struct stc another={0};assert(stc_begin(&another,&a,&c,20001)<0);assert(!stc_scan_acquire(&a,9));assert(!stc_scan_release(&a,9));checks+=5;
for(unsigned fail=0;fail<11;fail++){struct stc x={0};struct stc_arbiter b={0};prepare(&x,&b);if(fail<3){if(fail==0)assert(stc_tick(&x,10000)>=0);if(fail==1)assert(stc_cancel(&x,42,1)>=0);if(fail==2)assert(stc_eapol(&x,42,m1,4,1)>=0);cleanup(&x);}else{keys(&x);assert(stc_cancel(&x,42,1)>=0);if(fail==3){assert(stc_receipt(&x,42,x.action.ticket,0,1)>0&&x.phase==STC_REMOVE_PTK&&b.mode==2);assert(stc_receipt(&x,42,x.action.ticket,0,1)<0&&x.phase==STC_HALTED&&b.mode==2);checks+=2;}else if(fail==4){assert(stc_tick(&x,40000)<0&&x.phase==STC_HALTED&&b.mode==2);checks++;}else cleanup(&x);}}
/* Every mutation of the auth fixed response must fail or remain waiting, never advance. */
for(unsigned at=24;at<30;at++){struct stc x={0};struct stc_arbiter b={0};assert(stc_begin(&x,&b,&c,0)==1);ack(&x);ack(&x);assert(stc_grant(&x,42,0,1,1,1,1,1)==1);ack(&x);uint8_t p[30]={0};p[0]=0xb0;memcpy(p+4,own,6);memcpy(p+10,peer,6);memcpy(p+16,peer,6);p[26]=2;p[at]^=1;(void)stc_management(&x,42,p,30,1);assert(x.phase!=STC_ASSOC_TX);(void)stc_cancel(&x,42,1);cleanup(&x);checks++;}
/* Fail actual action boundaries, then require bounded cleanup with port
 * outcome non-authorized and no nonce/keys left. No fake generic ACK. */
const unsigned ops[]={STC_PREPARE_PEER,STC_CID_CHANNEL,STC_TX_AUTH,STC_TX_ASSOC,STC_CONFIGURE_PEER,STC_TX_EAPOL,STC_CID_PTK,STC_CID_GTK,STC_INSTALL_DATA_PATH};
for(unsigned i=0;i<sizeof ops/sizeof *ops;i++){struct stc x={0};struct stc_arbiter b={0};reach(&x,&b,ops[i]);uint64_t old=x.action.ticket;assert(stc_receipt(&x,42,old,0,1)>=0);assert(x.outcome==STC_FAILED&&x.phase!=STC_CONNECTED);assert(!stc_receipt(&x,42,old,1,1));cleanup(&x);checks+=3;}
for(unsigned i=0;i<sizeof ops/sizeof *ops;i++){struct stc x={0};struct stc_arbiter b={0};reach(&x,&b,ops[i]);assert(stc_cancel(&x,42,1)>=0);struct nwp z={0};assert(!memcmp(&x.wpa,&z,sizeof z));cleanup(&x);checks+=2;}
/* Peer deauthentication closes immediately without answering it with deauth. */
{struct stc x={0};struct stc_arbiter b={0};prepare(&x,&b);keys(&x);uint8_t p[26]={0};p[0]=0xc0;memcpy(p+4,own,6);memcpy(p+10,peer,6);memcpy(p+16,peer,6);p[24]=3;assert(stc_management(&x,42,p,26,1)==1&&x.outcome==STC_DISCONNECTED&&x.action.kind!=STC_TX_DEAUTH);cleanup(&x);checks++;}
/* Event24 arrives during a retransmitted M4 while PTK-ready is pending. */
{struct stc x={0};struct stc_arbiter b={0};reach(&x,&b,STC_CID_PTK);ack(&x);assert(x.phase==STC_PTK_READY);assert(stc_eapol(&x,42,m3,sizeof m3,1)==1&&x.resume_phase==STC_PTK_READY);uint8_t e[8]={0,0};memcpy(e+2,peer,6);assert(stc_key_event(&x,42,e,8,1)==1);ack(&x);assert(x.phase==STC_GTK);/* retry can queue during GTK submission */assert(stc_eapol(&x,42,m3,sizeof m3,1)==1);ack(&x);ack(&x);assert(x.phase==STC_EAPOL_TX&&x.outcome==STC_AUTHORIZED);ack(&x);assert(x.phase==STC_CONNECTED&&x.wpa.installed==1);assert(stc_cancel(&x,42,1)>=0);cleanup(&x);checks+=6;}
/* A first-key timeout cannot be converted into success by a late ready event. */
{struct stc x={0};struct stc_arbiter b={0};reach(&x,&b,STC_CID_PTK);ack(&x);assert(stc_tick(&x,10000)>=0&&x.outcome==STC_TIMEOUT);uint8_t e[8]={0,0};memcpy(e+2,peer,6);assert(stc_key_event(&x,42,e,8,10000)==0);cleanup(&x);checks+=2;}
/* A canceled action's success cannot consume its replacement cleanup ticket. */
{struct stc x={0};struct stc_arbiter b={0};reach(&x,&b,STC_CID_GTK);uint64_t old=x.action.ticket;assert(stc_cancel(&x,42,1)>=0);uint64_t cleanup_ticket=x.action.ticket;assert(!stc_receipt(&x,42,old,1,1)&&x.action.ticket==cleanup_ticket);cleanup(&x);checks+=2;}
/* Long-lived connections get a fresh finite cleanup deadline at cancellation. */
{struct stc x={0};struct stc_arbiter b={0};prepare(&x,&b);keys(&x);assert(stc_cancel(&x,42,1000000)>=0&&x.cleanup_deadline==1030000);cleanup(&x);checks++;}
/* Loss of the shared arbiter never emits another operation or frees a scan. */
{struct stc x={0};struct stc_arbiter b={0};assert(stc_begin(&x,&b,&c,0)==1);b.mode=1;b.generation=99;assert(stc_tick(&x,1)<0&&x.phase==STC_HALTED&&!stc_next(&x)&&b.mode==1&&b.generation==99);checks+=2;}
/* Explicit actual-owner channel release records only the matching token. */
{struct stc x={0};struct stc_arbiter b={0};prepare(&x,&b);assert(!stc_channel_released(&x,42,2,1)&&x.channel_held);assert(stc_channel_released(&x,42,1,1)==1&&!x.channel_held);assert(stc_cancel(&x,42,1)>=0);cleanup(&x);checks+=3;}
printf("PASS %u station transaction model checks; synthetic adapters, no native installed claim\n",checks);return 0;}
