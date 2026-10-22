#include "wifi_wpa_nst.h"
#include "wifi_station_diag.h"
#if defined(KERNEL) || defined(_KERNEL)
#include <sys/systm.h>
#else
#include <string.h>
#endif
static void clear(void*v,size_t n){volatile uint8_t*p=v;while(n--)*p++=0;}
static int close_native(void*ctx){struct nwn*s=ctx;nst_close_port(s->transport);return s->transport->controlled_port?-1:0;}
static int sync_cleanup(struct nwn*s){
 if(s->owner.transaction.phase<STC_DEAUTH||s->cleanup_started)return 0;
 uint64_t deadline=s->owner.transaction.cleanup_deadline;
 if(deadline>UINT64_MAX/1000||s->now_ms>UINT64_MAX/1000)return -1;
 uint64_t now=s->now_ms*1000;if(now<s->transport->last)now=s->transport->last;
 if(nst_cleanup_begin(s->transport,now,deadline*1000))return -1;s->cleanup_started=1;return 0;
}
static int submit_native(void*ctx,const struct stc_action*a,const struct nwo_install*m){
 struct nwn*s=ctx;struct nst*t=s->transport;
 if(sync_cleanup(s))return -1;
 if(t->wire_busy||t->air_pending)return 1;
 switch(a->kind){
 case STC_PREPARE_PEER:case STC_CONFIGURE_PEER:case STC_RETIRE_PEER:
  return s->peer_submit(s->peer_context,a,&s->owner.transaction);
 case STC_CID_CHANNEL:case STC_ABORT_CHANNEL:return nst_command(t,a->ticket,SW_CHANNEL,a->payload,a->bytes);
 case STC_CID_PTK:case STC_CID_GTK:case STC_DELETE_PTK:case STC_DELETE_GTK:{
  int r=nst_command(t,a->ticket,SW_KEY,a->payload,a->bytes);
  if(!r){s->install=*m;s->key_pairwise=a->kind==STC_CID_PTK;}
  return r;}
 case STC_TX_AUTH:case STC_TX_ASSOC:case STC_TX_DEAUTH:return nst_management(t,a->ticket,a->payload,a->bytes);
 case STC_TX_EAPOL:{
  uint8_t frame[654];if(a->bytes>640)return -1;
  memcpy(frame,t->peer.ap,6);memcpy(frame+6,t->peer.self,6);frame[12]=0x88;frame[13]=0x8e;memcpy(frame+14,a->payload,a->bytes);
  int r=nst_eapol(t,a->ticket,frame,a->bytes+14,0);clear(frame,sizeof frame);return r;}
 /* Pure readiness is performed by pump outside RX callback, then delivered
  * as an explicit local receipt, not a fabricated firmware acknowledgement. */
 case STC_INSTALL_DATA_PATH:return nst_authorize(t)?-1:0;
 default:return -1;
 }
}
int nwn_begin(struct nwn*s,struct nst*t,struct stc_arbiter*a,const struct stc_config*c,nwn_peer_submit peer,void*ctx,uint64_t now){
 if(!s||!t||!c||!peer||!t->prepared||t->halt||t->generation!=c->generation||t->peer.bss!=c->channel.bss||t->peer.wlan!=c->sta_wlan||t->peer.group_wlan!=c->group_wlan||memcmp(t->peer.ap,c->assoc.peer.ap,6)||memcmp(t->peer.self,c->assoc.peer.self,6))return -1;
 s->overall_deadline=c->deadline_ms;s->transport=t;s->peer_submit=peer;s->peer_context=ctx;s->now_ms=now;
 const struct nwo_ops ops={submit_native,close_native};return nwo_begin(&s->owner,a,c,&ops,s,now);
}
int nwn_pump(struct nwn*s,uint64_t now){
 if(!s)return -1;s->now_ms=now;int r=nwo_pump(&s->owner,now);if(r>=0&&sync_cleanup(s))return -1;
 if(r==1&&s->owner.in_kind==STC_INSTALL_DATA_PATH){uint64_t g=s->owner.in_generation,t=s->owner.in_ticket;return nwo_complete(&s->owner,g,t,NWO_DATA_PATH,1,now);}return r;
}
int nwn_peer_complete(struct nwn*s,uint64_t g,uint64_t t,unsigned type,int success,uint64_t now){if(!s||(type!=NWO_PREPARED&&type!=NWO_CONFIGURED&&type!=NWO_RETIRED))return -1;s->now_ms=now;int r=nwo_complete(&s->owner,g,t,type,success,now);return r>=0&&sync_cleanup(s)?-1:r;}
int nwn_cancel(struct nwn*s,uint64_t g,uint64_t now){if(!s)return -1;clear(&s->install,sizeof s->install);s->now_ms=now;int r=nwo_cancel(&s->owner,g,now);return r>=0&&sync_cleanup(s)?-1:r;}
static int event_inner(struct nwn*s,const struct nst_event*e,uint64_t now){
 if(!s||!e||e->generation!=s->owner.transaction.generation)return 0;
 switch(e->kind){
 case NST_COMMAND_DMA:{
  if(e->ticket!=s->owner.in_ticket)return 0;
  unsigned kind=s->owner.in_kind,type=NWO_COMMAND;int ok=e->status==0;
  if(kind==STC_TX_AUTH||kind==STC_TX_ASSOC||kind==STC_TX_EAPOL||kind==STC_TX_DEAUTH){if(ok)return 0;return nwo_complete(&s->owner,e->generation,e->ticket,NWO_AIR_TX,0,now);} /* DMA success is never air TX */
  if(kind==STC_CID_GTK)type=NWO_GTK_REPLAY;
  const struct stc_action *current=stc_next(&s->owner.transaction);
  if(ok&&current&&current->ticket==e->ticket&&(kind==STC_CID_PTK||kind==STC_CID_GTK))ok=nst_replay_install(s->transport,s->key_pairwise,s->key_pairwise?0:s->install.gtk_index,s->install.rsc)==0;
  int r=nwo_complete(&s->owner,e->generation,e->ticket,type,ok,now);clear(&s->install,sizeof s->install);return r;}
 case NST_FRAME_RESULT:return nwo_complete(&s->owner,e->generation,e->ticket,NWO_AIR_TX,e->status==0,now);
 case NST_CHANNEL_GRANT:{int r=nwo_grant(&s->owner,e->generation,e->bss,e->token,e->band,e->channel,e->status==0,now);
  if(r>0){uint64_t interval=e->interval_ms>10?e->interval_ms-10:0;if(now>UINT64_MAX-interval)return nwo_link_lost(&s->owner,e->generation,now);uint64_t limit=now+interval;if(limit<s->owner.transaction.deadline)s->owner.transaction.deadline=limit;}return r;}
 case NST_CHANNEL_RELEASED:{int r=stc_channel_released(&s->owner.transaction,e->generation,e->token,now);if(r>0&&s->owner.transaction.phase<STC_DEAUTH)s->owner.transaction.deadline=s->overall_deadline;return r;}
 case NST_KEY_READY:return nwo_key24(&s->owner,e->generation,e->body,e->bytes,now);
 case NST_MANAGEMENT:return nwo_management(&s->owner,e->generation,e->body,e->bytes,now);
 case NST_EAPOL:{const uint8_t*p=e->body;size_t n=e->bytes;
  if(!p||n<14||p[12]!=0x88||p[13]!=0x8e||memcmp(p,s->transport->peer.self,6)||memcmp(p+6,s->transport->peer.ap,6))return 0;
  {struct nstd_record record;nstd_eapol(&record,e->generation,s->owner.in_ticket,p+14,n-14);nstd_log(&record);}
  return nwo_eapol(&s->owner,e->generation,p+14,n-14,now);}
 case NST_LINK_LOST:return nwo_link_lost(&s->owner,e->generation,now);
 case NST_FATAL:return nwn_cancel(s,e->generation,now);
 default:return 0;
 }
}

int nwn_event(struct nwn*s,const struct nst_event*e,uint64_t now){if(!s)return -1;s->now_ms=now;int r=event_inner(s,e,now);return r>=0&&sync_cleanup(s)?-1:r;}
