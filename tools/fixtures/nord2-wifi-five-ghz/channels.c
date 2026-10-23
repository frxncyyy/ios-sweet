#include "wifi_normal_config.h"
#include "wifi_ios_policy.h"
#include "wifi_ios_scalar.h"
#include "wifi_ios_snapshot.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static unsigned tests;
#define C(x) do {assert(x);tests++;} while(0)
static void put(uint8_t*p,uint32_t x){for(unsigned j=0;j<4;j++)p[j]=(uint8_t)(x>>(8*j));}
static unsigned get(const uint8_t*p){return p[0]|(unsigned)p[1]<<8|(unsigned)p[2]<<16|(unsigned)p[3]<<24;}
static unsigned beacon(uint8_t*p,unsigned ch,unsigned suffix){
 memset(p,0,128);p[2]=1;p[3]=0x38;put(p+4,4u<<11);put(p+8,12u<<8);put(p+12,ch<<8);p[28]=100;p[29]=255;
 unsigned m=32;p[m]=0x80;p[m+10]=p[m+16]=2;p[m+15]=p[m+21]=(uint8_t)suffix;p[m+32]=100;p[m+34]=1;
 unsigned i=m+36;p[i++]=0;p[i++]=4;memcpy(p+i,"test",4);i+=4;p[i++]=1;p[i++]=1;p[i++]=12;
 p[i++]=61;p[i++]=22;p[i]=(uint8_t)ch;i+=22;p[0]=(uint8_t)i;p[1]=(uint8_t)(i>>8);return i;
}
int main(int argc,char**argv){
 C(argc==2);uint8_t board[6146];FILE*f=fopen(argv[1],"rb");C(f&&fread(board,1,sizeof board,f)==sizeof board);fclose(f);
 struct ncfg_input in={.board=board,.board_bytes=sizeof board,.project=20827,.caps={.phy_present=1,.band5=1,.max_bw=2,.nss=2,.path=15}};
 struct ncfg_plan p;struct ncfg_channel all[39];size_t count=0;C(!ncfg_prepare(&in,&p));C(!ncfg_channels(&in,&p,all,39,&count)&&count==39&&p.band5);
 struct nss_info info={.band5=1};unsigned dfs=0,non=0;
 for(unsigned j=0;j<count;j++)if(all[j].allowed){unsigned k=info.count++;info.channels[k]=(struct ncs_channel){all[j].band,all[j].channel};if(all[j].dfs){info.dfs_mask|=UINT64_C(1)<<k;dfs++;}else non++;}
 fprintf(stderr,"channel_counts total=%u dfs=%u nonDFS=%u\n",info.count,dfs,non);C(info.count==37&&dfs==15&&non==22);
 const char name[16]="en0";struct niw_request q={.operation=NIW_GET,.request=27,.payload_bytes=NIWS_CHANNEL_BYTES};memcpy(q.interface_name,name,16);
 uint8_t response[NIWS_CHANNEL_BYTES+1],before[sizeof response];size_t used=77;
 for(unsigned id=0;id<2;id++){q.request=id?254:27;memset(response,0xa5,sizeof response);C(!niws_channels(&q,name,&info,response,sizeof response,&used));C(used==NIWS_CHANNEL_BYTES&&get(response+4)==37&&response[NIWS_CHANNEL_BYTES]==0xa5);
 for(unsigned k=0;k<info.count;k++){C(get(response+12+12*k)==info.channels[k].channel);C(get(response+16+12*k)==(2u|(info.channels[k].band==1?8u:16u)|((info.dfs_mask>>k&1)?0x100u:0)));}}
 for(unsigned bad=0;bad<3;bad++){struct nss_info b=info;if(!bad)b.dfs_mask|=UINT64_C(1)<<39;if(bad==1)b.dfs_mask|=1;if(bad==2)b.count=40;memset(response,0xa5,sizeof response);memcpy(before,response,sizeof response);used=77;C(niws_channels(&q,name,&b,response,sizeof response,&used));C(used==77&&!memcmp(before,response,sizeof response));}
 uint8_t request[5456]={0};put(request,1);put(request+4,2);put(request+56,1);put(request+4880,0x80000000u);
 struct nss_request admitted,old;
 for(unsigned mode=1;mode<=3;mode++)for(unsigned explicit_flag=0;explicit_flag<2;explicit_flag++)for(unsigned active=0;active<2;active++){
 put(request+52,mode);put(request+68,info.count);
 for(unsigned k=0;k<info.count;k++){uint8_t*c=request+72+12*k;put(c,1);put(c+4,info.channels[k].channel);put(c+8,2u|(info.channels[k].band==1?8u:16u)|((explicit_flag&&(info.dfs_mask>>k&1))?0x100u:0u)|(active?128u:0u));}
 C(!niwi_scan_admit(request,sizeof request,&info,&admitted));C(admitted.count==37&&!admitted.dwell_tu&&!memcmp(admitted.channels,info.channels,sizeof(info.channels[0])*37));
 }
 /* Every numeric flag bit is classified; permission to probe never creates TX. */
 put(request+68,1);put(request+76,100);put(request+52,2);
 for(unsigned bit=0;bit<32;bit++){put(request+80,0x112u|(1u<<bit));memset(&admitted,0xa5,sizeof admitted);old=admitted;int r=niwi_scan_admit(request,sizeof request,&info,&admitted);unsigned ok=bit==1||bit==4||bit==7||bit==8;C((r==0)==ok);if(r)C(!memcmp(&old,&admitted,sizeof old));}
 for(unsigned ch=1;ch<=196;ch++){put(request+76,ch);put(request+80,0x112);unsigned allowed=0;for(unsigned k=0;k<info.count;k++)if(info.channels[k].channel==ch&&(info.dfs_mask>>k&1))allowed=1;C((niwi_scan_admit(request,sizeof request,&info,&admitted)==0)==allowed);}
 struct nsb_scan scan={0};uint8_t packet[128];C(!nsb_begin(&scan,9,1,10000));
 unsigned bytes=beacon(packet,1,1);C(!nsb_add(&scan,packet,bytes,2));bytes=beacon(packet,100,2);C(!nsb_add(&scan,packet,bytes,3));
 C(scan.count==2&&scan.records[1].hw_channel==100&&!scan.records[1].channel_mismatch);
 /* Same SSID, different BSSID/band remains two actual records. */
 scan.active=0;scan.done=1;scan.completion.state=7;scan.completion.sequence=9;
 struct niwa_snapshot snap={0};uint32_t ages[NSB_RESULTS]={0};size_t maps=0;const struct niwp_rate_map*map=niwi_rate_map(&maps);
 C(!niwa_snapshot_prepare(&scan,1,ages,2,map,maps,&snap));C(snap.count==2&&!snap.omitted);C(get(snap.rows[0]+8)==1&&get(snap.rows[1]+8)==100&&get(snap.rows[1]+12)==0x110);
 /* Existing overflow and subset-replacement behavior is explicit, unchanged. */
 struct nsb_scan over={0};C(!nsb_begin(&over,1,1,10000));for(unsigned i=0;i<NSB_RESULTS+1;i++){bytes=beacon(packet,i%2?100:1,i+1);int rc=nsb_add(&over,packet,bytes,i+2);C(rc==(i==NSB_RESULTS?1:0));}
 C(over.count==NSB_RESULTS&&over.dropped==1&&over.partial);over.active=0;over.done=1;over.completion.state=7;over.completion.sequence=1;
 memset(&snap,0,sizeof snap);C(niwa_snapshot_prepare(&over,2,ages,NSB_RESULTS,map,maps,&snap)==1&&snap.status==NIWA_PARTIAL&&!snap.count);
 printf("PASS fiveGHz: %u checks; authenticated37 channels/15DFS; channel ABI, passive subset flags, bounds, actual beacon decoder+snapshot, dedup and overflow semantics; no private values\n",tests);
}
