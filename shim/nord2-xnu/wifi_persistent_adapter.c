/* Platform-bound implementation; selected MT6893 GPL-2.0 derivative. */
#include "wifi_persistent_adapter.h"
#include "wifi_common.h"
struct field {uint32_t a,mask;};
static const struct field fields[]={
 {0x18001900,3},{0x10005460,0x70000},{0x1800400c,0x80ff},
 {0x18002018,~0u},{0x18003000,0x3e0001},{0x18003004,0xcf},
 {0x18003008,0x300000},{0x18003010,~0u},{0x18003018,~0u},
 {0x18003040,~0u},{0x18003100,~0u},{0x18003058,~0u},
 {0x18003078,~0u},{0x18003094,~0u},{0x180030f4,0x7fff7fff},
 {0x18060184,0xe00},{0x18001860,0xf201},
 {0x1800183c,~0u},{0x18001844,~0u},{0x1800184c,~0u},{0x18001854,~0u},
 {0x18001838,0x8000},{0x18001840,0x8000},{0x18001848,0x8000},{0x18001850,0x8000},
 {0x18001834,0x71},{0x18001c00,0x47ff1},{0x18001a00,0x10800000},
 {0x180010f0,0x1b},{0x1800f000,~0u},{0x18001b00,7},{0x18000090,~0u},
 {0x180601c4,0xfffff},{0x180601cc,0xfffff},{0x180601d4,0xfffff},
 {0x180601d8,0xfffff},{0x180601dc,0xfffff}
};
static int rawread(struct npa *s,uint32_t a,uint32_t *v){return s->hw->read32(s->cookie,a,v);}
static int ap(struct npa *s){return s->fatal||s->hw_handoff||s->hw->ap(s->cookie);}
static int cold(struct npa *s){
 static const struct field p[]={
 {0x18000010,1},{0x18000000,0x80},{0x180602cc,4},
 {0x18000008,0x80},{0x18000014,1},{0x18000114,0x03000000},
 {0x18000124,0x01000003},{0x18001874,1},{0x18001878,1},
 {0x18008040,1},{0x180601a8,1},{0x180601ac,1},{0x180601b0,1}};
 uint32_t v;
 if(ap(s)||s->hw->proof(s->cookie))return -1;
 for(unsigned i=0;i<sizeof(p)/sizeof(*p);i++){
  if(ap(s)||rawread(s,p[i].a,&v)||(v&p[i].mask))return -1;
 }
 /* RC positive source gate is AP-side; no clock-mux guessing. */
 if(ap(s)||rawread(s,0x10006504,&v)||!(v&1))return -1;
 return 0;
}
static uint32_t maskfor(uint32_t a){
 if(a>=0x18050000&&a<0x18054000&&!(a&3))return ~0u;
 for(unsigned i=0;i<NCC_COUNT;i++)if(ncc_table[i].address==a)return ncc_table[i].mask;
 for(unsigned i=0;i<sizeof(fields)/sizeof(*fields);i++)if(fields[i].a==a)return fields[i].mask;
 return 0;
}
static int ck(void *c,unsigned required){struct npa *s=c;
 return required!=NPC_REQUIRED||!s->entered||!s->remap_done||cold(s);
}
static int rd(void *c,uint32_t a,uint32_t *v){struct npa *s=c;
 if(!v||(a&3)||(!maskfor(a)&&a!=0x18001818&&a!=0x18005008))return -1;
 if(s->hw_handoff){
  if(s->fatal||s->hw_handoff!=1||a!=0x18001860)return -1;
 }else if(cold(s))return -1;
 if(rawread(s,a,v))return -1;
 s->last_address=a;s->last_value=*v;return 0;
}
static int rawwrite(struct npa *s,uint32_t a,uint32_t v){
 s->retained=1;s->mmio_writes++;
 return s->hw->write32(s->cookie,a,v);
}
static int wr(void *c,uint32_t a,uint32_t v){struct npa *s=c;uint32_t m=maskfor(a);
 if(s->fatal)return -1;
 if(s->hw_handoff){
  if(s->hw_handoff!=1||a!=0x18001860||s->last_address!=a||v!=(s->last_value|1u))return -1;
  int e=rawwrite(s,a,v);if(!e)s->hw_handoff=2;return e;
 }
 if(cold(s))return -1;
 if(a==0x10005054||a==0x10005154){if(v!=0x100000)return -1;}
 else{
  if(!m||(a&3))return -1;
  if(m!=~0u&&(s->last_address!=a||((v^s->last_value)&~m)))return -1;
 }
 /* Latch terminal path BEFORE exact source bus-SW-control clear. */
 if(a==0x18001a00&&s->result.phase==9&&!(v&(1u<<23)))s->hw_handoff=1;
 return rawwrite(s,a,v);
}
static int dl(void *c,unsigned usec){struct npa *s=c;
 if(usec!=1&&usec!=5&&usec!=10&&usec!=30&&usec!=50&&usec!=60&&usec!=800)return -1;
 return cold(s)||s->hw->delay_us(s->cookie,usec);
}
/* Source clock detector is measurement trigger, not analog restoration. */
static int clockok(struct npa *s){uint32_t v;
 if(cold(s)||rawread(s,0x18060000,&v))return -1;
 if(rawwrite(s,0x18060000,v|1))return -1;
 for(unsigned i=0;i<1600;i++){
  if(dl(s,i?10:1)||ap(s)||rawread(s,0x18060000,&v))return -1;
  s->clock_polls++;
  if((v&6)==6)break;
 }
 if((v&6)!=6||ap(s)||rawread(s,0x180602d4,&v)||!(v&1))return -1;
 return 0;
}
static int sem(void *c,unsigned n,int on){struct npa *s=c;uint32_t v;
 if(n!=11&&n!=19)return -1;
 unsigned *held=n==11?&s->sema11:&s->sema19;
 if(cold(s))return -1;
 if(on){
  if(*held||s->sema11||s->sema19||clockok(s))return -1;
  for(unsigned i=0;i<1000;i++){
   if(cold(s)||rawread(s,0x18072000+n*4,&v))return -1;
   if(v&1){*held=1;return 0;}
   if(dl(s,1))return -1;
  }
  return -1;
 }
 if(!*held||s->spi_inflight)return -1;
 if(rawwrite(s,0x18072200+n*4,1))return -1;
 *held=0;return 0;
}
static int busy(struct npa *s){uint32_t v;unsigned count=0;
 if(cold(s)||rawread(s,0x18004000,&v))return -1;
 while(v&0x20){
  if(count>100)return -1;
  if(cold(s)||rawread(s,0x18004000,&v)||dl(s,50))return -1;
  count++;
 }
 return 0;
}
static int spiok(unsigned a,int w){
 static const unsigned reads[]={0x02c,0x108,0x130,0x134,0x138,0x13c,0x034,0x038,0xa18,0xa1c};
 static const unsigned writes[]={0x064,0x0bc,0x108,0x034,0x038,0x070,0x380,0x390,0xa18,0xa1c,0x384,0x394};
 const unsigned *t=w?writes:reads;unsigned n=w?sizeof(writes)/sizeof(*writes):sizeof(reads)/sizeof(*reads);
 for(unsigned i=0;i<n;i++)if(t[i]==a)return 1;
 return 0;
}
static int spi(void *c,unsigned a,int write,uint32_t *v){struct npa *s=c;
 if(!v||!s->sema11||s->sema19||s->spi_inflight||!spiok(a,write)||busy(s))return -1;
 if(rawwrite(s,0x18004050,(write?0xa000:0xb000)|a))return -1;
 s->spi_inflight=1; /* trigger may commit even when callback reports failure */
 if(rawwrite(s,0x18004054,write?*v:0)||busy(s))return -1;
 s->spi_inflight=0;
 return write?0:(cold(s)||rawread(s,0x18004058,v));
}
static const struct npc_ops ops={ck,rd,wr,dl,sem,spi};
static int remap(struct npa *s){
 const uint32_t a[]={0x180601c4,0x180601d4,0x180601d8,0x180601dc,0x180601cc};
 uint32_t value[]={s->conn_base>>16,0x1000,0x1000,0x1000,s->mcif_base>>16},v;
 unsigned n=s->mcif_base?5:4;
 for(unsigned i=0;i<n;i++){
  if(cold(s)||rawread(s,a[i],&v)||rawwrite(s,a[i],(v&~0xfffffu)|(value[i]&0xfffff)))return -1;
  if(cold(s)||rawread(s,a[i],&v)||(v&0xfffff)!=(value[i]&0xfffff))return -1;
 }
 s->remap_done=1;return 0;
}
int npa_start(struct npa *s,const struct npa_hw *hw,void *c,unsigned opt,uint32_t conn,uint32_t mcif,unsigned build){
 if(opt!=1)return -1;
 if(!s||s->entered||s->retained||s->fatal||!hw||!hw->ap||!hw->proof||!hw->read32||!hw->write32||!hw->delay_us||conn!=0x8fb00000||(mcif&&mcif!=0x8a000000)||build>3)return -1;
 s->hw=hw;s->cookie=c;s->conn_base=conn;s->mcif_base=mcif;s->entered=1;
 if(cold(s)||remap(s)||npc_run(&ops,s,1,build,&s->result)){
  s->fatal=s->retained||s->result.halt||s->spi_inflight||s->sema11||s->sema19;
  return -1;
 }
 if(s->hw_handoff!=2){s->fatal=1;return -1;}
 return 0;
}
