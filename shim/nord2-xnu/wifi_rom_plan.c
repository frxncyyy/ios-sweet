/* Original pure host/kernel planner for pinned ROM_EMI_HEADER files.
 * Layout reference: gen4m/include/chips/soc3_0.h:218; selected soc3_0.c:3031
 * only consumes date[16], address and48-byte header size. Header type and CRC
 * words are not interpreted by that Linux download path. No tailer is used
 * for these two files (main RAM firmware has a different tailer format). */
#include "wifi_rom_plan.h"
#ifdef NWF_ROM_HOST_TEST
#include <openssl/sha.h>
#include <string.h>
#else
#include <libkern/crypto/sha2.h>
#include <libkern/libkern.h>
#endif
static uint32_t le32(const unsigned char *p)
{ return p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24); }
static int add64(uint64_t a,uint64_t b,uint64_t *result)
{ if(b>UINT64_MAX-a) return -1; *result=a+b; return 0; }
static int rom_hash(const unsigned char *p,size_t size,const unsigned char wanted[32])
{
 SHA256_CTX ctx; unsigned char hash[32]; unsigned diff=0;
 SHA256_Init(&ctx); SHA256_Update(&ctx,p,size); SHA256_Final(hash,&ctx);
 for(unsigned i=0;i<32;i++) diff|=hash[i]^wanted[i];
 return diff==0;
}
struct rom_expected { uint32_t bytes,target,type_word,date_offset; unsigned char hash[32]; };
static const struct rom_expected expected[2]={
 {49868u,0xf0230000u,0x04000000u,0x27f0e0u,{0xf9,0x5d,0xd9,0x56,0xa0,0x50,0xf3,0xc8,0x2c,0x70,0x5d,0x3b,0x16,0xa2,0x67,0xc8,0xfe,0xa0,0xb3,0x7d,0x8c,0x6e,0xff,0x4a,0x02,0x00,0xb4,0x35,0xa5,0xe4,0x2e,0x12}},
 {296916u,0xf02b0000u,0x03000000u,0x27f0f0u,{0x16,0xe8,0xc8,0xd5,0xfb,0xf4,0x74,0x95,0x71,0x10,0x19,0xb0,0xcd,0xae,0x6e,0x68,0x5d,0x53,0xcb,0xbe,0x53,0xbc,0x19,0x69,0xce,0x34,0xdb,0x33,0x16,0x39,0x55,0xac}}
};
int nwf_rom_make_plan(const unsigned char *mcu,size_t mcu_size,
 const unsigned char *wifi,size_t wifi_size,struct nwf_rom_plan *plan)
{
 const unsigned char *input[2]={mcu,wifi}; size_t sizes[2]={mcu_size,wifi_size};
 struct nwf_rom_plan candidate={0};
 if(!plan) return -1;
 memset(plan,0,sizeof(*plan));
 for(unsigned i=0;i<2;i++) {
  const struct rom_expected *e=&expected[i]; const unsigned char *p=input[i];
  if(!p || sizes[i]!=e->bytes || sizes[i]<48) return -1;
  /* Type is pinned as raw little-endian word, without claiming an unknown
   * protocol interpretation: wire bytes are00 00 00 04 /00 00 00 03.
   * Version/platform/build/CRC words are authenticated by full-file digest. */
  if(le32(p+24)!=e->target || le32(p+28)!=e->type_word || !rom_hash(p,sizes[i],e->hash)) return -1;
  uint32_t offset=e->target&0xffffffu;
  candidate.spans[i*2]=(struct nwf_rom_span){i,48,e->bytes-48,offset,0};
  candidate.spans[i*2+1]=(struct nwf_rom_span){i,0,16,e->date_offset,0};
 }
 for(unsigned i=0;i<NWF_ROM_SPANS;i++) {
  struct nwf_rom_span *s=&candidate.spans[i];
  if(s->blob>=2 || s->source_offset>sizes[s->blob] ||
     s->bytes>sizes[s->blob]-s->source_offset || !s->bytes ||
     s->emi_offset>NWF_ROM_EMI_BYTES || s->bytes>NWF_ROM_EMI_BYTES-s->emi_offset) return -1;
  uint64_t end;
  if(add64(NWF_ROM_EMI_BASE,s->emi_offset,&s->physical) ||
     add64(s->physical,s->bytes,&end)) return -1;
  for(unsigned j=0;j<i;j++) {
   const struct nwf_rom_span *previous=&candidate.spans[j];
   if(s->physical<previous->physical+previous->bytes &&
      previous->physical<end) return -1;
  }
 }
 *plan=candidate;
 return 0;
}
