#define OWNER_TEST_MAIN owner_tests_main
#include "test.c"
#undef OWNER_TEST_MAIN
#include "wifi_wpa_nst.h"
static unsigned mock_kind,mock_replays,mock_authorizes;
static uint64_t mock_ticket;
static uint8_t mock_frame[1600];static size_t mock_bytes;
int nst_command(struct nst*t,uint64_t ticket,uint8_t cid,const uint8_t*p,size_t n){assert(!t->wire_busy&&!t->air_pending);assert(cid==SW_KEY||cid==SW_CHANNEL);assert(n<sizeof mock_frame);memcpy(mock_frame,p,n);mock_bytes=n;mock_ticket=ticket;t->wire_busy=1;mock_kind=NST_COMMAND_DMA;return 0;}
int nst_management(struct nst*t,uint64_t ticket,const uint8_t*p,size_t n){assert(n>=24);mock_ticket=ticket;t->wire_busy=t->air_pending=1;mock_kind=NST_FRAME_RESULT;memcpy(mock_frame,p,n);mock_bytes=n;return 0;}
int nst_eapol(struct nst*t,uint64_t ticket,const uint8_t*p,size_t n,unsigned protect){assert(n>=14&&p[12]==0x88&&p[13]==0x8e);assert(!memcmp(p,t->peer.ap,6)&&!memcmp(p+6,t->peer.self,6));assert(!protect);mock_ticket=ticket;t->wire_busy=1;mock_kind=NST_SECURITY_SUBMITTED;memcpy(mock_frame,p,n);mock_bytes=n;return 0;}
int nst_replay_install(struct nst*t,unsigned pairwise,uint8_t keyid,const uint8_t rsc[8]){mock_replays++;if(pairwise){assert(!keyid);for(unsigned i=0;i<8;i++)assert(!rsc[i]);t->ptk.installed=1;}else{assert(keyid==1);t->gtk.installed=1;t->gtk.key_id=keyid;t->gtk_keys[keyid]=t->gtk;t->gtk_wlan[keyid]=t->peer.group_wlan;t->gtk_mapped|=1u<<keyid;}return 0;}
/* This older connector fixture does not emulate native rekey ownership. */
int nst_replay_replace_group(struct nst*t,uint8_t keyid,const uint8_t rsc[8]){(void)t;(void)keyid;(void)rsc;return -1;}
int nst_authorize(struct nst*t){if(!t->ptk_ready||!t->ptk.installed||!t->gtk.installed)return -1;t->controlled_port=1;mock_authorizes++;return 0;}
int nst_cleanup_begin(struct nst*t,uint64_t now,uint64_t deadline){assert(deadline>now);t->deadline=deadline;return 0;}
void nst_close_port(struct nst*t){t->controlled_port=0;}
static int peer_submit(void*x,const struct stc_action*a,const struct stc*s){(void)x;assert(s->generation==a->generation);mock_ticket=a->ticket;mock_kind=NST_NONE;return 0;}
static void transport_ack(struct nwn*s){assert(nwn_pump(s,1)==1);if(s->owner.transaction.phase==STC_CONNECTED)return;unsigned kind=s->owner.in_kind;uint64_t ticket=s->owner.in_ticket;assert(mock_ticket==ticket);
 if(kind==STC_PREPARE_PEER||kind==STC_CONFIGURE_PEER||kind==STC_RETIRE_PEER){assert(nwn_peer_complete(s,42,ticket,completion(kind),1,1)>=0);return;}
 struct nst_event e={.generation=42,.ticket=ticket,.kind=NST_COMMAND_DMA};
 if(mock_kind==NST_FRAME_RESULT){assert(nwn_event(s,&e,1)==0&&s->owner.in_ticket==ticket);e.kind=NST_FRAME_RESULT;}
 if(mock_kind==NST_SECURITY_SUBMITTED)e.kind=NST_SECURITY_SUBMITTED;
 s->transport->wire_busy=s->transport->air_pending=0;assert(nwn_event(s,&e,1)>=0);}
static void input_eapol(struct nwn*s,const uint8_t*p,size_t n){uint8_t f[654];memcpy(f,own,6);memcpy(f+6,peer,6);f[12]=0x88;f[13]=0x8e;memcpy(f+14,p,n);struct nst_event e={.kind=NST_EAPOL,.generation=42,.body=f,.bytes=n+14};assert(nwn_event(s,&e,1)==1);}
int main(void){assert(!owner_tests_main());struct nwn s={0};struct nst t={0};struct stc_arbiter a={0};struct stc_config c=cfg();t.prepared=1;t.generation=42;t.peer.bss=0;t.peer.wlan=1;t.peer.group_wlan=2;memcpy(t.peer.self,own,6);memcpy(t.peer.ap,peer,6);
assert(nwn_begin(&s,&t,&a,&c,peer_submit,0,0)==1);transport_ack(&s);transport_ack(&s);struct nst_event grant={.kind=NST_CHANNEL_GRANT,.generation=42,.bss=0,.token=1,.band=1,.channel=1,.interval_ms=4000};assert(nwn_event(&s,&grant,1)==1);response(&s.owner.transaction,0);transport_ack(&s);response(&s.owner.transaction,1);transport_ack(&s);transport_ack(&s);input_eapol(&s,m1,sizeof m1);transport_ack(&s);assert(mock_bytes==14+sizeof m2_oracle&&!memcmp(mock_frame+14,m2_oracle,sizeof m2_oracle));input_eapol(&s,m3,sizeof m3);transport_ack(&s);assert(mock_bytes==14+sizeof m4_oracle);transport_ack(&s);uint8_t key[8]={0,0};memcpy(key+2,peer,6);struct nst_event k={.kind=NST_KEY_READY,.generation=42,.body=key,.bytes=8};t.ptk_ready=1;assert(nwn_event(&s,&k,1)==1);transport_ack(&s);assert(!nwo_authorized(&s.owner)&&mock_replays==2);transport_ack(&s);assert(nwo_authorized(&s.owner)&&t.controlled_port&&mock_authorizes==1);assert(nwn_cancel(&s,42,1)>=0&&!t.controlled_port);printf("PASS concrete nst connector composition: real engine/codec, explicit mocked IO/peer seams\n");return 0;}
