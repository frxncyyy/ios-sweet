#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "wifi_station_data_diag.h"
#include "packets.h"
static void xid(uint8_t*p,unsigned id){p[46]=(uint8_t)(id>>24);p[47]=(uint8_t)(id>>16);p[48]=(uint8_t)(id>>8);p[49]=(uint8_t)id;}
static void client(uint8_t*p){memcpy(p+70,p+6,6);}
int main(void){
 uint8_t p[1518];struct nsdd s={0};size_t n=dhcp_packet(p,1,0,2);client(p);xid(p,0x12345678);
 nsdd_publish(&s,1,1,3,p,n);assert(s.identities==1);
 for(unsigned k=0;k<30;k++)nsdd_publish(&s,1,k+2,3,p,n);assert(s.identities==1);
 n=dhcp_packet(p,2,1,2);client(p);xid(p,0x12345678);nsdd_receive(&s,1,p,n);assert(s.reply_match[2]==1&&!s.reply_other[2]);
 p[49]^=1;nsdd_receive(&s,1,p,n);assert(s.reply_other[2]==1);p[49]^=1;p[70]^=2;nsdd_receive(&s,1,p,n);assert(s.reply_other[2]==2);p[70]^=2;
 p[42]=1;nsdd_receive(&s,1,p,n);assert(s.reply_match[2]==1&&s.reply_other[2]==2);p[42]=2;p[35]=68;p[37]=67;nsdd_receive(&s,1,p,n);assert(s.reply_match[2]==1&&s.reply_other[2]==2);
 for(unsigned k=0;k<n;k++)nsdd_receive(&s,1,p,k);assert(s.reply_match[2]==1&&s.reply_other[2]==2);
 for(unsigned k=0;k<5;k++){n=dhcp_packet(p,1,0,2);client(p);xid(p,k+1);nsdd_publish(&s,1,k+100,3,p,n);}assert(s.identities==4);
 n=dhcp_packet(p,5,1,2);client(p);xid(p,0x12345678);nsdd_receive(&s,1,p,n);assert(s.reply_other[5]==1);xid(p,5);nsdd_receive(&s,1,p,n);assert(s.reply_match[5]==1);
 for(unsigned k=0;k<1000;k++)nsdd_reject(&s,1,NSDD_RX_REPLAY);assert(s.rejected[NSDD_RX_REPLAY]==1000&&s.reject_logged[NSDD_RX_REPLAY]==2);
 nsdd_reject(&s,1,0);nsdd_reject(&s,1,NSDD_RX_REASON_END);assert(!s.rejected[0]);s.rejected[NSDD_RX_REPLAY]=UINT32_MAX;nsdd_reject(&s,1,NSDD_RX_REPLAY);assert(s.rejected[NSDD_RX_REPLAY]==UINT32_MAX);
 uint8_t tx[88]={0};tx[0]=127;tx[1]=3;tx[8]=1;tx[10]=2;tx[11]=3;tx[12]=1;tx[13]=254;tx[16]=0x78;tx[17]=0x56;tx[18]=0x34;tx[19]=0x12;tx[24]=0xef;tx[25]=0xcd;tx[26]=0xab;tx[27]=0x89;
 nsdd_event(&s,1,tx,88,1);tx[8]=0;nsdd_event(&s,1,tx,88,1);
 for(unsigned k=0;k<5000;k++)nsdd_receive(&s,1,p,n);assert(s.rx_samples==5&&s.match_logs==12);
 nsdd_summary(&s,1);for(unsigned k=0;k<sizeof s.identity;k++)assert(!((uint8_t*)s.identity)[k]);assert(!s.identities&&!s.identity_next);struct nsdd before=s;nsdd_receive(&s,1,p,n);nsdd_reject(&s,1,1);nsdd_rx_snapshot(&s,1);assert(!memcmp(&s,&before,sizeof s));
 puts("PASS bounded DHCP matching, rejected/malformed/nonreply separation, identity eviction/wipe, RX saturation, trace bounds and guarded source TXS fields");return 0;
}
