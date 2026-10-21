#include "wifi_ios_mapping.h"
int niwp_channel_to_mtk(const struct niwp_channel_map*m,size_t n,uint32_t c,uint32_t f,uint8_t*b,uint8_t*k){
 size_t found=n;if(!m||!b||!k||!n||n>400)return -1;
 for(size_t i=0;i<n;i++)if(m[i].ios_channel==c&&m[i].ios_flags==f){if(found!=n)return -1;found=i;}
 if(found==n||!m[found].mtk_band||!m[found].mtk_channel)return -1;
 *b=m[found].mtk_band;*k=m[found].mtk_channel;return 0;
}
int niwp_channel_from_mtk(const struct niwp_channel_map*m,size_t n,uint8_t b,uint8_t k,uint32_t*c,uint32_t*f){
 size_t found=n;if(!m||!c||!f||!n||n>400)return -1;
 for(size_t i=0;i<n;i++)if(m[i].mtk_band==b&&m[i].mtk_channel==k){if(found!=n)return -1;found=i;}
 if(found==n)return -1;*c=m[found].ios_channel;*f=m[found].ios_flags;return 0;
}
int niwp_rates(const struct niwp_rate_map*m,size_t n,const uint8_t*p,size_t z,uint32_t*out,size_t capacity,size_t*count){
 uint32_t v[15];size_t used=0;if(!m||!n||n>128||(!p&&z)||!out||!count||z>255)return -1;
 for(size_t i=0;i<z;i++){size_t hit=n;uint8_t r=p[i]&127;if(!r)return -1;
  for(size_t j=0;j<n;j++)if(m[j].ie_rate==r){if(hit!=n)return -1;hit=j;}
  if(hit==n)return -1;size_t j;for(j=0;j<used;j++)if(v[j]==m[hit].ios_rate)break;
  if(j==used){if(used==15)return -1;v[used++]=m[hit].ios_rate;}
 }
 if(used>capacity)return -1;
 for(size_t i=0;i<used;i++)out[i]=v[i];*count=used;return 0;
}
