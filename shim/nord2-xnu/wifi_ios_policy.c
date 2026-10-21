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
int niwi_scan_admit(const uint8_t*p,size_t n,const struct nss_info*info,struct nss_request*out){
 if(!p||n!=5456||!info||!info->count||info->count>39||!out)return -1;
 /* Precisely source-built null-dictionary discovery request. This driver
  * deliberately serves that unrestricted request with passive receive only;
  * raw mode1 is named default, never claimed recovered passive enum. */
 for(unsigned i=0;i<5456;i++){
  uint8_t expected=i==0||i==52||i==56?1:(i==4?3:0);
  if(p[i]!=expected)return -1;
 }
 *out=(struct nss_request){0};return 0;
}
const struct niwp_rate_map*niwi_rate_map(size_t*n){return niwr_production_map(n);}
