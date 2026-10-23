/* Actual hardware/board transport fixture; expected samples are independently
 * calculated and compared with packed writes into the modelled DMA SRAM. */
#define main na_balance_fixture_main
#include "test-full.c"
#undef main

static unsigned frame_cursor;
static uint64_t tick_time;
static unsigned magnitude(int x) { return x<0 ? (unsigned)-x:(unsigned)x; }
static void step(const int16_t *pcm,unsigned frames,unsigned top,unsigned bottom,
 unsigned *ramp,unsigned *peak0,unsigned *peak1)
{
 assert(frames && frames<=96);
 if(pcm)assert(nau_hw_feed(&hw,pcm,frames)==(int)frames);
 unsigned first=frame_cursor;
 frame_cursor=(frame_cursor+frames)%NAH_RING_FRAMES;
 cm.current=NAF_SRAM+4*frame_cursor;tick_time+=2000;
 assert(!nau_hw_tick(&hw,tick_time));
 for(unsigned i=0;i<frames;i++) {
  int left=pcm?pcm[2*i]:0,right=pcm?pcm[2*i+1]:0;
  if((left && top)||(right && bottom)) {if(*ramp<480)(*ramp)++;}
  int expected0=(int)((int64_t)left*top*(*ramp)/(INT64_C(32768)*480));
  int expected1=(int)((int64_t)right*bottom*(*ramp)/(INT64_C(32768)*480));
  uint32_t expected=(uint16_t)expected0|((uint32_t)(uint16_t)expected1<<16);
  uint32_t actual=cm.sram[(first+i)%NAH_RING_FRAMES];
  if(actual!=expected)fprintf(stderr,"gain=%u/%u frame=%u ramp=%u actual=%08x expected=%08x\n",top,bottom,i,*ramp,actual,expected);
  assert(actual==expected);
  assert(magnitude((int16_t)actual)<=top && magnitude((int16_t)(actual>>16))<=bottom);
  if(magnitude(expected0)>*peak0)*peak0=magnitude(expected0);
  if(magnitude(expected1)>*peak1)*peak1=magnitude(expected1);
 }
 assert(hw.status.gain_ramp_frames==*ramp);
 assert(hw.status.dma_peak[0]==*peak0 && hw.status.dma_peak[1]==*peak1);
}
static void begin(unsigned top,unsigned bottom,unsigned legacy)
{
 assert(!(legacy ? nau_hw_start(&hw,48000,2,16,0,top,0,tick_time):
  nau_hw_start_channels(&hw,48000,2,16,0,top,bottom,0,tick_time)));
 assert(hw.gain_q15[0]==top && hw.gain_q15[1]==bottom);
 assert(hw.status.gain_ramp_frames==0);
 assert(!hw.status.dma_peak[0]&&!hw.status.dma_peak[1]);
 frame_cursor=0;cm.current=NAF_SRAM;assert(!nau_hw_tick(&hw,tick_time));
}
static void lifecycle(unsigned top,unsigned bottom,unsigned legacy)
{
 full_prepare(0x0c74,0x0c74,0);
 assert(!nau_hw_init(&hw,&allio,0,nah_board_profile()));
 tick_time=100000;begin(top,bottom,legacy);
 unsigned ramp=0,peak0=0,peak1=0;
 int16_t zeros[192]={0},pcm[192];
 for(unsigned i=0;i<96;i++)pcm[2*i]=pcm[2*i+1]=(i&1)?INT16_MIN:INT16_MAX;
 /* Starvation and actual queued digital silence must not burn the fade-in. */
 step(0,96,top,bottom,&ramp,&peak0,&peak1);
 step(zeros,96,top,bottom,&ramp,&peak0,&peak1);
 for(unsigned n=0;n<6;n++)step(pcm,96,top,bottom,&ramp,&peak0,&peak1);
 assert(ramp==((top||bottom)?480u:0u));
 assert(peak0==top && peak1==bottom); /* includes negative full scale afterfade */
 assert(!nau_hw_stop(&hw));assert(!hw.status.gain_ramp_frames);
 assert(hw.status.dma_peak[0]==peak0&&hw.status.dma_peak[1]==peak1);
 m_check_restored(&bm);
 /* A new start resets peaks and ramps from frame1 again, not steady-state. */
 tick_time+=100000;begin(top,bottom,legacy);ramp=peak0=peak1=0;
 step(pcm,96,top,bottom,&ramp,&peak0,&peak1);
 assert(ramp==((top||bottom)?96u:0u));
 assert(!nau_hw_stop(&hw));assert(!hw.status.gain_ramp_frames);
 m_check_restored(&bm);
}
int main(int argc,char **argv)
{
 assert(argc==2);uint8_t blob[4163];FILE *f=fopen(argv[1],"rb");assert(f);
 assert(fread(blob,1,sizeof blob,f)==sizeof blob&&fgetc(f)==EOF);fclose(f);
 const uint8_t digest[32]={0x63,0x53,0x00,0xfb,0xc9,0x6f,0x25,0x8f,0xf9,0xa3,0x5f,0x82,0x43,0x5f,0x2e,0x9d,0x3f,0xf6,0xf1,0x37,0x0c,0x15,0x60,0x67,0xb4,0x01,0x46,0xe0,0x11,0x03,0x86,0x92};
 assert(!nah_board_load(blob,sizeof blob,digest));
 const unsigned gains[]={0,128,256,362,512,724,1024,2048,2896,4096,5793,8192,11585,16384,23170,32768};
 const unsigned count=sizeof gains/sizeof gains[0];
 for(unsigned a=0;a<count;a++)for(unsigned b=0;b<count;b++)lifecycle(gains[a],gains[b],0);
 for(unsigned a=0;a<count;a++)lifecycle(gains[a],gains[a],1);
 /* Gain validation is before AFE/controller reads OR writes, for eachside. */
 full_prepare(0x0c74,0x0c74,0);assert(!nau_hw_init(&hw,&allio,0,nah_board_profile()));
 unsigned writes=cm.writes,reads=cm.reads,transactions=bm.ntrans;
 assert(nau_hw_start_channels(&hw,48000,2,16,0,NAH_MAX_GAIN_Q15+1,256,0,0)==NAH_E_ARG);
 assert(nau_hw_start_channels(&hw,48000,2,16,0,256,NAH_MAX_GAIN_Q15+1,0,0)==NAH_E_ARG);
 assert(nau_hw_start_channels(&hw,48000,2,16,0,UINT32_MAX,0,0,0)==NAH_E_ARG);
 assert(nau_hw_start(&hw,48000,2,16,0,NAH_MAX_GAIN_Q15+1,0,0)==NAH_E_ARG);
 assert(cm.writes==writes&&cm.reads==reads&&bm.ntrans==transactions);
 /* Gain0 on one channel plus input solely there must not advance fade-in. */
 tick_time=100000;begin(0,1024,0);
 unsigned ramp=0,peak0=0,peak1=0;int16_t isolated[192];
 for(unsigned i=0;i<96;i++){isolated[2*i]=INT16_MIN;isolated[2*i+1]=0;}
 step(isolated,96,0,1024,&ramp,&peak0,&peak1);assert(!ramp);
 assert(!nau_hw_stop(&hw));
 /* Fatal DMA ownership loss aborts an active ramp, retaining observed peaks. */
 tick_time+=100000;begin(512,1024,0);ramp=peak0=peak1=0;
 for(unsigned i=0;i<192;i++)isolated[i]=INT16_MIN;
 step(isolated,96,512,1024,&ramp,&peak0,&peak1);
 cm.current=NAF_SRAM+1;
 assert(nau_hw_tick(&hw,tick_time+2000)==NAH_E_POINTER);
 assert(!hw.status.running&&!hw.status.gain_ramp_frames);
 assert(hw.status.dma_peak[0]==peak0&&hw.status.dma_peak[1]==peak1);
 /* Silent diagnostic consumes queued PCM but never gain-ramp frames. */
 full_prepare(0x0c74,0x0c74,0);assert(!nau_hw_init(&hw,&allio,0,nah_board_profile()));
 assert(!nau_hw_start_channels(&hw,48000,2,16,1,1024,724,0,100000));
 cm.current=NAF_SRAM;assert(!nau_hw_tick(&hw,100000));
 assert(nau_hw_feed(&hw,isolated,96)==96);
 cm.current=NAF_SRAM+4*96;assert(!nau_hw_tick(&hw,102000));
 assert(!hw.status.queued_frames&&!hw.status.gain_ramp_frames);
 assert(!hw.status.dma_peak[0]&&!hw.status.dma_peak[1]);
 for(unsigned i=0;i<96;i++)assert(!cm.sram[i]);
 assert(!nau_hw_stop(&hw));
 puts("PASS 256 channel pairs +16legacy, exact packedDMA/ramp/zero/isolation/peaks/restart/refusal through unity, real0c74 boardtransport (not acoustic safety)");
}
