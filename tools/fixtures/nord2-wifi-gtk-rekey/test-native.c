/* Existing actual NST hardware model, actual owner/transaction/engine composition.
 * Test-only observation of published bytes; no manufactured successful receipt. */
struct test;static void gtk_test_publish(struct test*);
#define main legacy_actual_main
#include "actual-transport-test.c"
#undef main
#include "gtk-vectors.h"
static unsigned adds[4],deletes[4],group_acks,last_group_wlan[4];
static void gtk_test_publish(struct test*t){
 const uint8_t*p=t->capture;
 if(!t->s.wire_type&&p[36]==7&&!p[66]){unsigned key=p[76];C(key<4);if(p[64]){adds[key]++;last_group_wlan[key]=p[78];}else{deletes[key]++;C(last_group_wlan[key]==p[78]);}}
 if(t->s.wire_type==2&&t->capture_n>=32+14+99&&p[51]==3&&p[52]==2){C(get32(p+12)&2);group_acks++;}
}
static void joined(struct composed*x,struct test*h,struct stc_arbiter*a,unsigned spares){
 setup(h);memset(x,0,sizeof *x);x->h=h;static const uint8_t rates[]={0x82,0x84,0x8b,0x96,96,108},name[]={1,2,3};
 x->profile=(struct nst_profile){.assoc={.ssid=name,.ssid_len=3,.rates=rates,.rates_len=6,.capability=0x431,.listen_interval=10},.beacon_interval=100,.rcpi=100,.sta_version=1};memcpy(&x->profile.assoc.peer,&h->s.peer,12);
 struct stc_config c={.generation=17,.deadline_ms=30000,.cleanup_timeout_ms=30000,.assoc=x->profile.assoc,.channel={0,1,0,1,6,4,4000},.sta_wlan=5,.group_wlan=4,.pmk=pmk_vector,.station_rsn=rsn,.ap_rsn=rsn,.rng=entropy};if(spares)memcpy(c.group_spare,h->s.group_spare,3);
 C(nwn_begin(&x->n,&h->s,a,&c,native_peer,x,1)==1);
 /* Fix test context after returning by value before any deferred callback. */
 return;
}
static void handshake(struct composed*x){
 x->n.peer_context=x;x->n.owner.context=&x->n;struct test*h=x->h;phase(x,STC_AUTH_RX);uint8_t response[38]={0};put16(response,0xb0);memcpy(response+4,own,6);memcpy(response+10,peer,6);memcpy(response+16,peer,6);put16(response+26,2);management(x,response,30);phase(x,STC_ASSOC_RX);association(response,h);management(x,response,38);phase(x,STC_EAPOL);eapol_rx(x,m1,sizeof m1);advance(x);phase(x,STC_EAPOL);eapol_rx(x,m3,sizeof m3);advance(x);phase(x,STC_CONNECTED);C(nwo_authorized(&x->n.owner)&&h->s.controlled_port&&h->s.gtk_keys[1].installed&&h->s.gtk_wlan[1]==4);
}
static void group_rx(struct composed*x,unsigned key,unsigned wlan,unsigned pn){
 uint8_t p[116]={0};put16(p,sizeof p);put16(p+2,2u<<11);put32(p+4,wlan|(9u<<11)|(4u<<16)|(key<<21));put32(p+8,1u<<13);put16(p+24,0x4208);memcpy(p+26,peer,6);p[40]=(uint8_t)pn;p[41]=(uint8_t)(pn>>8);uint8_t*m=p+56;memset(m,255,6);m[6]=2;m[11]=7;m[12]=8;deliver(x->h,0,p,sizeof p);advance(x);
}
static void rekey(struct composed*x,const uint8_t*p,size_t n){eapol_rx(x,p,n);advance(x);phase(x,STC_CONNECTED);C(x->n.owner.transaction.outcome==STC_AUTHORIZED&&nwo_authorized(&x->n.owner)&&x->h->s.controlled_port&&!x->h->s.halt);}
static void done(struct composed*x){C(nwn_cancel(&x->n,17,x->h->now/1000)>=0);phase(x,STC_DONE);C(!x->h->s.controlled_port&&!x->h->s.gtk.installed&&!x->h->s.ptk.installed);for(unsigned k=0;k<4;k++)C(!x->h->s.gtk_keys[k].installed);}
int main(void){struct test*h=calloc(1,sizeof *h);assert(h);struct stc_arbiter a={0};struct composed x={0};joined(&x,h,&a,1);handshake(&x);C(h->s.group_spare[0]==6&&h->s.group_spare[1]==7&&h->s.group_spare[2]==8);unsigned ptk_events=h->events[NST_KEY_READY];
 group_rx(&x,1,4,1);unsigned eth=h->events[NST_ETHERNET];C(eth==1);unsigned initial=adds[1];rekey(&x,g1_0,sizeof g1_0);C(adds[2]==1&&last_group_wlan[2]==6&&group_acks==1&&h->events[NST_KEY_READY]==ptk_events&&h->s.gtk_submission_serial==2&&h->s.gtk_replay_serial==2);C(h->s.gtk_keys[1].pn[16]==1&&h->s.gtk_keys[2].pn[16]==8);
 group_rx(&x,1,4,2);group_rx(&x,2,6,9);C(h->events[NST_ETHERNET]==eth+2);eth=h->events[NST_ETHERNET];group_rx(&x,2,6,9);group_rx(&x,2,7,10);C(h->events[NST_ETHERNET]==eth);
 unsigned pubs=h->published;rekey(&x,g1_0,sizeof g1_0);C(h->published==pubs+1&&adds[2]==1&&group_acks==2&&h->s.gtk_keys[2].pn[16]==9);
 rekey(&x,g1_same_newcounter,sizeof g1_same_newcounter);C(adds[2]==1&&h->s.gtk_keys[2].pn[16]==9&&group_acks==3);done(&x);C(deletes[1]==1&&deletes[2]==1&&adds[1]==initial);cleanup(h);
 memset(adds,0,sizeof adds);memset(deletes,0,sizeof deletes);memset(last_group_wlan,0,sizeof last_group_wlan);group_acks=0;a=(struct stc_arbiter){0};joined(&x,h,&a,1);handshake(&x);rekey(&x,g1_0,sizeof g1_0);rekey(&x,g1_1,sizeof g1_1);rekey(&x,g1_2,sizeof g1_2);C(last_group_wlan[1]==4&&last_group_wlan[2]==6&&last_group_wlan[3]==7&&last_group_wlan[0]==8);group_rx(&x,1,4,1);group_rx(&x,2,6,9);group_rx(&x,3,7,10);group_rx(&x,0,8,11);rekey(&x,g1_3,sizeof g1_3);C(last_group_wlan[1]==4&&adds[1]==2&&h->s.gtk_keys[1].pn[16]==11);C(nst_replay_replace_group(&h->s,1,(uint8_t[8]){0})<0);done(&x);for(unsigned k=0;k<4;k++)C(deletes[k]==1);C(a.mode==0&&a.connection_used);cleanup(h);
 /* Missing owned spares is a bounded refusal followed by actual cleanup,
  * never arbitrary WTBL selection or a fake Group2. */
 a=(struct stc_arbiter){0};joined(&x,h,&a,0);handshake(&x);unsigned ack=group_acks;eapol_rx(&x,g1_0,sizeof g1_0);advance(&x);phase(&x,STC_DONE);C(!h->s.halt&&group_acks==ack&&!h->s.controlled_port);cleanup(h);
 /* Cancellation after a real group command publication but before DDONE
  * retains adapter ownership, then deletes both old and attempted new slots. */
 memset(deletes,0,sizeof deletes);a=(struct stc_arbiter){0};joined(&x,h,&a,1);handshake(&x);eapol_rx(&x,g1_0,sizeof g1_0);advance(&x);h->defer_dma=1;advance(&x);C(x.n.owner.in_ticket&&h->s.wire_busy&&!h->s.dma_done&&h->s.gtk_submission_serial==1);ack=group_acks;C(nwn_cancel(&x.n,17,h->now/1000)>=0&&!h->s.controlled_port);unsigned slot=h->s.tx_next;put32(at(h,h->init.transport.tx_ring+16u*slot+4,4),0xc0000000);h->didx=h->cidx;h->defer_dma=0;phase(&x,STC_DONE);C(group_acks==ack&&deletes[1]==1&&deletes[2]==1&&!h->s.halt);cleanup(h);
 /* Ordinary data already published retains its buffer/lease. Group1 may be
  * authenticated/queued, but its key command and G2 wait for real DDONE. */
 a=(struct stc_arbiter){0};joined(&x,h,&a,1);handshake(&x);uint8_t data[60]={0};memset(data,255,6);memcpy(data+6,own,6);data[12]=8;h->defer_dma=1;C(!nst_data(&h->s,UINT64_MAX,data,sizeof data));advance(&x);C(h->s.wire_busy&&!h->s.dma_done);eapol_rx(&x,g1_0,sizeof g1_0);advance(&x);C(x.n.owner.transaction.action.kind==STC_REKEY_GTK&&x.n.owner.transaction.wpa.group_phase==1);ack=group_acks;unsigned group_serial=h->s.gtk_submission_serial;advance(&x);C(!x.n.owner.in_ticket&&h->s.gtk_submission_serial==group_serial&&group_acks==ack);slot=h->s.tx_next;
#ifdef NORD2_WIFI_STATION_TX_H
 put32(at(h,h->init.rings[0].descriptors+16u*h->s.normal.next+4,4),0xc0000000);h->data_didx=h->data_cidx;
#else
 put32(at(h,h->init.transport.tx_ring+16u*slot+4,4),0xc0000000);h->didx=h->cidx;
#endif
 h->defer_dma=0;phase(&x,STC_CONNECTED);C(group_acks==ack+1&&!h->s.halt&&nwo_authorized(&x.n.owner));done(&x);C(nst_replay_replace_group(&h->s,2,(uint8_t[8]){0})<0);cleanup(h);
 /* A verified Group1 never creates an unbounded management transaction.
  * While only queued (no new publication), local10s expiry closes the port
  * and uses the same real cleanup, with no Group2 or fresh key receipt. */
 a=(struct stc_arbiter){0};joined(&x,h,&a,1);handshake(&x);eapol_rx(&x,g1_0,sizeof g1_0);advance(&x);C(x.n.owner.transaction.group_active&&!x.n.owner.in_ticket);ack=group_acks;h->now=x.n.owner.transaction.group_deadline*1000;phase(&x,STC_DONE);C(x.n.owner.transaction.outcome==STC_TIMEOUT&&!h->s.halt&&group_acks==ack&&!h->s.controlled_port);cleanup(h);
 /* Every callback in a complete incoming Group1/install/Group2 cycle is
  * independently failed; native unknown ownership stays halted/closed. */
 a=(struct stc_arbiter){0};joined(&x,h,&a,1);handshake(&x);unsigned start=h->calls;rekey(&x,g1_0,sizeof g1_0);unsigned callbacks=h->calls-start;cleanup(h);
 for(unsigned f=1;f<=callbacks;f++){a=(struct stc_arbiter){0};joined(&x,h,&a,1);handshake(&x);h->fail_at=h->calls+f;eapol_rx(&x,g1_0,sizeof g1_0);unsigned failed=0;for(unsigned k=0;k<12;k++){int z=nwn_pump(&x.n,h->now/1000);if(z<0||nst_step(&h->s,&io,h,16,native_event,&x)<0){failed=1;break;}h->now+=1000;}C(failed&&h->s.halt&&!h->s.controlled_port);C(h->s.credit_available+h->s.credit_reserved==h->s.credit_capacity);cleanup(h);}
 free(h);printf("PASS %u actual-transport GTK assertions, %u callback faults: key map/PN overlap/duplicate no-reset/protectedG2/all-key cleanup/cancel; synthetic firmware only\n",checks,callbacks);return 0;}
