#if defined(KERNEL)
#include <pexpert/arm64/board_config.h>
#endif
#if defined(NORD2)
#include "nord2_battery.h"
static int get(const struct nbg_ops *o,void *c,unsigned bus,unsigned slave,unsigned reg,unsigned bytes,uint16_t *v){
 return o->owned(c)||o->read(c,bus,slave,reg,bytes,v);
}
static int capture(const struct nbg_ops *o,void *c,struct nbg_sample *s,unsigned bit,unsigned bus,unsigned slave,unsigned reg,unsigned bytes,uint16_t *v){
 int e=get(o,c,bus,slave,reg,bytes,v);if(!e)s->raw_valid|=1u<<bit;return e;
}
int nbg_collect(const struct nbg_ops *o,void *c,unsigned enabled,unsigned identified,struct nbg_sample *s){
 if(!s)return -1;
 *s=(struct nbg_sample){0};s->error=-1;
 if(enabled!=1||!o||!o->owned||!o->read||!o->delay_ms)return -1;
 if(capture(o,c,s,0,7,0x55,0x2c,2,&s->raw_soc[0]))return -1;
 if(o->owned(c)||o->delay_ms(c,10))return -1;
 if(capture(o,c,s,1,7,0x55,0x2c,2,&s->raw_soc[1]))return -1;
 unsigned a=s->raw_soc[0],b=s->raw_soc[1];
 if(a>100||b>100||(a>b?a-b:b-a)>3)return -1;
 s->percent=b;s->soc_valid=1;s->error=0;
 /* Optional ancillary observations never fabricate/erase a valid SOC. */
 if(!capture(o,c,s,2,7,0x55,8,2,&s->raw_voltage)&&s->raw_voltage>=5000&&s->raw_voltage<=10000){s->voltage_valid=1;s->pack_mv=s->raw_voltage;}
 if(!capture(o,c,s,3,7,0x55,6,2,&s->raw_temperature)&&s->raw_temperature>=2000&&s->raw_temperature<=4000){s->temperature_valid=1;s->temperature_centi_c=((int)s->raw_temperature-2731)*10;}
 /* Linux selects0c only after reading device type BQ28Z610/ZY0603.
  * Gauge current is positive charging; Linux getter negates for its API,
  * IOPMPowerSource uses positive charging so preserve signed word here. */
 if(identified==1&&!capture(o,c,s,4,7,0x55,0x0c,2,&s->raw_current)&&s->raw_current!=0x8000){
  s->current_ma=(s->raw_current&0x8000)?(int)s->raw_current-65536:s->raw_current;
  if(s->current_ma>=-15000&&s->current_ma<=15000)s->current_valid=1;
 }
 /* MP2650 source REG13 powergood active-low, charging[3:2], REG14 faults.
  * Independent optional charger bus; do not infer cable solely from current. */
 if(!capture(o,c,s,5,11,0x5c,0x13,1,&s->raw_status[0])&&
    !capture(o,c,s,6,11,0x5c,0x14,1,&s->raw_fault)&&
    !capture(o,c,s,7,11,0x5c,0x13,1,&s->raw_status[1])&&
    s->raw_status[0]<=255&&s->raw_status[1]<=255&&s->raw_fault<=255&&
    ((s->raw_status[0]^s->raw_status[1])&0x0e)==0){
  s->charger_valid=1;s->external=!(s->raw_status[1]&2);
  unsigned state=(s->raw_status[1]>>2)&3;
  s->charging=s->external&&(state==1||state==2)&&!(s->raw_fault&0x3f);
  s->full=s->external&&state==3&&!(s->raw_fault&0x3f);
 }
 return 0;
}

#endif /* NORD2 */
