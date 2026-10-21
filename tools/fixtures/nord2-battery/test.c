#include <assert.h>
#include <string.h>
#include "nord2_battery.h"
struct mock {int call,fail,soc,current,status,fault,owned,unknown;};
static int own(void *p){return ((struct mock*)p)->owned;}
static int read(void *p,unsigned bus,unsigned slave,unsigned reg,unsigned bytes,uint16_t *v){
 struct mock *m=p;if(++m->call==m->fail)return -1;
 assert((bus==7&&slave==0x55&&bytes==2)||(bus==11&&slave==0x5c&&bytes==1));
 if(bus==7){switch(reg){case 0x2c:*v=m->soc;break;case 8:*v=7900;break;case 6:*v=2981;break;case 0x0c:*v=m->current;break;default:assert(0);}}
 else if(reg==0x13)*v=m->status;else {assert(reg==0x14);*v=m->fault;}
 return 0;
}
static int delay(void *p,unsigned ms){(void)p;assert(ms==10);return 0;}
static const struct nbg_ops ops={own,read,delay};
int main(void){struct nbg_sample s;struct mock m={.soc=43,.current=1250,.status=8};
 assert(nbg_collect(&ops,&m,0,0,&s)&&!m.call&&!s.soc_valid);
 assert(!nbg_collect(&ops,&m,1,1,&s));assert(s.raw_valid==255&&s.percent==43&&s.soc_valid&&s.pack_mv==7900&&s.temperature_centi_c==2500&&s.current_ma==1250&&s.charging&&s.external&&!s.full);
 m.current=65536-600;m.status=2;assert(!nbg_collect(&ops,&m,1,1,&s));assert(s.current_ma==-600&&!s.charging&&!s.external);
 m.status=12;assert(!nbg_collect(&ops,&m,1,1,&s)&&s.full&&!s.charging);
 m.fault=8;m.status=8;assert(!nbg_collect(&ops,&m,1,1,&s)&&!s.charging);
 for(int soc=0;soc<=100;soc++){m=(struct mock){.soc=soc,.status=2};assert(!nbg_collect(&ops,&m,1,0,&s)&&s.percent==(unsigned)soc&&!s.current_valid);}
 for(int bad=101;bad<=65535;bad+=32667){m=(struct mock){.soc=bad};assert(nbg_collect(&ops,&m,1,0,&s)&&!s.soc_valid);}
 for(int f=1;f<=8;f++){m=(struct mock){.soc=50,.fail=f};(void)nbg_collect(&ops,&m,1,1,&s);if(f<=2)assert(!s.soc_valid);assert(!s.full&&!s.charging);}
 m=(struct mock){.owned=1};assert(nbg_collect(&ops,&m,1,1,&s)&&!m.call);
 return 0;
}
