#include "wifi_main_command.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
int main(void){uint8_t b[80],d[16];size_t n;
 for(unsigned seq=0;seq<256;seq++)for(unsigned cid=0x40;cid<=0x41;cid++){
 memset(b,0xcc,sizeof(b));assert(!nwt_finish(b,sizeof(b),cid,seq,&n));assert(n==(cid==0x40?68:72));assert(b[0]==n&&b[3]==1&&b[6]==1&&b[32]==n-32&&b[36]==cid&&b[37]==0xa0&&b[39]==seq&&b[n]==0xcc);
 for(size_t i=0;i<n;i++)if(i!=0&&i!=3&&i!=6&&i!=32&&i!=36&&i!=37&&i!=39)assert(!b[i]);
 assert(nwt_finish(b,n-1,cid,seq,&n));
 }
 assert(nwt_finish(b,sizeof(b),0x42,0,&n));assert(!nwt_tx_desc(d,0x90016000,72)&&d[6]==72&&d[7]==0x40);assert(nwt_tx_desc(d,0x100000000ULL,72));assert(nwt_tx_desc(d,0xfffffff0,72));assert(nwt_tx_desc(d,0x90000000,0x4000));
 assert(!nwt_rx_desc(d,0x90004000,3840));assert(nwt_rx_complete(d,0x90004000,3840,&n));d[6]=36;d[7]=0xc0;assert(!nwt_rx_complete(d,0x90004000,3840,&n)&&n==36);d[7]=0x80;assert(nwt_rx_complete(d,0x90004000,3840,&n));d[7]=0xc0;d[12]=0xff;assert(!nwt_rx_complete(d,0x90004000,3840,&n));d[8]=1;assert(nwt_rx_complete(d,0x90004000,3840,&n));d[8]=0;d[0]^=1;assert(nwt_rx_complete(d,0x90004000,3840,&n));puts("PASS sourcefinish512seqcases, exactzeroreservedpayload, selectedlow32DMAoverflow/descriptorownership/lengthguards; purebytes only");}
