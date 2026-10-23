#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "audio_live.h"
#include "audio_hw.h"
static uint32_t le(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
int main(void){
 _Static_assert(NAL_GAIN_MAX==NAH_MAX_GAIN_Q15,"protocol/hardware ceiling match");
 _Static_assert(NAL_F_GAIN_TOP==42 && NAL_F_DMA_PEAK_BOTTOM==47 && NAL_F_END==NAL_WORDS,"append-only reply layout");
 static const unsigned gains[16]={0,128,256,362,512,724,1024,2048,2896,4096,5793,8192,11585,16384,23170,32768};
 assert(NAL_GAIN_MAX==32768 && nal_gain_value(16)==UINT32_MAX && nal_gain_value(UINT32_MAX)==UINT32_MAX);
 for(unsigned i=0;i<16;i++)assert(nal_gain_value(i)==gains[i]);
 for(unsigned encoded=0;encoded<65536;encoded++)assert(nal_gain_index_valid(encoded)==(!(encoded&~255u)&&(encoded&15u)<16&&((encoded>>4)&15u)<16));
 assert(!nal_gain_index_valid(65536));
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
 assert(nal_caps(b,sizeof b)==NAL_CAP_BYTES && le(b+16)==0x1fe && le(b+56)==32768 && le(b+60)==16);
 /* Invalid arguments cannot consume the next token or pending slot. */
 for(unsigned encoded=0;encoded<65536;encoded++)if(!nal_gain_index_valid(encoded))
  assert(!nal_submit((NAL_GAIN<<12)|3,encoded));
 assert(!nal_submit((NAL_GAIN<<12)|3,65536));
 for(unsigned op=NAL_STATUS;op<=NAL_TONE_BOTTOM;op++)if(op!=NAL_GAIN)assert(!nal_submit((op<<12)|3,1));
 assert(nal_caps(b,sizeof b)==NAL_CAP_BYTES && le(b+40)==0 && le(b+44)==2);
 unsigned next=3;
 for(unsigned top=0;top<16;top++)for(unsigned bottom=0;bottom<16;bottom++){
  unsigned encoded=top|(bottom<<4),value=(NAL_GAIN<<12)|next;
  assert(nal_submit(value,encoded));assert(nal_submit(value,encoded));
  assert(!nal_submit(value,encoded^1));assert(nal_take(&q));assert(q.generation==encoded && q.op==NAL_GAIN);
  struct nal_request wrong=q;wrong.generation^=1;assert(nal_complete(&wrong,NAL_OK,&s)==-1);
  s.word[NAL_F_GAIN_TOP]=gains[top];s.word[NAL_F_GAIN_BOTTOM]=gains[bottom];s.word[NAL_F_GAIN_MAX]=32768;
  assert(!nal_complete(&q,NAL_OK,&s));assert(nal_reply(b,sizeof b)==NAL_BYTES);
  assert(le(b+28)==encoded && le(b+NAL_F_GAIN_TOP*4)==gains[top] && le(b+NAL_F_GAIN_BOTTOM*4)==gains[bottom] && le(b+NAL_F_GAIN_MAX*4)==32768);
  assert(nal_submit(value,encoded));assert(!nal_take(&q));next++;
 }
 for(unsigned op=NAL_TONE_TOP;op<=NAL_TONE_BOTTOM;op++){
  assert(nal_submit((op<<12)|next++,0));assert(nal_take(&q));assert(!q.generation);assert(!nal_complete(&q,NAL_OK,&s));
 }
 /* Actual zero-data USB setup carries the two nibble indices in wIndex. */
 unsigned value=(NAL_GAIN<<12)|next++;
 setup[0]=0x40;setup[1]=NAL_USB_COMMAND;setup[2]=(uint8_t)value;setup[3]=(uint8_t)(value>>8);setup[4]=0x12;setup[5]=setup[6]=setup[7]=0;
 assert(nal_usb(setup,b,sizeof b,&n)==2);assert(nal_usb(setup,b,sizeof b,&n)==2);
 setup[4]=0x13;assert(nal_usb(setup,b,sizeof b,&n)==0);
 assert(nal_take(&q)&&q.generation==0x12);assert(!nal_complete(&q,NAL_OK,&s));
 assert(nal_reply(b,sizeof b)==NAL_BYTES && le(b+28)==0x12);
 for(unsigned k=next;k<=NAL_MAX_TOKEN;k++){assert(nal_submit((NAL_STATUS<<12)|k,0));assert(nal_take(&q));assert(!nal_complete(&q,NAL_OK,&s));}
 assert(!nal_submit((NAL_STATUS<<12)|1,0));
 puts("audio USB legacy/framing/token/ACK-loss identity,256 gain pairs/all65536 encodings, argument echo/new tones/append-only fields/limit PASS");
}
