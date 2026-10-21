#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "wifi_capability_dispatcher.c"
int main(void){struct ncd_state s;struct ncd_event h;for(unsigned type=0;type<65536;type++){
 memset(&s,0,sizeof s);memset(&h,0,sizeof h);s.frame_bytes=48;s.frame[3]=0x38;s.frame[26]=(uint8_t)type;s.frame[27]=(uint8_t)(type>>8);s.frame[28]=0x27;s.frame[38]=3;
 assert(!event_kind(&s,&h)&&h.inner_type==type&&h.kind==2&&h.header_valid&&s.debug_events==1&&!s.halt);
 }
 const unsigned types[]={0x3800,0xe000,0xffff,7};
 for(unsigned k=0;k<4;k++)for(unsigned bad=0;bad<5;bad++){
  memset(&s,0,sizeof s);memset(&h,0,sizeof h);s.frame_bytes=48;s.frame[3]=0x38;s.frame[26]=(uint8_t)types[k];s.frame[27]=(uint8_t)(types[k]>>8);s.frame[28]=0x27;s.frame[38]=3;
  if (bad == 0) s.frame[3] = 0;
  if (bad == 1) s.frame_bytes = 47;
  if (bad == 2) s.frame[38] = 2;
  if (bad == 3) s.frame[44] = 1;
  if (bad == 4) s.frame[28] = 0x28;
  assert(event_kind(&s,&h)<0&&s.halt&&!s.debug_events);
 }
 puts("PASS EID27 debug opaque inner: all65536 values; outer/type/size/truncation/unknown guards retained");
 return 0;
}

