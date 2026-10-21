#include "wifi_basic_metadata.h"
static uint16_t u16(const uint8_t*p){return (uint16_t)p[0]|(uint16_t)p[1]<<8;}
static uint32_t u32(const uint8_t*p){return (uint32_t)u16(p)|(uint32_t)u16(p+2)<<16;}
int nbc_caps_decode(const uint8_t*p,size_t n,struct nbc_caps*out){
 struct nbc_caps r={0};size_t off=40;
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
