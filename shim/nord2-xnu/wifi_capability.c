#include "wifi_capability.h"
static uint16_t le16(const uint8_t *p){return (uint16_t)p[0]|(uint16_t)p[1]<<8;}
static uint32_t le32(const uint8_t *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static void put32(uint8_t *p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(v>>(8*i));}
static void copy(uint8_t *d,const uint8_t *s,unsigned n){for(unsigned i=0;i<n;i++)d[i]=s[i];}
int ncp_query(uint8_t *p,size_t bytes,uint8_t seq)
{
 if(!p||bytes<NCP_QUERY_BYTES)return -1;
 for(unsigned i=0;i<NCP_QUERY_BYTES;i++)p[i]=0;
 put32(p,64u|(2u<<23));put32(p+4,1u<<16);
 p[32]=32;p[36]=0x8a;p[37]=0xa0;p[39]=seq;return 0;
}
int ncp_parse(const uint8_t *p,size_t bytes,uint8_t seq,struct ncp_result *out)
{
 struct ncp_result r={0};size_t off=40;uint32_t seen=0;
 if(!p||!out||bytes<40||bytes>NCP_MAX_RX_BYTES||
    (le16(p+2)&0x380fu)!=0x3800u||p[28]!=0xec||p[29]!=seq)return -1;
 r.rxd_bytes=le16(p);r.event_bytes=le16(p+24);r.elements=le16(p+36);
 if(r.elements>(bytes-off)/8)return -1;
 for(unsigned i=0;i<r.elements;i++){
  if(bytes-off<8)return -1;
  uint32_t tag=le32(p+off),n=le32(p+off+4);off+=8;
  if(n>bytes-off)return -1;
  const uint8_t *body=p+off;unsigned minimum=0;
  switch(tag){case 0:minimum=4;break;case 5:minimum=20;break;
   case 6:minimum=24;break;case 7:minimum=8;break;
   case 8:minimum=12;break;case 9:minimum=4;break;default:break;}
  if(minimum){
   if(n<minimum||(seen&(1u<<tag)))return -1;
   seen|=1u<<tag;r.known++;
   switch(tag){
   case 0:{uint32_t version=le32(body);r.resource_version=(version&0x80000000u)?version:0;
    if(!(version&0x80000000u)){if(n<16)return -1;for(unsigned k=0;k<4;k++)r.resources[k]=le32(body+4*k);r.resources_supported=1;}
    else if(version==0x80000001u){if(n<28)return -1;for(unsigned k=0;k<5;k++)r.resources[k]=le32(body+4+4*k);r.resources[5]=body[24];r.resources_supported=1;}
    break;}
   case 5:r.product=le16(body);r.eco=body[2];r.mac_ip=le32(body+4);r.bb_ip=le32(body+8);r.top_ip=le32(body+12);r.config_id=le32(body+16);break;
   case 6:r.fw_version=le16(body);r.fw_build=le16(body+2);copy(r.branch,body+4,4);copy(r.date,body+8,16);break;
   case 7:{unsigned any=0;r.mac_present=1;copy(r.mac,body,6);for(unsigned k=0;k<6;k++)any|=body[k];r.mac_valid=any&&!(body[0]&1);break;}
   case 8:copy(r.phy,body,12);break;
   case 9:copy(r.mac_cap,body,4);break;
   }
  }else r.unknown++;
  off+=n;
 }
 r.decoded=1;r.consumed=(unsigned)off;r.trailing=(unsigned)(bytes-off);*out=r;return 0;
}
