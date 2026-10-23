#include "wifi_ios_station_view.h"
#include "wifi_ios_policy.h"
#include "wifi_ios_mapping.h"
#include "wifi_ios_scan_contract.h"
static void p16(uint8_t*p,uint16_t v){p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);}
static void p32(uint8_t*p,uint32_t v){p16(p,(uint16_t)v);p16(p+2,(uint16_t)(v>>16));}
static void cp(void*v,const void*s,size_t n){uint8_t*p=v;const uint8_t*q=s;while(n--)*p++=*q++;}
static int valid(const struct njs_snapshot*s){return s&&s->phase<=NJS_FAILED&&s->associated<=1&&s->ptk_ready<=1&&s->gtk_ready<=1&&s->data_ready<=1&&s->security<=NIWJ_OPEN&&s->involuntary<=1&&(s->result==0||s->result==1||s->result==2||s->result==9||s->result==21);}
int niwj_authorized(const struct njs_snapshot*s){return valid(s)&&s->native_generation&&s->ticket&&s->phase==NJS_AUTHORIZED&&s->result==1&&s->associated&&s->data_ready&&(s->security==NIWJ_OPEN?(!s->ptk_ready&&!s->gtk_ready&&s->peer.security==NIWJ_OPEN&&!s->peer.record.privacy&&!s->peer.record.rsn_present&&!(s->peer.record.capability&16)):(s->ptk_ready&&s->gtk_ready));}
int niwj_state_reply(const struct njs_snapshot*s,unsigned request,size_t payload,unsigned scanning,uint64_t now,void*v,size_t cap,size_t*used){
 uint8_t*p=v;size_t bytes=0;unsigned scalar=0;if(!used)return -1;*used=0;if(!p||!valid(s)||scanning>1)return -1;
 if(request==13||request==21){if(payload||cap<4)return -1;
  if(request==21){scalar=s->result;if(scalar==1&&!niwj_authorized(s))return 1;}
  else if(niwj_authorized(s))scalar=4;
  else if(s->phase==NJS_AUTHENTICATING)scalar=2;
  else if(s->phase==NJS_ASSOCIATING||s->phase==NJS_SECURING)scalar=3;
  else scalar=scanning?1:0;
  p32(p,scalar);*used=4;return 0;
 }
 switch(request){case 1:bytes=s->peer.record.ssid_len;break;case 9:bytes=6;break;case 2:case 4:bytes=16;break;case 16:bytes=52;break;case 103:bytes=1164;break;default:return -2;}
 /* Preserve the established IOUC fixed-shape refusal before no-association.
  * Unauthenticated state cannot make a malformed user output shape valid. */
 if(request==1){if(!payload||payload>32||!cap)return -1;}
 else if(payload!=bytes||cap<bytes)return -1;
 if(!niwj_authorized(s))return 1;
 if(!s->peer.record.ssid_present||!bytes||s->peer.record.ssid_len>32||s->peer.record.ie_bytes>NSB_IE_BYTES||s->peer.rate_count>12||
  (s->peer.band!=1&&s->peer.band!=2)||!s->peer.channel||s->peer.channel!=s->peer.record.channel)return -1;
 if(cap<bytes||(request==1?payload<bytes:payload!=bytes))return -1;
 if(request==103){struct niws_result r={0};size_t n=0,count=0;const struct niwp_rate_map*map=niwi_rate_map(&n);
  if(!now||!s->peer.record.observed_us||s->peer.record.observed_us>now||(now-s->peer.record.observed_us)/1000>UINT32_MAX||
   s->peer.record.ie_bytes!=s->peer.record.ie_total||s->peer.record.ies_truncated||
   niwp_rates(map,n,s->peer.rates,s->peer.rate_count,r.rates,NIWS_MAX_RATES,&count))return -1;
  r.channel=s->peer.channel;r.channel_flags=(s->peer.band==1?8:16)|((s->peer.band==2&&s->peer.channel>=52&&s->peer.channel<=144)?0x100:0);r.age=(uint32_t)((now-s->peer.record.observed_us)/1000);
  r.rssi=s->peer.rssi_dbm;r.beacon_interval=s->peer.record.interval;r.capabilities=s->peer.record.capability;
  r.rate_count=(uint8_t)count;r.ssid_bytes=s->peer.record.ssid_len;r.ie_bytes=s->peer.record.ie_bytes;
  cp(r.bssid,s->peer.record.bssid,6);cp(r.ssid,s->peer.record.ssid,r.ssid_bytes);cp(r.ie,s->peer.record.ies,r.ie_bytes);
  if(niws_encode_result(&r,p,1164))return -1;
 }else{
  niwj_clear(p,bytes);
  if(request==1)cp(p,s->peer.record.ssid,bytes);
  else if(request==9)cp(p,s->peer.record.bssid,6);
  else if(request==2){p32(p,1);p32(p+4,1);p32(p+8,s->security==NIWJ_OPEN?0:8);}
  else if(request==4){p32(p,1);p32(p+4,1);p32(p+8,s->peer.channel);p32(p+12,s->peer.band==1?8:16);}
  else{p32(p,1);p32(p+4,1);p32(p+12,(uint32_t)(int32_t)s->peer.rssi_dbm);p32(p+28,(uint32_t)(int32_t)s->peer.rssi_dbm);p32(p+32,(uint32_t)(int32_t)s->peer.rssi_dbm);p32(p+48,(uint32_t)(int32_t)s->peer.rssi_dbm);}
 }
 *used=bytes;return 0;
}
int niwj_notice_record(const struct njs_snapshot*s,unsigned event,const char name[16],void*v,size_t cap,size_t*used){
 uint8_t*p=v;size_t bytes=event==4?80:48;unsigned terminated=0;
 if(!used)return -1;*used=0;if(!valid(s)||!name||!name[0]||!p||cap<bytes)return -1;
 for(unsigned i=0;i<16;i++)if(!name[i])terminated=1;if(!terminated)return -1;
 if(event==9){if(s->result!=1&&s->result!=2&&s->result!=9&&s->result!=21)return -1;if(s->result==1&&!niwj_authorized(s))return -1;}
 else if(event==2||event==3){if(!niwj_authorized(s)&&s->phase!=NJS_IDLE&&s->phase!=NJS_FAILED&&s->phase!=NJS_LEAVING)return -1;}
 else if(event==4){if(!niwj_authorized(s)&&(s->associated||s->ptk_ready||s->gtk_ready||s->data_ready))return -1;}
 else return -1;
 niwj_clear(p,bytes);p32(p,87);p16(p+24,event==4?32:0);p32(p+28,event);for(unsigned i=0;i<16&&name[i];i++)p[32+i]=(uint8_t)name[i];
 if(event==4){p[48]=niwj_authorized(s)?0:1;p32(p+52,niwj_authorized(s)?(uint32_t)(int32_t)s->peer.rssi_dbm:0);p[64]=(uint8_t)s->involuntary;p32(p+68,s->reason);p32(p+72,s->subreason);}
 *used=bytes;return 0;
}
