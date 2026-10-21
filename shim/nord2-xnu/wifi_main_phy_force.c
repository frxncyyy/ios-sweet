#include "wifi_main_phy_force.h"
#ifdef NFA_HOST_TEST
#include <openssl/sha.h>
#include <string.h>
#else
#include <libkern/crypto/sha2.h>
#include <libkern/libkern.h>
#endif
static uint16_t le16(const uint8_t *p){return (uint16_t)p[0]|(uint16_t)p[1]<<8;}
static uint32_t le32(const uint8_t *p){return (uint32_t)le16(p)|((uint32_t)le16(p+2)<<16);}
static uint32_t be32(const uint8_t *p){return ((uint32_t)p[0]<<24)|((uint32_t)p[1]<<16)|((uint32_t)p[2]<<8)|p[3];}
static void put16(uint8_t *p,uint16_t v){p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);}
static void put32(uint8_t *p,uint32_t v){put16(p,(uint16_t)v);put16(p+2,(uint16_t)(v>>16));}
static int range(size_t off,size_t n,size_t total){return off<=total&&n<=total-off;}
static int name(const uint8_t *p,size_t n,const char *s){size_t i=0;while(s[i])i++;return i==n&&!memcmp(p,s,n);}
static int has(const uint8_t *p,size_t n,const char *s){size_t k=0;while(s[k])k++;for(size_t i=0;i+k<=n;i++)if(!memcmp(p+i,s,k))return 1;return 0;}
/* Focused current-FDT compatible scan. The accepted pool/CONN loader already
 * validates reservations; here absence of a ConnFem provider is rechecked
 * from actual original bytes, never inferred from saved DTS text hash. */
static int no_conn_fem(const void *fdt,size_t bytes){
 const uint8_t *f=fdt;if(!f||bytes<40||be32(f)!=0xd00dfeed)return -1;
 size_t total=be32(f+4),so=be32(f+8),st=be32(f+12),
  ss=be32(f+32),sz=be32(f+36),off,end;
 if(total<40||total>bytes||!range(so,sz,total)||!range(st,ss,total)||
    (so&3)||(st&3)||be32(f+20)!=17||be32(f+24)!=16)return -1;
 off=so;end=so+sz;unsigned depth=0,root_compatible=0,ended=0;
 while(range(off,4,end)){
  uint32_t token=be32(f+off);off+=4;
  if(token==1){
   while(off<end&&f[off])off++;
   if(off==end||depth>=32)return -1;
   off=(off+4)&~(size_t)3;depth++;
  }else if(token==2){if(!depth)return -1;depth--;}
  else if(token==3){
   if(!range(off,8,end))return -1;
   size_t n=be32(f+off),no=be32(f+off+4);off+=8;
   if(!range(off,n,end)||no>=ss)return -1;
   const uint8_t *key=f+st+no;size_t kn=0;
   while(kn<ss-no&&key[kn])kn++;
   if(kn==ss-no)return -1;
   if(name(key,kn,"compatible")){
    const uint8_t *v=f+off;
    if(depth==1&&has(v,n,"mediatek,MT6893"))root_compatible=1;
    if(has(v,n,"connfem"))return -1;
   }
   off=(off+n+3u)&~(size_t)3;
  }else if(token==4){}
  else if(token==9){if(depth||off!=end)return -1;ended=1;break;}
  else return -1;
 }
 return ended&&root_compatible?0:-1;
}
static int wifi_digest(const uint8_t *wifi,size_t size){
 static const uint8_t expected[32]={0x61,0xac,0x33,0x00,0xaa,0xd6,0xf6,0x53,
  0x64,0xf8,0x45,0x89,0x4b,0xb9,0x82,0xb7,0xc5,0xd0,0x77,0x34,0x6f,0x78,
  0x8c,0x29,0xdf,0x01,0x9c,0x93,0xc4,0xf3,0xf8,0x9f};
 uint8_t got[32];unsigned diff=0;
#ifdef NFA_HOST_TEST
 if(!SHA256(wifi,size,got))return -1;
#else
 SHA256_CTX h;
 SHA256_Init(&h);SHA256_Update(&h,wifi,size);SHA256_Final(got,&h);
#endif
 for(unsigned i=0;i<32;i++)diff|=got[i]^expected[i];
 volatile uint8_t *v=got;for(unsigned i=0;i<32;i++)v[i]=0;
 return diff?-1:0;
}
int nfa_prepare(const uint8_t *wifi,size_t size,const void *fdt,size_t fdt_bytes,
 struct nfa_board *out){
 struct nfa_board b={0};
 if(!wifi||!out||size!=NFA_WIFI_BYTES||wifi_digest(wifi,size)||
    no_conn_fem(fdt,fdt_bytes)||wifi[256]!=0||wifi[257]!=3||
    wifi[258]!=0||wifi[259]!=1||wifi[243]!=0)return -1;
 size_t total=256u+le16(wifi+260),off=256,start=0,stop=0;
 unsigned prior=0,seen=0,tag21=0;
 if(total!=4298u||total>size)return -1;
 while(off<total){
  if(!wifi[off]){unsigned z=1;for(size_t i=off;i<total;i++)if(wifi[i])z=0;if(z)break;}
  if(!range(off,3,total))return -1;
  unsigned tag=wifi[off];size_t n=le16(wifi+off+1),end=off+3+n;
  if(!n||end>total||tag>=32||(seen&(1u<<tag))||(tag&&tag<=prior))return -1;
  seen|=1u<<tag;prior=tag;
  if(tag==7)start=off;
  if(tag==10)stop=off;
  if(tag==21)tag21=1;
  off=end;
 }
 if(start!=3972u||stop!=4197u||stop-start!=225u||tag21||
    (seen&((1u<<7)|(1u<<8)|(1u<<9)|(1u<<10)))!=
       ((1u<<7)|(1u<<8)|(1u<<9)|(1u<<10)))return -1;
 memcpy(b.nvram,wifi+start,225u);/* gl_init zeroed OnlyPreCal; pad3 */
 b.verified=b.connfem_absent=1;b.coant=0;*out=b;return 0;
}
static size_t tag(uint8_t *p,unsigned id,const uint8_t *data,size_t n){
 put16(p,(uint16_t)id);put16(p+2,(uint16_t)n);
 if(data)memcpy(p+4,data,n);else memset(p+4,0,n);
 return n+4;
}
int nfa_force_frame(const struct nfa_board *b,uint8_t seq,uint8_t out[NFA_FRAME_BYTES]){
 if(!b||!out||!b->verified||!b->connfem_absent||b->coant||!seq)return -1;
 memset(out,0,NFA_FRAME_BYTES);
 put32(out,NFA_FRAME_BYTES|(2u<<23));put32(out+4,1u<<16);
 put16(out+32,NFA_FRAME_BYTES-32u);/* INIT_HIF_TX_HEADER u2TxByteCount */
 out[36]=8;out[37]=0xa0;out[39]=seq;
 uint8_t *p=out+64;put32(p,0x556789aau);p[4]=4;p[5]=1;put16(p+6,392);
 p+=8;const uint8_t cal[4]={1,0,0,0};
 p+=tag(p,1,cal,4);p+=tag(p,0,b->nvram,NFA_NVRAM_BYTES);
 p+=tag(p,2,0,104);p+=tag(p,3,0,40);
 return p==out+NFA_FRAME_BYTES?0:-1;
}
int nfa_force_response(const uint8_t *f,size_t n,uint8_t seq,
 uint32_t conn_bytes,struct nfa_result *out){
 struct nfa_result r={0};
 if(!f||!out||n!=NFA_RX_BYTES||!seq||!conn_bytes)return -1;
 const uint8_t *e=f+24,*p=e+8;
 if(e[2]!=0||e[3]!=0xe0||e[4]!=5||e[5]!=seq||
    le32(p)!=0x556789aau||p[4]!=1||p[5]!=1||le16(p+6)!=20||
    le16(p+8)!=1||le16(p+10)!=16||p[12]!=0x81||p[13]!=0||
    le16(p+14)!=0)return -1;
 r.emi_offset=le32(p+16)&0xffffffu;r.emi_bytes=le32(p+20);
 r.temperature_raw=le32(p+24);
 if(!r.emi_bytes||r.emi_offset>=conn_bytes||
    r.emi_bytes>conn_bytes-r.emi_offset)return -1;
 r.valid=r.force_success=r.backup_needed=1;*out=r;return 0;
}
