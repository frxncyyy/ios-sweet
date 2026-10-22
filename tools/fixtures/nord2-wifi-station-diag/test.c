#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "wifi_station_diag.h"
static void put(uint8_t*p,unsigned v){p[0]=v;p[1]=v>>8;p[2]=v>>16;p[3]=v>>24;}
int main(void){struct nstd_record r;unsigned checks=0;for(unsigned n=0;n<=160;n++){uint8_t*p=malloc(n?n:1);memset(p,0,n);nstd_txs(&r,1,2,p,n,1,0,0,0);assert(r.v[6]==(n<36?1:n<124?2:7));nstd_eapol(&r,1,2,p,n);nstd_tx(&r,1,2,p,n,2,0);assert(r.v[7]==(n<32));free(p);checks++;}
uint8_t p[160]={0};p[36]=7;p[37]=9;p[40]=5;nstd_txs(&r,1,2,p,124,1,7,5,0);assert(r.v[6]==7&&r.v[3]==7&&r.v[4]==5&&r.v[5]==9);nstd_txs(&r,1,2,p,124,0,7,5,0);assert(r.v[6]==3);nstd_txs(&r,1,2,p,124,1,8,5,0);assert(r.v[6]==4);nstd_txs(&r,1,2,p,124,1,7,6,0);assert(r.v[6]==5);nstd_txs(&r,1,2,p,124,1,7,5,1);assert(r.v[6]==6);checks+=5;
memset(p,0,sizeof p);p[0]=2;p[1]=3;p[3]=95;p[5]=0;p[6]=0x8a;nstd_eapol(&r,1,2,p,99);assert(r.v[4]==1&&r.v[6]);p[5]=0x13;p[6]=0xca;nstd_eapol(&r,1,2,p,99);assert(r.v[4]==3&&r.v[6]);p[6]=0xcb;nstd_eapol(&r,1,2,p,99);assert(!r.v[4]);checks+=3;
memset(p,0,sizeof p);put(p,155|(10u<<16)|(2u<<23)|(1u<<25));put(p+4,5|(1u<<15)|(1u<<31));put(p+20,7|(1u<<9));put(p+24,0x4b0000);nstd_tx(&r,1,2,p,155,2,0);assert(r.v[2]==7&&r.v[3]==5&&r.v[4]==1&&r.v[5]==0x4b&&r.v[6]==155);nstd_tx(&r,1,2,p,155,2,1);assert(r.v[2]==2&&!r.v[3]&&r.v[4]==1&&r.v[5]==1&&!r.v[6]&&r.v[7]==10);checks+=2;
/* Equal public headers, arbitrary secret portions: output must be identical. */
struct nstd_record before;r=(struct nstd_record){0};p[0]=2;p[1]=3;p[2]=0;p[3]=95;p[5]=0;p[6]=0x8a;nstd_eapol(&before,1,2,p,99);memset(p+7,0xa5,92);nstd_eapol(&r,1,2,p,99);assert(!memcmp(&r,&before,sizeof r));checks++;
for(unsigned q=0;q<5;q++){memset(p,0,sizeof p);p[2]=0;p[3]=0;nstd_rx(1,2,q,p,24);}nstd_summary(&r,1,2);for(unsigned q=0;q<5;q++)assert(r.v[q]==1);assert(r.v[5]==5&&!r.v[6]&&!r.v[7]);checks+=6;printf("PASS diagnostic decoders %u checks; bounded public metadata only\n",checks);return 0;}
