#include "wifi_vcn33.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static uint16_t regs[65536];static unsigned calls,fail_at,writes;static unsigned owner_ok=1;
static int fault(void){return ++calls==fail_at;}
static int owner(void*c){(void)c;return !owner_ok||fault();}
static int readreg(void*c,uint16_t a,uint16_t*v){(void)c;if(fault())return -1;*v=regs[a];return 0;}
static int write(void*c,uint16_t a,uint16_t v){(void)c;writes++;assert(a==0x1f98||a==0x1f9c||a==0x1be8||a==0x1c0e);regs[a]=v;return fault();}
static int rc(void*c,uint32_t*v){(void)c;*v=1;return fault();}
static const struct nvc_io io={owner,readreg,write,rc};
static void reset(void){memset(regs,0,sizeof(regs));regs[0x1be8]=regs[0x1c0e]=0x8000;regs[0x1f98]=0xaf05;regs[0x1f9c]=0xbf07;regs[0x18b4]=0x1234;calls=writes=0;owner_ok=1;}
int main(void){struct nvc_state s={0};reset();assert(!nvc_run(&io,0,&s)&&s.complete&&s.writes==5&&s.mutated&&writes==5);assert(regs[0x1f98]==0xad05&&regs[0x1f9c]==0xbd07&&regs[0x1be8]==0x8060&&regs[0x1c0e]==0x8040&&regs[0x18b4]==0x1234);unsigned total=calls;assert(nvc_run(&io,0,&s)<0);
 for(unsigned n=1;n<=total;n++){reset();s=(struct nvc_state){0};fail_at=n;assert(nvc_run(&io,0,&s)<0&&!s.complete);assert(!writes||s.mutated);fail_at=0;}
 const unsigned indices[]={0,1,2,3,4,5,6,7,8,9,10,11};
 const uint16_t masks[]={1,1,1,0x20,0x8000,0x100,1,1,1,0x40,0x8000,0x100};
 for(unsigned n=0;n<12;n++){reset();s=(struct nvc_state){0};regs[nvc_addresses[indices[n]]]^=masks[n];assert(nvc_run(&io,0,&s)<0&&!writes&&!s.mutated);}
 reset();s=(struct nvc_state){0};owner_ok=0;assert(nvc_run(&io,0,&s)<0&&!writes);
 printf("PASS VCN33 fivewrites/preservation/coldguards/oneuse and %u callback fault positions; no retries/rollback\n",total);
}
