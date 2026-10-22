#include "wifi_wpa_owner.h"
#if defined(KERNEL) || defined(_KERNEL)
#include <sys/systm.h>
#else
#include <string.h>
#endif
static void wipe(void*v,size_t n){volatile uint8_t*p=v;while(n--)*p++=0;}
static unsigned expected(unsigned k){switch(k){
case STC_PREPARE_PEER:return NWO_PREPARED;
case STC_TX_AUTH:case STC_TX_ASSOC:case STC_TX_EAPOL:case STC_TX_DEAUTH:return NWO_AIR_TX;
case STC_CONFIGURE_PEER:return NWO_CONFIGURED;
case STC_CID_GTK:return NWO_GTK_REPLAY;
case STC_INSTALL_DATA_PATH:return NWO_DATA_PATH;
case STC_RETIRE_PEER:return NWO_RETIRED;
case STC_CID_CHANNEL:case STC_CID_PTK:case STC_DELETE_PTK:case STC_DELETE_GTK:case STC_ABORT_CHANNEL:return NWO_COMMAND;
default:return 0;}}
static int close_port(struct nwo*s){if(s->ops.close_port(s->context)){s->halted=1;nwp_cancel(&s->transaction.wpa);wipe(&s->transaction.action,sizeof s->transaction.action);wipe(s->transaction.queued_eapol,sizeof s->transaction.queued_eapol);return -1;}s->closed=1;return 0;}
static int sync_state(struct nwo*s,int result){
 if(s->transaction.outcome!=STC_PENDING&&s->transaction.outcome!=STC_AUTHORIZED){if(close_port(s))return -1;}
 if(s->transaction.phase==STC_HALTED){s->halted=1;return -1;}return result;
}
int nwo_begin(struct nwo*s,struct stc_arbiter*a,const struct stc_config*c,const struct nwo_ops*ops,void*ctx,uint64_t now){
 if(!s||!ops||!ops->submit||!ops->close_port||s->transaction.attempted||s->halted)return -1;
 s->ops=*ops;s->context=ctx;if(close_port(s))return -1;
 return stc_begin(&s->transaction,a,c,now);
}
int nwo_authorized(const struct nwo*s){return s&&!s->halted&&!s->closed&&s->transaction.phase<STC_DEAUTH&&s->transaction.outcome==STC_AUTHORIZED;}
int nwo_pump(struct nwo*s,uint64_t now){
 if(!s||s->halted||s->issuing)return -1;
 int r=sync_state(s,stc_tick(&s->transaction,now));if(r<0)return r;
 if(s->in_ticket)return 0;const struct stc_action*a=stc_next(&s->transaction);if(!a)return 0;
 if(!expected(a->kind)){s->halted=1;close_port(s);return -1;}
 struct nwo_install meta={0};if(a->kind==STC_CID_GTK){meta.gtk_index=s->transaction.wpa.gtk_index;memcpy(meta.rsc,s->transaction.wpa.rsc,8);}
 s->in_generation=a->generation;s->in_ticket=a->ticket;s->in_kind=a->kind;s->issuing=1;
 r=s->ops.submit(s->context,a,&meta);s->issuing=0;wipe(&meta,sizeof meta);
 if(r==1){s->in_ticket=0;s->in_kind=0;return 0;}
 if(r){uint64_t g=s->in_generation,t=s->in_ticket;s->in_ticket=0;s->in_kind=0;return sync_state(s,stc_receipt(&s->transaction,g,t,0,now));}return 1;
}
int nwo_complete(struct nwo*s,uint64_t g,uint64_t t,unsigned receipt,int success,uint64_t now){
 if(!s||s->halted||s->issuing||!t||g!=s->in_generation||t!=s->in_ticket)return 0;
 if(receipt!=expected(s->in_kind)||!(success==0||success==1))return -1;
 unsigned kind=s->in_kind;s->in_ticket=0;s->in_kind=0;
 /* Canceled/timed-out transaction can now dispatch its cleanup; old receipts
  * retire adapter storage only and cannot complete the new model action. */
 const struct stc_action*a=stc_next(&s->transaction);
 if(!a||a->ticket!=t||a->generation!=g)return sync_state(s,0);
 int r=stc_receipt(&s->transaction,g,t,success,now);
 if(kind==STC_INSTALL_DATA_PATH&&success&&s->transaction.outcome==STC_AUTHORIZED)s->closed=0;
 return sync_state(s,r);
}
int nwo_link_lost(struct nwo*s,uint64_t g,uint64_t now){if(!s||s->halted||s->issuing||g!=s->transaction.generation)return 0;if(close_port(s))return -1;return sync_state(s,stc_link_lost(&s->transaction,g,now));}
int nwo_cancel(struct nwo*s,uint64_t g,uint64_t now){if(!s||s->halted||s->issuing||g!=s->transaction.generation)return 0;if(close_port(s))return -1;return sync_state(s,stc_cancel(&s->transaction,g,now));}
int nwo_eapol(struct nwo*s,uint64_t g,const uint8_t*p,size_t n,uint64_t now){if(!s||s->halted||s->issuing)return 0;return sync_state(s,stc_eapol(&s->transaction,g,p,n,now));}
int nwo_management(struct nwo*s,uint64_t g,const uint8_t*p,size_t n,uint64_t now){if(!s||s->halted||s->issuing)return 0;return sync_state(s,stc_management(&s->transaction,g,p,n,now));}
int nwo_grant(struct nwo*s,uint64_t g,uint8_t b,uint8_t t,uint8_t band,uint8_t ch,int ok,uint64_t now){if(!s||s->halted||s->issuing)return 0;return sync_state(s,stc_grant(&s->transaction,g,b,t,band,ch,ok,now));}
int nwo_key24(struct nwo*s,uint64_t g,const uint8_t*p,size_t n,uint64_t now){
 if(!s||s->halted||s->issuing)return 0;
 /* No RX event may qualify a PTK before the native submit has been accepted. */
 if(s->transaction.phase==STC_PTK&&s->in_kind!=STC_CID_PTK)return 0;
 return sync_state(s,stc_key_event(&s->transaction,g,p,n,now));
}
