#include "wifi_ios_join.h"
#include "wifi_ios_station_view.h"
#include "wifi_station_wire.h"
#include "wifi_station_source_layout.h"
#include "wifi_station_tx.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
static unsigned checks;
#define C(x) do{checks++;assert(x);}while(0)
static void w16(uint8_t*p,unsigned n){p[0]=n;p[1]=n>>8;}
static void w32(uint8_t*p,unsigned n){w16(p,n);w16(p+2,n>>16);}
static unsigned r32(const uint8_t*p){return p[0]|(unsigned)p[1]<<8|(unsigned)p[2]<<16|(unsigned)p[3]<<24;}
static void request(uint8_t*p){memset(p,0,592);w32(p,1);w16(p+4,2);w16(p+6,1);w32(p+12,4);memcpy(p+16,"open",4);}
static void record(struct nsb_record*r){memset(r,0,sizeof *r);r->ssid_present=1;r->ssid_len=4;memcpy(r->ssid,"open",4);r->bssid[0]=2;r->bssid[5]=2;r->capability=1;r->subtype=8;r->channel=r->hw_channel=1;r->observed_us=100;r->rxv_valid=1;r->group_valid=4;r->rxv[4]=120;r->rxv[5]=255;r->interval=100;r->ie_bytes=r->ie_total=6;r->ies[0]=1;r->ies[1]=4;r->ies[2]=0x82;r->ies[3]=0x84;r->ies[4]=0x8b;r->ies[5]=0x96;}
int main(void){uint8_t p[593],out[4096],old[4096];struct niwj_request q,z={0};struct niwj_diagnostic d;request(p);memset(p+72,0x5a,132);C(!niwj_decode(p,592,&q,&d)&&q.security==NIWJ_OPEN&&!memcmp(q.pmk,z.pmk,32)&&!memcmp(q.station_rsn,z.station_rsn,22));
 for(unsigned n=0;n<594;n++){if(n==592)continue;C(niwj_decode(p,n,&q,&d)<0&&!memcmp(&q,&z,sizeof q));}
 const unsigned active[]={4,6,8,56,60,64,68,70,204,468,504,580,584,588};
 for(unsigned n=0;n<sizeof active/sizeof active[0];n++){request(p);p[active[n]]=3;C(niwj_decode(p,592,&q,&d)<0&&!memcmp(&q,&z,sizeof q));}
 request(p);for(unsigned n=72;n<204;n++){p[n]=0xa5;C(!niwj_decode(p,592,&q,&d)&&!memcmp(q.pmk,z.pmk,32));} /*inactive storage*/
 request(p);p[464]=64;w32(p+536,3);memcpy(p+540,"abc",4);C(!niwj_decode(p,592,&q,&d));
 struct nsb_scan scan={0};scan.entered=scan.done=1;scan.started=1;scan.last=101;scan.deadline=1000;scan.count=scan.frames=1;scan.completion.state=7;record(scan.records);struct nss_info info={0};info.count=1;info.channels[0].band=info.channels[0].channel=1;struct niwj_target target;
 C(!niwj_select(&q,&scan,&info,200,&target)&&target.security==NIWJ_OPEN&&!target.ap_rsn_bytes&&target.rate_count==4);
 scan.records[0].privacy=1;scan.records[0].capability|=16;C(niwj_select(&q,&scan,&info,200,&target)<0);record(scan.records);scan.records[0].rsn_present=1;C(niwj_select(&q,&scan,&info,200,&target)<0);record(scan.records);
 scan.records[0].ies[6]=221;scan.records[0].ies[7]=4;memcpy(scan.records[0].ies+8,"\0\x50\xf2\1",4);scan.records[0].ie_bytes=scan.records[0].ie_total=12;C(niwj_select(&q,&scan,&info,200,&target)<0);record(scan.records);q.security=NIWJ_WPA2_PSK_CCMP;C(niwj_select(&q,&scan,&info,200,&target)<0);q.security=NIWJ_OPEN;q.pmk[0]=1;C(niwj_select(&q,&scan,&info,200,&target)<0);q.pmk[0]=0;C(!niwj_select(&q,&scan,&info,200,&target));
 struct sw_assoc a={.peer={{2,0,0,0,0,1},{2,0,0,0,0,2}},.ssid=(const uint8_t*)"open",.ssid_len=4,.rates=target.rates,.rates_len=target.rate_count,.capability=1,.listen_interval=10,.security=SW_SECURITY_OPEN};size_t used;char name[16]="en0";
 C(!sw_assoc(out,sizeof out,&used,&a)&&used==40&&out[24]==1&&out[28]==0&&out[29]==4);for(size_t n=28;n<used;){C(out[n]!=48);n+=out[n+1]+2;}
 a.capability|=16;memset(out,0x5a,sizeof out);memcpy(old,out,sizeof out);C(sw_assoc(out,sizeof out,&used,&a)<0&&!used&&!memcmp(out,old,sizeof out));a.security=SW_SECURITY_WPA2;C(!sw_assoc(out,sizeof out,&used,&a)&&used==62&&out[40]==48&&out[41]==20);
 struct sw_bss b={.rlm={0,1,1,0,0,0,1,1},.ssid=(const uint8_t*)"open",.ssid_len=4,.ap={2,0,0,0,0,2},.connected=1,.ap_sta=0,.bmc_wlan=4,.nonht_phy=0,.phy_set=1,.dbdc_band=4,.rates=15,.basic_rates=3,.security=SW_SECURITY_OPEN};C(!sw_bss(out,sizeof out,&used,&b)&&out[SW_OFF_CMD_SET_BSS_INFO_ucAuthMode]==0&&out[SW_OFF_CMD_SET_BSS_INFO_ucEncStatus]==1&&out[2]==0&&out[1]==0);b.security=0;C(!sw_bss(out,sizeof out,&used,&b)&&out[SW_OFF_CMD_SET_BSS_INFO_ucAuthMode]==7&&out[SW_OFF_CMD_SET_BSS_INFO_ucEncStatus]==6);
 uint8_t eth[60]={0};memset(eth,255,6);memcpy(eth+6,a.peer.self,6);eth[12]=8;eth[13]=6;struct sw_txd tx={.wlan=5,.own_mac=1,.queue=1,.pid=64,.retry_limit=30,.pse_header_len=8,.lifetime_ms=2000};
 C(!sw_open_data_txd(out,sizeof out,&used,&tx,eth,60)&&!(r32(out+12)&2));C(sw_data_txd(out,sizeof out,&used,&tx,eth,60)<0);C(!nstx_build_profile(out,4096,&used,&tx,eth,60,0,1,0x90855000,0,&a.peer)&&used==124&&!(r32(out+12)&2));C(nstx_build_profile(out,4096,&used,&tx,eth,60,0,1,0x90855000,1,&a.peer)<0);tx.protected_frame=1;C(sw_open_data_txd(out,sizeof out,&used,&tx,eth,60)<0);
 struct njs_snapshot s={.native_generation=1,.ticket=1,.phase=NJS_AUTHORIZED,.result=1,.associated=1,.data_ready=1,.security=NIWJ_OPEN};s.peer=target;C(niwj_authorized(&s));C(!niwj_state_reply(&s,21,0,0,200,out,sizeof out,&used)&&r32(out)==1);C(!niwj_state_reply(&s,2,16,0,200,out,sizeof out,&used)&&r32(out+8)==0);C(!niwj_notice_record(&s,9,name,out,sizeof out,&used));s.ptk_ready=1;C(!niwj_authorized(&s));s.ptk_ready=0;s.security=1;C(!niwj_authorized(&s));s.security=0;C(!niwj_authorized(&s));
 printf("PASS %u OPEN codec/selection/wire/view bounds and WPA2 separation checks\n",checks);return 0;}
