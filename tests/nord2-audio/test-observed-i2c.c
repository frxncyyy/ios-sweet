#include <assert.h>
#include <stdio.h>
#include "audio_i2c8.h"
#include "test-i2c-model.h"
static unsigned reads,writes;
static int rd(void*c,uint32_t p,unsigned w,uint32_t*v){reads++;*v=m_read(c,p,w);return 0;}
static int wr(void*c,uint32_t p,unsigned w,uint32_t v){writes++;m_write(c,p,w,v);return 0;}
static int us(void*c,unsigned n){(void)c;assert(n<=10);return 0;}
static const struct na8_io io={rd,wr,us};
int nah_board_register_allowed(unsigned r){(void)r;return 0;}
int nah_board_change_allowed(unsigned r,uint16_t a,uint16_t b){(void)r;(void)a;(void)b;return 0;}
int main(void){for(unsigned slave=0x34;slave<=0x37;slave+=3){struct i2c8_model m;m_init(&m);struct na8_state s={0};uint16_t v=0;assert(!na8_read16(&io,&m,&s,slave,0x13,&v));assert(v==0xdead);unsigned before=reads+writes;assert(na8_write16(&io,&m,&s,slave,0x13,0)<0);assert(reads+writes==before);m_check_restored(&m);assert(!m.slave[5].accessed&&!m.slave[6].accessed);}puts("PASS fixed STATUS13 reads on both owned amps; write denied before MMIO, controller restored, no unrelated slaves");}
