#include "wifi_ios_join.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static unsigned checks;
#define C(x) do{checks++;if(!(x)){fprintf(stderr,"line %d\n",__LINE__);assert(x);}}while(0)
static const uint8_t rsn[22]={48,20,1,0,0,15,172,4,1,0,0,15,172,4,1,0,0,15,172,2,0,0};
static void put16(uint8_t*p,unsigned n){p[0]=n;p[1]=n>>8;}
static void put32(uint8_t*p,unsigned n){put16(p,n);put16(p+2,n>>16);}
static void wire(uint8_t*p){memset(p,0,592);put32(p,1);put16(p+4,2);put16(p+6,1);put16(p+8,8);put32(p+12,4);memcpy(p+16,"test",4);put32(p+56,1);put32(p+60,32);put32(p+64,6);for(unsigned i=0;i<32;i++)p[72+i]=i;put16(p+204,22);memcpy(p+206,rsn,22);put32(p+504,4);}
static void record(struct nsb_record*r,unsigned suffix,unsigned rcpi){memset(r,0,sizeof(*r));r->bssid[0]=2;r->bssid[5]=suffix;r->ssid_len=4;r->ssid_present=1;memcpy(r->ssid,"test",4);r->subtype=8;r->channel=r->hw_channel=1;r->privacy=r->rsn_present=1;r->capability=0x11;r->observed_us=100;r->rxv_valid=1;r->group_valid=4;r->rxv[4]=rcpi;r->rxv[5]=255;r->ies[0]=1;r->ies[1]=4;r->ies[2]=0x82;r->ies[3]=0x84;r->ies[4]=11;r->ies[5]=22;memcpy(r->ies+6,rsn,22);r->ie_bytes=r->ie_total=28;}
static void absent(const struct niwj_request*q,const struct nsb_scan*s,const struct nss_info*i){struct niwj_target out,old;memset(&out,0xa5,sizeof out);old=out;C(niwj_select(q,s,i,200,&out)!=0);C(!memcmp(&out,&old,sizeof out));}
static void trailer_tests(void){
 uint8_t p[592];struct niwj_request q,expected,z={0};struct niwj_diagnostic d;
 wire(p);C(!niwj_decode(p,sizeof p,&expected,&d));
 unsigned lengths[]={22,24,28};unsigned suites[]={6,11,12,13};
 for(unsigned k=0;k<3;k++)for(unsigned t=0;t<(k==2?4u:1u);t++){
  wire(p);unsigned n=lengths[k];put16(p+204,n);p[207]=(uint8_t)(n-2);if(n==28){p[230]=0;p[231]=15;p[232]=172;p[233]=(uint8_t)suites[t];}
  memset(p+234,0xa5,463-234);C(!niwj_decode(p,sizeof p,&q,&d));C(!memcmp(&q,&expected,sizeof q));C(d.mode==2&&d.auth_lower==1&&d.auth_upper==8&&d.rsn_bytes==n);
  if(n>22){for(unsigned i=208;i<206+n;i++){uint8_t old=p[i];p[i]^=128;C(niwj_decode(p,sizeof p,&q,&d)==NIWJ_SECURITY);C(!memcmp(&q,&z,sizeof q));p[i]=old;}}
 }
 for(unsigned n=0;n<387;n++){wire(p);put16(p+204,n);if(n==22)continue;C(niwj_decode(p,sizeof p,&q,&d)==NIWJ_SECURITY);C(d.rsn_bytes==n);}
 for(unsigned caps=0;caps<65536;caps++){wire(p);put16(p+204,24);p[207]=22;put16(p+226,caps);C((niwj_decode(p,sizeof p,&q,&d)==0)==!caps);if(caps)C(!memcmp(&q,&z,sizeof q));}
 for(unsigned type=0;type<256;type++){wire(p);put16(p+204,28);p[207]=26;p[230]=0;p[231]=15;p[232]=172;p[233]=(uint8_t)type;C((niwj_decode(p,sizeof p,&q,&d)==0)==(type==6||type==11||type==12||type==13));}
 wire(p);put16(p+8,1024);C(niwj_decode(p,sizeof p,&q,&d)==NIWJ_PROFILE&&d.auth_upper==1024); /* AKM6 remains distinct. */
}
int main(void){uint8_t p[593];struct niwj_request q;struct niwj_diagnostic d;wire(p);C(!niwj_decode(p,592,&q,&d));C(q.native_flags==4&&q.security==1&&q.ssid_length==4&&!memcmp(q.pmk,p+72,32));
 put32(p+504,0);C(!niwj_decode(p,592,&q,&d));
 for(unsigned n=0;n<592;n++){memset(&q,0xa5,sizeof q);C(niwj_decode(p,n,&q,&d)!=0);struct niwj_request z={0};C(!memcmp(&q,&z,sizeof q));}C(niwj_decode(p,593,&q,&d)!=0);C(niwj_decode(NULL,592,&q,&d)!=0);C(niwj_decode(p,592,NULL,&d)!=0);
 for(unsigned reason=0;reason<256;reason++){wire(p);p[465]=(uint8_t)reason;int result=niwj_decode(p,592,&q,&d);C((result==0)==(reason<=2));C(d.association_reason==reason);if(!result)C(q.association_reason==reason);}
 for(unsigned i=0;i<32;i++){wire(p);put32(p+504,1u<<i);C((niwj_decode(p,592,&q,&d)==0)==(i==2));}
 for(unsigned i=0;i<22;i++){wire(p);p[206+i]^=1;C(niwj_decode(p,592,&q,&d)==NIWJ_SECURITY);}
 unsigned active[]={0,4,6,8,12,56,60,64,68,70,204,463,464,465,468,514,536,580,584,588};for(unsigned i=0;i<sizeof active/sizeof active[0];i++){wire(p);p[active[i]]^=0x40;C(niwj_decode(p,592,&q,&d)!=0);}
 for(unsigned i=0;i<592;i++){int inactive=(i==10||i==11||i==54||i==55||(i>=20&&i<48)||(i>=104&&i<204)||(i>=228&&i<463)||i==466||i==467||(i>=472&&i<504)||(i>=508&&i<514)||(i>=515&&i<536)||(i>=540&&i<580));if(inactive){wire(p);p[i]=0xa5;C(!niwj_decode(p,592,&q,&d));}}
 wire(p);p[48]=2;p[53]=1;p[463]=32;C(!niwj_decode(p,592,&q,&d)&&q.has_target);p[48]=3;C(niwj_decode(p,592,&q,&d)!=0);wire(p);p[463]=32;C(niwj_decode(p,592,&q,&d)!=0);
 wire(p);C(!niwj_decode(p,592,&q,&d));struct nsb_scan s={0};s.entered=s.done=1;s.completion.state=7;s.count=s.frames=2;s.started=1;s.last=101;s.deadline=1000;record(s.records,1,80);record(s.records+1,2,120);struct nss_info info={0};info.count=1;info.channels[0].band=1;info.channels[0].channel=1;struct niwj_target out;C(!niwj_select(&q,&s,&info,200,&out));C(out.record.bssid[5]==2&&out.rssi_dbm==-50&&out.rate_count==4&&out.rates[0]==0x82&&out.ap_rsn_bytes==22);
 q.has_target=1;q.target[0]=2;q.target[5]=1;C(!niwj_select(&q,&s,&info,200,&out)&&out.record.bssid[5]==1);q.target[5]=3;absent(&q,&s,&info);q.has_target=0;
 struct nsb_record baseline=s.records[0];s.count=1;
 for(unsigned i=0;i<22;i++){s.records[0]=baseline;s.records[0].ies[6+i]^=1;if(i==20||i==21)continue;absent(&q,&s,&info);} /*Mutations which retain an optional capability are checked separately.*/
 s.records[0]=baseline;s.records[0].ies[26]=128;C(!niwj_select(&q,&s,&info,200,&out));s.records[0].ies[26]=64;absent(&q,&s,&info);s.records[0].ies[26]=2;absent(&q,&s,&info);
 /* Mixed PSK+SAE offer, optional PMF: preserve complete26B AP IE; station remains22. */
 s.records[0]=baseline;uint8_t*m=s.records[0].ies+6;memmove(m+24,m+20,2);m[1]=24;put16(m+14,2);m[20]=0;m[21]=15;m[22]=172;m[23]=8;m[24]=128;s.records[0].ie_bytes=s.records[0].ie_total=32;C(!niwj_select(&q,&s,&info,200,&out));C(out.ap_rsn_bytes==26&&out.ap_rsn[23]==8&&q.station_rsn[20]==0);
 s.records[0]=baseline;s.records[0].ies[2]=255;absent(&q,&s,&info);s.records[0]=baseline;s.records[0].ies[2]=0x80|99;absent(&q,&s,&info);s.records[0]=baseline;s.records[0].ies[5]=99;C(!niwj_select(&q,&s,&info,200,&out)&&out.rate_count==3);
 for(unsigned n=0;n<28;n++){s.records[0]=baseline;s.records[0].ie_bytes=s.records[0].ie_total=n;absent(&q,&s,&info);}
 s.records[0]=baseline;s.active=1;absent(&q,&s,&info);s.active=0;s.partial=1;absent(&q,&s,&info);s.partial=0;s.timed_out=1;absent(&q,&s,&info);s.timed_out=0;s.completion.state=6;absent(&q,&s,&info);s.completion.state=7;
 s.records[0].observed_us=201;absent(&q,&s,&info);s.records[0]=baseline;s.records[0].ies_truncated=1;absent(&q,&s,&info);s.records[0]=baseline;s.records[0].channel_mismatch=1;absent(&q,&s,&info);s.records[0]=baseline;s.records[0].rxv[4]=255;absent(&q,&s,&info);s.records[0]=baseline;info.channels[0].channel=2;absent(&q,&s,&info);info.channels[0].channel=1;info.count=2;info.channels[1]=info.channels[0];absent(&q,&s,&info);
 /* Terminal state7 overflow preserves source-owned records, never unfinished scans. */
 info.count=1;info.channels[0].band=info.channels[0].channel=1;s=(struct nsb_scan){0};s.entered=s.done=1;s.completion.state=7;s.started=1;s.last=101;s.deadline=1000;s.count=NSB_RESULTS;s.frames=NSB_RESULTS+1;s.partial=s.dropped=1;record(&s.records[0],1,80);
 C(niwj_scan_usable(&s));C(!niwj_select(&q,&s,&info,200,&out));
 s.records[0].ies[27]=32;C(!niwj_select(&q,&s,&info,200,&out));C(out.ap_rsn[21]==32&&out.ap_rsn_bytes==22); /* AP optionalExtendedKeyID; station remainscaps0. */
 s.dropped=0;absent(&q,&s,&info);s.dropped=1;s.count=15;absent(&q,&s,&info);s.count=16;s.timed_out=1;absent(&q,&s,&info);s.timed_out=0;s.active=1;absent(&q,&s,&info);s.active=0;s.done=0;absent(&q,&s,&info);s.done=1;s.last=s.deadline;absent(&q,&s,&info);s.last=101;s.dropped=2;absent(&q,&s,&info);s.dropped=1;s.completion.sequence=1;absent(&q,&s,&info);
 trailer_tests();
 niwj_clear(&q,sizeof q);struct niwj_request zero={0};C(!memcmp(&q,&zero,sizeof q));printf("join-policy: %u checks PASS (synthetic; no native readiness)\n",checks);return 0;}
