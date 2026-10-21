#include "wifi_normal_init.h"
static void cp(void *to,const void *from,size_t n){uint8_t *d=to;const uint8_t *s=from;while(n--)*d++=*s++;}
static void put16(uint8_t *p,uint16_t x){p[0]=(uint8_t)x;p[1]=(uint8_t)(x>>8);}
static void put32(uint8_t *p,uint32_t x){put16(p,(uint16_t)x);put16(p+2,(uint16_t)(x>>16));}
static void header(uint8_t *p,unsigned bytes,uint8_t cid,uint8_t seq){
 for(unsigned i=0;i<bytes;i++)p[i]=0;
 put32(p,bytes|(2u<<23));put32(p+4,1u<<16);
 put16(p+32,(uint16_t)(bytes-32));p[36]=cid;p[37]=0xa0;p[38]=1;p[39]=seq;
}
int nni_nvram_plan(const uint8_t *p,size_t bytes,struct nni_nvram *out){
 struct nni_nvram plan={0};size_t off=256,start=0;
 if(!p||!out||bytes<259||bytes>UINT32_MAX)return -1;
 for(unsigned i=0;i<=NNI_MAX_FRAGMENTS;i++){
  if(bytes-off<3)return -1;
  unsigned tag=p[off],n=(unsigned)p[off+1]|(unsigned)p[off+2]<<8;
  if(!tag&&!n){
   if(!plan.fragments)return -1;
   plan.source_bytes=(uint32_t)bytes;plan.used_bytes=(uint32_t)off;
   plan.terminator=(uint32_t)off;plan.ignored_bytes=(uint32_t)(bytes-off-3);
   *out=plan;return 0;
  }
  if(i==NNI_MAX_FRAGMENTS||(!i&&tag!=0)||n>bytes-off-3)return -1;
  size_t end=off+3+n,fragment=end-start;
  if(fragment>NNI_NVRAM_BYTES)return -1;
  plan.span[i]=(struct nni_fragment){(uint32_t)start,(uint32_t)fragment,(uint8_t)tag};
  plan.fragments++;start=off=end;
 }
 return -1;
}
static int same_plan(const struct nni_nvram *a,const struct nni_nvram *b){
 if(a->fragments!=b->fragments||a->source_bytes!=b->source_bytes||a->used_bytes!=b->used_bytes||a->terminator!=b->terminator||a->ignored_bytes!=b->ignored_bytes)return 0;
 for(unsigned i=0;i<a->fragments;i++)if(a->span[i].offset!=b->span[i].offset||a->span[i].bytes!=b->span[i].bytes||a->span[i].tag!=b->span[i].tag)return 0;
 return 1;
}
int nni_nvram_fragment(const uint8_t *p,size_t bytes,const struct nni_nvram *plan,
 unsigned index,uint8_t seq,uint8_t *out,size_t capacity){
 struct nni_nvram current;
 if(!plan||!out||capacity<NNI_FRAGMENT_BYTES||nni_nvram_plan(p,bytes,&current)||
    !same_plan(plan,&current)||index>=current.fragments)return -1;
 header(out,NNI_FRAGMENT_BYTES,0x48,seq);
 cp(out+64,p+current.span[index].offset,current.span[index].bytes);return 0;
}
static int valid(const uint8_t *p){unsigned bits=0;for(unsigned i=0;i<6;i++)bits|=p[i];return bits&&!(p[0]&1);}
int nni_address_select(const uint8_t *board,size_t bytes,const struct ncp_result *cap,struct nni_address *out){
 struct nni_address r={0};if(!out)return -1;
 if(board&&bytes>=10&&valid(board+4)){cp(r.mac,board+4,6);r.source=NNI_ADDRESS_BOARD;}
 else if(cap&&cap->decoded&&cap->mac_present&&cap->mac_valid&&valid(cap->mac)){
  cp(r.mac,cap->mac,6);r.source=NNI_ADDRESS_FIRMWARE;
 }else return -1;
 *out=r;return 0;
}
