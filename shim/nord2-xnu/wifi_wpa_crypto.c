/* New bounded RFC2104/8018/3394 and WPA PRF glue. No Redwood arithmetic copied.
 * BearSSL dependencies retain their own Thomas Pornin MIT notices. */
#include "wifi_wpa_crypto.h"
#if defined(KERNEL) || defined(_KERNEL)
/* Canonical placement is xnu/bsd/dev; reuse the already staged MIT library. */
#include "../../libkern/crypto/sweet_bearssl/inc/bearssl_hash.h"
#include "../../libkern/crypto/sweet_bearssl/inc/bearssl_block.h"
#else
#include "bearssl_hash.h"
#include "bearssl_block.h"
#endif
#if defined(KERNEL) || defined(_KERNEL)
#include <sys/systm.h>
#else
#include <string.h>
#endif
static void clear(void*v,size_t n){volatile uint8_t*p=v;while(n--)*p++=0;}
int nwpc_hmac(const uint8_t*k,size_t kn,const uint8_t*const*p,const size_t*n,size_t count,uint8_t out[20]){
 if(!out)return -1;
 if(!k||kn>64||!p||!n||count>4){clear(out,20);return -1;}
 for(size_t i=0;i<count;i++)if(n[i]>640||(!p[i]&&n[i])){clear(out,20);return -1;}
 uint8_t pad[64],digest[20];br_sha1_context c;
 for(unsigned i=0;i<64;i++)pad[i]=(i<kn?k[i]:0)^0x36;
 br_sha1_init(&c);br_sha1_update(&c,pad,64);
 for(size_t i=0;i<count;i++)if(n[i])br_sha1_update(&c,p[i],n[i]);
 br_sha1_out(&c,digest);
 for(unsigned i=0;i<64;i++)pad[i]^=0x36^0x5c;
 br_sha1_init(&c);br_sha1_update(&c,pad,64);br_sha1_update(&c,digest,20);br_sha1_out(&c,out);
 clear(&c,sizeof c);clear(pad,64);clear(digest,20);return 0;
}
int nwpc_pmk(const uint8_t*p,size_t pn,const uint8_t*s,size_t sn,uint8_t out[32]){
 if(!out)return -1;
 if(!p||pn<8||pn>63||!s||!sn||sn>32){clear(out,32);return -1;}
 uint8_t u[20],x[20];
 for(unsigned block=1;block<=2;block++){
  uint8_t counter[4]={0,0,0,(uint8_t)block};const uint8_t*parts[2]={s,counter};size_t ns[2]={sn,4};
  (void)nwpc_hmac(p,pn,parts,ns,2,u);memcpy(x,u,20);
  for(unsigned round=1;round<4096;round++){const uint8_t*q[1]={u};size_t size[1]={20};(void)nwpc_hmac(p,pn,q,size,1,u);for(unsigned i=0;i<20;i++)x[i]^=u[i];}
  memcpy(out+(block-1)*20,x,block==1?20:12);
 }
 clear(u,20);clear(x,20);return 0;
}
int nwpc_ptk(const uint8_t key[32],const uint8_t context[76],uint8_t out[48]){
 if(!out)return -1;
 if(!key||!context){clear(out,48);return -1;}
 static const uint8_t label[]="Pairwise key expansion";uint8_t d[20],counter;
 for(counter=0;counter<3;counter++){
  const uint8_t*p[3]={label,context,&counter};size_t ns[3]={sizeof label,76,1};
  (void)nwpc_hmac(key,32,p,ns,3,d);memcpy(out+20*counter,d,counter==2?8:20);
 }
 clear(d,20);return 0;
}
int nwpc_unwrap(const uint8_t key[16],const uint8_t*in,size_t n,uint8_t*out,size_t cap,size_t*used){
 if(used)*used=0;
 if(!key||!in||!out||!used||n<24||n>640||(n&7)||cap<n-8)return -1;
 uint8_t plain[632],a[8],block[16],iv[16];br_aes_ct_cbcdec_keys ctx;
 br_aes_ct_cbcdec_init(&ctx,key,16);memcpy(a,in,8);memcpy(plain,in+8,n-8);size_t blocks=n/8-1;
 for(int j=5;j>=0;j--)for(size_t i=blocks;i;i--){
  memcpy(block,a,8);uint64_t t=blocks*(unsigned)j+i;for(unsigned k=0;k<8;k++)block[7-k]^=(uint8_t)(t>>(k*8));
  memcpy(block+8,plain+8*(i-1),8);memset(iv,0,16);br_aes_ct_cbcdec_run(&ctx,iv,block,16);
  memcpy(a,block,8);memcpy(plain+8*(i-1),block+8,8);
 }
 unsigned bad=0;for(unsigned i=0;i<8;i++)bad|=a[i]^0xa6;
 if(!bad){memcpy(out,plain,n-8);*used=n-8;}
 clear(&ctx,sizeof ctx);clear(plain,sizeof plain);clear(a,8);clear(block,16);clear(iv,16);return bad?-1:0;
}
