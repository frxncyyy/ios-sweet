#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "audio_hw.h"
#include "audio_stream.h"
#include "audio_live.h"
static struct nau_hw hardware;
static struct nsa_stream stream;
static struct {unsigned tone_active,tone_frames,tone_phase;uint64_t last_pending_us;}device;
static unsigned pending,result;
static struct nal_request request;
#define lck_mtx_lock(x) ((void)(x))
#define lck_mtx_unlock(x) ((void)(x))
static int nsa_lock;
int nal_take(struct nal_request *r){if(!pending)return 0;*r=request;pending=0;return 1;}
int nal_complete(const struct nal_request*r,unsigned v,const struct nal_snapshot*s){(void)r;(void)s;result=v;return 0;}
static void snapshot(struct nal_snapshot*s){memset(s,0,sizeof*s);}
int nau_hw_stop(struct nau_hw*h){h->status.running=0;return 0;}
static int start(unsigned s,unsigned d){(void)s;(void)d;hardware.status.running=1;return 0;}
#include "device-policy.inc"
static void enable_request(void){request=(struct nal_request){1,NAL_ENABLE,0};pending=1;commands();}
int main(void){
 const int16_t data[8]={12000,-12000,0,0,8,-8,-32768,32767};int16_t out[8];uint64_t last=0;
 nsa_initialize(&stream,1);assert(nsa_enqueue(&stream,data,4,1000000)==4&&stream.sound_frames==2);
 /* A successful init may take a long time: no acceptedPCM silentlydiscarded. */
 nsa_flush_idle(&stream,10000000);assert(stream.count==4&&stream.sound_frames==2);
 assert(!nsa_idle_ready(&stream,10000000,0,&last,23334)&&last==10000000);
 assert(nsa_dequeue(&stream,out,1)==1&&stream.sound_frames==1);
 assert(nsa_dequeue(&stream,out,3)==3&&stream.sound_frames==0);
 /* Wrapped downstreamqueue and actualceiling-scale eliminate falsekeepalive. */
 hardware.gain_q15=256;hardware.qr=NAH_QUEUE_FRAMES-1;hardware.qcount=2;
 hardware.queue[0]=12000;assert(hardware_sound_pending());
 assert(!nsa_idle_ready(&stream,10000001,hardware_sound_pending(),&last,23334));
 hardware.queue[0]=1;assert(!hardware_sound_pending());
 assert(!nsa_idle_ready(&stream,10020000,0,&last,23334));
 assert(nsa_idle_ready(&stream,10023335,0,&last,23334));
 hardware.qcount=0;
 /* Continuous HAL zerotails do not refresh sound or prevent quietstop. */
 const int16_t zero[8]={0};
 for(unsigned i=0;i<100;i++){
  assert(nsa_enqueue(&stream,zero,4,11000000+i*2000)==4);
  assert(!stream.sound_frames&&nsa_idle_ready(&stream,11000000+i*2000,0,&last,23334));
  nsa_flush_idle(&stream,11000000+i*2000);assert(!stream.count);
 }
 /* Muting anddisabling overridependingPCM immediately andbalancecounters. */
 assert(nsa_enqueue(&stream,data,4,12000000)==4);nsa_volume(&stream,0,1);assert(!stream.count&&!stream.sound_frames);
 nsa_volume(&stream,0,0);assert(nsa_enqueue(&stream,data,4,12000000)==4);nsa_enable(&stream,0);assert(!stream.count&&!stream.sound_frames);
 nsa_initialize(&stream,1);
 for(unsigned i=0;i<10000;i++){
  assert(nsa_enqueue(&stream,data,4,13000000+i)==4&&stream.sound_frames==2);
  assert(nsa_dequeue(&stream,out,4)==4&&!stream.sound_frames&&!stream.count);
 }
 nsa_enable(&stream,0);
 hardware.status.initialized=1;hardware.status.error=NAH_E_CLOCK_STALLED;
 enable_request();assert(result==NAL_UNAVAILABLE&&!stream.enabled&&hardware.status.error==NAH_E_CLOCK_STALLED);
 hardware.status.error=0;enable_request();assert(result==NAL_OK&&stream.enabled);
 nsa_enable(&stream,0);hardware.status.uncertain=1;enable_request();assert(result==NAL_UNAVAILABLE&&!stream.enabled);
 hardware.status.uncertain=0;hardware.status.stop_error=NAH_E_IO;enable_request();assert(result==NAL_UNAVAILABLE&&!stream.enabled);
 hardware.status.stop_error=0;hardware.status.initialized=0;enable_request();assert(result==NAL_UNAVAILABLE&&!stream.enabled);
 puts("PASS actualcandidate:longstartupPCMretained,ringtaildrain,wrappeddownstreamgain,zerotailidle,mute/disablecounters,stickyENABLErefusalwithoutreset");
}
