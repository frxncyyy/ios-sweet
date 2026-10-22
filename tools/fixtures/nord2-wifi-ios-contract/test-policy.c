#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "wifi_ios_policy.h"
/* Exact selected Redwood/iOS5456 wire layout; independent typed fixture. */
struct channel {uint32_t version,number,flags;};
struct scan {uint32_t version,bss;uint8_t bssid[6],r0[2];uint32_t ssid_len;uint8_t ssid[32];uint32_t type,phy;uint16_t dwell,reserved;uint32_t rest,count;struct channel channel[400];uint64_t flags;uint32_t rssi_threshold,names;uint8_t name[10][56];uint32_t short_ssid,reserved3;};
_Static_assert(sizeof(struct scan)==5456,"actual wire extent");
_Static_assert(offsetof(struct scan,type)==52&&offsetof(struct scan,count)==68&&offsetof(struct scan,channel)==72,"source wire offsets");
_Static_assert(offsetof(struct scan,flags)==4872&&offsetof(struct scan,names)==4884,"options offsets");
static unsigned checks;
static struct scan packet(unsigned type){struct scan s={0};s.version=1;s.bss=3;s.type=type;s.phy=1;return s;}
static void no(const void*p,size_t n,const struct nss_info*info){struct nss_request out,before;memset(&out,0xa5,sizeof out);before=out;assert(niwi_scan_admit(p,n,info,&out)<0&&!memcmp(&out,&before,sizeof out));checks++;}
int main(void){struct nss_info info={.band5=1,.count=4,.channels={{1,1},{1,6},{2,36},{2,44}}};struct nss_request out;struct scan s=packet(1);
 assert(!niwi_scan_admit((uint8_t*)&s,sizeof s,&info,&out)&&out.count==0);checks++;
 s.type=2;assert(!niwi_scan_admit((uint8_t*)&s,sizeof s,&info,&out)&&out.count==0);checks++;
 const unsigned flags2g[]={0,2,8,10,128,130,136,138},flags5g[]={0,2,16,18,128,130,144,146};
 for(unsigned mode=1;mode<=2;mode++)for(unsigned a=0;a<8;a++)for(unsigned b=0;b<8;b++){s=packet(mode);s.count=2;s.channel[0]=(struct channel){1,44,flags5g[b]};s.channel[1]=(struct channel){1,6,flags2g[a]};assert(!niwi_scan_admit((uint8_t*)&s,sizeof s,&info,&out)&&out.count==2&&out.channels[0].band==2&&out.channels[0].channel==44&&out.channels[1].band==1&&out.channels[1].channel==6);checks++;}
 s=packet(2);s.count=4;for(unsigned i=0;i<4;i++)s.channel[i]=(struct channel){1,info.channels[i].channel,info.channels[i].band==1?8:16};assert(!niwi_scan_admit((uint8_t*)&s,sizeof s,&info,&out)&&out.count==4);checks++;
 s=packet(2);for(unsigned n=0;n<sizeof s;n++)no(&s,n,&info);no(&s,sizeof s+1,&info);no(0,sizeof s,&info);no(&s,sizeof s,0);
 for(unsigned i=0;i<sizeof s;i++){
  ((uint8_t*)&s)[i]^=0x80;
  int unused=(i>=14&&i<16)||(i>=20&&i<52)||(i>=62&&i<64)||(i>=72&&i<4872)||(i>=4888&&i<5448)||i>=5452;
  if(unused||i==4883){assert(!niwi_scan_admit((uint8_t*)&s,sizeof s,&info,&out));checks++;}
  else no(&s,sizeof s,&info);
  ((uint8_t*)&s)[i]^=0x80;
 }
 for(unsigned mode=0;mode<7;mode++)if(mode!=1&&mode!=2){s=packet(mode);no(&s,sizeof s,&info);}
 s=packet(2);s.count=40;no(&s,sizeof s,&info);s.count=1;s.channel[0]=(struct channel){1,165,16};no(&s,sizeof s,&info);s.channel[0].number=6;s.channel[0].flags=16;no(&s,sizeof s,&info);s.channel[0]=(struct channel){0,6,8};no(&s,sizeof s,&info);s.channel[0].version=1;
 const unsigned badflags[]={1,4,12,24,0x40,0x100,0x800,0xffffffff};for(unsigned i=0;i<sizeof badflags/sizeof*badflags;i++){s.channel[0].flags=badflags[i];no(&s,sizeof s,&info);}s.channel[0]=(struct channel){1,6,8};s.count=2;s.channel[1]=s.channel[0];no(&s,sizeof s,&info);s.count=1;s.channel[1]=(struct channel){0};s.type=1;assert(!niwi_scan_admit((uint8_t*)&s,sizeof s,&info,&out)&&out.count==1);checks++;
 s=packet(2);struct nss_info bad=info;bad.count=0;no(&s,sizeof s,&bad);bad=info;bad.count=40;no(&s,sizeof s,&bad);bad=info;bad.channels[3]=bad.channels[0];no(&s,sizeof s,&bad);bad=info;bad.channels[3].band=0;no(&s,sizeof s,&bad);
 bad=info;bad.channels[3]=(struct ncs_channel){2,6};s.count=1;s.channel[0]=(struct channel){1,6,0};no(&s,sizeof s,&bad);s.channel[0].flags=8;assert(!niwi_scan_admit((uint8_t*)&s,sizeof s,&bad,&out)&&out.channels[0].band==1);checks++;
 printf("PASS %u policy checks: actual5456 layout/default+explicitpassive/all+subset/exactchannelallowlist, atomic refusal/truncation/options/advancedmode/directed/timing/flags; synthetic only\n",checks);return 0;}
