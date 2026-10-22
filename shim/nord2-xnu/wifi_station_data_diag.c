#include "wifi_station_data_diag.h"
#if defined(KERNEL) || defined(_KERNEL)
#include <sys/systm.h>
#else
#include <stdio.h>
#endif
static uint32_t be16(const uint8_t*p){return (uint32_t)p[0]<<8|p[1];}
static uint32_t le16(const uint8_t*p){return p[0]|(uint32_t)p[1]<<8;}
static void inc(uint32_t*p){if(*p!=UINT32_MAX)(*p)++;}
static unsigned etype(uint32_t e){return e==0x0800?0:e==0x0806?1:e==0x86dd?2:e==0x888e?3:4;}
static uint32_t sum3(uint32_t a,uint32_t b,uint32_t c){uint64_t n=(uint64_t)a+b+c;return n>UINT32_MAX?UINT32_MAX:(uint32_t)n;}
int nsdd_classify(struct nsdd_frame*r,const uint8_t*p,size_t n){
 if(!r)return -1;*r=(struct nsdd_frame){0};if(!p||n<14||n>1518)return -1;
 r->bytes=(uint32_t)n;r->ether=be16(p+12);r->destination=(p[0]&1)?1:0;
 if(r->destination){unsigned all=1;for(unsigned k=0;k<6;k++)if(p[k]!=255)all=0;if(all)r->destination=2;}
 if(r->ether!=0x0800)return 0;
 const uint8_t*ip=p+14;size_t available=n-14;if(available<20){r->parse=NSDD_IP_SHORT;return 0;}
 size_t ihl=(ip[0]&15u)*4u;if(ip[0]>>4!=4||ihl<20||ihl>available){r->parse=NSDD_IP_HEADER;return 0;}
 size_t total=be16(ip+2);if(total<ihl||total>available){r->parse=NSDD_IP_EXTENT;return 0;}
 if(be16(ip+6)&0x3fff){r->parse=NSDD_FRAGMENT;return 0;}if(ip[9]!=17)return 0;
 if(total-ihl<8){r->parse=NSDD_UDP_EXTENT;return 0;}const uint8_t*u=ip+ihl;size_t un=be16(u+4);
 if(un<8||un>total-ihl){r->parse=NSDD_UDP_EXTENT;return 0;}
 unsigned src=be16(u),dst=be16(u+2);if(!((src==67&&dst==68)||(src==68&&dst==67)))return 0;
 const uint8_t*b=u+8;size_t bn=un-8;if(bn<240){r->parse=NSDD_BOOTP_SHORT;return 0;}
 if((b[0]!=1&&b[0]!=2)||b[1]!=1||b[2]!=6){r->parse=NSDD_BOOTP_HEADER;return 0;}
 if(b[236]!=99||b[237]!=130||b[238]!=83||b[239]!=99){r->parse=NSDD_COOKIE;return 0;}
 unsigned found=0,type=0,steps=0;size_t at=240;
 while(at<bn){if(++steps>256){r->parse=NSDD_OPTION_LIMIT;return 0;}unsigned tag=b[at++];if(!tag)continue;if(tag==255)break;
  if(at==bn||b[at]>bn-at-1){r->parse=NSDD_OPTIONS;return 0;}unsigned bytes=b[at++];
  if(tag==52){r->parse=NSDD_OVERLOAD;return 0;} /* Never inspect file/sname. */
  if(tag==53){if(found++){r->parse=NSDD_DUP_TYPE;return 0;}if(bytes!=1||!b[at]||b[at]>8){r->parse=NSDD_TYPE;return 0;}type=b[at];}
  at+=bytes;
 }
 if(!found){r->parse=NSDD_NO_TYPE;return 0;}r->dhcp=type;r->parse=NSDD_DHCP;return 0;
}
int nsdd_decode_txs(struct nsdd_txs*r,const uint8_t*p,size_t n){
 if(!r)return -1;*r=(struct nsdd_txs){0};if(!p||n<88||n>4096)return -1;
 r->bytes=(uint32_t)n;r->pid=p[0];r->status=p[1];r->wlan=p[4];r->flags=p[8];r->advanced=!!(p[8]&1);
 /* These fields are only authoritative when source TXS_WITH_ADVANCED_INFO. */
 if(r->advanced){r->sequence=le16(p+2);r->tx_count=p[5];r->rate=le16(p+6);r->tid=p[9];r->flush=p[14];}
 return 0;
}
static void header(struct nsdd*s,uint64_t g,unsigned dir,const struct nsdd_frame*r){
 uint32_t*budget=r->dhcp?(dir==3?&s->dhcp_rx:&s->dhcp_tx):(dir==3?&s->headers_rx:&s->headers_tx);
 if(*budget>=(r->dhcp?12u:4u))return;(*budget)++;s->records++;
 printf("NORD2 WIFI DATAHDR: n=%u g=%llu dir=%u dst=%u eth=%u bytes=%u dhcp=%u parse=%u\n",s->records,(unsigned long long)g,dir,r->destination,r->ether,r->bytes,r->dhcp,r->parse);
}
void nsdd_publish(struct nsdd*s,uint64_t g,uint32_t submitted,unsigned type,const uint8_t*p,size_t n){
 if(!s||s->summary||(type!=2&&type!=3))return;struct nsdd_frame r;if(nsdd_classify(&r,p,n))return;
 inc(r.destination?&s->tx_group:&s->tx_unicast);inc(&s->tx_ether[etype(r.ether)]);s->last_kind=type;s->last_pub=submitted;if(type==2)inc(&s->tx_security);else inc(&s->tx_data);
 if(r.dhcp){inc(&s->tx_dhcp);inc(&s->tx_type[r.dhcp]);}header(s,g,type==2?1:2,&r);
}
void nsdd_receive(struct nsdd*s,uint64_t g,const uint8_t*p,size_t n){
 if(!s||s->summary)return;struct nsdd_frame r;if(nsdd_classify(&r,p,n))return;
 inc(&s->rx_ether[etype(r.ether)]);inc(r.destination?&s->rx_group:&s->rx_unicast);if(r.dhcp){inc(&s->rx_dhcp);inc(&s->rx_type[r.dhcp]);}header(s,g,3,&r);
}
void nsdd_event(struct nsdd*s,uint64_t g,const uint8_t*p,size_t n,unsigned authorized){
 if(!s||s->summary||authorized>1)return;struct nsdd_txs r;if(nsdd_decode_txs(&r,p,n))return;
 if(r.status)inc(&s->txs_fail);else inc(&s->txs_ok);if(r.pid==127)inc(&s->ambiguous);
 uint32_t*budget=authorized?&s->txs_live:&s->txs_early;if(*budget>=(authorized?16u:8u))return;(*budget)++;s->records++;
 printf("NORD2 WIFI DATATXS: n=%u part=1 g=%llu pid=%u wlan=%u status=%u ambiguous=%u last_kind=%u last_pub=%u\n",s->records,(unsigned long long)g,r.pid,r.wlan,r.status,r.pid==127,s->last_kind,s->last_pub);
 printf("NORD2 WIFI DATATXS: n=%u part=2 seq=%u tx_count=%u rate=%u flags=%u adv=%u tid=%u flush=%u\n",s->records,r.sequence,r.tx_count,r.rate,r.flags,r.advanced,r.tid,r.flush);
}
void nsdd_summary(struct nsdd*s,uint64_t g){
 if(!s||s->summary)return;s->summary=1;
 printf("NORD2 WIFI DATACOUNT: g=%llu tx_sec=%u tx_data=%u rx_uni=%u rx_group=%u tx_dhcp=%u rx_dhcp=%u txs_ok=%u txs_fail=%u ambiguous=%u\n",(unsigned long long)g,s->tx_security,s->tx_data,s->rx_unicast,s->rx_group,s->tx_dhcp,s->rx_dhcp,s->txs_ok,s->txs_fail,s->ambiguous);
 for(unsigned dir=0;dir<2;dir++){const uint32_t*e=dir?s->rx_ether:s->tx_ether;
  printf("NORD2 WIFI DATATYPE: g=%llu dir=%u uni=%u group=%u ipvfour=%u arp=%u ipvsix=%u eapol=%u other=%u\n",(unsigned long long)g,dir+1,dir?s->rx_unicast:s->tx_unicast,dir?s->rx_group:s->tx_group,e[0],e[1],e[2],e[3],e[4]);
  const uint32_t*v=dir?s->rx_type:s->tx_type;
  printf("NORD2 WIFI DHCPSTAT: g=%llu dir=%u discover=%u offer=%u request=%u ack=%u nak=%u other=%u\n",(unsigned long long)g,dir+1,v[1],v[2],v[3],v[5],v[6],sum3(v[4],v[7],v[8]));}
}
