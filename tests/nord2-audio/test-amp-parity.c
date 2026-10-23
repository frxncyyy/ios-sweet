#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "audio_hw.h"
#include "audio_amp.h"
#include "audio_board.h"
static uint16_t registers[256];
static unsigned reset_count,reset_polls,stuck,writes,delays,clear_after=3;
static struct {unsigned reg;uint16_t value;} log_ops[128];
static int wait_us(void*c,unsigned us){(void)c;assert(us==1000);delays++;return 0;}
int na8_amp_i2cr(const struct na8_io*i,void*c,struct na8_state*s,unsigned a){(void)i;(void)c;(void)s;assert(a==0x34);assert(!writes);assert((registers[0]&9)==1);reset_count++;memset(registers,0,sizeof(registers));registers[0]=3;registers[0xfb]=0x1234;return 0;}
int na8_read16(const struct na8_io*i,void*c,struct na8_state*s,unsigned a,unsigned r,uint16_t*v){(void)i;(void)c;(void)s;assert(a==0x34);if(r==0&&reset_count&&!writes){reset_polls++;if(!stuck&&reset_polls>=clear_after)registers[0]=1;}*v=registers[r];return 0;}
int na8_write16(const struct na8_io*i,void*c,struct na8_state*s,unsigned a,unsigned r,uint16_t v){(void)i;(void)c;(void)s;assert(a==0x34);assert(reset_count==1&&reset_polls>=clear_after);assert((registers[0]&11)==1);assert(r!=0xa3&&r!=0xa7&&r!=0xa8&&r!=0xf0);assert(writes<128);log_ops[writes].reg=r;log_ops[writes++].value=v;registers[r]=v;return 0;}
static const struct na8_io io={0,0,wait_us};
int main(int argc,char**argv){assert(argc==2);unsigned char blob[4163];FILE*f=fopen(argv[1],"rb");assert(f);assert(fread(blob,1,sizeof(blob),f)==sizeof(blob));fclose(f);static const uint8_t hash[32]={0x63,0x53,0x00,0xfb,0xc9,0x6f,0x25,0x8f,0xf9,0xa3,0x5f,0x82,0x43,0x5f,0x2e,0x9d,0x3f,0xf6,0xf1,0x37,0x0c,0x15,0x60,0x67,0xb4,0x01,0x46,0xe0,0x11,0x03,0x86,0x92};assert(!nah_board_load(blob,sizeof(blob),hash));
 struct nau_hw h={0};h.amps[0].addr=0x34;h.amps[0].rev=0x0b74;registers[0]=1;registers[0x52]=0xffff;
 assert(!nah_amp_cold_init(&io,&h,&h.bus,0x34,0x0b74));assert(reset_count==1&&delays==2);unsigned silicon_end=writes;assert(silicon_end>0);assert(log_ops[0].reg==0x0f&&log_ops[1].reg==0xa0&&log_ops[1].value==(0x1234^0x5a));assert(registers[0x52]==0x57dc);
 assert(!nah_board_profile()->apply(&h,&io,&h.bus,0));assert(writes==silicon_end+3+52);assert(log_ops[silicon_end].reg==0xf&&log_ops[silicon_end].value==0x5a6b);assert(log_ops[silicon_end+1].reg==0xa1&&log_ops[silicon_end+1].value==0x5a);assert(log_ops[silicon_end+2].reg==0xa0&&log_ops[silicon_end+2].value==0x5a);assert((registers[0]&9)==1);assert(!nah_amp_lock(&io,&h,&h.bus,0x34));assert(registers[0xa1]==0);
 for(unsigned boundary=100;boundary<=101;boundary++){
  clear_after=boundary;reset_count=reset_polls=writes=delays=0;registers[0]=1;
  assert(!nah_amp_cold_init(&io,&h,&h.bus,0x34,0x0b74));assert(reset_count==1&&reset_polls==boundary&&delays==boundary-1);
 }
 reset_count=reset_polls=writes=delays=0;registers[0]=1;stuck=1;assert(nah_amp_cold_init(&io,&h,&h.bus,0x34,0x0b74)<0);assert(reset_count==1&&reset_polls==101&&delays==100&&writes==0);
 puts("PASS cold I2CR erases stale tuning, waits selfclear then silicon/table/container keys/52 ops/lock; stuck reset aborts before keys/tables, no MTP");return 0;}
