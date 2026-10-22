#define main original_model_main
#include "model-test.c"
#undef main
#include "wifi_wpa_owner.h"
static unsigned submitted,port_closes,submit_fail,close_fail,inline_refused;
static struct nwo *owner;
static int submit(void*x,const struct stc_action*a,const struct nwo_install*m){(void)x;assert(a->ticket&&a->generation==42);submitted++;if(a->kind==STC_CID_GTK){assert(m->gtk_index==owner->transaction.wpa.gtk_index);assert(!memcmp(m->rsc,owner->transaction.wpa.rsc,8));}assert(!nwo_complete(owner,42,a->ticket,NWO_COMMAND,1,1));inline_refused++;return submit_fail;}
static int close_test(void*x){(void)x;port_closes++;return close_fail;}
static const struct nwo_ops native_ops={submit,close_test};
static unsigned completion(unsigned kind){switch(kind){case STC_PREPARE_PEER:return NWO_PREPARED;case STC_TX_AUTH:case STC_TX_ASSOC:case STC_TX_EAPOL:case STC_TX_DEAUTH:return NWO_AIR_TX;case STC_CONFIGURE_PEER:return NWO_CONFIGURED;case STC_CID_GTK:return NWO_GTK_REPLAY;case STC_INSTALL_DATA_PATH:return NWO_DATA_PATH;case STC_RETIRE_PEER:return NWO_RETIRED;default:return NWO_COMMAND;}}
static void native_ack(struct nwo*s){owner=s;uint64_t now=s->transaction.last_ms;assert(nwo_pump(s,now)==1);uint64_t ticket=s->in_ticket;unsigned type=completion(s->in_kind);assert(nwo_complete(s,41,ticket,type,1,now)==0);assert(nwo_complete(s,42,ticket,0,1,now)==-1&&s->in_ticket==ticket);assert(nwo_complete(s,42,ticket,type,1,now)>=0);assert(!nwo_complete(s,42,ticket,type,1,now));checks+=5;}
static void native_prepare(struct nwo*s,struct stc_arbiter*a){struct stc_config c=cfg();owner=s;assert(nwo_begin(s,a,&c,&native_ops,0,0)==1);native_ack(s);native_ack(s);assert(nwo_grant(s,42,0,1,1,1,1,1)==1);response(&s->transaction,0);native_ack(s);response(&s->transaction,1);native_ack(s);native_ack(s);assert(s->transaction.phase==STC_EAPOL&&!nwo_authorized(s));}
static void native_keys(struct nwo*s){assert(nwo_eapol(s,42,m1,sizeof m1,1)==1);native_ack(s);assert(nwo_eapol(s,42,m3,sizeof m3,1)==1);native_ack(s);uint8_t e[8]={0,0};memcpy(e+2,peer,6);assert(nwo_key24(s,42,e,8,1)==0);owner=s;assert(nwo_pump(s,1)==1);assert(nwo_key24(s,42,e,8,1)==1);assert(nwo_complete(s,42,s->in_ticket,NWO_COMMAND,1,1)>=0);assert(s->transaction.phase==STC_GTK&&!nwo_authorized(s));native_ack(s);assert(!nwo_authorized(s));native_ack(s);assert(nwo_authorized(s));checks+=8;}
#ifndef OWNER_TEST_MAIN
#define OWNER_TEST_MAIN main
#endif
int OWNER_TEST_MAIN(void){assert(!original_model_main());struct nwo s={0};struct stc_arbiter a={0};native_prepare(&s,&a);native_keys(&s);
assert(nwo_eapol(&s,42,m3,sizeof m3,1)==1&&nwo_authorized(&s));native_ack(&s);assert(nwo_authorized(&s)&&s.transaction.wpa.installed==1);
assert(nwo_cancel(&s,42,2)>=0&&!nwo_authorized(&s));while(stc_next(&s.transaction))native_ack(&s);assert(s.transaction.phase==STC_DONE);checks+=4;
/* Cancel cannot free DMA lifetime or dispatch cleanup ahead of outstanding TX. */
{struct nwo x={0};struct stc_arbiter b={0};native_prepare(&x,&b);assert(nwo_eapol(&x,42,m1,sizeof m1,1)==1);owner=&x;assert(nwo_pump(&x,1)==1);uint64_t old=x.in_ticket;assert(nwo_cancel(&x,42,1)>=0);uint64_t cleanup=x.transaction.action.ticket;assert(!nwo_pump(&x,1)&&x.transaction.action.ticket==cleanup);assert(nwo_complete(&x,42,old,NWO_AIR_TX,1,1)==0);assert(x.transaction.action.ticket==cleanup);while(stc_next(&x.transaction))native_ack(&x);assert(x.transaction.phase==STC_DONE);checks+=5;}
/* Native BUSY preserves exact ticket and secrets for a later bounded poll. */
{struct nwo x={0};struct stc_arbiter b={0};struct stc_config c=cfg();owner=&x;assert(nwo_begin(&x,&b,&c,&native_ops,0,0)==1);uint64_t t=x.transaction.action.ticket;submit_fail=1;assert(!nwo_pump(&x,1)&&!x.in_ticket&&x.transaction.action.ticket==t);submit_fail=0;native_ack(&x);assert(x.transaction.phase==STC_CHANNEL);assert(nwo_cancel(&x,42,1)>=0);while(stc_next(&x.transaction))native_ack(&x);checks+=3;}
/* Native refusal and port closure failures never authorize. */
{struct nwo x={0};struct stc_arbiter b={0};struct stc_config c=cfg();owner=&x;assert(nwo_begin(&x,&b,&c,&native_ops,0,0)==1);submit_fail=-1;assert(nwo_pump(&x,1)>=0&&!nwo_authorized(&x));submit_fail=0;while(stc_next(&x.transaction))native_ack(&x);checks++;}
{struct nwo x={0};struct stc_arbiter b={0};struct stc_config c=cfg();close_fail=1;assert(nwo_begin(&x,&b,&c,&native_ops,0,0)<0&&x.halted&&!nwo_authorized(&x));close_fail=0;checks++;}
/* Exercise every native action failure, preserving inflight retirement and
 * immediate port closure; old generation and duplicate completions do nothing. */
const unsigned actions[]={STC_PREPARE_PEER,STC_CID_CHANNEL,STC_TX_AUTH,STC_TX_ASSOC,STC_CONFIGURE_PEER,STC_TX_EAPOL,STC_CID_PTK,STC_CID_GTK,STC_INSTALL_DATA_PATH};
for(unsigned i=0;i<sizeof actions/sizeof *actions;i++){
 struct nwo x={0};struct stc_arbiter b={0};reach(&x.transaction,&b,actions[i]);x.ops=native_ops;x.closed=1;owner=&x;
 assert(nwo_pump(&x,1)==1);uint64_t ticket=x.in_ticket;unsigned type=completion(x.in_kind);
 assert(!nwo_complete(&x,43,ticket,type,1,1)&&x.in_ticket==ticket);
 assert(nwo_complete(&x,42,ticket,type,0,1)>=0&&!nwo_authorized(&x));
 struct nwp zero={0};assert(!memcmp(&x.transaction.wpa,&zero,sizeof zero));
 assert(!nwo_complete(&x,42,ticket,type,1,1));while(stc_next(&x.transaction))native_ack(&x);assert(x.transaction.phase==STC_DONE);checks+=6;
}
{struct nwo x={0};struct stc_arbiter b={0};native_prepare(&x,&b);assert(nwo_eapol(&x,42,m1,sizeof m1,1)==1);owner=&x;assert(nwo_pump(&x,1)==1);uint64_t ticket=x.in_ticket;assert(nwo_pump(&x,10000)>=0&&x.transaction.outcome==STC_TIMEOUT&&!nwo_authorized(&x));assert(nwo_complete(&x,42,ticket,NWO_AIR_TX,1,10000)==0);while(stc_next(&x.transaction))native_ack(&x);assert(x.transaction.phase==STC_DONE);checks+=5;}
uint8_t mixed[26]={48,24,1,0,0,15,172,4,1,0,0,15,172,4,2,0,0,15,172,2,0,15,172,8,128,0};assert(!nwp_ap_rsn(mixed,26));for(size_t n=0;n<26;n++){assert(nwp_ap_rsn(mixed,n)<0);checks++;}mixed[24]|=64;assert(nwp_ap_rsn(mixed,26)<0);mixed[24]=128;mixed[19]=8;assert(nwp_ap_rsn(mixed,26)<0);checks+=3;
/* Real cryptography on a mixed PSK+SAE/MFPC AP, full authenticated RSN. */
{struct nwp w={0};struct nwp_result out;assert(!nwp_begin_ap(&w,42,pmk_vector,own,peer,rsn,mixed_rsn,sizeof mixed_rsn,rng,0));assert(nwp_receive(&w,m1,sizeof m1,&out)==NWP_TX);assert(!memcmp(out.frame,m2_oracle,sizeof m2_oracle));assert(nwp_tx_done(&w,42,out.ticket,1)==NWP_DROP);assert(nwp_receive(&w,mixed_m3,sizeof mixed_m3,&out)==NWP_TX);assert(nwp_tx_done(&w,42,out.ticket,1)==NWP_INSTALL);assert(nwp_keys_ready(&w,42,w.install_ticket,7,1)==NWP_AUTH);assert(nwp_receive(&w,mixed_m3,sizeof mixed_m3,&out)==NWP_TX);assert(nwp_tx_done(&w,42,out.ticket,1)==NWP_DROP&&w.installed==1);nwp_cancel(&w);checks+=9;}
{struct nwp w={0};struct nwp_result out;assert(!nwp_begin_ap(&w,42,pmk_vector,own,peer,rsn,mixed_rsn,sizeof mixed_rsn,rng,0));assert(nwp_receive(&w,m1,sizeof m1,&out)==NWP_TX);assert(nwp_tx_done(&w,42,out.ticket,1)==NWP_DROP);assert(nwp_receive(&w,m3,sizeof m3,&out)==NWP_FAIL);struct nwp zero={0};assert(!memcmp(&w,&zero,sizeof w));checks+=5;}
/* GTK index0 is a valid two-bit key identifier; source codec allows0..3. */
{struct nwp w={0};struct nwp_result out;assert(!nwp_begin(&w,42,pmk_vector,own,peer,rsn,rsn,rng,0));assert(nwp_receive(&w,m1,sizeof m1,&out)==NWP_TX);assert(nwp_tx_done(&w,42,out.ticket,1)==NWP_DROP);assert(nwp_receive(&w,gtk0_m3,sizeof gtk0_m3,&out)==NWP_TX&&w.gtk_index==0);nwp_cancel(&w);checks+=4;}
assert(submitted==inline_refused&&port_closes>0);printf("PASS %u owner/model assertions; typed native receipts, mixed RSN parser, no hardware claim\n",checks);return 0;}
