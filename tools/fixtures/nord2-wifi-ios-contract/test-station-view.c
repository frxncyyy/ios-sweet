#include "wifi_ios_station_view.h"
#include "wifi_ios_scan_contract.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
static unsigned checks;
#define C(x) do{checks++;assert(x);}while(0)
static unsigned g32(const unsigned char*p){return p[0]|(unsigned)p[1]<<8|(unsigned)p[2]<<16|(unsigned)p[3]<<24;}
int main(void){struct njs_snapshot s={0};unsigned char out[1200],old[1200];size_t used=77;char name[16]="en0";s.phase=NJS_IDLE;C(!niwj_state_reply(&s,13,0,1,100,out,sizeof out,&used)&&used==4&&g32(out)==1);C(!niwj_state_reply(&s,21,0,0,100,out,sizeof out,&used)&&g32(out)==0);C(niwj_state_reply(&s,103,1164,0,100,out,sizeof out,&used)==1&&used==0);
 unsigned before_requests[]={9,2,4,16,103},before_bytes[]={6,16,16,52,1164};
 for(unsigned i=0;i<5;i++){memset(out,0xa5,sizeof out);memcpy(old,out,sizeof out);C(niwj_state_reply(&s,before_requests[i],before_bytes[i],0,100,out,before_bytes[i]-1,&used)==-1);C(!used&&!memcmp(out,old,sizeof out));C(niwj_state_reply(&s,before_requests[i],before_bytes[i]-1,0,100,out,sizeof out,&used)==-1);C(niwj_state_reply(&s,before_requests[i],before_bytes[i],0,100,out,sizeof out,&used)==1);}
 C(niwj_state_reply(&s,1,32,0,100,out,0,&used)==-1);
 s.phase=NJS_AUTHENTICATING;C(!niwj_state_reply(&s,13,0,0,100,out,sizeof out,&used)&&g32(out)==2);s.phase=NJS_SECURING;C(!niwj_state_reply(&s,13,0,0,100,out,sizeof out,&used)&&g32(out)==3);
 s.phase=NJS_AUTHORIZED;s.result=1;s.native_generation=3;s.ticket=2;s.associated=s.ptk_ready=s.gtk_ready=s.data_ready=1;s.peer.band=1;s.peer.channel=6;s.peer.rssi_dbm=-53;s.peer.record.channel=6;s.peer.record.ssid_present=1;s.peer.record.ssid_len=4;memcpy(s.peer.record.ssid,"test",4);s.peer.record.bssid[0]=2;s.peer.record.observed_us=100;s.peer.record.interval=100;s.peer.record.capability=0x11;s.peer.rate_count=2;s.peer.rates[0]=0x82;s.peer.rates[1]=11;s.peer.record.ie_bytes=s.peer.record.ie_total=3;s.peer.record.ies[0]=1;s.peer.record.ies[1]=1;s.peer.record.ies[2]=0x82;
 C(niwj_authorized(&s));C(!niwj_state_reply(&s,13,0,1,2100,out,sizeof out,&used)&&g32(out)==4);C(!niwj_state_reply(&s,21,0,0,2100,out,sizeof out,&used)&&g32(out)==1);
 unsigned requests[]={1,9,2,4,16,103};unsigned payloads[]={32,6,16,16,52,1164},bytes[]={4,6,16,16,52,1164};for(unsigned i=0;i<6;i++){for(unsigned cap=0;cap<bytes[i];cap++){memset(out,0xa5,sizeof out);memcpy(old,out,sizeof out);used=44;C(niwj_state_reply(&s,requests[i],payloads[i],0,2100,out,cap,&used)!=0);C(!used&&!memcmp(out,old,sizeof out));}int result=niwj_state_reply(&s,requests[i],payloads[i],0,2100,out,sizeof out,&used);if(result)fprintf(stderr,"request %u result %d\n",requests[i],result);C(!result&&used==bytes[i]);}
 C(!niwj_state_reply(&s,103,1164,0,2100,out,sizeof out,&used));struct niws_result row;C(!niws_decode_result(out,1164,&row)&&row.age==2&&row.rssi==-53&&row.rates[0]==1&&row.rates[1]==5&&row.ie[2]==0x82);
 C(!niwj_state_reply(&s,16,52,0,2100,out,sizeof out,&used));C((int)g32(out+12)==-53&&(int)g32(out+28)==-53&&(int)g32(out+32)==-53&&(int)g32(out+48)==-53);
 for(unsigned missing=0;missing<4;missing++){unsigned*flags[]={&s.associated,&s.ptk_ready,&s.gtk_ready,&s.data_ready};*flags[missing]=0;C(!niwj_authorized(&s));C(niwj_state_reply(&s,21,0,0,2100,out,sizeof out,&used)==1);C(niwj_notice_record(&s,9,name,out,sizeof out,&used)!=0);*flags[missing]=1;}
 unsigned events[]={2,3,4,9};for(unsigned i=0;i<4;i++){unsigned z=events[i]==4?80:48;for(unsigned n=0;n<z;n++){memset(out,0xa5,sizeof out);memcpy(old,out,sizeof out);C(niwj_notice_record(&s,events[i],name,out,n,&used)!=0);C(!used&&!memcmp(out,old,sizeof out));}C(!niwj_notice_record(&s,events[i],name,out,sizeof out,&used)&&used==z&&g32(out)==87&&g32(out+28)==events[i]);C(out[24]==(events[i]==4?32:0)&&!memcmp(out+32,name,16));}
 C(!niwj_notice_record(&s,4,name,out,sizeof out,&used)&&out[48]==0&&(int)g32(out+52)==-53);s.phase=NJS_LEAVING;s.associated=s.ptk_ready=s.gtk_ready=s.data_ready=0;s.involuntary=1;s.reason=4;s.subreason=9;C(!niwj_notice_record(&s,4,name,out,sizeof out,&used)&&out[48]==1&&out[64]==1&&g32(out+68)==4&&g32(out+72)==9);s.phase=NJS_FAILED;s.result=9;C(!niwj_notice_record(&s,9,name,out,sizeof out,&used)&&used==48);s.result=0;C(niwj_notice_record(&s,9,name,out,sizeof out,&used)!=0);
 printf("station-view: %u checks PASS (synthetic receipts)\n",checks);return 0;}
