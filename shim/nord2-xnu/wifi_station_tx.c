#include "wifi_station_tx.h"
#include <string.h>
static uint16_t l16(const uint8_t*p){return p[0]|(uint16_t)p[1]<<8;}
static uint32_t l32(const uint8_t*p){return l16(p)|(uint32_t)l16(p+2)<<16;}
static unsigned b16(const uint8_t*p){return (unsigned)p[0]<<8|p[1];}
static void p16(uint8_t*p,unsigned n){p[0]=(uint8_t)n;p[1]=(uint8_t)(n>>8);}
static void p32(uint8_t*p,uint32_t n){p16(p,n);p16(p+2,n>>16);}
static void inc(unsigned*p){if(*p!=UINT32_MAX)(*p)++;}
/* Source classifies DHCP by BOOTP ports/cookie. Extra bounds here classify
 * only complete IPv4/UDP storage; classification never admits a frame. */
static int special(const uint8_t*p,size_t n){
 unsigned e=b16(p+12);if(e==0x888e||e==0x0806)return 1;
 if(e!=0x0800||n<34)return 0;const uint8_t*i=p+14;unsigned h=(i[0]&15u)*4u;
 if(i[0]>>4!=4||h<20||h>n-14||i[9]!=17||(b16(i+6)&0x3fff))return 0;
 unsigned z=b16(i+2);if(z<h+8||z>n-14)return 0;const uint8_t*u=i+h;unsigned un=b16(u+4),port=b16(u+2);
 if(un<248||un>z-h||(port!=67&&port!=68))return 0;
 const uint8_t*b=u+8;return b[236]==99&&b[237]==130&&b[238]==83&&b[239]==99;
}
int nstx_build_profile(uint8_t*out,size_t cap,size_t*used,const struct sw_txd*d,const uint8_t*f,size_t n,unsigned security,unsigned token,uint64_t buffer,unsigned profile,const struct sw_peer*peer){
 if(used)*used=0;if(!out||cap!=NSTX_PAGE_BYTES||!used||!d||!f||security>1||profile>NSTX_PROFILE_TODS||(profile&&security)||token>=NSTX_TOKEN_COUNT||buffer>UINT32_MAX-NSTX_PAGE_BYTES||buffer&4095)return -1;
 uint8_t b[1600]={0};size_t z=0,payload=n;int r;
 if(profile==NSTX_PROFILE_TODS){
  if(sw_data_tods(b+64,sizeof b-64,&payload,peer,f,n))return -1;
  r=sw_tods_data_txd(b,32,&z,d,b+64,payload);
 }else r=security?sw_security_txd(b,32,&z,d,f,n):d->protected_frame?sw_data_txd(b,32,&z,d,f,n):sw_open_data_txd(b,32,&z,d,f,n);
 if(r||z!=32||payload>sizeof b-64)return -1;
 /* Selected AXI soc3_0: formatTXD0, LMAC TC1/AC1 WMM0, VTA1;
  * TX byte count excludes append32; payload is fixed before publication. */
 uint32_t w=l32(b)&~((3u<<23)|(127u<<25));w|=1u<<25;p32(b,w);p32(b+4,l32(b+4)|(1u<<10));
 if(special(f,n))p32(b+12,l32(b+12)|(1u<<28));
 else {p32(b+8,l32(b+8)&~((1u<<31)|(1u<<30)));p32(b+24,0);}
 p16(b+32,token|0x8000u);p32(b+40,(uint32_t)(buffer+64));p16(b+44,(unsigned)payload|0x8000u);
 if(!profile)memcpy(b+64,f,n);memset(out,0,cap);memcpy(out,b,payload+64);*used=payload+64;return 0;
}
int nstx_build(uint8_t*out,size_t cap,size_t*used,const struct sw_txd*d,const uint8_t*f,size_t n,unsigned security,unsigned token,uint64_t buffer){
 return nstx_build_profile(out,cap,used,d,f,n,security,token,buffer,NSTX_PROFILE_ETHERNET,0);
}
int nstx_decode_report(struct nstx_report*out,const uint8_t*p,size_t n){
 if(!out||!p||n<8||n>3840||l16(p)<8||l16(p)>n||(l16(p+2)>>11)!=6)return -1;n=l16(p);
 struct nstx_report r={0};r.version=(l32(p+4)>>16)&7;unsigned count=(l32(p)>>16)&(r.version==3?1023u:127u);
 if(r.version>3||count>NSTX_REPORT_IDS)return -1;
 size_t at=8;unsigned words=0;
 while(r.count<count){
  if(n-at<4||++words>(3840u-8)/4)return -1;uint32_t w=l32(p+at);at+=4;
  if(r.version==3&&(w>>31))continue;
  unsigned id=r.version==0?w&65535u:r.version==1?w&65535u:r.version==2?w&32767u:(w>>16)&32767u;
  if(id>=NSTX_TOKEN_COUNT)return -1;r.ids[r.count++]=(uint16_t)id;
  if(r.version==0&&r.count<count){id=w>>16;if(id>=NSTX_TOKEN_COUNT)return -1;r.ids[r.count++]=(uint16_t)id;}
 }
 /* Source count is the active extent; bounded trailing report storage is not
  * interpreted as more tokens. Duplicate IDs invalidate the whole batch. */
 for(unsigned i=0;i<r.count;i++)for(unsigned j=0;j<i;j++)if(r.ids[i]==r.ids[j])return -1;
 *out=r;return 0;
}
int nstx_report(struct nstx*s,const uint8_t*p,size_t n){
 if(!s)return 0;struct nstx_report r;if(nstx_decode_report(&r,p,n)){inc(&s->invalid_reports);return 0;}
 if(!s->published||s->report_done||r.count!=1||r.ids[0]!=s->token){inc(&s->invalid_reports);return 0;}
 s->report_done=1;inc(&s->reports);return 1;
}
int nstx_idle(const struct nstx*s){return s&&!s->published&&!s->dma_done&&!s->report_done&&s->submitted==s->completed;}
