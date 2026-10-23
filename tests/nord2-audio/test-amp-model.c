#include <assert.h>
#include <stdio.h>
#include "audio_hw.h"
#include "audio_amp.h"
#include "audio_board.h"
#include "test-i2c-model.h"
static int rd(void*c,uint32_t p,unsigned w,uint32_t*v){struct nau_hw*h=c;*v=m_read(h->cookie,p,w);return 0;}
static int wr(void*c,uint32_t p,unsigned w,uint32_t v){struct nau_hw*h=c;m_write(h->cookie,p,w,v);return 0;}
static unsigned waits;
static int us(void*c,unsigned n){(void)c;assert(n<=1000);if(n==1000)waits++;return 0;}
static const struct na8_io io={rd,wr,us};
int main(int argc,char**argv){assert(argc==2);unsigned char blob[4163];FILE*f=fopen(argv[1],"rb");assert(f);assert(fread(blob,1,sizeof(blob),f)==sizeof(blob));fclose(f);static const uint8_t hash[32]={0x63,0x53,0x00,0xfb,0xc9,0x6f,0x25,0x8f,0xf9,0xa3,0x5f,0x82,0x43,0x5f,0x2e,0x9d,0x3f,0xf6,0xf1,0x37,0x0c,0x15,0x60,0x67,0xb4,0x01,0x46,0xe0,0x11,0x03,0x86,0x92};assert(!nah_board_load(blob,sizeof(blob),hash));
 struct i2c8_model m;m_init(&m);struct nau_hw h={0};h.cookie=&m;h.amps[0].addr=0x34;h.amps[0].rev=0x0b74;
 for(unsigned k=1;k<=2;k++){
  m.slave[0].reg[0x62]=0xffff;waits=0;
  assert(!nah_amp_cold_init(&io,&h,&h.bus,0x34,0x0b74));assert(m.slave[0].i2crs==k&&m.slave[0].reset_reads==3&&waits==2);assert(m.slave[0].reg[0x62]==0);assert(m.slave[0].reg[2]==0x22a8);
  assert(!nah_board_profile()->apply(&h,&io,&h.bus,0));assert(m.slave[0].container_keys==k);assert(m.slave[0].reg[0xa0]==0x5a);assert(!nah_amp_lock(&io,&h,&h.bus,0x34));assert(m.slave[0].reg[0xa1]==0);
 }
 m.i2cr_stuck=1;unsigned keys=m.slave[0].container_keys;waits=0;assert(nah_amp_cold_init(&io,&h,&h.bus,0x34,0x0b74)<0);assert(m.slave[0].i2crs==3&&m.slave[0].reset_reads==101&&waits==100);assert(m.slave[0].container_keys==keys&&m.slave[0].reg[0xa0]==0);m_check_restored(&m);
 assert(!m.slave[1].accessed&&!m.slave[5].accessed&&!m.slave[6].accessed);
 puts("PASS canonical controller model coldreset/selfclear/syntheticdefaults/keypreamble over two real amp+board cycles; stuckreset stops beforekeys/tables; no unrelated slave");}
