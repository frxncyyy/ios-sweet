#include "wifi_main_layout.h"
#include "wifi_main_phy_force.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
static void put16(uint8_t*p,unsigned n){p[0]=(uint8_t)n;p[1]=(uint8_t)(n>>8);}
static void put32(uint8_t*p,uint32_t n){put16(p,n);put16(p+2,n>>16);}
int main(void){
 uint8_t result[36]={0},phy[60]={0};struct nfa_result r;
 for(unsigned type=7;type<=0xe000;type=0xe000){
  put16(result+26,type);result[28]=1;result[29]=9;result[32]=0;result[33]=0xff;
  assert(!nmt_validate_init_result(result,sizeof(result),9,0x40));
  assert(!nmt_validate_init_result(result,sizeof(result),9,0x41));
  assert(nmt_validate_init_result(result,sizeof(result),9,0x42)==-1);
  result[28]=5;assert(nmt_validate_init_result(result,sizeof(result),9,0x40)==-1);result[28]=1;
  result[32]=1;assert(nmt_validate_init_result(result,sizeof(result),9,0x40)==-1);result[32]=0;
  assert(nmt_validate_init_result(result,sizeof(result),8,0x40)==-1);
  assert(nmt_validate_init_result(result,sizeof(result)-1,9,0x40)==-1);
  put16(phy+26,type);phy[28]=5;phy[29]=9;put32(phy+32,0x556789aa);phy[36]=1;phy[37]=1;put16(phy+38,20);put16(phy+40,1);put16(phy+42,16);phy[44]=0x81;put32(phy+48,0x100);put32(phy+52,0x800);
  assert(!nfa_force_response(phy,sizeof(phy),9,0x450000,&r)&&r.valid);
  phy[28]=1;assert(nfa_force_response(phy,sizeof(phy),9,0x450000,&r)==-1);phy[28]=5;
  put32(phy+48,0x44ffff);assert(nfa_force_response(phy,sizeof(phy),9,0x450000,&r)==-1);put32(phy+48,0x100);
  phy[45]=1;assert(nfa_force_response(phy,sizeof(phy),9,0x450000,&r)==-1);phy[45]=0;
  assert(nfa_force_response(phy,sizeof(phy),8,0x450000,&r)==-1);
  assert(nfa_force_response(phy,sizeof(phy)-1,9,0x450000,&r)==-1);
  phy[40]=2;assert(nfa_force_response(phy,sizeof(phy),9,0x450000,&r)==-1);phy[40]=1;
  if(type==0xe000)break;
 }
 const unsigned bad[]={0,8,0xa0,0x3807,0xe007,0xffff};
 for(unsigned k=0;k<sizeof(bad)/sizeof(bad[0]);k++){
  put16(result+26,bad[k]);put16(phy+26,bad[k]);assert(nmt_validate_init_result(result,sizeof(result),9,0x40)==-1);assert(nfa_force_response(phy,sizeof(phy),9,0x450000,&r)==-1);
 }
 puts("PASS command-specific INIT parsers: exact0007/e000, source EID/seq/status, no CID echo assertion, PHY TLV/status/bounds/strict60 preserved; unknown types reject. Synthetic only.");
}
