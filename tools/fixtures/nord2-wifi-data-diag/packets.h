#ifndef NSDD_TEST_PACKETS_H
#define NSDD_TEST_PACKETS_H
#include <stdint.h>
#include <stddef.h>
#include <string.h>
static void dput16(uint8_t*p,unsigned n){p[0]=(uint8_t)(n>>8);p[1]=(uint8_t)n;}
static size_t dhcp_packet(uint8_t*p,unsigned type,unsigned reply,unsigned group){
 size_t n=14+20+8+240+4;memset(p,0,n);p[0]=group?255:2;for(unsigned k=1;k<6;k++)p[k]=group?255:0;p[5]=group?255:1;p[6]=2;p[11]=2;dput16(p+12,0x0800);
 uint8_t*ip=p+14;ip[0]=0x45;ip[9]=17;dput16(ip+2,(unsigned)n-14);uint8_t*u=ip+20;dput16(u,reply?67:68);dput16(u+2,reply?68:67);dput16(u+4,(unsigned)n-34);
 uint8_t*b=u+8;b[0]=reply?2:1;b[1]=1;b[2]=6;b[236]=99;b[237]=130;b[238]=83;b[239]=99;b[240]=53;b[241]=1;b[242]=(uint8_t)type;b[243]=255;return n;
}
#endif
