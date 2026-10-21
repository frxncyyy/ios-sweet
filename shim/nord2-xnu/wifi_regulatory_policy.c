#include "wifi_regulatory_policy.h"
static uint16_t u16(const uint8_t*p){return (uint16_t)p[0]|(uint16_t)p[1]<<8;}
static uint32_t u32(const uint8_t*p){return (uint32_t)u16(p)|(uint32_t)u16(p+2)<<16;}
static void p16(uint8_t*p,uint16_t v){p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);}
static void p32(uint8_t*p,uint32_t v){p16(p,(uint16_t)v);p16(p+2,(uint16_t)(v>>16));}
int nrp_caps_decode(const uint8_t*p,size_t n,struct nrp_caps*out){
 struct nrp_caps r={0};size_t off=40;
 if(!p||!out||n<40||n>3840||(u16(p+2)&0x380f)!=0x3800||p[28]!=0xec)return -1;
 unsigned count=u16(p+36);if(count>(n-40)/8)return -1;r.sequence=p[29];
 for(unsigned i=0;i<count;i++){
  if(n-off<8)return -1;
  uint32_t tag=u32(p+off),bytes=u32(p+off+4);off+=8;if(bytes>n-off)return -1;
  if(tag==3){if(r.sku_present||bytes<12)return -1;r.sku_present=1;r.efuse_country=u32(p+off);r.efuse_valid=p[off+4];}
  if(tag==8){if(r.phy_present||bytes<12)return -1;r.phy_present=1;r.band5=p[off+2];r.max_bw=p[off+3];r.nss=p[off+4];r.path=p[off+10];}
  off+=bytes;
 }
 *out=r;return 0;
}
int nrp_board_decode(const uint8_t*p,size_t n,struct nrp_board*out){
 if(!p||!out||n<256)return -1;
 struct nrp_board r={{p[10],p[11]},p[197],p[240]};*out=r;return 0;
}
int nrp_country(const struct nrp_caps*c,uint32_t*out){
 if(!c||!out||c->sku_present!=1)return -1;
 *out=c->efuse_valid?c->efuse_country:0x3030;return 0;
}
/* Membership only, not regulatory authorization: gl_init.c254–355. */
static int supported(unsigned band,unsigned ch){
 static const uint8_t five[25]={36,40,44,48,52,56,60,64,100,104,108,112,116,120,124,128,132,136,140,144,149,153,157,161,165};
 if(!band)return ch>=1&&ch<=14;
 for(unsigned i=0;i<25;i++)if(ch==five[i])return 1;
 return 0;
}
static int channels(const struct nrp_caps*c,const struct nrp_board*b,
 const struct nrp_channel*p,unsigned n){
 uint32_t country;
 if(!c||!b||!p||!n||n>NRP_CHANNELS||nrp_country(c,&country)||c->phy_present!=1||!c->nss)return -1;
 unsigned counts[2]={0};
 for(unsigned i=0;i<n;i++){
  if(p[i].band>1||!supported(p[i].band,p[i].channel)||(p[i].flags&~0x1ffbu))return -1;
  if(i&&p[i].band<p[i-1].band)return -1;
  if(p[i].band&&(!c->band5||!b->support5||!b->enable5))return -1;
  if(++counts[p[i].band]>(p[i].band?25u:14u))return -1;
  for(unsigned j=0;j<i;j++)if(p[i].band==p[j].band&&p[i].channel==p[j].channel)return -1;
 }
 return 0;
}
int nrp_domain_frame(const struct nrp_caps*c,const struct nrp_board*b,
 const struct nrp_channel*p,unsigned n,uint8_t seq,uint8_t*out,size_t cap,size_t*used){
 uint32_t country;
 if(!out||!used||channels(c,b,p,n)||nrp_country(c,&country))return -1;
 /* Source V2 list contains only channels not marked DISABLED. */
 for(unsigned i=0;i<n;i++)if(p[i].flags&1)return -1;
 size_t bytes=76+8u*n;if(cap<bytes)return -1;
 for(size_t i=0;i<bytes;i++)out[i]=0;
 p32(out,(uint32_t)bytes|(2u<<23));p32(out+4,1u<<16);p16(out+32,(uint16_t)(bytes-32));
 out[36]=0x0f;out[37]=0xa0;out[38]=1;out[39]=seq;p32(out+64,country);
 out[68]=out[69]=out[70]=1; /* source CONFIG_BW_20M; intentional narrow profile */
 for(unsigned i=0;i<n;i++){
  out[72+p[i].band]++;p16(out+76+8*i,p[i].channel);p32(out+80+8*i,p[i].flags);
 }
 *used=bytes;return 0;
}
int nrp_passive_plan(const struct nrp_caps*c,const struct nrp_board*b,
 const struct nrp_channel*p,unsigned n,struct nrp_passive*out){
 if(!out||channels(c,b,p,n))return -1;
 struct nrp_passive r={0};
 for(unsigned i=0;i<n;i++)if(!(p[i].flags&(1u|8u|512u|2048u)))r.channel[r.count++]=p[i];
 if(!r.count)return -1;
 *out=r;return 0;
}
