#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "wifi_ios_policy.h"
static unsigned checks;
static void put(unsigned char*p,unsigned v){for(unsigned i=0;i<4;i++)p[i]=(unsigned char)(v>>(8*i));}
/* Same-IPSW dictionary builder known-field projection. Absent options remain
 * zero after bzero5456; dictionary path writes INT32_MIN unconditionally.
 * No SSID/BSSID identity bytes from hardware are loaded by this test. */
static void caller(unsigned char*p){memset(p,0,5456);put(p,1);put(p+4,3);put(p+52,1);put(p+56,1);put(p+68,13);put(p+4880,0x80000000u);for(unsigned i=0;i<13;i++){put(p+72+12*i,1);put(p+76+12*i,i+1);put(p+80+12*i,10|128);}}
static void yes(unsigned char*p,struct nss_info*i){struct nss_request r;assert(!niwi_scan_admit(p,5456,i,&r)&&r.count==13);checks++;}
static void no(unsigned char*p,struct nss_info*i){struct nss_request r,b;memset(&r,0xa5,sizeof r);b=r;assert(niwi_scan_admit(p,5456,i,&r)<0&&!memcmp(&r,&b,sizeof r));checks++;}
int main(void){unsigned char p[5456],base[5456],power[24]={0};struct nss_info info={0};info.count=13;for(unsigned i=0;i<13;i++){info.channels[i].band=1;info.channels[i].channel=i+1;}caller(base);memcpy(p,base,sizeof p);yes(p,&info);
/* Nonsemantic fields/inactive storage are independently poisoned one byte at
 * a time. This is robustness coverage, not a claim the real caller poisons. */
for(unsigned j=0;j<5456;j++){if((j>=14&&j<16)||(j>=20&&j<52)||(j>=62&&j<64)||(j>=72+13*12&&j<4872)||(j>=4888&&j<5448)||j>=5452){memcpy(p,base,sizeof p);p[j]=0xa5;yes(p,&info);}}
memcpy(p,base,sizeof p);memset(p+8,255,6);yes(p,&info);p[8]=254;no(p,&info);
const unsigned fields[]={0,4,16,52,56,60,64,68,4872,4876,4880,4884,5448};for(unsigned i=0;i<sizeof fields/sizeof *fields;i++){memcpy(p,base,sizeof p);put(p+fields[i],0x12345678);no(p,&info);}
for(unsigned i=0;i<13;i++){memcpy(p,base,sizeof p);put(p+76+12*i,255);no(p,&info);memcpy(p,base,sizeof p);put(p+80+12*i,4);no(p,&info);}
put(power,1);put(power+4,1);put(power+8,1);assert(!niwi_power_on_admit(power,24));checks++;
for(unsigned j=12;j<24;j++){power[j]=0xa5;assert(!niwi_power_on_admit(power,24));checks++;}
put(power+8,0);assert(niwi_power_on_admit(power,24)<0);checks++;put(power+8,1);put(power+4,2);assert(niwi_power_on_admit(power,24)<0);checks++;
for(unsigned n=0;n<24;n++){assert(niwi_power_on_admit(power,n)<0);checks++;}
printf("PASS %u caller-projection/semantic/atomic/power checks; synthetic only\n",checks);}
