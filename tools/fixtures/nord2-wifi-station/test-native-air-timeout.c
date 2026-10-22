/* Synthetic peer fixtures only. Actual canonical engine, connector and NST. */
#define main existing_composition_main
#include "actual-transport-test.c"
#undef main
#ifndef NST_TX_LOCAL_TIMEOUT
#define NST_TX_LOCAL_TIMEOUT 0x100u /* Expectation permits old-source negative control. */
#endif

static void init_join(struct test*h,struct composed*x,struct stc_arbiter*arb){
 setup(h);memset(x,0,sizeof *x);x->h=h;
 static const uint8_t rates[]={0x82,0x84,0x8b,0x96,96,108},name[]={1,2,3};
 x->profile=(struct nst_profile){.assoc={.ssid=name,.ssid_len=3,.rates=rates,.rates_len=6,.capability=0x431,.listen_interval=10},.beacon_interval=100,.rcpi=100,.sta_version=1};memcpy(&x->profile.assoc.peer,&h->s.peer,12);
 struct stc_config c={.generation=17,.deadline_ms=30000,.cleanup_timeout_ms=30000,.assoc=x->profile.assoc,.channel={0,1,0,1,6,4,4000},.sta_wlan=5,.group_wlan=4,.pmk=pmk_vector,.station_rsn=rsn,.ap_rsn=rsn,.rng=entropy};
 C(nwn_begin(&x->n,&h->s,arb,&c,native_peer,x,1)==1);phase(x,STC_AUTH_RX);
 uint8_t response[38]={0};put16(response,0xb0);memcpy(response+4,own,6);memcpy(response+10,peer,6);memcpy(response+16,peer,6);put16(response+26,2);management(x,response,30);phase(x,STC_ASSOC_RX);
 association(response,h);management(x,response,38);phase(x,STC_EAPOL);C(h->s.sta_active&&!h->s.channel_held&&!h->s.ptk_ready&&!h->s.controlled_port);
}

static void pending_m2(struct composed*x,unsigned defer_dma){
 struct test*h=x->h;h->defer_air=1;h->defer_dma=defer_dma;eapol_rx(x,m1,sizeof m1);advance(x);advance(x);
 C(x->n.owner.in_kind==STC_TX_EAPOL&&h->s.wire_type==2&&h->s.air_pending&&h->s.published);
 C(h->s.dma_done==!defer_dma);C(!h->events[NST_KEY_READY]&&!h->s.controlled_port);
}

int main(void){
 struct test*h=calloc(1,sizeof *h);assert(h);struct composed x;struct stc_arbiter arb={0};
 init_join(h,&x,&arb);pending_m2(&x,0);
 unsigned oldpid=h->s.wire_pid,oldwlan=h->s.wire_wlan,events=h->events[NST_FRAME_RESULT];
 uint64_t op=h->s.operation_deadline;h->now=op-1;advance(&x);
 C(h->s.air_pending&&!h->s.halt&&h->events[NST_FRAME_RESULT]==events);
 h->now=op;advance(&x);
 C(!h->s.halt&&!h->init.halt&&!h->init.transport.halt&&!h->init.transport.dirty);
 C(!h->s.air_pending&&!h->s.controlled_port&&!nwo_authorized(&x.n.owner));
 C(h->events[NST_FRAME_RESULT]==events+1&&x.n.owner.transaction.phase==STC_DEAUTH&&x.n.owner.transaction.outcome==STC_FAILED);
 C(h->s.next_pid!=oldpid&&h->s.cleanup_started);
 advance(&x);C(x.n.owner.in_kind==STC_TX_DEAUTH&&h->s.air_pending&&h->s.wire_pid!=oldpid&&h->s.dma_done);
 unsigned cleanup_pid=h->s.wire_pid;txdone(h,oldpid,oldwlan,0);advance(&x);
 C(x.n.owner.in_kind==STC_TX_DEAUTH&&h->s.air_pending&&h->events[NST_FRAME_RESULT]==events+1);
 txdone(h,cleanup_pid,oldwlan^1u,0);advance(&x);C(h->s.air_pending&&x.n.owner.in_kind==STC_TX_DEAUTH);
 txdone(h,cleanup_pid,oldwlan,0);h->defer_air=0;phase(&x,STC_DONE);
 C(!h->s.halt&&!h->init.transport.dirty&&!h->s.sta_active&&!h->s.ptk.installed&&!h->s.gtk.installed&&!nwo_authorized(&x.n.owner));
 C(arb.mode==0&&arb.connection_used);struct nwp zero={0};C(!memcmp(&x.n.owner.transaction.wpa,&zero,sizeof zero));cleanup(h);
 /* Missing cleanup deauthentication TXS cannot become a second panic after
  * the safely retired M2. Every subsequent peer command still publishes. */
 for(unsigned cancel=0;cancel<2;cancel++){
  memset(&arb,0,sizeof arb);init_join(h,&x,&arb);pending_m2(&x,0);
  if(cancel)C(nwn_cancel(&x.n,17,h->now/1000)>=0);
  h->now=h->s.operation_deadline;advance(&x);advance(&x);
  C(x.n.owner.in_kind==STC_TX_DEAUTH&&h->s.air_pending&&h->s.dma_done);
  unsigned publications=h->published;h->now=h->s.operation_deadline;advance(&x);
  C(!h->s.halt&&!h->s.air_pending&&x.n.owner.transaction.phase==STC_RETIRE);
  phase(&x,STC_DONE);C(h->published==publications+3&&!h->s.sta_active&&!h->s.controlled_port);
  C(!h->init.transport.dirty&&!nwo_authorized(&x.n.owner)&&!memcmp(&x.n.owner.transaction.wpa,&zero,sizeof zero));
  C(x.n.owner.transaction.outcome==(cancel?STC_CANCELED:STC_FAILED));
  /* Negative peer/key command completion has not been generalized away. */
  struct stc bad=x.n.owner.transaction;bad.phase=STC_REMOVE_PTK;bad.action.kind=STC_DELETE_PTK;bad.action.ticket=999;bad.action.generation=17;bad.arbiter=&arb;arb.mode=2;arb.generation=17;bad.cleanup_deadline=bad.last_ms+30000;
  C(stc_receipt(&bad,17,999,0,bad.last_ms)<0&&bad.phase==STC_HALTED);
  cleanup(h);
 }
 /* An actual queued matching outcome remains necessary for success. */
 memset(&arb,0,sizeof arb);init_join(h,&x,&arb);pending_m2(&x,0);oldpid=h->s.wire_pid;oldwlan=h->s.wire_wlan;
 txdone(h,oldpid,oldwlan,0);h->now=h->s.operation_deadline;advance(&x);
 C(x.n.owner.transaction.phase==STC_EAPOL&&!h->s.air_pending&&!h->s.halt&&!h->s.ptk_ready&&!h->s.controlled_port);cleanup(h);
 /* The same M4 failure after authenticated M3 must erase engine keys and
  * drive real cleanup, without installing GTK/authorizing the port. */
 memset(&arb,0,sizeof arb);init_join(h,&x,&arb);eapol_rx(&x,m1,sizeof m1);advance(&x);phase(&x,STC_EAPOL);
 h->defer_air=1;eapol_rx(&x,m3,sizeof m3);advance(&x);advance(&x);
 C(x.n.owner.in_kind==STC_TX_EAPOL&&h->s.air_pending&&h->s.dma_done&&!h->s.gtk.installed);
 h->now=h->s.operation_deadline;advance(&x);C(x.n.owner.transaction.outcome==STC_FAILED&&!nwo_authorized(&x.n.owner));
 h->defer_air=0;phase(&x,STC_DONE);C(!h->s.halt&&!h->s.ptk.installed&&!h->s.gtk.installed&&!h->s.sta_active&&!memcmp(&x.n.owner.transaction.wpa,&zero,sizeof zero));cleanup(h);
 /* Authorized M3 retransmission still cannot reinstall keys. If its M4 air
  * status is missing, actual PTK+GTK deletion commands must precede retire. */
 memset(&arb,0,sizeof arb);init_join(h,&x,&arb);eapol_rx(&x,m1,sizeof m1);advance(&x);phase(&x,STC_EAPOL);
 eapol_rx(&x,m3,sizeof m3);advance(&x);phase(&x,STC_CONNECTED);C(h->s.ptk.installed&&h->s.gtk.installed&&nwo_authorized(&x.n.owner));
 h->defer_air=1;eapol_rx(&x,m3,sizeof m3);advance(&x);advance(&x);C(h->s.air_pending&&h->s.dma_done&&x.n.owner.transaction.wpa.installed==1);
 h->now=h->s.operation_deadline;advance(&x);unsigned after_m4=h->published;h->defer_air=0;phase(&x,STC_DONE);
 C(h->published==after_m4+6&&!h->s.halt&&!h->s.ptk.installed&&!h->s.gtk.installed&&!h->s.controlled_port&&!memcmp(&x.n.owner.transaction.wpa,&zero,sizeof zero));cleanup(h);
 /* Explicit cleanup does not forgive its own unknown DMA lifetime. */
 memset(&arb,0,sizeof arb);init_join(h,&x,&arb);pending_m2(&x,0);h->now=h->s.operation_deadline;advance(&x);
 h->defer_dma=1;advance(&x);C(x.n.owner.in_kind==STC_TX_DEAUTH&&h->s.wire_busy&&!h->s.dma_done);
 h->now=h->s.operation_deadline;C(nst_step(&h->s,&io,h,16,native_event,&x)<0);C(h->s.halt&&h->s.error==3&&h->init.transport.dirty);cleanup(h);
 /* One final bounded RX drain deliberately accepts a genuine correlated
  * result already queued at/after the soft air deadline. It neither creates
  * a status nor extends the native global/cleanup lease. */
 for(unsigned stage=0;stage<3;stage++)for(unsigned late=0;late<2;late++){
  memset(&arb,0,sizeof arb);init_join(h,&x,&arb);
  if(stage==1){eapol_rx(&x,m1,sizeof m1);advance(&x);phase(&x,STC_EAPOL);h->defer_air=1;eapol_rx(&x,m3,sizeof m3);advance(&x);advance(&x);}
  else pending_m2(&x,0);
  uint64_t old_ticket=x.n.owner.in_ticket;
  if(stage==2)C(nwn_cancel(&x.n,17,h->now/1000)>=0);
  txdone(h,h->s.wire_pid,h->s.wire_wlan,0);h->now=h->s.operation_deadline+(late?1000:0);advance(&x);
  C(!h->s.halt&&!h->s.air_pending&&!h->s.controlled_port&&!nwo_authorized(&x.n.owner));
  C(!h->s.ptk.installed&&!h->s.gtk.installed&&!x.n.owner.in_ticket);
  C(x.n.owner.transaction.phase==(stage==0?STC_EAPOL:stage==1?STC_PTK:STC_DEAUTH));
  if(stage==2){C(x.n.owner.transaction.action.ticket!=old_ticket&&x.n.owner.transaction.outcome==STC_CANCELED);h->defer_air=0;phase(&x,STC_DONE);}
  cleanup(h);
 }
 for(unsigned cleanup_lease=0;cleanup_lease<2;cleanup_lease++){
  memset(&arb,0,sizeof arb);init_join(h,&x,&arb);pending_m2(&x,0);
  if(cleanup_lease){h->now=h->s.operation_deadline;advance(&x);advance(&x);C(h->s.cleanup_started&&h->s.air_pending&&h->s.dma_done);}
  txdone(h,h->s.wire_pid,h->s.wire_wlan,0);unsigned prior=h->events[NST_FRAME_RESULT];h->now=h->s.deadline;
  C(nst_step(&h->s,&io,h,16,native_event,&x)<0);C(h->s.halt&&h->s.error==2&&h->events[NST_FRAME_RESULT]==prior&&!nwo_authorized(&x.n.owner));cleanup(h);
 }
 /* Unknown DMA remains fatal. No expiry can retire/reuse its storage. */
 memset(&arb,0,sizeof arb);init_join(h,&x,&arb);pending_m2(&x,1);h->now=h->s.operation_deadline;
 C(nst_step(&h->s,&io,h,16,native_event,&x)<0);C(h->s.halt&&h->s.error==3&&h->s.wire_busy&&!h->s.dma_done&&h->init.transport.dirty);cleanup(h);
 /* Clock/owner checks remain binding even for DDONE-retired security. */
 memset(&arb,0,sizeof arb);init_join(h,&x,&arb);pending_m2(&x,0);h->now=h->s.last-1;
 C(nst_step(&h->s,&io,h,16,native_event,&x)<0);C(h->s.halt&&h->s.error==1);cleanup(h);
 /* Do not generalize this exception to management or ordinary data. */
 setup(h);uint8_t auth[30];size_t used;struct sw_peer peer_copy;memcpy(&peer_copy,&h->s.peer,12);C(!sw_auth(auth,sizeof auth,&used,&peer_copy));h->defer_air=1;C(!nst_management(&h->s,31,auth,used));step(h);h->now=h->s.operation_deadline;
 C(nst_step(&h->s,&io,h,16,callback,h)<0);C(h->s.halt&&h->s.error==3);cleanup(h);
 setup(h);h->s.controlled_port=1;uint8_t eth[60]={0};memcpy(eth+6,h->s.peer.self,6);eth[0]=255;eth[12]=8;h->defer_air=1;C(!nst_data(&h->s,32,eth,sizeof eth));step(h);h->now=h->s.operation_deadline;
 C(nst_step(&h->s,&io,h,16,callback,h)<0);C(h->s.halt&&h->s.error==3);cleanup(h);
 /* The new status ABI is explicit local negative, outside firmware8-bit. */
 setup(h);uint8_t eap[18]={0};memcpy(eap,h->s.peer.ap,6);memcpy(eap+6,h->s.peer.self,6);eap[12]=0x88;eap[13]=0x8e;h->defer_air=1;
 C(!nst_eapol(&h->s,33,eap,sizeof eap,0));step(h);h->now=h->s.operation_deadline;step(h);
 C(h->events[NST_FRAME_RESULT]==1&&h->last_status==NST_TX_LOCAL_TIMEOUT&&NST_TX_LOCAL_TIMEOUT>255&&!h->s.halt&&!h->s.controlled_port);cleanup(h);free(h);
 printf("PASS %u assertions: missing M2 TXS after DDONE yields negative cleanup, late oldPID/wrongWLAN refused, real TXS required, preDDONE/owner/other-route timeouts remain fatal; synthetic firmware only\n",checks);return 0;
}
