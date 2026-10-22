#include "wifi_ios_join.h"
static uint16_t j16(const uint8_t*p){return p[0]|(uint16_t)p[1]<<8;}
static uint32_t j32(const uint8_t*p){return j16(p)|(uint32_t)j16(p+2)<<16;}
static int same(const uint8_t*a,const uint8_t*b,size_t n){unsigned d=0;while(n--)d|=*a++^*b++;return !d;}
static int nz(const uint8_t*p,size_t n){unsigned d=0;while(n--)d|=*p++;return d!=0;}
static void copy(void*v,const void*s,size_t n){uint8_t*d=v;const uint8_t*p=s;while(n--)*d++=*p++;}
void niwj_clear(void*v,size_t n){volatile uint8_t*p=v;if(p)while(n--)*p++=0;}
static const uint8_t station_rsn[22]={48,20,1,0,0,15,172,4,1,0,0,15,172,4,1,0,0,15,172,2,0,0};
/* Selected producer 1d3ec138c..15c0 emits optional PMKID0/BIP trailer
 * even with station caps0. We negotiate no PMF: native association/M2 retain
 * exactly station_rsn22, never adopt AP-required capabilities or PMKIDs. */
static int station_rsn_ok(const uint8_t*p,unsigned n){
 if(n==22)return same(p,station_rsn,22);
 if((n!=24&&n!=28)||p[0]!=48||p[1]!=n-2||!same(p+2,station_rsn+2,20)||p[22]||p[23])return 0;
 return n==24||(!p[24]&&p[25]==15&&p[26]==172&&(p[27]==6||p[27]==11||p[27]==12||p[27]==13));
}
static int refuse(struct niwj_diagnostic*d,int reason){if(d)d->reason=(uint32_t)-reason;return reason;}
int niwj_decode_identity(const void*v,size_t n,const uint8_t owner_mac[6],struct niwj_request*out,struct niwj_diagnostic*d){
 const uint8_t*p=v;if(d)niwj_clear(d,sizeof(*d));if(out)niwj_clear(out,sizeof(*out));
 if(!out||!p||n!=NIWJ_ASSOC_BYTES)return refuse(d,NIWJ_SHAPE);
 if(d){d->mode=j16(p+4);d->auth_lower=j16(p+6);d->auth_upper=j16(p+8);d->rsn_bytes=j16(p+204);d->key_kind=j32(p+64);d->key_bytes=j32(p+60);d->native_flags=j32(p+504);d->association_flags=j16(p+463);d->association_reason=p[465];
  d->cached_pmk_bytes=j32(p+468);d->blacklist_count=p[514];d->scope_id=j32(p+536);d->conditional_option=j32(p+580);d->transition_disable=j32(p+584);d->band_preference=j32(p+588);
  d->private_address_present=(p[464]&8u)!=0;
  d->owner_address_valid=owner_mac&&nz(owner_mac,6)&&!(owner_mac[0]&1u);
  d->private_address_matches=d->private_address_present&&d->owner_address_valid&&same(p+508,owner_mac,6);}
 if(j32(p)!=1||!j32(p+12)||j32(p+12)>32||(nz(p+48,6)&&(p[48]&1)))return refuse(d,NIWJ_SHAPE);
 if(j16(p+4)!=2||j16(p+6)!=1||j16(p+8)!=8)return refuse(d,NIWJ_PROFILE);
 if(j32(p+56)!=1||j32(p+60)!=32||j32(p+64)!=6||j16(p+68)||j16(p+70)||j32(p+468))return refuse(d,NIWJ_CREDENTIAL);
 if(!station_rsn_ok(p+206,j16(p+204)))return refuse(d,NIWJ_SECURITY);
 /* Port policy: producer's PMK6 WPA2 profile flags0/4 are handled by our
  * host WPA2 engine. No claim of undocumented vendor offload semantics. */
 /* Exact producer copies a valid LAA to508 and sets bit08; it is not a
  * permission to ignore a different requested self address. Existing native
  * BSS/TX/ifnet identity is immutable, so require actual retained equality. */
 if((p[463]&~0x20u)||(p[464]&~0x48u)||p[465]>2||((p[463]&0x20u)&&!nz(p+48,6))||
    (j32(p+504)!=0&&j32(p+504)!=4)||p[514]||j32(p+580)||j32(p+584)||j32(p+588))return refuse(d,NIWJ_OPTIONS);
 if((p[464]&8u)&&(!owner_mac||!nz(owner_mac,6)||(owner_mac[0]&1u)||
    !nz(p+508,6)||(p[508]&3u)!=2u||!same(p+508,owner_mac,6)))return refuse(d,NIWJ_OPTIONS);
 /* Producer CFStringGetCString(...,37,UTF8), strlen, then stores length536.
  * Match the first terminator exactly; unused bytes after it are inactive.
  * Absence of bit40 leaves this storage inactive but length must be zero. */
 unsigned scope=j32(p+536);
 if(p[464]&0x40u){
  if(!scope||scope>36||p[540+scope])return refuse(d,NIWJ_OPTIONS);
  for(unsigned i=0;i<scope;i++)if(!p[540+i])return refuse(d,NIWJ_OPTIONS);
 }else if(scope)return refuse(d,NIWJ_OPTIONS);
 out->ssid_length=(uint8_t)j32(p+12);copy(out->ssid,p+16,out->ssid_length);
 out->has_target=(uint8_t)nz(p+48,6);copy(out->target,p+48,6);copy(out->pmk,p+72,32);
 copy(out->station_rsn,station_rsn,22);out->association_reason=p[465];out->security=NIWJ_WPA2_PSK_CCMP;out->native_flags=j32(p+504);return 0;
}
int niwj_decode(const void*v,size_t n,struct niwj_request*out,struct niwj_diagnostic*d){
 return niwj_decode_identity(v,n,0,out,d);
}
/* AP offer may contain SAE in addition to PSK. We negotiate PSK/CCMP only,
 * never SAE/PMF. Unknown optional suites do not become our capabilities. */
static int suite(const uint8_t*p,unsigned type){return !p[0]&&p[1]==15&&p[2]==172&&p[3]==type;}
static int rsn_ok(const uint8_t*p,size_t z){
 size_t at=2;unsigned count,psk=0,ccmp=0;
 if(z<22||z>NIWJ_AP_RSN_BYTES||p[0]!=48||(size_t)p[1]+2!=z||j16(p+at)!=1)return 0;at+=2;
 if(!suite(p+at,4))return 0;at+=4;
 count=j16(p+at);at+=2;if(!count||count>(z-at)/4)return 0;
 while(count--){ccmp|=suite(p+at,4);at+=4;}if(!ccmp||z-at<2)return 0;
 count=j16(p+at);at+=2;if(!count||count>(z-at)/4)return 0;
 while(count--){psk|=suite(p+at,2);at+=4;}if(!psk)return 0;
 if(z-at<2)return 0;unsigned caps=j16(p+at);at+=2;if(caps&(64u|2u))return 0; /*MFPR or NoPairwise*/
 /* AP ExtendedKeyID support is optional; station caps0 negotiates key ID0. */
 if(at==z)return 1;
 if(z-at<2)return 0;count=j16(p+at);at+=2;if(count>(z-at)/16)return 0;at+=16*count;
 if(at==z)return 1;
 /* Group-management cipher may be advertised with optional PMF; never used. */
 return z-at==4&&(caps&128u)!=0&&!p[at]&&p[at+1]==15&&p[at+2]==172;
}
static int legacy_rate(unsigned v){return v==2||v==4||v==11||v==22||v==12||v==18||v==24||v==36||v==48||v==72||v==96||v==108;}
struct parsed {unsigned rsn_at,rsn_bytes,count;uint8_t rates[12];int16_t rssi;};
static int candidate(const struct nsb_record*r,const struct niwj_request*q,const struct nss_info*info,uint64_t now,struct parsed*a){
 unsigned seen=0;niwj_clear(a,sizeof(*a));
 if(!r->ssid_present||r->hidden||r->ssid_len!=q->ssid_length||!same(r->ssid,q->ssid,r->ssid_len)||
  !nz(r->bssid,6)||(r->bssid[0]&1)||(q->has_target&&!same(q->target,r->bssid,6))||
  !r->privacy||!(r->capability&1)||(r->capability&2)||r->channel_mismatch||!r->channel||r->channel!=r->hw_channel||
  (r->subtype!=8&&r->subtype!=5)||r->ies_truncated||r->ie_bytes!=r->ie_total||r->ie_bytes>NSB_IE_BYTES||
  !r->rsn_present||!r->observed_us||r->observed_us>now||!r->rxv_valid||!(r->group_valid&4))return 0;
 unsigned band=r->hw_channel<=14?1:2,allowed=0;
 for(unsigned i=0;i<info->count;i++)if(info->channels[i].band==band&&info->channels[i].channel==r->channel)allowed=1;
 if(!allowed)return 0;
 unsigned x=r->rxv[4],y=r->rxv[5];if(x==255&&y==255)return 0;if(x==255)x=0;if(y==255)y=0;x=x>y?x:y;if(x>220)x=220;a->rssi=(int16_t)((int)x/2-110);
 for(size_t i=0;i<r->ie_bytes;){if(r->ie_bytes-i<2)return 0;unsigned id=r->ies[i],z=r->ies[i+1];size_t body=i+2;if(z>r->ie_bytes-body)return 0;
  if(id==48){if(seen&4||!rsn_ok(r->ies+i,z+2))return 0;seen|=4;a->rsn_at=(unsigned)i;a->rsn_bytes=z+2;}
  if(id==1||id==50){unsigned flag=id==1?1:2;if((seen&flag)||!z||(id==1&&z>8))return 0;seen|=flag;
   for(unsigned j=0;j<z;j++){unsigned raw=r->ies[body+j],rate=raw&127;if(!legacy_rate(rate)){if(raw&128)return 0;continue;}
    if(band==2&&(rate==2||rate==4||rate==11||rate==22)){if(raw&128)return 0;continue;}
    unsigned k=0;while(k<a->count&&(a->rates[k]&127)!=rate)k++;if(k==a->count){if(k==12)return 0;a->rates[a->count++]=(uint8_t)raw;}else a->rates[k]|=(uint8_t)(raw&128);}
  }i=body+z;
 }
 return (seen&5)==5&&a->count!=0;
}
int niwj_select(const struct niwj_request*q,const struct nsb_scan*s,const struct nss_info*info,uint64_t now,struct niwj_target*out){
 const struct nsb_record*best=0;struct parsed a,b={0};
 if(!q||!s||!info||!out||q->security!=NIWJ_WPA2_PSK_CCMP||!q->ssid_length||q->ssid_length>32||q->association_reason>2||
  (q->has_target!=0&&q->has_target!=1)||(q->has_target&&(!nz(q->target,6)||(q->target[0]&1)))||
  !same(q->station_rsn,station_rsn,22)||!niwj_scan_usable(s)||(q->native_flags!=0&&q->native_flags!=4)||!now||!info->count||info->count>NSS_CHANNELS||info->band5>1)return NIWJ_TARGET;
 for(unsigned i=0;i<info->count;i++){if((info->channels[i].band!=1&&info->channels[i].band!=2)||!info->channels[i].channel||
   (info->channels[i].band==1&&info->channels[i].channel>14)||
   (info->channels[i].band==2&&(!info->band5||info->channels[i].channel<=14||info->channels[i].channel>196)))return NIWJ_TARGET;
  for(unsigned j=0;j<i;j++)if(info->channels[j].band==info->channels[i].band&&info->channels[j].channel==info->channels[i].channel)return NIWJ_TARGET;}
 for(unsigned i=0;i<s->count;i++)if(candidate(&s->records[i],q,info,now,&a)&&(!best||a.rssi>b.rssi)){best=&s->records[i];b=a;}
 if(!best)return NIWJ_TARGET;
 niwj_clear(out,sizeof(*out));out->record=*best;out->band=best->hw_channel<=14?1:2;out->channel=best->channel;out->rssi_dbm=b.rssi;
 out->rate_count=(uint8_t)b.count;copy(out->rates,b.rates,b.count);out->ap_rsn_bytes=(uint16_t)b.rsn_bytes;copy(out->ap_rsn,best->ies+b.rsn_at,b.rsn_bytes);return 0;
}
