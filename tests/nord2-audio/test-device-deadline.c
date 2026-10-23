/* Tests current production start(), extracted at test time into a temporary include.
 * Low-level start/stop are mocked to inject bounded startup latency/errors.
 * This does NOT claim clock/I2C/hardware execution coverage. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "audio_hw.h"
#include "audio_live.h"
static unsigned output_gain[2]={256,256};
static struct nau_hw hardware;
static struct {unsigned starts,tone_mode;}device;
static uint64_t clock_us,latency;
static int bad_clock,start_error,stop_error,stop_calls,console_calls,diagnostic_calls,clock_calls;
static uint64_t now_us(void){clock_calls++;return clock_us;}
static void diagnose(const char *where,int e,unsigned initial){(void)where;(void)e;(void)initial;diagnostic_calls++;}
static int nau_hw_start_mock(struct nau_hw *h,unsigned rate,unsigned channels,unsigned bits,unsigned silent,unsigned top,unsigned bottom,unsigned duration,uint64_t begin){
 assert(rate==48000&&channels==2&&bits==16&&silent<=1&&top==256&&bottom==256);
 if(duration>NAH_MAX_TEST_US||begin>UINT64_MAX-duration)return NAH_E_ARG;
 if(start_error)return start_error;
 h->status.running=1;h->deadline_us=duration?begin+duration:0;
 clock_us=bad_clock==1?begin-1:(bad_clock==2?UINT64_MAX-duration/2:begin+latency);
 return 0;
}
static int nau_hw_stop_mock(struct nau_hw *h){stop_calls++;h->status.running=0;h->status.stop_error=stop_error;h->status.uncertain=!!stop_error;return stop_error;}
#define nau_hw_start_channels nau_hw_start_mock
#define nau_hw_stop nau_hw_stop_mock
#define printf(...) ((void)++console_calls)
#include "audio-start-under-test.inc"
#undef printf
static void reset(void){hardware=(struct nau_hw){0};device.starts=0;clock_us=1000000;latency=0;bad_clock=start_error=stop_error=stop_calls=console_calls=diagnostic_calls=clock_calls=0;}
int main(void){
 (void)&nau_hw_stop_mock; /* Keep old-source negative control warning-clean. */

 uint64_t latency_us[]={1200000,0,50000,250000};
 for(unsigned silent=0;silent<=1;silent++)for(unsigned i=0;i<4;i++){
 reset();latency=latency_us[i];assert(!start(silent,1000000));
 assert(hardware.deadline_us-clock_us==1000000);assert(!console_calls&&!diagnostic_calls&&!stop_calls&&device.starts==1);
 }
 reset();latency=1200000;assert(!start(0,0));assert(!hardware.deadline_us&&clock_calls==1&&!console_calls); /* ContinuousHAL unchanged. */
 reset();assert(start(0,NAH_MAX_TEST_US+1)==NAH_E_ARG);assert(!hardware.status.running&&!device.starts&&!stop_calls&&diagnostic_calls==1);
 reset();start_error=NAH_E_AMPS;assert(start(0,1000000)==NAH_E_AMPS);assert(!stop_calls&&!device.starts&&diagnostic_calls==1);
 for(int bad=1;bad<=2;bad++)for(int stop_fails=0;stop_fails<=1;stop_fails++){
 reset();bad_clock=bad;stop_error=stop_fails?NAH_E_IO:0;
 assert(start(0,1000000)==NAH_E_DEADLINE);assert(stop_calls==1&&!hardware.status.running&&!device.starts&&diagnostic_calls==1);
 assert(hardware.status.error==NAH_E_DEADLINE&&hardware.status.stop_error==stop_error&&hardware.status.uncertain==(unsigned)stop_fails);
 }
 puts("PASS fixed: full1s AFTER boundedstartup, continuousHAL unchanged, maxdurationvalidation, startfailure, clockregression/overflow cleanup, noDMAconsole");
}
