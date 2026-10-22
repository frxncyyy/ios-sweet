#define NST_TEST_NO_MAIN 1
#include "transport-hardware.inc"
#include "wifi_station_transaction.h"
static void prepared(struct test*t){
 setup(t);uint8_t rates[6]={0x82,0x84,0x8b,0x96,96,108},ssid[3]={'l','a','b'};
 struct nst_profile profile={.assoc={.ssid=ssid,.ssid_len=3,.rates=rates,.rates_len=6,.capability=0x431,.listen_interval=10},.beacon_interval=100,.rcpi=100,.sta_version=1};
 memcpy(&profile.assoc.peer,&t->s.peer,12);
 C(!nst_peer_begin(&t->s,1,NST_PREPARE_PEER,&profile,NULL,0));until(t,NST_PEER_PREPARED);
}
static void positive(struct test*t,unsigned retry){
 prepared(t);uint8_t ch[24];size_t used;struct sw_channel channel={0,1,0,1,6,4,4000};
 C(!sw_channel(ch,sizeof ch,&used,&channel));C(!nst_command(&t->s,2,28,ch,used));until(t,NST_CHANNEL_GRANT);
 uint8_t response[38];association(response,t);put16(response,0x10|(retry?0x0800:0));
 struct stc tx={0};struct stc_arbiter arb={.mode=2,.generation=17};
 tx.attempted=1;tx.generation=17;tx.arbiter=&arb;memcpy(&tx.peer,&t->s.peer,sizeof tx.peer);tx.phase=STC_ASSOC_RX;tx.deadline=30000;tx.cleanup_timeout_ms=30000;
 C(stc_management(&tx,17,response,sizeof response,t->now/1000)==1);C(tx.associated&&tx.phase==STC_CONFIGURE);
 const struct stc_action*a=stc_next(&tx);C(a&&a->kind==STC_CONFIGURE_PEER&&a->bytes==sizeof response);C(!memcmp(a->payload,response,sizeof response));
 int native=nst_peer_begin(&t->s,3,NST_CONFIGURE_PEER,NULL,a->payload,a->bytes);
 if(native)fprintf(stderr,"peer handoff refused Retry=%u\n",retry);C(!native);
 until(t,NST_PEER_CONFIGURED);C(t->s.sta_active&&t->events[NST_CHANNEL_RELEASED]&&!t->s.channel_held);
 C(!t->s.controlled_port&&!t->s.ptk_ready&&!t->s.ptk.installed&&!t->s.gtk.installed);cleanup(t);
}
int main(void){struct test*t=calloc(1,sizeof*t);assert(t);positive(t,0);positive(t,1);
 prepared(t);uint8_t response[38];association(response,t);
 for(unsigned bit=0;bit<16;bit++){
  if(bit==4||bit==11)continue;
  put16(response,0x10|(1u<<bit));struct nst before=t->s;
  C(nst_peer_begin(&t->s,3,NST_CONFIGURE_PEER,NULL,response,sizeof response)<0);C(!memcmp(&t->s,&before,sizeof before));
 }
 put16(response,0x0810);
 for(unsigned fault=0;fault<8;fault++){
  association(response,t);put16(response,0x0810);
  if(fault<3)response[4+6*fault]^=2;
  else if(fault==3)response[26]=1;
  else if(fault==4)put16(response+28,0);
  else if(fault==5)response[24]&=~16u;
  else if(fault==6)response[24]&=~1u;
  else response[31]=7;
  struct nst before=t->s;C(nst_peer_begin(&t->s,3,NST_CONFIGURE_PEER,NULL,response,sizeof response)<0);C(!memcmp(&t->s,&before,sizeof before));
 }
 cleanup(t);free(t);printf("PASS %u assertions: actualtransaction Retry/ordinary response→native peer/CID/STA/channelrelease, forbidden FC and peer/status/AID/rate refusal atomicity; no key/carrier success\n",checks);return 0;}
