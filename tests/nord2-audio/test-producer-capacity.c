#define main na_source_full_fixture_main
#include "test-full.c"
#undef main
#include "audio_stream.h"
static struct nsa_stream producer;
static int16_t chunk[512];
static unsigned expected_old;
static void burst(uint64_t now) {
 for(unsigned n=0;n<16384;n+=256)nsa_enqueue(&producer,chunk,256,now);
}
int main(int argc,char **argv){
 assert(argc==2);expected_old=0;
 uint8_t blob[4163];FILE*f=fopen(argv[1],"rb");assert(f&&fread(blob,1,sizeof blob,f)==sizeof blob);fclose(f);
 const uint8_t digest[32]={0x63,0x53,0x00,0xfb,0xc9,0x6f,0x25,0x8f,0xf9,0xa3,0x5f,0x82,0x43,0x5f,0x2e,0x9d,0x3f,0xf6,0xf1,0x37,0x0c,0x15,0x60,0x67,0xb4,0x01,0x46,0xe0,0x11,0x03,0x86,0x92};
 assert(nah_board_load(blob,sizeof blob,digest)==0);
 for(unsigned i=0;i<512;i++)chunk[i]=(i&1)?-12000:12000;
 nsa_initialize(&producer,1);burst(1);
 if(expected_old)assert(producer.accepted==8192&&producer.dropped==8192);
 else assert(producer.accepted==16384&&!producer.dropped);
 full_prepare(0x0b74,0x0a74,0);assert(!nau_hw_init(&hw,&allio,0,nah_board_profile()));
 int start_rc=nau_hw_start(&hw,48000,2,16,0,256,0,200000);if(start_rc)fprintf(stderr,"STARTrc=%d afe=%u/%u bus=%u/%u gate=%u\n",start_rc,hw.afe.phase,hw.afe.failure,hw.bus.stage,hw.bus.failure,hw.status.gate);assert(!start_rc);
 cm.current=NAF_SRAM;assert(!nau_hw_tick(&hw,200000));
 unsigned next=1,consumed=0;
 for(uint64_t now=202000;now<=1700000;now+=2000){
  while(next<4&&now>=(uint64_t)next*16384*1000000/48000){burst(now);next++;}
  unsigned room=NAH_QUEUE_FRAMES-hw.status.queued_frames;if(room>256)room=256;
  int16_t pcm[512];unsigned n=nsa_dequeue(&producer,pcm,room);if(n)assert(nau_hw_feed(&hw,pcm,n)==(int)n);
  consumed+=96;cm.current=NAF_SRAM+4*(consumed%NAH_RING_FRAMES);int rc=nau_hw_tick(&hw,now);if(rc){fprintf(stderr,"tickfailed now=%llu rc=%d queued=%u fed=%u pcm=%u\n",(unsigned long long)now,rc,hw.status.queued_frames,hw.status.fed_frames,hw.status.pcm_frames);assert(!rc);}
 }
 assert(next==4&&producer.written==65536&&producer.count==0&&hw.status.queued_frames==0);
 assert(hw.status.pcm_frames==producer.accepted&&!producer.sound_frames);
 if(expected_old)assert(producer.dropped>0);else assert(producer.accepted==65536&&!producer.dropped);
 assert(!nau_hw_stop(&hw));m_check_restored(&bm);
 /* Nonblocking backpressure remains explicit andbounded beyond capacity. */
 nsa_flush(&producer);unsigned cap=NSA_QUEUE_FRAMES;for(unsigned n=0;n<cap/256;n++)assert(nsa_enqueue(&producer,chunk,256,3000000)==256);
 assert(nsa_enqueue(&producer,chunk,256,3000001)==0&&producer.count==cap);
 printf("PASS queue=%u written65536 accepted=%llu dropped=%llu realhardwarePCM=%u\n",NSA_QUEUE_FRAMES,(unsigned long long)producer.accepted-cap,(unsigned long long)producer.dropped-256,hw.status.pcm_frames);
}
