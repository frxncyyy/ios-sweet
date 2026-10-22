/* New private transaction model. No kernel IO, waits, publication or secrets log. */
#include "wifi_station_transaction.h"
#if defined(KERNEL) || defined(_KERNEL)
#include <sys/systm.h>
#else
#include <string.h>
#endif
static void erase(void*v,size_t n){volatile uint8_t*p=v;while(n--)*p++=0;}
static unsigned le16(const uint8_t*p){return p[0]|(unsigned)p[1]<<8;}
int stc_scan_acquire(struct stc_arbiter*a,uint64_t g){if(!a||!g||a->mode)return -1;a->mode=1;a->generation=g;return 0;}
int stc_scan_release(struct stc_arbiter*a,uint64_t g){if(!a||a->mode!=1||a->generation!=g)return -1;a->mode=0;a->generation=0;return 0;}
static int emit(struct stc*s,unsigned phase,unsigned kind){
 erase(&s->action,sizeof s->action);s->phase=phase;
 if(s->serial==UINT64_MAX){s->phase=STC_HALTED;nwp_cancel(&s->wpa);return -1;}
 s->action.generation=s->generation;s->action.ticket=++s->serial;s->action.kind=kind;return 1;
}
static int halt(struct stc*s){if(s->outcome==STC_AUTHORIZED)s->outcome=STC_FAILED;erase(s->queued_eapol,sizeof s->queued_eapol);nwp_cancel(&s->wpa);erase(&s->action,sizeof s->action);erase(s->response,sizeof s->response);s->phase=STC_HALTED;return -1;}
static int release_done(struct stc*s){
 if(!s->arbiter||s->arbiter->mode!=2||s->arbiter->generation!=s->generation)return halt(s);
 s->arbiter->mode=0;s->arbiter->generation=0;s->authenticated=s->associated=0;s->phase=STC_DONE;erase(&s->action,sizeof s->action);erase(&s->peer,sizeof s->peer);return 1;
}
static int command_key(struct stc*s,int group,int remove){
 struct sw_key k={0};size_t used=0;k.bss=s->channel.bss;k.wlan=group?s->group_wlan:s->sta_wlan;k.pairwise=!group;k.key_id=group?s->gtk_index:0;memcpy(k.ap,s->peer.ap,6);
 if(!remove){k.key=group?s->wpa.gtk:s->wpa.tk;k.key_len=16;if(group)s->gtk_attempted=1;else s->ptk_attempted=1;}
 if(emit(s,remove?(group?STC_REMOVE_GTK:STC_REMOVE_PTK):(group?STC_GTK:STC_PTK),remove?(group?STC_DELETE_GTK:STC_DELETE_PTK):(group?STC_CID_GTK:STC_CID_PTK))<0)return -1;
 int r=remove?sw_key_remove(s->action.payload,sizeof s->action.payload,&used,&k):sw_key_add(s->action.payload,sizeof s->action.payload,&used,&k);
 if(r)return halt(s);s->action.bytes=(unsigned)used;return 1;
}
static int abort_channel(struct stc*s){s->channel_held=0;struct sw_channel c=s->channel;c.abort=1;size_t used=0;if(emit(s,STC_RELEASE,STC_ABORT_CHANNEL)<0)return -1;if(sw_channel(s->action.payload,sizeof s->action.payload,&used,&c))return halt(s);s->action.bytes=(unsigned)used;return 1;}
static int cleanup_next(struct stc*s){
 if(s->cleanup_abort_first&&s->channel_held){s->cleanup_abort_first=0;return abort_channel(s);}
 if(s->authenticated){s->authenticated=0;if(emit(s,STC_DEAUTH,STC_TX_DEAUTH)<0)return -1;
  uint8_t*p=s->action.payload;p[0]=0xc0;memcpy(p+4,s->peer.ap,6);memcpy(p+10,s->peer.self,6);memcpy(p+16,s->peer.ap,6);p[24]=3;s->action.bytes=26;return 1;
 }
 if(s->ptk_attempted){s->ptk_attempted=0;return command_key(s,0,1);}
 if(s->gtk_attempted){s->gtk_attempted=0;return command_key(s,1,1);}
 if(s->peer_prepared){s->peer_prepared=0;return emit(s,STC_RETIRE,STC_RETIRE_PEER);}
 if(s->channel_held)return abort_channel(s);
 return release_done(s);
}
static int stop(struct stc*s,unsigned outcome){
 if(s->phase>=STC_DEAUTH)return 0;
 s->outcome=outcome;if(s->last_ms>UINT64_MAX-s->cleanup_timeout_ms)return halt(s);s->cleanup_deadline=s->last_ms+s->cleanup_timeout_ms;nwp_cancel(&s->wpa);erase(s->ssid,sizeof s->ssid);erase(s->rates,sizeof s->rates);erase(s->response,sizeof s->response);erase(s->queued_eapol,sizeof s->queued_eapol);s->eapol_pending=s->eapol_bytes=0;s->response_bytes=s->response_seen=0;
 if(s->phase<STC_EAPOL){s->cleanup_abort_first=s->channel_held;if(!s->associated)s->authenticated=0;}

 erase(&s->action,sizeof s->action);return cleanup_next(s);
}
static int send_auth(struct stc*);
static int send_assoc(struct stc*);
int stc_tick(struct stc*s,uint64_t now){
 if(!s||!s->attempted||s->phase==STC_NEW||s->phase==STC_DONE||s->phase==STC_HALTED)return 0;
 if(!s->arbiter||s->arbiter->mode!=2||s->arbiter->generation!=s->generation)return halt(s);
 if(now<s->last_ms)return halt(s);s->last_ms=now;
 if(s->phase>=STC_DEAUTH){if(now>=s->cleanup_deadline)return halt(s);return 0;}
 if(s->outcome!=STC_AUTHORIZED&&now>=s->deadline)return stop(s,STC_TIMEOUT);
 /* Start response timer only after authoritative air completion. Two total
  * requests per stage, always within the actual remaining grant deadline. */
 if((s->phase==STC_AUTH_RX||s->phase==STC_ASSOC_RX)&&!s->response_seen&&s->response_deadline&&now>=s->response_deadline){
  unsigned attempts=s->phase==STC_AUTH_RX?s->auth_attempts:s->assoc_attempts;
  if(attempts>=2)return stop(s,STC_TIMEOUT);
  return s->phase==STC_AUTH_RX?send_auth(s):send_assoc(s);
 }
 return 0;
}
static int current(struct stc*s,uint64_t gen,uint64_t now){
 if(!s||gen!=s->generation||!s->attempted||s->phase==STC_DONE||s->phase==STC_HALTED)return 0;
 unsigned before=s->phase;if(stc_tick(s,now)<0||s->phase!=before)return 0;return 1;
}
int stc_begin(struct stc*s,struct stc_arbiter*a,const struct stc_config*c,uint64_t now){
 if(!s||!a||!c||s->attempted||a->mode||a->connection_used||!c->generation||c->deadline_ms<=now||!c->cleanup_timeout_ms||c->sta_wlan==255||c->group_wlan==255||c->sta_wlan==c->group_wlan)return -1;
 uint8_t preflight[640];size_t used;
 if(sw_auth(preflight,sizeof preflight,&used,&c->assoc.peer)||sw_assoc(preflight,sizeof preflight,&used,&c->assoc)||sw_channel(preflight,sizeof preflight,&used,&c->channel)||c->channel.abort)return -1;
 memset(s,0,sizeof *s);s->attempted=1;s->generation=c->generation;s->last_ms=now;s->deadline=c->deadline_ms;s->cleanup_timeout_ms=c->cleanup_timeout_ms;
 if(nwp_begin_ap(&s->wpa,c->generation,c->pmk,c->assoc.peer.self,c->assoc.peer.ap,c->station_rsn,c->ap_rsn,c->ap_rsn_bytes?c->ap_rsn_bytes:22,c->rng,c->rng_context)||nwp_copy_ap_rsnxe(&s->wpa,c->ap_ies,c->ap_ies_bytes)){erase(s,sizeof *s);s->attempted=1;return -1;}
 s->arbiter=a;a->mode=2;a->connection_used=1;a->generation=c->generation;s->peer=c->assoc.peer;s->channel=c->channel;s->sta_wlan=c->sta_wlan;s->group_wlan=c->group_wlan;s->assoc=c->assoc;
 memcpy(s->ssid,c->assoc.ssid,c->assoc.ssid_len);memcpy(s->rates,c->assoc.rates,c->assoc.rates_len);s->assoc.ssid=s->ssid;s->assoc.rates=s->rates;s->peer_prepared=1;return emit(s,STC_PREPARE,STC_PREPARE_PEER);
}
const struct stc_action*stc_next(const struct stc*s){return s&&s->action.kind?&s->action:0;}
static int send_auth(struct stc*s){size_t n=0;s->response_seen=0;s->response_deadline=0;s->auth_attempts++;if(emit(s,STC_AUTH_TX,STC_TX_AUTH)<0)return -1;if(sw_auth(s->action.payload,sizeof s->action.payload,&n,&s->peer))return stop(s,STC_FAILED);s->action.bytes=(unsigned)n;return 1;}
static int send_assoc(struct stc*s){size_t n=0;s->response_seen=0;s->response_deadline=0;s->assoc_attempts++;if(emit(s,STC_ASSOC_TX,STC_TX_ASSOC)<0)return -1;if(sw_assoc(s->action.payload,sizeof s->action.payload,&n,&s->assoc))return stop(s,STC_FAILED);s->action.bytes=(unsigned)n;return 1;}
static int response_done(struct stc*s){
 if(!s->response_seen)return 0;
 if(s->phase==STC_AUTH_RX){s->authenticated=1;return send_assoc(s);}
 if(s->phase==STC_ASSOC_RX){s->associated=1;if(emit(s,STC_CONFIGURE,STC_CONFIGURE_PEER)<0)return -1;memcpy(s->action.payload,s->response,s->response_bytes);s->action.bytes=s->response_bytes;s->response_seen=0;erase(s->response,sizeof s->response);return 1;}
 return 0;
}
static int consume_eapol(struct stc*s,uint64_t now){
 if(!s->eapol_pending)return 1;
 if(s->phase!=STC_EAPOL&&s->phase!=STC_PTK_READY&&s->phase!=STC_CONNECTED)return 1;
 uint8_t p[640];unsigned n=s->eapol_bytes;memcpy(p,s->queued_eapol,n);erase(s->queued_eapol,sizeof s->queued_eapol);s->eapol_pending=s->eapol_bytes=0;
 int r=stc_eapol(s,s->generation,p,n,now);erase(p,sizeof p);return r;
}
int stc_receipt(struct stc*s,uint64_t gen,uint64_t ticket,int success,uint64_t now){
 if(!current(s,gen,now)||!ticket||ticket!=s->action.ticket||!s->action.kind)return 0;
 unsigned phase=s->phase;erase(&s->action,sizeof s->action);
 /* Deauthentication is best-effort over air, not a firmware retirement
  * barrier. A negative air result must still run actual key/peer cleanup.
  * Failed command deletion/retirement remains fatal. Native unknown-DMA
  * failures never deliver an air receipt and keep their retained halt. */
 if(success!=1){if(phase==STC_DEAUTH)return cleanup_next(s);if(phase>=STC_DEAUTH)return halt(s);return stop(s,STC_FAILED);}
 size_t n=0;
 switch(phase){
 case STC_PREPARE:s->channel_held=1;if(emit(s,STC_CHANNEL,STC_CID_CHANNEL)<0)return -1;if(sw_channel(s->action.payload,sizeof s->action.payload,&n,&s->channel))return stop(s,STC_FAILED);s->action.bytes=(unsigned)n;return 1;
 case STC_CHANNEL:s->phase=STC_GRANT;if(s->grant_seen)return stc_grant(s,gen,s->channel.bss,s->channel.token,s->channel.band,s->channel.channel,1,now);return 1;
 case STC_AUTH_TX:s->phase=STC_AUTH_RX;s->response_deadline=now>UINT64_MAX-524?UINT64_MAX:now+524;return response_done(s);
 case STC_ASSOC_TX:s->phase=STC_ASSOC_RX;s->response_deadline=now>UINT64_MAX-524?UINT64_MAX:now+524;return response_done(s);
 case STC_CONFIGURE:/* Actual peer/BSS/RLM and station-activation receipt, no invented grant release. */s->phase=STC_EAPOL;return consume_eapol(s,now);
 case STC_EAPOL_TX:{int r=nwp_security_submitted(&s->wpa,gen,s->wpa_tx_ticket,1);s->wpa_tx_ticket=0;if(r==NWP_INSTALL){s->gtk_index=s->wpa.gtk_index;return command_key(s,0,0);}if(r<0)return stop(s,STC_FAILED);s->phase=s->resume_phase;s->resume_phase=0;if(s->phase==STC_PTK_READY&&s->key_event_seen){if(nwp_keys_ready(&s->wpa,gen,s->wpa.install_ticket,1,1)<0)return stop(s,STC_FAILED);return command_key(s,1,0);}return consume_eapol(s,now);}
 case STC_PTK:s->phase=STC_PTK_READY;if(s->key_event_seen){if(nwp_keys_ready(&s->wpa,gen,s->wpa.install_ticket,1,1)<0)return stop(s,STC_FAILED);return command_key(s,1,0);}return consume_eapol(s,now); /* submission, not key-ready */
 case STC_GTK:/* Source no-response host completion/replay initialization, NOT firmware ACK. */if(nwp_keys_ready(&s->wpa,gen,s->wpa.install_ticket,2,1)<0)return stop(s,STC_FAILED);return emit(s,STC_DATA,STC_INSTALL_DATA_PATH);
 case STC_DATA:if(nwp_keys_ready(&s->wpa,gen,s->wpa.install_ticket,4,1)!=NWP_AUTH)return stop(s,STC_FAILED);s->phase=STC_CONNECTED;s->outcome=STC_AUTHORIZED;return consume_eapol(s,now);
 case STC_DEAUTH:case STC_REMOVE_PTK:case STC_REMOVE_GTK:case STC_RETIRE:case STC_RELEASE:return cleanup_next(s);
 default:return halt(s);
 }
}
int stc_grant(struct stc*s,uint64_t gen,uint8_t bss,uint8_t token,uint8_t band,uint8_t ch,int granted,uint64_t now){
 if(!current(s,gen,now)||(s->phase!=STC_GRANT&&s->phase!=STC_CHANNEL)||bss!=s->channel.bss||token!=s->channel.token||band!=s->channel.band||ch!=s->channel.channel)return 0;
 if(granted!=1)return stop(s,STC_FAILED);s->grant_seen=1;if(s->phase==STC_CHANNEL)return 1;return send_auth(s);
}
int stc_management(struct stc*s,uint64_t gen,const uint8_t*p,size_t n,uint64_t now){
 if(!current(s,gen,now)||!p||n<24)return 0;
 unsigned fc=le16(p);if((fc&0x4703)||memcmp(p+4,s->peer.self,6)||memcmp(p+10,s->peer.ap,6)||memcmp(p+16,s->peer.ap,6))return 0;
 if((fc&0xfc)==0xc0||(fc&0xfc)==0xa0){if(n<26)return 0;s->authenticated=0;return stop(s,STC_DISCONNECTED);}
 int auth=s->phase==STC_AUTH_TX||s->phase==STC_AUTH_RX,assoc=s->phase==STC_ASSOC_TX||s->phase==STC_ASSOC_RX;
 if((!auth&&!assoc)||s->response_seen)return 0;
 if(n<30||n>sizeof s->response||(fc&0xfc)!=(auth?0xb0u:0x10u))return 0;
 if(auth){if(le16(p+24)!=0||le16(p+26)!=2)return 0;for(size_t i=30;i<n;){if(n-i<2||p[i+1]>n-i-2)return 0;i+=2+p[i+1];}if(le16(p+28))return stop(s,STC_FAILED);}
 else{if(le16(p+26))return stop(s,STC_FAILED);unsigned aid=le16(p+28)&0x3fff;if(!aid||aid>2007)return stop(s,STC_FAILED);for(size_t i=30;i<n;){if(n-i<2||p[i+1]>n-i-2)return stop(s,STC_FAILED);i+=2+p[i+1];}s->aid=(uint16_t)aid;}
 memcpy(s->response,p,n);s->response_bytes=(unsigned)n;s->response_seen=1;return response_done(s);
}
int stc_eapol(struct stc*s,uint64_t gen,const uint8_t*p,size_t n,uint64_t now){
 if(!current(s,gen,now)||s->phase<STC_CONFIGURE||s->phase>=STC_DEAUTH)return 0;
 if(!p||!n||n>640)return stop(s,STC_FAILED);
 if(s->phase!=STC_EAPOL&&s->phase!=STC_PTK_READY&&s->phase!=STC_CONNECTED){
  if(s->eapol_pending)return s->eapol_bytes==n&&!memcmp(s->queued_eapol,p,n)?0:2;
  memcpy(s->queued_eapol,p,n);s->eapol_bytes=(unsigned)n;s->eapol_pending=1;return 1;
 }
 unsigned resume=s->phase;struct nwp_result r;int result=nwp_receive(&s->wpa,p,n,&r);
 if(result<0){erase(&r,sizeof r);return stop(s,STC_FAILED);}if(result!=NWP_TX){erase(&r,sizeof r);return 0;}
 if(emit(s,STC_EAPOL_TX,STC_TX_EAPOL)<0){erase(&r,sizeof r);return -1;}s->resume_phase=resume;s->wpa_tx_ticket=r.ticket;memcpy(s->action.payload,r.frame,r.bytes);s->action.bytes=r.bytes;erase(&r,sizeof r);return 1;
}
int stc_key_event(struct stc*s,uint64_t gen,const uint8_t*p,size_t n,uint64_t now){
 if(!current(s,gen,now)||(s->phase!=STC_PTK_READY&&s->phase!=STC_PTK&&!(s->phase==STC_EAPOL_TX&&s->resume_phase==STC_PTK_READY))||!p||n!=8||p[0]!=s->channel.bss||memcmp(p+2,s->peer.ap,6))return 0;
 s->key_event_seen=1;if(s->phase!=STC_PTK_READY)return 1;
 if(nwp_keys_ready(&s->wpa,gen,s->wpa.install_ticket,1,1)<0)return stop(s,STC_FAILED);return command_key(s,1,0);
}
int stc_link_lost(struct stc*s,uint64_t gen,uint64_t now){if(!current(s,gen,now))return 0;s->authenticated=0;return stop(s,STC_DISCONNECTED);}
int stc_cancel(struct stc*s,uint64_t gen,uint64_t now){if(!current(s,gen,now))return 0;return stop(s,STC_CANCELED);}

int stc_channel_released(struct stc*s,uint64_t gen,uint8_t token,uint64_t now){if(!current(s,gen,now)||token!=s->channel.token||s->phase>=STC_DEAUTH)return 0;s->channel_held=0;return 1;}
