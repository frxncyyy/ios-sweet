#include "wifi_common.h"
const struct ncc_reg ncc_table[NCC_COUNT]={
 {0x18000038u,0xffffffffu,0x00000000u},
 {0x1800003cu,0xffffffffu,0x000000ffu},
 {0x18000050u,0xffffffffu,0x00000000u},
 {0x18000054u,0xffffffffu,0x00000001u},
 {0x18005070u,0xffffffffu,0x0af40a04u},
 {0x18005074u,0xffffffffu,0x00a00090u},
 {0x18005078u,0xffffffffu,0x009c008cu},
 {0x1800507cu,0x00000fffu,0x00000a08u},
 {0x18005080u,0x00000fffu,0x00000094u},
 {0x18005084u,0x00000fffu,0x0000002cu},
 {0x18005088u,0xffffffffu,0x0afc0a0cu},
 {0x1800508cu,0x00000fffu,0x000000f0u},
 {0x18005090u,0x00000fffu,0x000000f4u},
 {0x18005094u,0x00000fffu,0x000000fcu},
 {0x18005098u,0x00000fffu,0x000000f8u},
 {0x18005004u,0x0000001fu,0x00000008u},
 {0x18005010u,0x0000ffffu,0x0000a03cu},
 {0x18005014u,0x0000ffffu,0x0000a03cu},
 {0x18005018u,0x0000ffffu,0x0000aa18u},
 {0x1800501cu,0x0000ffffu,0x0000aa18u},
 {0x18005020u,0x0000ffffu,0x0000a0c8u},
 {0x18005024u,0x0000ffffu,0x0000aa00u},
 {0x18005028u,0x0000ffffu,0x0000a0b4u},
 {0x1800502cu,0x0000ffffu,0x0000a34cu},
 {0x18005030u,0xffffffffu,0x00000000u},
 {0x18005034u,0xffffffffu,0x00000000u},
 {0x18005038u,0xffffffffu,0x74e0fff5u},
 {0x1800503cu,0xffffffffu,0x76e8fff5u},
 {0x18005040u,0xffffffffu,0x00000000u},
 {0x18005044u,0xffffffffu,0xffffffffu},
 {0x18005048u,0xffffffffu,0x00000019u},
 {0x1800504cu,0xffffffffu,0x00010400u},
 {0x18005050u,0xffffffffu,0x57400000u},
 {0x18005054u,0xffffffffu,0x57400000u},
 {0x18005058u,0xffffffffu,0x44e0fff5u},
 {0x1800505cu,0xffffffffu,0x44e0fff5u},
 {0x18005060u,0xffffffffu,0x00000001u},
 {0x18005064u,0xffffffffu,0x00000000u},
 {0x18005068u,0xffffffffu,0x00040019u},
 {0x1800506cu,0xffffffffu,0x00410440u},
};
static int guard(const struct ncc_ops *o,void *c){return o->check(c,NCC_REQUIRED);}
int ncc_run(const struct ncc_ops *o,void *c,unsigned opt,unsigned stage,struct ncc_result *r)
{
 unsigned i,n; uint32_t v,w; int e;
 if(!r)return -1;
 *r=(struct ncc_result){0}; r->clean=1;
 if(opt!=1||stage<1||stage>2||!o||!o->check||!o->read||!o->write||!o->barrier){r->error=-1;return -1;}
 n=stage==1?4:NCC_COUNT; r->phase=1;
 for(i=0;i<n;i++){
  if(guard(o,c)||o->read(c,ncc_table[i].address,&r->before[i])){r->error=-2;return -1;}
  r->snapshots++;
 }
 r->phase=2;
 for(i=0;i<n;i++){
  /* Recheck full snapshot before writing: unexpected concurrent changes fail. */
  if(guard(o,c)||o->read(c,ncc_table[i].address,&v)||v!=r->before[i]){r->error=-3;break;}
  w=(v&~ncc_table[i].mask)|(ncc_table[i].value&ncc_table[i].mask);
  if(guard(o,c)){r->error=-3;break;}
  r->touched|=UINT64_C(1)<<i; r->clean=0;
  e=o->write(c,ncc_table[i].address,w);
  if(e||guard(o,c)||o->barrier(c)||guard(o,c)||o->read(c,ncc_table[i].address,&v)||v!=w){r->error=-4;break;}
  r->verified++;
 }
 r->phase=3;
 /* Best effort for every touched register. Never claim restoration merely
  * because a failing write may actually have reached the device. */
 for(i=n;i>0;){i--;if(!(r->touched&(UINT64_C(1)<<i)))continue;
  if(guard(o,c)){r->restore_error=-5;continue;}
  e=o->write(c,ncc_table[i].address,r->before[i]);
  if(e||guard(o,c)||o->barrier(c)||guard(o,c)||o->read(c,ncc_table[i].address,&v)||v!=r->before[i]){r->restore_error=-6;continue;}
  r->restored|=UINT64_C(1)<<i;
 }
 r->clean=(r->restored==r->touched);
 if(!r->error&&!r->restore_error)r->phase=4;
 return r->error||r->restore_error?-1:0;
}
