#define NAH_TEST_FULL 1
#include "test-stream.c"
#include "test-i2c-model.h"
#include "audio_board.h"
extern const struct nah_board *nah_board_profile(void);
static struct i2c8_model bm;
static int bus_address(uint32_t pa)
{ return (pa>=0x11d04000 && pa<0x11d04400) || (pa>=0x11000780 && pa<0x110007a0) || pa==0x11d05e00 || pa==0x100053f0 || pa==0x10005230 || pa==0x11d10070; }
static int allread(void *c,uint32_t pa,unsigned w,uint32_t *v)
{
 (void)c;
 if (bus_address(pa)) { *v=m_read(&bm,pa,w); return 0; }
 assert(w==4); return model_read(&cm,pa,v);
}
static int allwrite(void *c,uint32_t pa,unsigned w,uint32_t v)
{
 (void)c;
 if (pa>=0x11d04100 && pa<=0x11d0414c) { m_write(&bm,pa,w,v); return 0; }
 if (pa==0x11d05e04) { assert(v==16);bm.gate&=~v;return 0; }
 if (pa==0x11d05e08) { assert(v==16);bm.gate|=v;return 0; }
 if (pa==0x100000b8) { assert(v==32768);*regp(&cm,0x100000b0)&=~v;return 0; }
 if (pa==0x100000b4) { assert(v==32768);*regp(&cm,0x100000b0)|=v;return 0; }
 if (pa==0x10005034 || pa==0x10005044 || pa==0x10005134 || pa==0x10005144) {
  assert(v==(pa==0x10005034 || pa==0x10005134 ? 1u<<15:1u<<20));
  *regp(&cm,pa-4)|=v; return 0;
 }
 if (pa==0x10005038 || pa==0x10005048 || pa==0x10005138 || pa==0x10005148) {
  assert(v==(pa==0x10005038 || pa==0x10005138 ? 1u<<15:1u<<20));
  *regp(&cm,pa-8)&=~v; return 0;
 }
 if (hw.bus.uncertain && pa==NAF_AFE_DAC_CON0 && !(v&16)) {
  assert((*regp(&cm,0x10005130)&(1u<<15)) && (*regp(&cm,0x10005140)&(1u<<20)));
  assert((*regp(&cm,0x10005030)&(1u<<15)) && (*regp(&cm,0x10005040)&(1u<<20)));
 }
 assert(w==4); return model_write(&cm,pa,v);
}
static int alldelay(void *c,unsigned us) { (void)c;assert(us<=1000);return 0; }
static uintptr_t allworker(void *c) { (void)c;return cm.owner; }
static int alllease(void *c,unsigned acq) { (void)c;(void)acq;return 0; }
static const struct nah_io allio={allread,allwrite,alldelay,allworker,alllease};
int main(int argc,char **argv)
{
 assert(argc==2);
 static uint8_t blob[4163];
 static const uint8_t digest[32]={0x63,0x53,0x00,0xfb,0xc9,0x6f,0x25,0x8f,0xf9,0xa3,0x5f,0x82,0x43,0x5f,0x2e,0x9d,0x3f,0xf6,0xf1,0x37,0x0c,0x15,0x60,0x67,0xb4,0x01,0x46,0xe0,0x11,0x03,0x86,0x92};
 FILE *input=fopen(argv[1],"rb");assert(input);
 assert(fread(blob,1,sizeof(blob),input)==sizeof(blob));assert(fgetc(input)==EOF);fclose(input);
 assert(nah_board_load(blob,sizeof(blob),digest)==0);
 setup(); /* Initializes independent cold AFE register model. */
 memset(&hw,0,sizeof(hw));
 m_init(&bm);
 bm.slave[1].reg[0]=1;bm.slave[1].reg[0x14]=0;
 bm.slave[0].reg[0x11]=0x80;bm.slave[1].reg[0x11]=0x80;
 bm.slave[0].reg[0x10]=0;bm.slave[1].reg[0x10]=0;
 *regp(&cm,0x10001094)|=1u<<18;
 /* Truly cold clocks and asserted/reset input GPIOs, source setup needed. */
 bm.gate=16;*regp(&cm,0x100000b0)=32768;
 *regp(&cm,0x10005130)=1u<<15;*regp(&cm,0x10005140)=1u<<20;
 assert(nau_hw_init(&hw,&allio,0,nah_board_profile())==0);
 assert(hw.status.initialized && hw.amps[0].rev==0x0b74 && hw.amps[1].rev==0x0a74);
 assert(!bm.gate && !(*regp(&cm,0x100000b0)&32768));
 assert(!(*regp(&cm,0x10005130)&(1u<<15)) && !(*regp(&cm,0x10005140)&(1u<<20)));
 assert(nau_hw_start(&hw,48000,2,16,0,256,0,0)==0);
 assert((bm.slave[0].reg[0]&9)==8 && (bm.slave[1].reg[0]&9)==8);
 assert((bm.slave[0].reg[2]&15)==8 && (bm.slave[1].reg[2]&15)==8);
 assert((bm.slave[0].reg[0x26]&15)==1 && (bm.slave[1].reg[0x26]&15)==0);
 assert((bm.slave[0].reg[0x21]&0x1f0)==0x1f0);
 assert((bm.slave[0].reg[0x22]&0x7c)==0x7c);
 assert(bm.slave[0].reg[0xa1]==0 && bm.slave[1].reg[0xa1]==0);
 assert(*regp(&cm,NAF_AFE_I2S_CON3)&2); /* 32bit serial slots. */
 assert(nau_hw_tick(&hw,0)==0);
 int16_t data[480];for(unsigned i=0;i<480;i++)data[i]=(i&1)?-32768:32767;
 for(unsigned i=1;i<=12;i++) {
  assert(nau_hw_feed(&hw,data,240)==240);
  cm.current=NAF_SRAM+4*(240*i%1024);
  assert(nau_hw_tick(&hw,5000*i)==0);
 }
 assert(hw.status.pcm_frames==2880 && hw.status.ticks==12);
 assert(nau_hw_stop(&hw)==0);
 assert((bm.slave[0].reg[0]&9)==1 && (bm.slave[1].reg[0]&9)==1);
 assert(bm.gate==16);
 assert(!(*regp(&cm,0x100000b0)&32768) && hw.status.retained_shared_clock);
 assert((*regp(&cm,0x10005130)&(1u<<15)) && (*regp(&cm,0x10005140)&(1u<<20)));
 assert(!(*regp(&cm,0x10005030)&(1u<<15)) && !(*regp(&cm,0x10005040)&(1u<<20)));
 /* Continuous HAL mode survives >2seconds with producer traffic. */
 assert(nau_hw_start(&hw,48000,2,16,0,256,0,1000000)==0);
 cm.current=NAF_SRAM; assert(nau_hw_tick(&hw,1000000)==0);
 for(unsigned i=1;i<=420;i++) {
  assert(nau_hw_feed(&hw,data,240)==240);
  cm.current=NAF_SRAM+4*(240*i%1024);
  assert(nau_hw_tick(&hw,1000000+5000*i)==0);
 }
 assert(hw.status.running);
 /* No input for750ms: ring keeps clearing, then graceful bounded stop. */
 for(unsigned i=1;i<150;i++) {
  cm.current=NAF_SRAM+4*(240*(420+i)%1024);
  assert(nau_hw_tick(&hw,3100000+5000*i)==0);
 }
 cm.current=NAF_SRAM+4*(240*570%1024);
 assert(nau_hw_tick(&hw,3850000)==NAH_E_IDLE);
 assert(!hw.status.running && !hw.status.uncertain);
 /* Third complete lifecycle on the same object after idle-driven stop. */
 assert(nau_hw_start(&hw,48000,2,16,0,256,2000000,4000000)==0);
 cm.current=NAF_SRAM; assert(nau_hw_tick(&hw,4000000)==0);
 for(unsigned i=1;i<=12;i++) {
  assert(nau_hw_feed(&hw,data,240)==240);
  cm.current=NAF_SRAM+4*(240*i%1024);
  assert(nau_hw_tick(&hw,4000000+5000*i)==0);
 }
 assert(nau_hw_stop(&hw)==0 && !hw.status.uncertain);
 assert(bm.gate==16 && !hw.reset_changed);
 m_check_restored(&bm);
 for(unsigned i=2;i<M_SLAVES;i++)assert(!bm.slave[i].accessed);
 for(unsigned i=0;i<bm.ntrans;i++) {
  assert(bm.trans[i].addr==0x34 || bm.trans[i].addr==0x37);
  assert(bm.trans[i].ptr!=0xa3 && bm.trans[i].ptr!=0xa7 && bm.trans[i].ptr!=0xa8);
 }
 /* Fault during live monitor after AMPE latched: source transport must
  * not touch the next amp/bystanders; both local resets assert BEFORE DL1
  * disable and stay asserted despite inherited input directions. */
 assert(nau_hw_start(&hw,48000,2,16,0,256,0,5000000)==0);
 cm.current=NAF_SRAM;assert(nau_hw_tick(&hw,5000000)==0);
 assert((bm.slave[0].reg[0]&9)==8 && (bm.slave[1].reg[0]&9)==8);
 unsigned starts=bm.starts,ops=hw.status.ticks;
 bm.behaviour=M_BUSERR;
 /* Advance to the next4-tick monitor without relying on cumulative phase. */
 int fault=0;
 for(unsigned i=1;i<=4 && !fault;i++) {
  cm.current=NAF_SRAM+4*(96*i%1024);
  fault=nau_hw_tick(&hw,5000000+2000*i);
 }
 assert(fault==NAH_E_IO && hw.status.uncertain && hw.status.amp_reset_held);
 /* Model leaves AMPE latched: external reset is the independent safety path. */
 assert((bm.slave[0].reg[0]&8) && (bm.slave[1].reg[0]&8));
 assert(hw.status.ticks>ops && bm.starts==starts+1);
 assert(!hw.status.running && !hw.afe.active);
 assert((*regp(&cm,0x10005130)&(1u<<15)) && (*regp(&cm,0x10005140)&(1u<<20)));
 assert((*regp(&cm,0x10005030)&(1u<<15)) && (*regp(&cm,0x10005040)&(1u<<20)));
 assert(!(*regp(&cm,NAF_AFE_DAC_CON0)&16));
 assert(!bm.slave[5].accessed && !bm.slave[6].accessed);
 assert(nau_hw_start(&hw,48000,2,16,0,256,0,6000000)==NAH_E_ARG);
 printf("PASS: full cold-init/reset/PLL/AFE/silicon+container profile/amp start, continuous PCM+status, stop and reset/clock restoration (%u amp transfers, no rail or MTP commands)\n",bm.ntrans);
}
