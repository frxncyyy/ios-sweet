#include "wifi_adie_strap.h"
const struct nis_field nis_fields[4]={
 {0x10005050,0x100000,0x100000}, /*GPIO180 direction output*/
 {0x10005150,0x100000,0x100000}, /*GPIO180 data high*/
 {0x10005460,0x70000,0},        /*GPIO180 muxGPIO*/
 {0x18001900,1,1}              /*A-die resetdeassert*/
};
int nis_prepare(const struct nis_ops *o,void *c,struct nis_result *r)
{
 if(!r)return -1;
 *r=(struct nis_result){0};r->clean=1;
 if(!o||!o->check||!o->read||!o->write_mask||!o->barrier){r->error=-1;return -1;}
 for(unsigned i=0;i<4;i++){
  if(o->check(c)||o->read(c,nis_fields[i].address,&r->before[i])){r->error=-2;return -1;}
  r->snapshots++;
 }
 if(r->before[3]&1){r->error=-3;return -1;}
 r->prepared=1;return 0;
}
int nis_execute(const struct nis_ops *o,const struct nai_ops *id,void *c,struct nis_result *r)
{
 uint32_t v;int e;
 if(!r||!r->prepared||r->snapshots!=4||!o||!o->check||!o->read||!o->write_mask||!o->barrier)return -1;
 for(unsigned i=0;i<4;i++){
  const struct nis_field *f=&nis_fields[i];
  if(o->check(c)||o->read(c,f->address,&v)||v!=r->before[i]){r->error=-4;break;}
  if(o->check(c)){r->error=-4;break;}
  r->touched|=1u<<i;r->clean=0;
  if(o->write_mask(c,f->address,f->mask,f->value)||o->check(c)||o->barrier(c)||o->check(c)||o->read(c,f->address,&v)||v!=((r->before[i]&~f->mask)|f->value)){r->error=-5;break;}
 }
 if(!r->error){
  e=nai_read_id(id,c,1,&r->id);
  if(!r->id.clean){r->error=-6;return -1;} /* noreset/strap/common cleanup */
  if(e)r->error=-7;
 }
 for(unsigned i=4;i>0;){i--;if(!(r->touched&(1u<<i)))continue;
  const struct nis_field *f=&nis_fields[i];
  if(o->check(c)||o->read(c,f->address,&v)||(v&~f->mask)!=(r->before[i]&~f->mask))continue;
  if(o->check(c)||o->write_mask(c,f->address,f->mask,r->before[i]&f->mask)||o->check(c)||o->barrier(c)||o->check(c)||o->read(c,f->address,&v)||v!=r->before[i])continue;
  r->restored|=1u<<i;
 }
 r->clean=r->restored==r->touched;
 if(!r->clean&&!r->error)r->error=-8;
 return r->error||!r->clean?-1:0;
}
