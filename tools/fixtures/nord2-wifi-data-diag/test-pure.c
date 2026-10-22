#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wifi_station_data_diag.h"
#include "packets.h"
static unsigned checks;
#define C(x) do{checks++;assert(x);}while(0)
int main(void){uint8_t p[1518],good[1518];struct nsdd_frame r,original;size_t n=dhcp_packet(good,1,0,2);C(!nsdd_classify(&r,good,n)&&r.dhcp==1&&r.parse==NSDD_DHCP&&r.destination==2&&r.ether==2048);original=r;
for(size_t k=0;k<n;k++){uint8_t*b=malloc(k?k:1);memcpy(b,good,k);C(nsdd_classify(&r,b,k)==(k<14?-1:0));C(!r.dhcp);free(b);}
for(unsigned type=1;type<=8;type++){n=dhcp_packet(p,type,type==2||type==5||type==6,0);C(!nsdd_classify(&r,p,n)&&r.dhcp==type&&r.destination==0);}
n=dhcp_packet(p,1,0,2);/* Vary private identities, XID and opaque vendor data. */memset(p+6,0x5a,6);memset(p+26,0xa5,8);memset(p+42+4,0x6b,232);C(!nsdd_classify(&r,p,n)&&!memcmp(&r,&original,sizeof r));
for(unsigned k=0;k<14;k++){memcpy(p,good,n);uint8_t*ip=p+14,*u=p+34,*b=p+42;switch(k){case 0:ip[0]=0x65;break;case 1:ip[0]=0x44;break;case 2:ip[0]=0x4f;break;case 3:dput16(ip+2,19);break;case 4:dput16(ip+2,1518);break;case 5:ip[6]=0x20;break;case 6:ip[7]=1;break;case 7:dput16(u+4,7);break;case 8:dput16(u+4,1500);break;case 9:b[0]=3;break;case 10:b[1]=2;break;case 11:b[2]=7;break;case 12:b[236]^=1;break;case 13:b[241]=4;}
 C(!nsdd_classify(&r,p,n)&&!r.dhcp&&r.parse!=NSDD_DHCP);}
/* END terminates active options; inactive storage is not zero-swept. */
memcpy(p,good,n);memset(p+n,0xa5,20);dput16(p+16,(unsigned)n+20-14);dput16(p+38,(unsigned)n+20-34);C(!nsdd_classify(&r,p,n+20)&&r.dhcp==1);
memcpy(p,good,n);p[285]=53;p[286]=1;p[287]=2;p[288]=255;dput16(p+16,275);dput16(p+38,255);C(!nsdd_classify(&r,p,289)&&r.parse==NSDD_DUP_TYPE&&!r.dhcp);
memcpy(p,good,n);p[282]=52;C(!nsdd_classify(&r,p,n)&&r.parse==NSDD_OVERLOAD&&!r.dhcp);
memcpy(p,good,n);p[284]=9;C(!nsdd_classify(&r,p,n)&&r.parse==NSDD_TYPE&&!r.dhcp);
/* More than256 active option steps yields unknown, never out-of-bounds. */
memcpy(p,good,n);memset(p+282,0,300);p[582]=255;dput16(p+16,569);dput16(p+38,549);C(!nsdd_classify(&r,p,583)&&r.parse==NSDD_OPTION_LIMIT&&!r.dhcp);
struct nsdd_txs t;for(unsigned k=0;k<=96;k++){uint8_t*q=calloc(k?k:1,1);C(nsdd_decode_txs(&t,q,k)==(k<88?-1:0));if(k>=88)C(!t.advanced&&!t.sequence&&!t.rate);free(q);}
uint8_t tx[88]={0};tx[0]=127;tx[1]=3;tx[2]=0x34;tx[3]=0x12;tx[4]=5;tx[5]=30;tx[6]=75;tx[8]=1;tx[9]=7;tx[14]=4;C(!nsdd_decode_txs(&t,tx,sizeof tx)&&t.pid==127&&t.status==3&&t.sequence==0x1234&&t.tx_count==30&&t.rate==75&&t.advanced&&t.tid==7&&t.flush==4);
struct nsdd_txs before=t;memset(tx+16,0xab,72);C(!nsdd_decode_txs(&t,tx,sizeof tx)&&!memcmp(&before,&t,sizeof t));tx[8]=0;C(!nsdd_decode_txs(&t,tx,sizeof tx)&&!t.advanced&&!t.sequence&&!t.tx_count&&!t.rate&&!t.tid&&!t.flush);
struct nsdd s={0};n=dhcp_packet(p,1,0,2);for(unsigned k=0;k<1000;k++){nsdd_publish(&s,17,k+1,3,p,n);nsdd_receive(&s,17,p,n);nsdd_event(&s,17,tx,sizeof tx,0);nsdd_event(&s,17,tx,sizeof tx,1);}C(s.tx_data==1000&&s.rx_group==1000&&s.tx_dhcp==1000&&s.rx_dhcp==1000&&s.tx_type[1]==1000&&s.rx_type[1]==1000&&s.ambiguous==2000&&s.txs_fail==2000&&s.dhcp_tx==12&&s.dhcp_rx==12&&s.txs_early==8&&s.txs_live==16);
p[12]=8;p[13]=6;for(unsigned k=0;k<100;k++){nsdd_publish(&s,17,1001+k,2,p,n);nsdd_receive(&s,17,p,n);}C(s.headers_tx==4&&s.headers_rx==4&&s.tx_security==100&&s.tx_data==1000);nsdd_summary(&s,17);struct nsdd closed=s;nsdd_summary(&s,17);nsdd_publish(&s,17,9999,3,p,n);nsdd_receive(&s,17,p,n);nsdd_event(&s,17,tx,sizeof tx,1);C(!memcmp(&closed,&s,sizeof s));
/* Every EtherType bucket and unicast/group destination is independent. */
struct nsdd buckets={0};const unsigned ethers[5]={0x0800,0x0806,0x86dd,0x888e,0x1234};for(unsigned k=0;k<5;k++){memset(p,0,14);p[0]=(k&1)?1:2;dput16(p+12,ethers[k]);nsdd_publish(&buckets,18,k+1,3,p,14);nsdd_receive(&buckets,18,p,14);C(buckets.tx_ether[k]==1&&buckets.rx_ether[k]==1);}C(buckets.tx_unicast==3&&buckets.tx_group==2&&buckets.rx_unicast==3&&buckets.rx_group==2);
/* Max-width formatting/counter saturation does not print any private bytes. */
memset(&s,0,sizeof s);s.tx_security=s.tx_data=s.rx_unicast=s.rx_group=s.tx_dhcp=s.rx_dhcp=s.txs_ok=s.txs_fail=s.ambiguous=UINT32_MAX;s.tx_unicast=s.tx_group=UINT32_MAX;for(unsigned k=0;k<5;k++)s.tx_ether[k]=s.rx_ether[k]=UINT32_MAX;for(unsigned k=0;k<9;k++)s.tx_type[k]=s.rx_type[k]=UINT32_MAX;nsdd_summary(&s,UINT64_MAX);
printf("PASS %u pure bounded public-header/status/privacy assertions\n",checks);return 0;}
