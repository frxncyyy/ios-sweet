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
static void tods_oracles(void){
 struct sw_peer p={{2,0,0,0,0,1},{2,0,0,0,0,2}};
 struct sw_txd d={.wlan=5,.own_mac=1,.queue=1,.pid=64,.retry_limit=30,.fixed_rate=11,.pse_header_len=8,.lifetime_ms=2000,.protected_frame=1};
 uint8_t f[1515]={0},old[4096],out[4096],legacy[4096],raw[1532],saved[1515];size_t used=0,z=0;
 memset(f,255,6);memcpy(f+6,p.self,6);f[12]=8;f[13]=6;for(unsigned k=14;k<sizeof f;k++)f[k]=(uint8_t)k;memcpy(saved,f,sizeof f);memset(old,0x5a,sizeof old);
 C(!nstx_build_profile(out,4096,&used,&d,f,60,0,19,0x90855000,NSTX_PROFILE_TODS,&p)&&used==142);
 C((u32(out)&65535)==110&&((u32(out)>>16)&127)==19&&((u32(out)>>23)&3)==0&&(u32(out)>>25)==1);
 C((u32(out+4)&255)==5&&((u32(out+4)>>16)&3)==2&&((u32(out+4)>>11)&31)==12&&(u32(out+4)&1024));
 C((u32(out+8)&63)==32&&(u32(out+28)&(3u<<20))==(2u<<20));
 C((u32(out+12)&2)&&!(u32(out+12)&((1u<<0)|(1u<<29)|(1u<<30)))&&(u32(out+12)&(1u<<28)));
 C((u32(out+8)&0x80000000u)&&u32(out+24)==11u<<16);C(out[44]==78&&out[45]==128&&u32(out+40)==0x90855040);
 const uint8_t*r=out+64;C(r[0]==8&&r[1]==65&&!r[2]&&!r[3]&&!memcmp(r+4,p.ap,6)&&!memcmp(r+10,p.self,6)&&!memcmp(r+16,f,6)&&!r[22]&&!r[23]);
 static const uint8_t llc[6]={170,170,3,0,0,0};C(!memcmp(r+24,llc,6)&&!memcmp(r+30,f+12,48)&&!memcmp(f,saved,sizeof f));for(unsigned k=used;k<4096;k++)C(!out[k]);
 /* Independent broadcast DHCP and IPv6 multicast vectors use the same
  * unicast AP receiver/PTK; only original destination goes into address3. */
 uint8_t dhcp[342]={0};memset(dhcp,255,6);memcpy(dhcp+6,p.self,6);dhcp[12]=8;dhcp[14]=0x45;dhcp[16]=1;dhcp[17]=72;dhcp[23]=17;dhcp[35]=68;dhcp[37]=67;dhcp[38]=1;dhcp[39]=52;dhcp[278]=99;dhcp[279]=130;dhcp[280]=83;dhcp[281]=99;
 C(!nstx_build_profile(out,4096,&used,&d,dhcp,sizeof dhcp,0,19,0x90855000,1,&p)&&used==424&&(u32(out)&65535)==392&&out[44]==104&&out[45]==129);
 C((u32(out+8)&0x80000000u)&&(u32(out+12)&(1u<<28))&&!memcmp(out+68,p.ap,6)&&!memcmp(out+80,dhcp,6)&&!memcmp(out+94,dhcp+12,sizeof dhcp-12));
 f[0]=f[1]=0x33;f[12]=0x86;f[13]=0xdd;C(!nstx_build_profile(out,4096,&used,&d,f,78,0,19,0x90855000,1,&p)&&used==160&&!memcmp(out+68,p.ap,6)&&!memcmp(out+80,f,6)&&!(u32(out+8)&0x80000000u));f[12]=8;
 /* Ordinary non-special rate policy derives from original Ethernet offset12,
  * independent of raw MAC/LLC contents. Unicast DA does not change RA/AP. */
 f[0]=2;f[13]=0;C(!nstx_build_profile(out,4096,&used,&d,f,60,0,19,0x90855000,1,&p));C(!(u32(out+8)&0xc0000000)&&!u32(out+24)&&!(u32(out+12)&(1u<<28))&&!memcmp(out+68,p.ap,6)&&!memcmp(out+80,f,6));
 C(!nstx_build_profile(out,4096,&used,&d,f,1514,0,19,0x90855000,1,&p)&&used==1596&&(u32(out)&65535)==1564&&((out[44]|out[45]<<8)&32767)==1532);
 /* Default wrapper remains identical to explicit Ethernet profile. */
 C(!nstx_build(legacy,4096,&z,&d,f,60,0,19,0x90855000)&&!nstx_build_profile(out,4096,&used,&d,f,60,0,19,0x90855000,0,0)&&used==z&&!memcmp(legacy,out,4096));
 C(!sw_data_tods(raw,sizeof raw,&z,&p,f,1514)&&z==1532);
 uint8_t overlap[1532];memcpy(overlap,f,1514);C(!sw_data_tods(overlap,sizeof overlap,&used,&p,overlap,1514)&&used==z&&!memcmp(overlap,raw,z));
 for(unsigned k=0;k<1532;k++){uint8_t*o=malloc(k?k:1);memset(o,0x5a,k);used=99;C(sw_data_tods(o,k,&used,&p,f,1514)==SW_CAPACITY&&!used);for(unsigned a=0;a<k;a++)C(o[a]==0x5a);free(o);}
 const unsigned types[]={0x888e,0x80f3,0x8137,1500};
 for(unsigned k=0;k<4;k++){f[12]=types[k]>>8;f[13]=types[k];memcpy(out,old,4096);C(nstx_build_profile(out,4096,&used,&d,f,60,0,19,0x90855000,1,&p)<0&&!used&&!memcmp(out,old,4096));}
 f[12]=8;f[13]=6;for(unsigned k=0;k<14;k++){memcpy(out,old,4096);C(nstx_build_profile(out,4096,&used,&d,f,k,0,19,0x90855000,1,&p)<0&&!used&&!memcmp(out,old,4096));}
 f[6]^=4;C(sw_tods_ethernet_valid(&p,f,60)<0);f[6]^=4;memset(f,0,6);C(sw_tods_ethernet_valid(&p,f,60)<0);memset(f,255,6);
 C(nstx_build_profile(out,4096,&used,&d,f,1515,0,19,0x90855000,1,&p)<0);C(nstx_build_profile(out,4096,&used,&d,f,60,1,19,0x90855000,1,&p)<0);C(nstx_build_profile(out,4096,&used,&d,f,60,0,19,0x90855000,2,&p)<0);C(nstx_build_profile(out,4096,&used,&d,f,60,0,19,0x90855000,1,0)<0);
 C(!sw_data_tods(raw,sizeof raw,&z,&p,f,60));uint8_t txd[32];raw[1]&=~64;C(sw_tods_data_txd(txd,32,&used,&d,raw,z)<0);raw[1]|=64;raw[1]|=8;C(sw_tods_data_txd(txd,32,&used,&d,raw,z)<0);raw[1]=65;raw[22]=1;C(sw_tods_data_txd(txd,32,&used,&d,raw,z)<0);raw[22]=0;d.protected_frame=0;C(sw_tods_data_txd(txd,32,&used,&d,raw,z)<0);
}
int main(void){report_oracles();tods_oracles();uint8_t out[4096],old[4096],f[1518]={0};memset(out,0x5a,sizeof out);memcpy(old,out,sizeof old);f[0]=2;f[6]=2;f[12]=8;struct sw_txd d={.wlan=5,.own_mac=1,.queue=1,.pid=127,.retry_limit=30,.fixed_rate=0,.pse_header_len=8,.lifetime_ms=2000,.protected_frame=1};size_t used;
 C(!nstx_build(out,sizeof out,&used,&d,f,60,0,17,0x90855000));C(used==124&&((u32(out)>>23)&3)==0&&(u32(out)&65535)==92&&(u32(out+4)&1024)&&!(u32(out+8)&0x80000000)&&u32(out+40)==0x90855040);C(out[32]==17&&out[33]==128&&out[44]==60&&out[45]==128&&!memcmp(out+64,f,60));for(unsigned n=used;n<4096;n++)C(!out[n]);
 for(unsigned n=0;n<14;n++){uint8_t*p=malloc(n?n:1);memcpy(p,f,n);memcpy(out,old,sizeof out);C(nstx_build(out,sizeof out,&used,&d,p,n,0,0,0x90855000)<0&&!used&&!memcmp(out,old,sizeof out));free(p);}

 memcpy(out,old,sizeof out);C(nstx_build(out,4095,&used,&d,f,60,0,1,0x90855000)<0&&!used&&!memcmp(out,old,sizeof out));
 C(nstx_build(out,4096,&used,&d,f,60,0,2048,0x90855000)<0&&!used&&!memcmp(out,old,sizeof out));C(nstx_build(out,4096,&used,&d,f,60,0,1,0x90855001)<0&&!used&&!memcmp(out,old,sizeof out));C(nstx_build(out,4096,&used,&d,f,60,0,1,0xfffff000)<0&&!used&&!memcmp(out,old,sizeof out));
 C(!nstx_build(out,4096,&used,&d,f,1514,0,2047,0x90855000)&&used==1578);memcpy(out,old,sizeof out);C(nstx_build(out,4096,&used,&d,f,1515,0,1,0x90855000)<0&&!used&&!memcmp(out,old,sizeof out));
 uint8_t b[12]={0};p16(b,12);p16(b+2,(6u<<11)|1);p32(b+4,2u<<16);p32(b+8,17|(5u<<15));struct nstx_report r;C(!nstx_decode_report(&r,b,sizeof b)&&r.version==2&&r.count==1&&r.ids[0]==17);for(unsigned n=0;n<12;n++){uint8_t*p=malloc(n?n:1);memcpy(p,b,n);C(nstx_decode_report(&r,p,n)<0);free(p);}struct nstx s={.published=1,.token=17};C(nstx_report(&s,b,sizeof b)==1&&s.report_done);C(!nstx_report(&s,b,sizeof b)&&s.report_done&&s.reports==1);
 printf("PASS %u normal TXP/atomic bounds/report ownership checks\n",checks);return 0;}
