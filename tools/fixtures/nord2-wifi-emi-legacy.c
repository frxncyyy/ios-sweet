#include <assert.h>
#include <string.h>
#include "wifi_emi_legacy.h"
static struct wifi_emi_legacy_snapshot out;
static unsigned calls;static int failure=-1,kind;
static int call(void *ctx,const uint64_t a[8],uint64_t raw[4]){
 (void)ctx;assert(wifi_emi_legacy_args_valid(a));uint32_t off;
 assert(!wifi_emi_legacy_offset(calls,&off)&&a[1]==off);
 unsigned index=calls++;
 raw[0]=index%2?2:0;raw[1]=UINT64_MAX;raw[2]=0x1234567887654321ull;raw[3]=3;
 if((int)index==failure){
  if(kind==0)return -1;
  if(kind<=4)raw[0]=UINT64_MAX-(unsigned)(kind-1);
  else if(kind<=8)raw[0]=(uint32_t)(0u-(unsigned)(kind-4));
  else raw[0]=UINT64_C(1)<<32;
 }
 return 0;
}
int main(void){
 assert(sizeof(out)>=5120&&sizeof(out)<=8192);
 unsigned allowed=0;
 for(unsigned offset=0;offset<=0x1000;offset++){
  uint64_t a[8]={WIFI_EMI_LEGACY_FID,offset,0,0,0,0,0,0};
  int want=offset%4==0&&((offset>=0x100&&offset<=0x17c)||(offset>=0x200&&offset<=0x27c)||(offset>=0x300&&offset<=0x37c)||(offset>=0x400&&offset<=0x47c));
  assert(wifi_emi_legacy_args_valid(a)==want);allowed+=want;
  for(unsigned j=2;j<8;j++){a[j]=1;assert(!wifi_emi_legacy_args_valid(a));a[j]=0;}
  a[0]=0xc2000261;assert(!wifi_emi_legacy_args_valid(a));
 }
 assert(allowed==128);uint32_t dummy;assert(wifi_emi_legacy_offset(128,&dummy));
 assert(!wifi_emi_legacy_args_valid(NULL));
 assert(wifi_emi_legacy_collect(NULL,NULL,&out)==-1);
 assert(wifi_emi_legacy_collect(call,NULL,NULL)==-1);
 assert(!wifi_emi_legacy_collect(call,NULL,&out)&&calls==128&&out.issued==128);
 for(unsigned i=0;i<128;i++)assert(out.field[i].raw[0]==(i%2?2:0)&&out.field[i].status==WIFI_EMI_LEGACY_VALUE);
 for(kind=0;kind<10;kind++)for(failure=0;failure<128;failure++){
  calls=0;memset(&out,0xa5,sizeof(out));assert(wifi_emi_legacy_collect(call,NULL,&out)==1);
  assert(calls==(unsigned)failure+1&&out.issued==calls);
  for(unsigned i=0;i<128;i++){
   assert(!!(out.valid[i/32]&(1u<<(i%32)))==(i<(unsigned)failure));
   if(i>(unsigned)failure)assert(out.field[i].status==WIFI_EMI_LEGACY_NOT_CALLED);
  }
  assert(out.field[failure].status==(kind==0?WIFI_EMI_LEGACY_TRANSPORT:kind==9?WIFI_EMI_LEGACY_INVALID:WIFI_EMI_LEGACY_ERROR));
  if(kind)assert(out.field[failure].raw[1]==UINT64_MAX&&out.field[failure].raw[2]==0x1234567887654321ull);
 }
 return 0;
}
