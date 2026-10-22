#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "wifi_ios_policy.h"
static unsigned checks;
static void put(unsigned char*p,unsigned v){for(unsigned i=0;i<4;i++)p[i]=(unsigned char)(v>>(8*i));}
/* Captured real caller scalars:5456/version1/BSS3/type1/PHY1/default timing,
 * count13/LOW_LATENCY0x20/INT32_MIN RSSI/no identities or names. Exact hardware
 * channel numbers/flags were not logged:1..13/flags10 here are a constrained
 * synthetic channel projection of the driver's published GET27 entries.
 * This is not a captured whole-payload replay or execution of Apple code. */
static void caller(unsigned char*p){memset(p,0,5456);put(p,1);put(p+4,3);put(p+52,1);put(p+56,1);put(p+68,13);put(p+4872,NIWI_SCAN_LOW_LATENCY);put(p+4880,0x80000000u);for(unsigned i=0;i<13;i++){put(p+72+12*i,1);put(p+76+12*i,i+1);put(p+80+12*i,10);}}
static void yes(unsigned char*p,struct nss_info*i){struct nss_request r;assert(!niwi_scan_admit(p,5456,i,&r)&&r.count==13);for(unsigned k=0;k<13;k++)assert(r.channels[k].band==1&&r.channels[k].channel==k+1);checks++;}
static void reject(const unsigned char*p,size_t n,struct nss_info*i,int reason){struct nss_request r,b;memset(&r,0xa5,sizeof r);b=r;assert(niwi_scan_admit(p,n,i,&r)==reason&&!memcmp(&r,&b,sizeof r));checks++;}
static void no(unsigned char*p,struct nss_info*i){struct nss_request r,b;memset(&r,0xa5,sizeof r);b=r;assert(niwi_scan_admit(p,5456,i,&r)<0&&!memcmp(&r,&b,sizeof r));checks++;}
int main(void){unsigned char p[5456],base[5456],power[24]={0};struct nss_info info={0};info.count=13;for(unsigned i=0;i<13;i++){info.channels[i].band=1;info.channels[i].channel=i+1;}caller(base);memcpy(p,base,sizeof p);yes(p,&info);
/* Both independent scheduling hints and legacy no-hint preserve channel order.
 * Running is a snapshot, not an atomic idle receipt or priority scheduler. */
for(unsigned k=0;k<3;k++){const unsigned hints[]={0,NIWI_SCAN_LOW_PRIORITY,NIWI_SCAN_LOW_LATENCY};memcpy(p,base,sizeof p);put(p+4872,hints[k]);yes(p,&info);info.scanning=1;yes(p,&info);info.scanning=0;}
for(unsigned bit=0;bit<64;bit++){memcpy(p,base,sizeof p);put(p+4872,0);put(p+4876,0);put(p+4872+4*(bit/32),1u<<(bit%32));if(bit==1||bit==5)yes(p,&info);else reject(p,sizeof p,&info,NIWI_ADMIT_FLAGS);}
memcpy(p,base,sizeof p);put(p+4872,NIWI_SCAN_LOW_PRIORITY|NIWI_SCAN_LOW_LATENCY);reject(p,sizeof p,&info,NIWI_ADMIT_FLAGS);
/* Each refusal family is separately identifiable; output stays unchanged. */
reject(base,5455,&info,NIWI_ADMIT_ARGUMENT);reject(base,SIZE_MAX,&info,NIWI_ADMIT_ARGUMENT);reject(0,5456,&info,NIWI_ADMIT_ARGUMENT);
struct nss_info bad=info;bad.count=0;reject(base,sizeof base,&bad,NIWI_ADMIT_OWNER);
const unsigned offset[]={0,4880,4872,8,72,80,76};
const unsigned value[]={2,1,8,2,0,4,255};
const int reason[]={NIWI_ADMIT_PROFILE,NIWI_ADMIT_OPTION,NIWI_ADMIT_FLAGS,NIWI_ADMIT_PEER,NIWI_ADMIT_CHANNEL,NIWI_ADMIT_CHANNEL_FLAGS,NIWI_ADMIT_ALLOWLIST};
for(unsigned k=0;k<sizeof offset/sizeof*offset;k++){memcpy(p,base,sizeof p);put(p+offset[k],value[k]);reject(p,sizeof p,&info,reason[k]);}
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
