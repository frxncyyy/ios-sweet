#if defined(KERNEL)
#include <pexpert/arm64/board_config.h>
#endif
#if defined(NORD2)
#include "audio_board.h"
#include "audio_hw.h"
#include "audio_board_offsets.h"
struct board_op { uint8_t reg; uint16_t mask,value; };
static struct board_op plan[2][52];
static unsigned loaded;
static const uint8_t pinned[32]={0x63,0x53,0x00,0xfb,0xc9,0x6f,0x25,0x8f,0xf9,0xa3,0x5f,0x82,0x43,0x5f,0x2e,0x9d,0x3f,0xf6,0xf1,0x37,0x0c,0x15,0x60,0x67,0xb4,0x01,0x46,0xe0,0x11,0x03,0x86,0x92};
static uint16_t u16(const uint8_t *p){return (uint16_t)(p[0]|(uint16_t)p[1]<<8);}
static uint32_t u32(const uint8_t *p){return (uint32_t)u16(p)|(uint32_t)u16(p+2)<<16;}
static int supports(uint16_t r,unsigned addr)
{return loaded && (addr==0x34||addr==0x37) && (r==0x0a74||r==0x0b74||r==0x0c74);}
int nah_board_load(const uint8_t *b,size_t n,const uint8_t digest[32])
{
 if(loaded || !b || !digest || n!=4163)return -1;
 unsigned difference=0;
 for(unsigned i=0;i<32;i++)difference|=digest[i]^pinned[i];
 if(difference || b[0]!='P'||b[1]!='M'||b[2]!='3'||b[3]!='_'||b[4]!='0'||b[5]!='1'||u32(b+6)!=n || u16(b+40)!=2)return -1;
 /* Pinned SHA proves complete graph identity. Offsets below are immutable
  * descriptor payload locations, decoded with explicit byte order/bounds.
  * Three volatileunlock ops and patch/default markers are intentionally absent. */
 for(unsigned amp=0;amp<2;amp++) {
  uint32_t dev=u32(b+46+4*amp)&0xffffff;
  if(dev>n-12 || b[dev+2]!=(amp?0x37:0x34))return -1;
  for(unsigned i=0;i<52;i++) {
   unsigned off=offsets[amp][i];
   if(off>n-4)return -1;
   struct board_op op={0};
   /* All selected payloads are bitfields. This is asserted by generator. */
   uint16_t value=u16(b+off),field=u16(b+off+2);
   unsigned pos=(field>>4)&15,width=(field&15)+1;
   if(pos+width>16 || (uint32_t)value>=(UINT32_C(1)<<width))return -1;
   op.reg=(uint8_t)(field>>8);op.mask=(uint16_t)(((UINT32_C(1)<<width)-1)<<pos);op.value=(uint16_t)(value<<pos);
   if(op.reg==0x0f || (op.reg>=0xa0 && op.reg!=0xb0 && op.reg!=0xc4))return -1;
   plan[amp][i]=op;
  }
 }
 loaded=1;return 0;
}
int nah_board_register_allowed(unsigned reg)
{
 if(!loaded)return 0;
 for(unsigned a=0;a<2;a++)for(unsigned i=0;i<52;i++)if(plan[a][i].reg==reg)return 1;
 return 0;
}
int nah_board_change_allowed(unsigned reg,uint16_t old,uint16_t next)
{
 if(!loaded)return 0;
 for(unsigned a=0;a<2;a++)for(unsigned i=0;i<52;i++) {
  const struct board_op *p=&plan[a][i];
  if(p->reg==reg && !((old^next)&(uint16_t)~p->mask) && (next&p->mask)==p->value)return 1;
 }
 return 0;
}
static int apply(void *cookie,const struct na8_io *io,struct na8_state *bus,unsigned amp)
{
 struct nau_hw *h=cookie;
 if(!h||!io||!bus||amp>1||h->amps[amp].addr!=(amp?0x37:0x34)||!supports(h->amps[amp].rev,h->amps[amp].addr))return NAH_E_BOARD_CONFIG;
 /* Exact pinned device preamble, replayed by Android's
  * tfaContWriteRegsDev_v6 AFTER silicon init and before all bitfields.
  * These are volatile key latches, not MTP programming commands. */
 if(na8_write16(io,h,bus,h->amps[amp].addr,0x0f,0x5a6b) ||
    na8_write16(io,h,bus,h->amps[amp].addr,0xa1,0x005a) ||
    na8_write16(io,h,bus,h->amps[amp].addr,0xa0,0x005a))return NAH_E_IO;
 for(unsigned i=0;i<52;i++) {
  const struct board_op *p=&plan[amp][i];uint16_t old=0,check=0;
  if(na8_read16(io,h,bus,h->amps[amp].addr,p->reg,&old))return NAH_E_IO;
  uint16_t next=(uint16_t)((old&~p->mask)|(p->value&p->mask));
  if(na8_write16(io,h,bus,h->amps[amp].addr,p->reg,next)||na8_read16(io,h,bus,h->amps[amp].addr,p->reg,&check)||((check^next)&p->mask))return NAH_E_IO;
 }
 return 0;
}
static const struct nah_board board={{0,0},{0x63,0x53,0x00,0xfb,0xc9,0x6f,0x25,0x8f,0xf9,0xa3,0x5f,0x82,0x43,0x5f,0x2e,0x9d,0x3f,0xf6,0xf1,0x37,0x0c,0x15,0x60,0x67,0xb4,0x01,0x46,0xe0,0x11,0x03,0x86,0x92},apply,supports};
const struct nah_board *nah_board_profile(void){return loaded?&board:0;}
#endif
