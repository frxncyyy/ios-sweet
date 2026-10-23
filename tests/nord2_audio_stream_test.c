#include <assert.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include "audio_stream.h"
static struct nsa_stream s;
static int16_t input[NSA_QUEUE_FRAMES*2], output[NSA_QUEUE_FRAMES*2];
int main(void){
 for(unsigned i=0;i<768;i++){
  double expected=32768.0*pow(10.0,-(double)i/160.0);
  assert(fabs((double)nsa_gain(i)-expected)<0.51);
 }
 assert(!nsa_gain(768)&&!nsa_gain(UINT32_MAX));
 nsa_initialize(&s,1);
 for(unsigned i=0;i<NSA_QUEUE_FRAMES*2;i++)input[i]=(int16_t)(i*17);
 input[0]=-32768;
 for(unsigned round=0;round<50;round++){
  assert(nsa_enqueue(&s,input,NSA_QUEUE_FRAMES,1000+round)==NSA_QUEUE_FRAMES);
  assert(!nsa_enqueue(&s,input,1,2000));
  unsigned at=0;
  while(at<NSA_QUEUE_FRAMES){unsigned n=nsa_dequeue(&s,output,137);
   for(unsigned i=0;i<n*2;i++)assert(output[i]==input[at*2+i]);
   at+=n;}
  assert(!s.count);
 }
 assert(s.dropped==50 && s.peak==32768);
 assert(nsa_enqueue(&s,input,100,3000)==100);
 nsa_volume(&s,48,1);assert(!s.count&&s.muted&&!s.gain_q15);
 assert(!nsa_enqueue(&s,input,100,3000));
 nsa_volume(&s,48,0);input[0]=-32768;input[1]=32767;
 assert(nsa_enqueue(&s,input,1,4000)==1);assert(nsa_dequeue(&s,output,1)==1);
 assert(output[0]==-(int)s.gain_q15);assert(output[1]>=0&&output[1]<(int)s.gain_q15);
 nsa_enable(&s,0);assert(!nsa_enqueue(&s,input,1,4000));nsa_enable(&s,1);
 assert(!nsa_enqueue(&s,0,1,4000));assert(!nsa_enqueue(&s,input,NSA_QUEUE_FRAMES+1,4000));
 puts("audio PCM queue/volume/wrap/overflow/mute PASS");
}
