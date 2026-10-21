#include "wifi_main_command.h"
static void put32(uint8_t *p,uint32_t x){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(x>>(8*i));}
static uint32_t get32(const uint8_t *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static void zero(uint8_t *p,size_t n){for(size_t i=0;i<n;i++)p[i]=0;}
static int range(uint64_t dma,size_t n){return !dma||!n||n>0x3fff||dma>UINT32_MAX||n-1>UINT32_MAX-dma;}
int nwt_finish(uint8_t *out,size_t capacity,uint8_t cid,uint8_t seq,size_t *used){
 if(!out||!used||(cid!=0x40&&cid!=0x41))return -1;
 size_t len=cid==0x40?68:72;if(capacity<len)return -1;
 zero(out,len);put32(out,(uint32_t)len|(2u<<23));put32(out+4,1u<<16);
 /* MAC32 + INIT32; INIT PQ_ID remains zero in selected fill function,
  * notwithstanding stale struct comment saying8000. Zeroed reservedDW. */
 out[32]=(uint8_t)(len-32);out[36]=cid;out[37]=0xa0;out[39]=seq;
 /* patch ucCheckCrc0; N9 override0,address0 sourceexact payloads. */
 *used=len;return 0;
}
int nwt_tx_desc(uint8_t out[16],uint64_t dma,size_t n){
 if(!out||range(dma,n))return -1;
 zero(out,16);put32(out,(uint32_t)dma);
 put32(out+4,((uint32_t)n<<16)|(1u<<30));return 0;
}
int nwt_rx_desc(uint8_t out[16],uint64_t dma,size_t n){
 if(!out||range(dma,n))return -1;
 zero(out,16);put32(out,(uint32_t)dma);put32(out+4,(uint32_t)n<<16);return 0;
}
int nwt_rx_complete(const uint8_t d[16],uint64_t dma,size_t capacity,size_t *bytes){
 if(!d||!bytes||range(dma,capacity)||get32(d)!=(uint32_t)dma||get32(d+8))return -1;
 uint32_t flags=get32(d+4);size_t n=(flags>>16)&0x3fff;
 if((flags&0xc0000000u)!=0xc0000000u||!n||n>capacity||(flags&0x3fff))return -1;
 *bytes=n;return 0;
}
