#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "audio_i2c8.h"
#include "test-i2c-model.h"
static unsigned trace[64],n,launched,clear_sent,sticky,readfail,verify_order=1;
static int rd(void *c,uint32_t pa,unsigned w,uint32_t *v){
 if(readfail && launched && clear_sent && pa==0x11d0410c)return -1;
 *v=m_read(c,pa,w);return 0;
}
static int wr(void *c,uint32_t pa,unsigned w,uint32_t v){
 if(!launched && pa!=0x11d0410c){assert(n<64);trace[n++]=pa&255;}
 if(pa==0x11d04124){if(verify_order)assert(n>=2 && trace[n-2]==0x40 && trace[n-1]==0x24);launched=1;}
 if(pa==0x11d04138)assert(v==5);
 if(pa==0x11d0410c && launched){clear_sent=1;if(sticky)return 0;}
 m_write(c,pa,w,v);return 0;
}
static int us(void *c,unsigned t){(void)c;assert(t<=10);return 0;}
static void reset(struct i2c8_model *m,struct na8_state *s){m_init(m);*s=(struct na8_state){0};m->ch[1][0x24/2]=2;m->ch[1][0xec/2]=0x8800;n=launched=clear_sent=sticky=readfail=0;}
int main(void){
 const struct na8_io io={rd,wr,us};struct i2c8_model m;struct na8_state s;uint16_t v;
 reset(&m,&s);

 assert(!na8_read16(&io,&m,&s,0x34,3,&v));
 const unsigned expected[]={0x4c,0x48,0x10,0x28,0x1c,0x34,0x20,0x2c,0x30,4,0x38,8,0x14,0x44,0x18,0,0x40,0x24};
 assert(n==sizeof expected/sizeof expected[0] && !memcmp(trace,expected,sizeof expected));m_check_restored(&m);
 reset(&m,&s);assert(!na8_read16(&io,&m,&s,0x34,0,&v));
 n=launched=clear_sent=0;assert(!na8_write16(&io,&m,&s,0x34,0,(uint16_t)(v^8)));
 assert(n==20 && trace[15]==0 && trace[16]==0 && trace[17]==0 && trace[18]==0x40 && trace[19]==0x24);m_check_restored(&m);
 for(unsigned fault=0;fault<3;fault++){
  reset(&m,&s);sticky=fault==0;m.irq_sticky=fault==1;readfail=fault==2;
  assert(na8_read16(&io,&m,&s,0x34,3,&v)==-2 && s.uncertain && s.offline);
  /* No restore of inherited mask or other config after a clear/read fault. */
  assert(m.ch[1][8/2]==0x129 && m.ch[1][4/2]==0x68);
  unsigned starts=m.starts;assert(na8_read16(&io,&m,&s,0x37,3,&v)==-1 && m.starts==starts);
 }
 puts("PASS Android-derived setup ordering/MCUlast, normal read/write restore, sticky/dropped-clear/readfault refuses before mask restore");
}
