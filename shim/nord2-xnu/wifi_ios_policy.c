#include "wifi_ios_policy.h"
#include "wifi_ios_rates.h"
static uint32_t u32(const uint8_t*p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
int niwi_scan_envelope(unsigned selector,const void*v,size_t n,const char name[16]){
 const uint8_t*p=v;size_t cap;if(!p||!name)return -1;
 if(selector==0&&n==15408)cap=15360;else if(selector==6&&n==245808)cap=245760;else return -1;
 if(u32(p)!=0x802869c8||u32(p+4)!=5456||u32(p+8+cap+16)!=10||u32(p+8+cap+20))return -1;
 for(unsigned i=0;i<16;i++)if(p[8+cap+i]!=(uint8_t)name[i])return -1;return 0;
}
int niwi_set_shape(unsigned selector,const void*v,size_t n,const char name[16],uint32_t*out){
 const uint8_t*p=v;size_t cap;if(!p||!name||!out)return -1;
 if(selector==0&&n==15408)cap=15360;else if(selector==6&&n==245808)cap=245760;else return -1;
 if(u32(p)!=0x802869c8||u32(p+4)>cap)return -1;
 for(unsigned i=0;i<16;i++)if(p[8+cap+i]!=(uint8_t)name[i])return -1;
 *out=u32(p+8+cap+16);return 0;
}
/* Passive discovery only. Raw1/2 nondirected/default-timing requests are served by our explicit passive
 * discovery policy; source raw2 means passive. Channels may be a retained-owner
 * subset. Active probes are not claimed. Directed BSS/SSID, dwell/rest and
 * filter/override flags refuse. info must be the authoritative native owner
 * snapshot. Scheduling hints are best effort; no priority/deadline guarantee. */
int niwi_scan_admit(const uint8_t*p,size_t n,const struct nss_info*info,struct nss_request*out){
 if(!p||n!=5456||!info||!out)return NIWI_ADMIT_ARGUMENT;
 if(!info->count||info->count>NSS_CHANNELS)return NIWI_ADMIT_OWNER;
 uint32_t mode=u32(p+52),count=u32(p+68),options=u32(p+4872);
 if(u32(p)!=1||u32(p+4)!=3||u32(p+56)!=1||count>NSS_CHANNELS||
    (mode!=1&&mode!=2))return NIWI_ADMIT_PROFILE;
 for(unsigned j=0;j<info->count;j++){
  if((info->channels[j].band!=1&&info->channels[j].band!=2)||!info->channels[j].channel)return NIWI_ADMIT_OWNER;
  for(unsigned k=0;k<j;k++)if(info->channels[j].band==info->channels[k].band&&
      info->channels[j].channel==info->channels[k].channel)return NIWI_ADMIT_OWNER;
 }
 /* Compare semantic fields, not padding or inactive array storage. The actual
  * framework dictionary builder initializes RSSI threshold to INT32_MIN.
  * Zero is retained for the existing null/default request profile. Explicit
  * thresholds, timing, named/short SSIDs are not implemented. */
 if(u32(p+16)||p[60]||p[61]||u32(p+64)||
    (u32(p+4880)!=0&&u32(p+4880)!=0x80000000u)||u32(p+4884)||u32(p+5448))return NIWI_ADMIT_OPTION;
 /* Exact builder0x1d3ebb970..9bc maps true LOW_PRIORITY to2 and LOW_LATENCY
  * to0x20. Neither changes RF/filter semantics. The dedicated scan-only worker
  * wakes on enqueue and serializes FIFO without artificial throttling. Admit
  * either advisory hint in that scheduling policy; do not claim preemption,
  * priority reordering or a latency bound. Future shared-radio work must review
  * arbitration. Mixed priority/latency, filters, overrides and6GHz bits refuse. */
 if(u32(p+4876)||(options&&options!=NIWI_SCAN_LOW_PRIORITY&&options!=NIWI_SCAN_LOW_LATENCY))return NIWI_ADMIT_FLAGS;
 unsigned zero=1,broadcast=1;
 for(unsigned i=8;i<14;i++){if(p[i])zero=0;if(p[i]!=255)broadcast=0;}
 if(!zero&&!broadcast)return NIWI_ADMIT_PEER; /* Neither directed peer nor fabricated filter. */
 struct nss_request result={0};
 for(unsigned j=0;j<count;j++){
  const uint8_t*c=p+72+12*j;uint32_t channel=u32(c+4),flags=u32(c+8);unsigned band=0;
  if(u32(c)!=1||!channel||channel>255)return NIWI_ADMIT_CHANNEL;
  /* ACTIVE0x80 grants channel permission; it does not require probe TX.
   * We remain passive. DFS/other widths and unknown attributes refuse. */
  flags&=~128u;
  if(flags==8||flags==10)band=1;
  else if(flags==16||flags==18)band=2;
  else if(flags!=0&&flags!=2)return NIWI_ADMIT_CHANNEL_FLAGS;
  unsigned found=info->count;
  for(unsigned k=0;k<info->count;k++)if(info->channels[k].channel==channel&&
      (!band||info->channels[k].band==band)){
   if(found!=info->count)return NIWI_ADMIT_ALLOWLIST;found=k;
  }
  if(found==info->count)return NIWI_ADMIT_ALLOWLIST;
  for(unsigned k=0;k<j;k++)if(result.channels[k].band==info->channels[found].band&&
      result.channels[k].channel==info->channels[found].channel)return NIWI_ADMIT_ALLOWLIST;
  result.channels[j]=info->channels[found];
 }
 result.count=count;*out=result;return 0;
}
const struct niwp_rate_map*niwi_rate_map(size_t*n){return niwr_production_map(n);}

int niwi_power_on_admit(const void *v,size_t n){
 const uint8_t*p=v;
 return p&&n==24&&u32(p)==1&&u32(p+4)==1&&u32(p+8)==1?0:-1;
}
