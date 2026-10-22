#include "wifi_station_tx.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
static unsigned checks;
#define C(x) do{checks++;assert(x);}while(0)
static void p16(uint8_t*p,unsigned n){p[0]=n;p[1]=n>>8;}
static void p32(uint8_t*p,unsigned n){p16(p,n);p16(p+2,n>>16);}
static uint32_t u32(const uint8_t*p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static void report_oracles(void){
 uint8_t b[32];struct nstx_report r,old;memset(&old,0xa5,sizeof old);
 /* Independently encode the selected source's four union layouts, including
  * odd v0 inactive high half and v3 P1 entries that do not count as IDs. */
 for(unsigned v=0;v<4;v++){memset(b,0,sizeof b);p16(b,20);p16(b+2,(6u<<11)|3);p32(b+4,v<<16);
  if(v==0){p32(b+8,7u|(11u<<16));p32(b+12,13u|0xffff0000u);}
  else if(v==1){p32(b+8,7);p32(b+12,11);p32(b+16,13);}
  else if(v==2){p32(b+8,7u|(5u<<15));p32(b+12,11u|(9u<<15));p32(b+16,13u|(10u<<15));}
  else {p16(b,24);p32(b+8,0x80000000);p32(b+12,7u<<16);p32(b+16,11u<<16);p32(b+20,13u<<16);}
  C(!nstx_decode_report(&r,b,sizeof b)&&r.version==v&&r.count==3&&r.ids[0]==7&&r.ids[1]==11&&r.ids[2]==13);
  unsigned z=b[0];for(unsigned n=0;n<z;n++){r=old;C(nstx_decode_report(&r,b,n)<0&&!memcmp(&r,&old,sizeof r));}
  /* Header-declared extent, not stale bytes in a larger owned buffer, bounds
   * parsing; IDs beyond the source count are inactive storage. */
  b[31]=0xff;C(!nstx_decode_report(&r,b,sizeof b));
 }
 memset(b,0,sizeof b);p16(b,16);p16(b+2,(6u<<11)|2);p32(b+4,2u<<16);p32(b+8,3);p32(b+12,3);r=old;C(nstx_decode_report(&r,b,16)<0&&!memcmp(&r,&old,sizeof r));p32(b+12,2048);C(nstx_decode_report(&r,b,16)<0);p32(b+12,4);p32(b+4,4u<<16);C(nstx_decode_report(&r,b,16)<0);
 for(unsigned id=0;id<2048;id++){p16(b,12);p16(b+2,(6u<<11)|1);p32(b+4,2u<<16);p32(b+8,id);C(!nstx_decode_report(&r,b,12)&&r.ids[0]==id);}
}
int main(void){report_oracles();uint8_t out[4096],old[4096],f[1518]={0};memset(out,0x5a,sizeof out);memcpy(old,out,sizeof old);f[0]=2;f[6]=2;f[12]=8;struct sw_txd d={.wlan=5,.own_mac=1,.queue=1,.pid=127,.retry_limit=30,.fixed_rate=0,.pse_header_len=8,.lifetime_ms=2000,.protected_frame=1};size_t used;
 C(!nstx_build(out,sizeof out,&used,&d,f,60,0,17,0x90855000));C(used==124&&((u32(out)>>23)&3)==0&&(u32(out)&65535)==92&&(u32(out+4)&1024)&&!(u32(out+8)&0x80000000)&&u32(out+40)==0x90855040);C(out[32]==17&&out[33]==128&&out[44]==60&&out[45]==128&&!memcmp(out+64,f,60));for(unsigned n=used;n<4096;n++)C(!out[n]);
 for(unsigned n=0;n<14;n++){uint8_t*p=malloc(n?n:1);memcpy(p,f,n);memcpy(out,old,sizeof out);C(nstx_build(out,sizeof out,&used,&d,p,n,0,0,0x90855000)<0&&!used&&!memcmp(out,old,sizeof out));free(p);}

 memcpy(out,old,sizeof out);C(nstx_build(out,4095,&used,&d,f,60,0,1,0x90855000)<0&&!used&&!memcmp(out,old,sizeof out));
 C(nstx_build(out,4096,&used,&d,f,60,0,2048,0x90855000)<0&&!used&&!memcmp(out,old,sizeof out));C(nstx_build(out,4096,&used,&d,f,60,0,1,0x90855001)<0&&!used&&!memcmp(out,old,sizeof out));C(nstx_build(out,4096,&used,&d,f,60,0,1,0xfffff000)<0&&!used&&!memcmp(out,old,sizeof out));
 C(!nstx_build(out,4096,&used,&d,f,1514,0,2047,0x90855000)&&used==1578);memcpy(out,old,sizeof out);C(nstx_build(out,4096,&used,&d,f,1515,0,1,0x90855000)<0&&!used&&!memcmp(out,old,sizeof out));
 uint8_t b[12]={0};p16(b,12);p16(b+2,(6u<<11)|1);p32(b+4,2u<<16);p32(b+8,17|(5u<<15));struct nstx_report r;C(!nstx_decode_report(&r,b,sizeof b)&&r.version==2&&r.count==1&&r.ids[0]==17);for(unsigned n=0;n<12;n++){uint8_t*p=malloc(n?n:1);memcpy(p,b,n);C(nstx_decode_report(&r,p,n)<0);free(p);}struct nstx s={.published=1,.token=17};C(nstx_report(&s,b,sizeof b)==1&&s.report_done);C(!nstx_report(&s,b,sizeof b)&&s.report_done&&s.reports==1);
 printf("PASS %u normal TXP/atomic bounds/report ownership checks\n",checks);return 0;}
