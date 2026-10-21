#include "wifi_adie_id.h"
#define STA 0x18004000u
#define ADDR 0x18004050u
#define WD 0x18004054u
#define RD 0x18004058u
static int nai_guard(const struct nai_ops *o,void *c){return o->check(c,NAI_REQUIRED);}
static int poll(const struct nai_ops *o,void *c,struct nai_result *r)
{
 uint32_t v;
 for(unsigned i=0;i<100;i++){
  if(nai_guard(o,c)||o->read(c,STA,&v))return -1;
  r->polls++;
  if(!(v&0x20))return 0;
  if(i<99&&(nai_guard(o,c)||o->delay_us(c,50)))return -1;
 }
 return -1;
}
int nai_read_id(const struct nai_ops *o,void *c,unsigned opt,struct nai_result *r)
{
 int e;uint32_t v;
 if(!r)return -1;
 *r=(struct nai_result){0};r->clean=1;
 if(opt!=1||!o||!o->check||!o->acquire||!o->release||!o->read||!o->write||!o->barrier||!o->delay_us){r->error=-1;return -1;}
 r->phase=1;if(nai_guard(o,c)){r->error=-2;return -1;}
 e=o->acquire(c,11);
 if(e){r->error=-3;if(e!=1){r->semaphore_owned=1;r->clean=0;}return -1;}
 r->semaphore_owned=1;r->clean=0;
 if(poll(o,c,r)||nai_guard(o,c)||o->read(c,ADDR,&r->original_address)){r->error=-4;goto cleanup;}
 r->phase=2;
 if(nai_guard(o,c)){r->error=-5;goto cleanup;}
 r->address_touched=1;
 if(o->write(c,ADDR,0x0000b02c)||nai_guard(o,c)||o->barrier(c)||nai_guard(o,c)||o->read(c,ADDR,&v)||v!=0xb02c){r->error=-5;goto cleanup;}
 if(nai_guard(o,c)){r->error=-5;goto cleanup;}
 r->command_touched=1;
 if(o->write(c,WD,0)||nai_guard(o,c)||o->barrier(c)||poll(o,c,r)){
  r->error=-6;return -1; /* no replay/release while completion unknown */
 }
 r->command_complete=1;r->phase=3;
 if(nai_guard(o,c)||o->read(c,RD,&r->id)){r->error=-7;goto cleanup;}
 r->id_valid=1;
 r->known_chip=r->id==0x66358a00||r->id==0x66358a10||r->id==0x66358a11;
cleanup:
 r->phase=4;
 if(r->address_touched){
  if(nai_guard(o,c)||o->write(c,ADDR,r->original_address)||nai_guard(o,c)||o->barrier(c)||nai_guard(o,c)||o->read(c,ADDR,&v)||v!=r->original_address){r->restore_error=-8;return -1;}
  r->address_touched=0;
 }
 if(nai_guard(o,c)||o->release(c,11)){r->restore_error=-9;return -1;}
 r->semaphore_owned=0;r->clean=1;r->phase=5;
 return r->error?-1:!r->known_chip;
}
