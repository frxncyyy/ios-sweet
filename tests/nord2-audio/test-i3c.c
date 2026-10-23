#include <assert.h>
#include <stdio.h>
#include "audio_i2c8.h"
#include "test-i2c-model.h"
static unsigned writes;
static int rd(void *c,uint32_t pa,unsigned w,uint32_t *v){*v=m_read(c,pa,w);return 0;}
static int wr(void *c,uint32_t pa,unsigned w,uint32_t v){writes++;m_write(c,pa,w,v);return 0;}
static int us(void *c,unsigned n){(void)c;assert(n<=10);return 0;}
static const struct na8_io io={rd,wr,us};
int main(void)
{
 struct i2c8_model m;struct na8_state s;uint16_t v=0;
 for(unsigned inherited=0;inherited<65536;inherited++) {
  m_init(&m);s=(struct na8_state){0};m.ch[1][0xec/2]=(uint16_t)inherited;
  m.ch[1][0x24/2]=2;m.ch[1][0xc/2]=1; /* exact measured prior completion */
  assert(!na8_read16(&io,&m,&s,0x34,3,&v) && v==0x0b74);
  assert(s.i3c==inherited && m.ch[1][0xec/2]==inherited && m.ch[1][0x24/2]==2);
  m_check_restored(&m);assert(!m.slave[5].accessed && !m.slave[6].accessed);
 }
 /* Measured8800 exercises write + cleanNACK and next-slave read. */
 m_init(&m);s=(struct na8_state){0};m.ch[1][0xec/2]=0x8800;m.ch[1][0x24/2]=2;
 assert(!na8_read16(&io,&m,&s,0x34,0,&v));
 assert(!na8_write16(&io,&m,&s,0x34,0,(uint16_t)(v^8)));
 m.slave[0].nack=1;assert(na8_read16(&io,&m,&s,0x34,3,&v)==1 && !s.uncertain);
 assert(!na8_read16(&io,&m,&s,0x37,3,&v));m_check_restored(&m);
 assert(m.ch[1][0xec/2]==0x8800 && m.ch[1][0x24/2]==2);
 /* Every preflight idle guard remains effective with inheritedI3C set. */
 for(unsigned fault=0;fault<7;fault++) {
  m_init(&m);s=(struct na8_state){0};writes=0;m.ch[1][0xec/2]=0x8800;m.ch[1][0x24/2]=2;
  if(fault==0)m.ch[1][0x24/2]=3;
  if(fault==1)m.ch[0][0x24/2]=1;
  if(fault==2)m.ch[2][0x24/2]=1;
  if(fault==3)m.ch[1][0x10/2]|=4;
  if(fault==4)m.ch[1][0xf4/2]=1;
  if(fault==5)m.ch[1][0xc/2]=2;
  if(fault==6){m.ifr1=m.ifr2=0;m.dma[2]=1;}
  assert(na8_read16(&io,&m,&s,0x34,3,&v)<0 && !writes && !m.starts);
 }
 puts("PASS all65536 inherited debug values preserved at100k, APSTART2/IRQadoption/write/NACK/restore, seven idle guards; model forbids debug/HFIFO/reset/shadow writes");
}
