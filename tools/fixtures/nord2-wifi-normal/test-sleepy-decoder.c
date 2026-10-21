#include "wifi_async_event.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
int main(void){uint8_t p[48]={0},s;unsigned checks=0;p[3]=0x38;p[28]=7;
 for(unsigned state=0;state<256;state++)for(unsigned inner=0;inner<65536;inner+=257){p[36]=(uint8_t)state;p[26]=(uint8_t)inner;p[27]=(uint8_t)(inner>>8);p[29]=255;p[0]=p[1]=p[24]=p[25]=255;p[37]=p[38]=p[39]=255;s=0;assert(nwe_sleepy(p,40,&s)==1&&s==state);checks++;}
 for(unsigned n=0;n<40;n++){s=0xaa;assert(nwe_sleepy(p,n,&s)<0&&s==0xaa);checks++;}
 p[28]=0x28;s=0xaa;assert(!nwe_sleepy(p,40,&s)&&s==0xaa);p[28]=7;p[2]=1;assert(!nwe_sleepy(p,40,&s)&&s==0xaa);checks+=2;
 printf("PASS %u shared sleepy decoder rawuint8/opaqueheaders/actualextent/outer/EID/atomic-refusal checks. Synthetic only.\n",checks);return 0;}
