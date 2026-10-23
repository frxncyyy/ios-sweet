#include <assert.h>
#include <stdio.h>
#include "audio_i2c8.h"
#include "test-i2c-model.h"
static unsigned reads,writes;
static int rd(void*c,uint32_t p,unsigned w,uint32_t*v){reads++;*v=m_read(c,p,w);return 0;}
static int wr(void*c,uint32_t p,unsigned w,uint32_t v){writes++;m_write(c,p,w,v);return 0;}
static int us(void*c,unsigned n){(void)c;assert(n<=10);return 0;}
int nah_board_register_allowed(unsigned r){(void)r;return 0;}
int nah_board_change_allowed(unsigned r,uint16_t a,uint16_t b){(void)r;(void)a;(void)b;return 0;}
static const struct na8_io io={rd,wr,us};
int main(void){struct i2c8_model m;struct na8_state s;uint16_t v;
 for(unsigned slave=0x34;slave<=0x37;slave+=3){m_init(&m);s=(struct na8_state){0};m.slave[slave==0x37].reg[0]=1;assert(!na8_read16(&io,&m,&s,slave,0,&v));assert(!na8_amp_i2cr(&io,&m,&s,slave));assert(m.slave[slave==0x37].reg[0]==3 && m.trans[m.ntrans-1].value==2);assert(!s.prior_amp_valid);assert(na8_amp_i2cr(&io,&m,&s,slave)<0);m_check_restored(&m);assert(!m.slave[5].accessed&&!m.slave[6].accessed);}
 for(unsigned fault=0;fault<7;fault++){m_init(&m);s=(struct na8_state){0};s.prior_amp_valid=1;s.prior_amp_slave=0x34;s.prior_amp_reg=0;s.prior_amp_value=1;unsigned slave=0x34;if(fault==0)slave=0x51;if(fault==1)s.prior_amp_valid=0;if(fault==2)s.prior_amp_slave=0x37;if(fault==3)s.prior_amp_reg=1;if(fault==4)s.prior_amp_value=9;if(fault==5)s.prior_amp_value=0;if(fault==6)s.prior_amp_value=3;reads=writes=0;assert(na8_amp_i2cr(&io,&m,&s,slave)<0);assert(!reads&&!writes&&!m.starts);}
 m_init(&m);s=(struct na8_state){0};assert(!na8_read16(&io,&m,&s,0x34,0,&v));reads=writes=0;assert(na8_write16(&io,&m,&s,0x34,0,2)<0);assert(!reads&&!writes);m.slave[0].reg[0]=3;assert(!na8_read16(&io,&m,&s,0x34,0,&v));reads=writes=0;assert(na8_write16(&io,&m,&s,0x34,0,3)<0);assert(!reads&&!writes);puts("PASS real transport fixed I2CR literal2 only after sameamp cold SYS0, consumes evidence; runtime reset and seven unsafe paths blocked before MMIO");}
