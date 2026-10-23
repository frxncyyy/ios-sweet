/* Actual production transport, crypto/connector helpers; synthetic memory/AP. */
#define SUBMISSION_MAIN unused_security_main
#include "../nord2-wifi-station/test-security-submission.c"
#undef SUBMISSION_MAIN
#ifndef NST_TX_LOCAL_RETIRE
#define NST_TX_LOCAL_RETIRE 257u /* baseline must fail behavior, not compilation */
#endif
static void deauth_packet(struct test*h,uint8_t p[26]){memset(p,0,26);p[0]=0xc0;memcpy(p+4,h->s.peer.ap,6);memcpy(p+10,h->s.peer.self,6);memcpy(p+16,h->s.peer.ap,6);p[24]=3;}
static void begin_deauth(struct test*h){setup(h);uint8_t p[26];deauth_packet(h,p);C(!nst_cleanup_begin(&h->s,h->now,h->now+30000000));C(!nst_management(&h->s,51,p,26));}
int main(void){struct test*h=calloc(1,sizeof *h);assert(h);
 /* No air receipt needed for exact cleanup deauth after DDONE and buffer wipe. */
 begin_deauth(h);h->defer_air=1;uint64_t started=h->now;step(h);unsigned pid=h->s.wire_pid,events=h->events[NST_FRAME_RESULT];C(events==1&&h->last_status==NST_TX_LOCAL_RETIRE&&!h->s.wire_busy&&!h->s.air_pending&&h->s.dma_done&&!h->s.controlled_port&&!h->s.halt&&h->s.submitted==h->s.dma_completed&&h->s.last-started<100000);assert_wiped(h);
 txdone(h,pid,h->s.peer.wlan,0);step(h);txdone(h,pid,h->s.peer.wlan,7);step(h);C(h->events[NST_FRAME_RESULT]==events&&!h->s.controlled_port&&!h->s.halt);cleanup(h);
 /* Already consumed real TXS, good or bad, retains its exact result. */
 for(unsigned status=0;status<8;status+=7){begin_deauth(h);h->defer_air=h->defer_dma=1;step(h);C(h->s.wire_busy&&!h->s.dma_done&&h->s.air_pending);txdone(h,h->s.wire_pid,h->s.peer.wlan,status);step(h);C(h->s.air_done&&!h->events[NST_FRAME_RESULT]);dma_retire(h);step(h);C(h->last_status==status&&h->events[NST_FRAME_RESULT]==1&&!h->s.air_pending&&!h->s.halt);assert_wiped(h);cleanup(h);}
 /* Unknown descriptor retirement remains a hard failure, even with true TXS. */
 for(unsigned air=0;air<2;air++){begin_deauth(h);h->defer_air=h->defer_dma=1;step(h);if(air){txdone(h,h->s.wire_pid,h->s.peer.wlan,0);step(h);}h->now=h->s.operation_deadline;C(nst_step(&h->s,&io,h,16,callback,h)<0);C(h->s.halt&&h->s.error==3&&h->s.wire_busy&&!h->s.dma_done&&!h->events[NST_FRAME_RESULT]);cleanup(h);}
 /* DIDX progress alone never proves DDONE. */
 begin_deauth(h);h->defer_air=h->defer_dma=1;step(h);h->didx=h->cidx;step(h);C(h->s.wire_busy&&!h->s.dma_done&&!h->events[NST_FRAME_RESULT]);cleanup(h);
 /* Non-cleanup deauth and initial AUTH/ASSOC keep authoritative air semantics. */
 for(unsigned which=0;which<3;which++){setup(h);uint8_t p[160];size_t n=0;struct sw_peer peer;memcpy(&peer,&h->s.peer,12);if(!which){deauth_packet(h,p);n=26;}else if(which==1){C(!sw_auth(p,sizeof p,&n,&peer));}else{uint8_t rates[]={0x82},ssid[]={1};struct sw_assoc a={.peer=peer,.ssid=ssid,.ssid_len=1,.rates=rates,.rates_len=1,.listen_interval=10,.capability=1,.security=SW_SECURITY_OPEN};C(!sw_assoc(p,sizeof p,&n,&a));}h->defer_air=1;C(!nst_management(&h->s,1,p,n));step(h);C(h->s.air_pending&&h->s.dma_done&&!h->events[NST_FRAME_RESULT]);h->now=h->s.operation_deadline;C(nst_step(&h->s,&io,h,16,callback,h)<0&&h->s.error==3);cleanup(h);}
 /* Callback failures never bypass retained ownership or claim successful air. */
 begin_deauth(h);h->defer_air=1;step(h);unsigned boundaries=h->calls;cleanup(h);
 for(unsigned f=1;f<=boundaries;f++){begin_deauth(h);h->defer_air=1;h->fail_at=f;C(nst_step(&h->s,&io,h,16,callback,h)<0&&h->s.halt);C(!h->s.controlled_port);if(h->events[NST_FRAME_RESULT]){C(h->events[NST_FRAME_RESULT]==1&&h->s.dma_done&&!h->s.wire_busy&&h->last_status==NST_TX_LOCAL_RETIRE);assert_wiped(h);}cleanup(h);}
 /* Real WPA2 connector cleanup must still perform all actual key/peer steps. */
 struct composed x;struct stc_arbiter arb={0};init_join(h,&x,&arb);m2_retired(&x);eapol_rx(&x,m3,sizeof m3);advance(&x);finish_keys(&x);unsigned before=h->published;h->defer_air=1;C(nwn_cancel(&x.n,17,h->now/1000)>=0);phase(&x,STC_DONE);C(h->published>=before+6&&!h->s.sta_active&&!h->s.ptk.installed&&!h->s.gtk.installed&&!h->s.controlled_port&&!h->s.halt&&!x.n.owner.halted&&x.n.owner.transaction.outcome==STC_CANCELED);unsigned oldpid=h->s.wire_pid,air_events=h->events[NST_FRAME_RESULT];txdone(h,oldpid,h->s.peer.wlan,0);step(h);txdone(h,oldpid,h->s.peer.wlan,7);step(h);C(h->events[NST_FRAME_RESULT]==air_events&&!nwo_authorized(&x.n.owner)&&!h->s.controlled_port);cleanup(h);
 C(NST_TX_LOCAL_RETIRE==257);C(NST_TX_LOCAL_TIMEOUT==256);free(h);printf("PASS %u actual-source checks; exact deauth DDONE local-negative257, real statuses preserved, lateTXS ignored, auth/assoc/unknownDMA/keydelete guards retained; no hardware claim\n",checks);return 0;}
