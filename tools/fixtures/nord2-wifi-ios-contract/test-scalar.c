#include "wifi_ios_scalar.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
static int scalar_test(void){
 struct niw_request q={.operation=NIW_GET};char name[16]="en0";unsigned checks=0;
 memcpy(q.interface_name,name,16);
 for(unsigned id=0;id<512;id++)for(int state=-1;state<=3;state++)for(unsigned cap=0;cap<=5;cap++){
  uint8_t out[8];memset(out,0xa5,sizeof out);size_t used=99;q.request=id;
  int good=(id==13||id==15)&&(state==1||state==2)&&cap>=4;
  assert((niws_reply(&q,name,(enum niws_native_state)state,out,cap,&used)==0)==good);
  if(good){assert(used==4&&out[0]==(id==15||state==2)&&!out[1]&&!out[2]&&!out[3]&&out[4]==0xa5);}
  else{assert(used==99);for(unsigned j=0;j<8;j++)assert(out[j]==0xa5);}
  checks++;
 }
 q.request=13;uint8_t out[8];size_t used;
 for(unsigned mode=0;mode<5;mode++){
  struct niw_request bad=q;char wrong[16];memcpy(wrong,name,16);memset(out,0xa5,8);used=99;
  if(mode==0)bad.value=1;if(mode==1)bad.payload_bytes=4;if(mode==2)bad.operation=0;
  if(mode==3)wrong[2]='1';if(mode==4)memset(wrong,'X',16);
  assert(niws_reply(&bad,wrong,NIWS_IDLE,out,8,&used));assert(used==99&&out[0]==0xa5);checks++;
 }
 assert(niws_reply(0,name,NIWS_IDLE,out,8,&used));
 assert(niws_reply(&q,0,NIWS_IDLE,out,8,&used));
 assert(niws_reply(&q,name,NIWS_IDLE,0,8,&used));
 assert(niws_reply(&q,name,NIWS_IDLE,out,8,0));
 return checks==15365?0:1;
}

static uint32_t rd(const uint8_t*p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
int main(void){
 assert(!scalar_test());
 struct nss_info info={.band5=1,.count=22};
 /* Synthetic 22-channel owner metadata, not a claim of runtime country. */
 for(unsigned i=0;i<13;i++)info.channels[i]=(struct ncs_channel){1,(uint8_t)(i+1)};
 const uint8_t upper[]={36,40,44,48,149,153,157,161,165};
 for(unsigned i=0;i<9;i++)info.channels[i+13]=(struct ncs_channel){2,upper[i]};
 char name[16]="en0";struct niw_request q={.operation=NIW_GET,.payload_bytes=NIWS_CHANNEL_BYTES,.request=27};memcpy(q.interface_name,name,16);
 uint8_t out[NIWS_CHANNEL_BYTES+8],before[sizeof out];size_t used=99;
 for(unsigned id=0;id<2;id++){
  q.request=id?254:27;memset(out,0xa5,sizeof out);
  assert(!niws_channels(&q,name,&info,out,sizeof out,&used)&&used==4824&&rd(out)==1&&rd(out+4)==22);
  for(unsigned j=0;j<22;j++){assert(rd(out+8+j*12)==1&&rd(out+12+j*12)==info.channels[j].channel&&rd(out+16+j*12)==(info.channels[j].band==1?10u:18u));}
  for(unsigned j=8+22*12;j<4824;j++)assert(!out[j]);for(unsigned j=4824;j<sizeof out;j++)assert(out[j]==0xa5);
 }
 for(unsigned cap=0;cap<4824;cap++){memset(out,0xa5,sizeof out);memcpy(before,out,sizeof out);used=99;assert(niws_channels(&q,name,&info,out,cap,&used));assert(used==99&&!memcmp(before,out,sizeof out));}
 for(unsigned bad=0;bad<12;bad++){
  struct nss_info n=info;struct niw_request a=q;char nm[16];memcpy(nm,name,16);
  if(bad==0)n.count=0;if(bad==1)n.count=40;if(bad==2)n.channels[1]=n.channels[0];
  if(bad==3)n.channels[0].band=0;if(bad==4)n.channels[0].channel=0;if(bad==5)n.channels[0].channel=15;
  if(bad==6)n.band5=0;if(bad==7)n.channels[13].channel=197;if(bad==8)a.payload_bytes--;
  if(bad==9)a.value=1;if(bad==10)a.request=96;if(bad==11)nm[2]='1';
  memset(out,0xa5,sizeof out);memcpy(before,out,sizeof out);used=99;
  assert(niws_channels(&a,nm,&n,out,sizeof out,&used)&&used==99&&!memcmp(before,out,sizeof out));
 }
 assert(niws_channels(0,name,&info,out,sizeof out,&used));assert(niws_channels(&q,0,&info,out,sizeof out,&used));
 assert(niws_channels(&q,name,0,out,sizeof out,&used));assert(niws_channels(&q,name,&info,0,sizeof out,&used));assert(niws_channels(&q,name,&info,out,sizeof out,0));
 puts("PASS scalar: 15365 request/state/capacity/malformed checks + 4 null refusals; channels: 2 layouts, 4824 capacity refusals, 12 malformed cases + 5 null refusals; synthetic only");
 return 0;
}
