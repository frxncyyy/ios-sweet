#include "wifi_ios_bss.h"
int niwb_bss_metadata(const struct nsb_record*s,struct niwb_bss*out){
 struct niwb_bss r={0};unsigned rate_ies=0;
 if(!s||!out||!s->hw_channel||!s->channel||s->channel_mismatch||s->ssid_len>32||s->ie_bytes>NSB_IE_BYTES||s->ies_truncated||s->ie_bytes!=s->ie_total||(s->subtype!=8&&s->subtype!=5))return -1;
 r.band=s->hw_channel<=14?1:2;r.channel=s->channel;r.interval=s->interval;r.capabilities=s->capability;r.observed_us=s->observed_us;
 r.ssid_bytes=s->ssid_len;r.ssid_present=s->ssid_present;r.hidden=s->hidden;r.subtype=s->subtype;
 for(unsigned i=0;i<6;i++)r.bssid[i]=s->bssid[i];for(unsigned i=0;i<s->ssid_len;i++)r.ssid[i]=s->ssid[i];
 if(s->rxv_valid&&(s->group_valid&4)){
  uint8_t a=s->rxv[4],b=s->rxv[5];r.rssi_valid=a!=255||b!=255;
  if(a==255)a=0;if(b==255)b=0;r.rcpi=a>b?a:b;
  r.rssi_dbm=(int16_t)((int32_t)((r.rcpi>220?220:r.rcpi)/2)-110);
 }
 for(size_t i=0;i<s->ie_bytes;){if(s->ie_bytes-i<2)return -1;unsigned id=s->ies[i++],z=s->ies[i++];if(z>s->ie_bytes-i)return -1;
  if(id==1||id==50){unsigned flag=id==1?1:2;if(rate_ies&flag)return -1;rate_ies|=flag;
   for(unsigned j=0;j<z;j++){uint8_t x=s->ies[i+j]&127;if(!x)return -1;unsigned k;for(k=0;k<r.rate_count;k++)if(r.ie_rates[k]==x)break;if(k==r.rate_count){if(k==16)return -1;r.ie_rates[r.rate_count++]=x;}}
  }i+=z;
 }
 r.ie_bytes=s->ie_bytes;for(unsigned i=0;i<s->ie_bytes;i++)r.ies[i]=s->ies[i];*out=r;return 0;
}
