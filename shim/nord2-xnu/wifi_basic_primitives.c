#include "wifi_basic_primitives.h"
static void put16(uint8_t *p,uint16_t x){p[0]=(uint8_t)x;p[1]=(uint8_t)(x>>8);}
static void put32(uint8_t *p,uint32_t x){put16(p,(uint16_t)x);put16(p+2,(uint16_t)(x>>16));}
static void header(uint8_t *p,unsigned bytes,uint8_t cid,uint8_t seq){
 for(unsigned i=0;i<bytes;i++)p[i]=0;
 put32(p,bytes|(2u<<23));put32(p+4,1u<<16);
 put16(p+32,(uint16_t)(bytes-32));p[36]=cid;p[37]=0xa0;p[38]=1;p[39]=seq;
}
int nni_basic(uint8_t *p,size_t capacity,uint8_t seq,unsigned path,unsigned level){
 if(!p||capacity<NNI_BASIC_BYTES||path>1||level>2)return -1;
 header(p,NNI_BASIC_BYTES,2,seq);p[65]=(uint8_t)path;p[66]=(uint8_t)level;
 /* Native80211=0, RX/TX checksum=0 and segment/reserved bytes=0, as
  * source zero-initialization before optional checksum enable. */
 return 0;
}
