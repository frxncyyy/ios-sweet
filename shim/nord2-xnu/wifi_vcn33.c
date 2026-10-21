/* SPDX-License-Identifier: GPL-2.0
 * Derived from selected MediaTek MT6893/MT6359P RC rail control.
 * Voltage-first is a deliberate safety reorder under verified cold guards. */
#include "wifi_vcn33.h"
const uint16_t nvc_addresses[NVC_REGS]={0x1be2,0x1be6,0x1be8,0x1bee,0x1bf4,0x1f98,0x1c08,0x1c0c,0x1c0e,0x1c14,0x1c1a,0x1f9c,0x18b4};
static int read(const struct nvc_io*o,void*c,uint16_t a,uint16_t*v){return o->owner(c)||o->read(c,a,v);}
int nvc_capture(const struct nvc_io*o,void*c,struct nvc_snapshot*s){
 if(!o||!o->owner||!o->read||!o->rc||!s)return -1;
 *s=(struct nvc_snapshot){0};
 if(o->owner(c)||o->rc(c,&s->rc))return -1;
 for(unsigned i=0;i<NVC_REGS;i++){if(read(o,c,nvc_addresses[i],&s->value[i]))return -1;s->valid|=1u<<i;}
 return 0;
}
int nvc_run(const struct nvc_io*o,void*c,struct nvc_state*s){
 if(!s||s->attempted||!o||!o->write)return -1;
 s->attempted=1;s->error=1;
 if(nvc_capture(o,c,&s->before))return -1;
 const uint16_t*v=s->before.value;
 if(!(s->before.rc&1)||(v[0]&3)||(v[1]&3)||v[2]!=0x8000||(v[3]&0x60)||(v[4]&0x8000)||(v[5]&0xf00)!=0xf00||
    (v[6]&3)||(v[7]&3)||v[8]!=0x8000||(v[9]&0x40)||(v[10]&0x8000)||(v[11]&0xf00)!=0xf00){s->error=2;return -1;}
 /* Only two masked selectors then three source masked OP_EN updates. No SW_EN/CFG/VS2 writes. */
 const uint16_t address[5]={0x1f98,0x1f9c,0x1be8,0x1be8,0x1c0e};
 const uint16_t base[5]={0x1f98,0x1f9c,0x1be8,0x1be8,0x1c0e};
 const uint16_t previous[5]={v[5],v[11],0x8000,0x8040,0x8000};
 const uint16_t desired[5]={(uint16_t)((v[5]&~0xf00u)|0xd00),(uint16_t)((v[11]&~0xf00u)|0xd00),0x8040,0x8060,0x8040};
 for(unsigned i=0;i<5;i++){
  uint16_t actual=0;s->error=3+i;
  if(read(o,c,base[i],&actual)||actual!=previous[i]||o->owner(c))return -1;
  s->mutated=1; /* Set BEFORE callback: write may commit despite failure. */
  if(o->write(c,address[i],desired[i]))return -1;
  s->writes++;
  if(read(o,c,base[i],&actual)||actual!=desired[i])return -1;
 }
 s->error=8;if(nvc_capture(o,c,&s->after))return -1;
 for(unsigned i=0;i<NVC_REGS;i++){
  uint16_t want=v[i];if(i==2)want=0x8060;if(i==8)want=0x8040;if(i==5||i==11)want=(want&~0xf00u)|0xd00;
  /* MON is dynamic RC status, not a completion requirement. VS2 read-only. */
  if(i!=1&&i!=7&&i!=12&&s->after.value[i]!=want)return -1;
 }
 s->complete=1;s->error=0;return 0;
}
