#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "audio_live.h"
static uint32_t le(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
int main(void){
 uint8_t b[NAL_BYTES],setup[8]={0xc0,NAL_USB_CAP,0,0,0,0,64,0};uint32_t n=0;
 assert(nal_usb(setup,b,sizeof b,&n)==1&&n==64&&le(b+36)==0);
 assert(!nal_initialize(0x123456789abcdef0ULL));assert(nal_initialize(1)==-1);
 assert(!nal_submit((NAL_TONE<<12)|1,1));assert(!nal_submit(1,0));
 assert(nal_submit((NAL_TONE<<12)|1,0));assert(nal_submit((NAL_TONE<<12)|1,0));
 assert(!nal_submit((NAL_STOP<<12)|1,0));assert(!nal_submit((NAL_STOP<<12)|2,0));
 struct nal_request q;assert(nal_take(&q));assert(!nal_take(&q));
 struct nal_snapshot s={0};s.word[NAL_F_ERROR]=(uint32_t)-7;
 assert(!nal_complete(&q,NAL_REFUSED,&s));assert(nal_reply(b,sizeof b)==NAL_BYTES);
 assert(le(b)==NAL_MAGIC&&le(b+20)==1&&le(b+NAL_F_ERROR*4)==(uint32_t)-7);
 assert(nal_submit((NAL_TONE<<12)|1,0));assert(!nal_take(&q));
 assert(nal_submit((NAL_STATUS<<12)|2,0));assert(nal_take(&q));assert(!nal_complete(&q,NAL_OK,&s));
 setup[0]=0x40;assert(nal_usb(setup,b,sizeof b,&n)==0);
 setup[0]=0xc0;setup[1]=0x60;assert(nal_usb(setup,b,sizeof b,&n)==-1);
 setup[1]=NAL_USB_COMMAND;setup[6]=0;assert(nal_usb(setup,b,sizeof b,&n)==0);
 for(unsigned k=3;k<=NAL_MAX_TOKEN;k++){assert(nal_submit((NAL_STATUS<<12)|k,0));assert(nal_take(&q));assert(!nal_complete(&q,NAL_OK,&s));}
 assert(!nal_submit((NAL_STATUS<<12)|1,0));
 puts("audio USB queue/token/retry/framing/limit PASS");
}
