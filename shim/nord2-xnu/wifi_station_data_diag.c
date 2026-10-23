#include "wifi_station_data_diag.h"
#if defined(KERNEL) || defined(_KERNEL)
#include <sys/systm.h>
#else
#include <stdio.h>
#endif
static uint32_t be16(const uint8_t*p){return (uint32_t)p[0]<<8|p[1];}
static uint32_t be32(const uint8_t*p){return (uint32_t)p[0]<<24|(uint32_t)p[1]<<16|(uint32_t)p[2]<<8|p[3];}
static uint32_t le32(const uint8_t*p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static int eq(const uint8_t*a,const uint8_t*b,unsigned n){unsigned diff=0;for(unsigned i=0;i<n;i++)diff|=a[i]^b[i];return !diff;}
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
/* One serialized native worker owns this boot-static ledger. No secrets or
 * payload pointers retained. Source driver PID range1..127; our partition and
 * no-reuse are deliberate bounded diagnostic policy, not vendor wrap policy. */
static struct nsdd_pid_record pid_ledger[127];
static unsigned pid_next[3]={NSDD_PID_MANAGEMENT_MIN,NSDD_PID_SECURITY_MIN,NSDD_PID_DATA_MIN};
static uint32_t pid_notes[7],pid_exhausted[3];
static void pid_note(uint64_t g,uint64_t original,unsigned pid,unsigned wlan,unsigned reason){
 if(!reason||reason>6)return;inc(&pid_notes[reason]);if(pid_notes[reason]>2)return;
 printf("NORD2 WIFI TXNOTE: g=%llu original=%llu pid=%u wlan=%u reason=%u count=%u\n",(unsigned long long)g,(unsigned long long)original,pid,wlan,reason,pid_notes[reason]);
}
unsigned nsdd_pid_reserve(uint64_t g,unsigned wlan,unsigned kind,unsigned protect,const uint8_t*p,size_t n){
 if(!g||wlan>254||kind<1||kind>3||protect>1||!p)return 0;
 struct nsdd_frame f={0};unsigned c=0;
 if(kind==1){if(n<26||n>1568)return 0;c=(p[0]>>4)&15;}
 else{if(nsdd_classify(&f,p,n))return 0;if(kind==2&&f.ether==0x888e&&n>=21&&p[15]==3){unsigned info=be16(p+19);c=info==0x010a?1:info==0x030a?2:info==0x0302?3:4;}}
 unsigned k=kind-1,max=kind==1?NSDD_PID_MANAGEMENT_MAX:kind==2?NSDD_PID_SECURITY_MAX:NSDD_PID_DATA_MAX;
 if(pid_next[k]>max){inc(&pid_exhausted[k]);if(pid_exhausted[k]==1)printf("NORD2 WIFI TXLIMIT: g=%llu kind=%u limit=%u fallback=%u\n",(unsigned long long)g,kind,max,kind==1?0:NSDD_PID_SHARED);return kind==1?0:NSDD_PID_SHARED;}
 unsigned pid=pid_next[k]++;struct nsdd_pid_record r={0};r.generation=g;r.pid=pid;r.wlan=wlan;r.kind=kind;r.protected_frame=protect;r.bytes=(uint32_t)n;r.ether=f.ether;r.dhcp=f.dhcp;r.parse=f.parse;r.eapol_class=c;r.status=256;pid_ledger[pid]=r;return pid;
}
static struct nsdd_pid_record*pid_current(uint64_t g,unsigned pid,unsigned wlan){
 if(!g||pid<1||pid>126||wlan>254)return 0;
 struct nsdd_pid_record*r=&pid_ledger[pid];if(!r->generation||r->generation!=g||r->wlan!=wlan||r->closed)return 0;return r;
}
void nsdd_pid_attempt(uint64_t g,unsigned pid,unsigned wlan){struct nsdd_pid_record*r=pid_current(g,pid,wlan);if(r)r->attempted=1;}
void nsdd_pid_publish(uint64_t g,unsigned pid,unsigned wlan,unsigned publication){
 struct nsdd_pid_record*r=pid_current(g,pid,wlan);if(!r||!r->attempted||r->published||!publication)return;r->published=1;r->publication=publication;
 printf("NORD2 WIFI TXPUB: g=%llu pid=%u wlan=%u kind=%u protected=%u ether=%u dhcp=%u parse=%u bytes=%u class=%u pub=%u\n",(unsigned long long)g,pid,wlan,r->kind,r->protected_frame,r->ether,r->dhcp,r->parse,r->bytes,r->eapol_class,publication);
}
void nsdd_pid_local(uint64_t g,unsigned pid,unsigned wlan){struct nsdd_pid_record*r=pid_current(g,pid,wlan);if(r&&r->published)r->local=1;}
void nsdd_pid_status(uint64_t g,const struct nsdd_txs*x){
 if(!g||!x||x->pid>255||x->wlan>255||x->status>255||x->flags>255||x->advanced>1||x->sequence>65535||x->tx_count>255||x->rate>65535||x->tid>255||x->flush>255)return;unsigned pid=x->pid;if(pid==127)return;
 if(!pid||pid>126||!pid_ledger[pid].generation){pid_note(g,0,pid,x->wlan,1);return;}
 struct nsdd_pid_record*r=&pid_ledger[pid];
 if(r->wlan!=x->wlan){pid_note(g,r->generation,pid,x->wlan,2);return;}
 if(r->generation!=g){pid_note(g,r->generation,pid,x->wlan,3);return;}
 if(r->closed){pid_note(g,r->generation,pid,x->wlan,4);return;}
 if(!r->published){pid_note(g,r->generation,pid,x->wlan,5);return;}
 if(r->seen){pid_note(g,r->generation,pid,x->wlan,6);return;}
 r->seen=1;r->status=x->status;
 printf("NORD2 WIFI TXSTATUS: g=%llu pid=%u wlan=%u status=%u count=%u rate=%u flags=%u adv=%u seq=%u tid=%u flush=%u\n",(unsigned long long)g,pid,x->wlan,x->status,x->tx_count,x->rate,x->flags,x->advanced,x->sequence,x->tid,x->flush);
}
void nsdd_pid_finish(uint64_t g){
 if(!g)return;for(unsigned pid=1;pid<=126;pid++){struct nsdd_pid_record*r=&pid_ledger[pid];if(r->generation!=g||r->closed)continue;r->closed=1;
 printf("NORD2 WIFI TXFINAL: g=%llu pid=%u wlan=%u kind=%u attempt=%u pub=%u local=%u seen=%u status=%u\n",(unsigned long long)g,pid,r->wlan,r->kind,r->attempted,r->publication,r->local,r->seen,r->status);}
}
#if !defined(KERNEL) && !defined(_KERNEL)
void nsdd_test_boot_reset(void){for(unsigned k=0;k<127;k++)pid_ledger[k]=(struct nsdd_pid_record){0};pid_next[0]=1;pid_next[1]=32;pid_next[2]=64;for(unsigned k=0;k<7;k++)pid_notes[k]=0;for(unsigned k=0;k<3;k++)pid_exhausted[k]=0;}
const struct nsdd_pid_record*nsdd_test_pid(unsigned pid){return pid&&pid<=126?&pid_ledger[pid]:0;}
#endif
/* Called only after nsdd_classify verified all IPv4/UDP/BOOTP extents. */
static void correlate(struct nsdd*s,uint64_t g,const uint8_t*p,const struct nsdd_frame*r,unsigned tx){
 if(!r->dhcp)return;
 const uint8_t*u=p+14+4u*(p[14]&15u),*b=u+8;
 uint32_t xid=be32(b+4);
 if(tx){
  if(b[0]!=1||be16(u)!=68||be16(u+2)!=67||!eq(b+28,p+6,6))return;
  for(unsigned k=0;k<s->identities;k++)if(s->identity[k].xid==xid&&eq(s->identity[k].client,b+28,6))return;
  struct nsdd_identity*v=&s->identity[s->identity_next];v->xid=xid;for(unsigned k=0;k<6;k++)v->client[k]=b[28+k];
  s->identity_next=(s->identity_next+1u)%4u;if(s->identities<4)s->identities++;
 }else if(b[0]==2&&be16(u)==67&&be16(u+2)==68){
  unsigned match=0;for(unsigned k=0;k<s->identities;k++)if(s->identity[k].xid==xid&&eq(s->identity[k].client,b+28,6))match=1;
  inc(match?&s->reply_match[r->dhcp]:&s->reply_other[r->dhcp]);
  if(s->match_logs<12){s->match_logs++;printf("NORD2 WIFI DHCPMATCH: g=%llu type=%u matched=%u tracked=%u count=%u\n",(unsigned long long)g,r->dhcp,match,s->identities,match?s->reply_match[r->dhcp]:s->reply_other[r->dhcp]);}
 }
}
/* Selected nic_connac2x_rx.h DW1/2/3/4. The masks are exactly the
 * existing native error gate, not a new admission policy. A1 is fixed RXD
 * metadata; even errored/nontranslated payload is never dereferenced here. */
int nsdd_rx_error_take(struct nsdd*s,struct nsdd_rx_error*r,uint64_t g,const uint8_t*p,size_t n){
 if(!s||!r||!g||s->summary||s->rx_error_logs>=8||!p||n<24||n>3840)return 0;
 unsigned bytes=le16(p),type=(le16(p+2)>>11)&31;
 if(bytes<24||bytes>n||(type!=2&&!(type==7&&(le16(p+2)&0x380f)==0x3801)))return 0;
 uint32_t d1=le32(p+4),d2=le32(p+8),d3=le32(p+12),d4=le32(p+16);
 struct nsdd_rx_error v={0};v.d1=d1&0x0f800000u;v.d2=d2&0x0b800000u;v.d4=d4&0x1803u;
 if(!(v.d1||v.d2||v.d4))return 0;
 v.generation=g;v.n=s->rx_error_logs+1;v.wlan=d1&1023;v.sec=(d1>>16)&31;v.key=(d1>>21)&3;
 v.ht=(d2>>13)&1;v.groups=(d1>>11)&31;v.type=type;v.a1=(d3>>16)&3;
 v.dst=!v.a1?0:v.a1==1?1:2;v.tid=(d2>>16)&15;
 *r=v;s->rx_error_logs++;return 1;
}
void nsdd_rx_error(struct nsdd*s,uint64_t g,const uint8_t*p,size_t n){
 struct nsdd_rx_error r;if(!nsdd_rx_error_take(s,&r,g,p,n))return;
 printf("NORD2 WIFI RXERROR: g=%llu n=%u d1=%u d2=%u d4=%u wlan=%u sec=%u key=%u ht=%u groups=%u type=%u a1=%u dst=%u tid=%u\n",(unsigned long long)r.generation,r.n,r.d1,r.d2,r.d4,r.wlan,r.sec,r.key,r.ht,r.groups,r.type,r.a1,r.dst,r.tid);
}
void nsdd_reject(struct nsdd*s,uint64_t g,unsigned reason){
 if(!s||s->summary||!reason||reason>=NSDD_RX_REASON_END)return;
 inc(&s->rejected[reason]);if(s->reject_logged[reason]>=2)return;s->reject_logged[reason]++;
 printf("NORD2 WIFI RXREJECT: g=%llu reason=%u count=%u\n",(unsigned long long)g,reason,s->rejected[reason]);
}
void nsdd_rx_snapshot(struct nsdd*s,uint64_t g){
 if(!s||s->summary||s->rx_samples>=5)return;
 uint64_t count=(uint64_t)s->rx_unicast+s->rx_group;const uint32_t limits[]={1,32,256,1024,4096};
 if(count<limits[s->rx_samples])return;s->rx_samples++;
 printf("NORD2 WIFI RXCOUNTS: g=%llu sample=%u uni=%u group=%u error=%u peer=%u key=%u replay=%u plain=%u\n",(unsigned long long)g,s->rx_samples,s->rx_unicast,s->rx_group,s->rejected[NSDD_RX_ERROR],s->rejected[NSDD_RX_PEER],s->rejected[NSDD_RX_KEY],s->rejected[NSDD_RX_REPLAY],s->rejected[NSDD_RX_PLAINTEXT]);
}
void nsdd_capture(struct nsdd*s,unsigned mode,unsigned epoch){
 if(!s||s->summary||mode>2||epoch>32||(!epoch&&mode))return;
 s->live_mode=mode;s->live_epoch=epoch;s->live_tx=s->live_rx=0;
}
static void header(struct nsdd*s,uint64_t g,unsigned dir,const struct nsdd_frame*r){
 if(s->live_mode&&(s->live_mode==2||r->dhcp)){
  uint32_t*quota=dir==3?&s->live_rx:&s->live_tx;
  if(*quota<64){(*quota)++;printf("NORD2 WIFI LIVEFRAME: epoch=%u g=%llu dir=%u n=%u dst=%u ether=%u bytes=%u dhcp=%u parse=%u\n",s->live_epoch,(unsigned long long)g,dir,*quota,r->destination,r->ether,r->bytes,r->dhcp,r->parse);}
 }
 uint32_t*budget=r->dhcp?(dir==3?&s->dhcp_rx:&s->dhcp_tx):(dir==3?&s->headers_rx:&s->headers_tx);
 if(*budget>=(r->dhcp?12u:4u))return;(*budget)++;s->records++;
 printf("NORD2 WIFI DATAHDR: n=%u g=%llu dir=%u dst=%u eth=%u bytes=%u dhcp=%u parse=%u\n",s->records,(unsigned long long)g,dir,r->destination,r->ether,r->bytes,r->dhcp,r->parse);
}
void nsdd_publish(struct nsdd*s,uint64_t g,uint32_t submitted,unsigned type,const uint8_t*p,size_t n){
 if(!s||s->summary||(type!=2&&type!=3))return;struct nsdd_frame r;if(nsdd_classify(&r,p,n))return;
 inc(r.destination?&s->tx_group:&s->tx_unicast);inc(&s->tx_ether[etype(r.ether)]);s->last_kind=type;s->last_pub=submitted;if(type==2)inc(&s->tx_security);else inc(&s->tx_data);
 correlate(s,g,p,&r,1);if(r.dhcp){inc(&s->tx_dhcp);inc(&s->tx_type[r.dhcp]);}header(s,g,type==2?1:2,&r);
}
void nsdd_receive(struct nsdd*s,uint64_t g,const uint8_t*p,size_t n){
 if(!s||s->summary)return;struct nsdd_frame r;if(nsdd_classify(&r,p,n))return;
 correlate(s,g,p,&r,0);inc(&s->rx_ether[etype(r.ether)]);inc(r.destination?&s->rx_group:&s->rx_unicast);if(r.dhcp){inc(&s->rx_dhcp);inc(&s->rx_type[r.dhcp]);}header(s,g,3,&r);nsdd_rx_snapshot(s,g);
}
void nsdd_event(struct nsdd*s,uint64_t g,const uint8_t*p,size_t n,unsigned authorized){
 if(!s||s->summary||authorized>1)return;struct nsdd_txs r;if(nsdd_decode_txs(&r,p,n))return;
 nsdd_pid_status(g,&r);
 if(r.status)inc(&s->txs_fail);else inc(&s->txs_ok);if(r.pid==127)inc(&s->ambiguous);
 uint32_t*budget=authorized?&s->txs_live:&s->txs_early;if(*budget>=(authorized?16u:8u))return;(*budget)++;s->records++;
 printf("NORD2 WIFI DATATXS: n=%u part=1 g=%llu pid=%u wlan=%u status=%u ambiguous=%u last_kind=%u last_pub=%u\n",s->records,(unsigned long long)g,r.pid,r.wlan,r.status,r.pid==127,s->last_kind,s->last_pub);
 printf("NORD2 WIFI DATATXS: n=%u part=2 seq=%u tx_count=%u rate=%u flags=%u adv=%u tid=%u flush=%u\n",s->records,r.sequence,r.tx_count,r.rate,r.flags,r.advanced,r.tid,r.flush);
 if(r.advanced)printf("NORD2 WIFI TXPHY: g=%llu n=%u pid=%u status=%u response=%u table=%u width=%u power=%u delay=%u applied=%u\n",(unsigned long long)g,s->records,r.pid,r.status,p[10],p[11],p[12],p[13],le32(p+16),le32(p+24));
}
void nsdd_summary(struct nsdd*s,uint64_t g){
 if(!s||s->summary)return;s->summary=1;nsdd_pid_finish(g);
 for(unsigned k=1;k<NSDD_RX_REASON_END;k++)if(s->rejected[k])printf("NORD2 WIFI RXTOTAL: g=%llu reason=%u count=%u\n",(unsigned long long)g,k,s->rejected[k]);
 printf("NORD2 WIFI DHCPTOTAL: g=%llu offer=%u ack=%u nak=%u other_offer=%u other_ack=%u other_nak=%u\n",(unsigned long long)g,s->reply_match[2],s->reply_match[5],s->reply_match[6],s->reply_other[2],s->reply_other[5],s->reply_other[6]);
 volatile uint8_t*erase=(volatile uint8_t*)s->identity;for(unsigned k=0;k<sizeof s->identity;k++)erase[k]=0;s->identities=s->identity_next=0;
 printf("NORD2 WIFI DATACOUNT: g=%llu tx_sec=%u tx_data=%u rx_uni=%u rx_group=%u tx_dhcp=%u rx_dhcp=%u txs_ok=%u txs_fail=%u ambiguous=%u\n",(unsigned long long)g,s->tx_security,s->tx_data,s->rx_unicast,s->rx_group,s->tx_dhcp,s->rx_dhcp,s->txs_ok,s->txs_fail,s->ambiguous);
 for(unsigned dir=0;dir<2;dir++){const uint32_t*e=dir?s->rx_ether:s->tx_ether;
  printf("NORD2 WIFI DATATYPE: g=%llu dir=%u uni=%u group=%u ipvfour=%u arp=%u ipvsix=%u eapol=%u other=%u\n",(unsigned long long)g,dir+1,dir?s->rx_unicast:s->tx_unicast,dir?s->rx_group:s->tx_group,e[0],e[1],e[2],e[3],e[4]);
  const uint32_t*v=dir?s->rx_type:s->tx_type;
  printf("NORD2 WIFI DHCPSTAT: g=%llu dir=%u discover=%u offer=%u request=%u ack=%u nak=%u other=%u\n",(unsigned long long)g,dir+1,v[1],v[2],v[3],v[5],v[6],sum3(v[4],v[7],v[8]));}
}
