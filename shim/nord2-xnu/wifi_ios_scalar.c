#include "wifi_ios_scalar.h"
int niws_reply(const struct niw_request *q,const char name[16],
 enum niws_native_state state,void *output,size_t capacity,size_t *used){
 uint32_t value;uint8_t *p=output;unsigned terminated=0;
 if(!q||!name||!p||!used||capacity<4||q->operation!=NIW_GET||q->payload_bytes||q->value||
    (state!=NIWS_IDLE&&state!=NIWS_SCANNING))return -1;
 for(unsigned i=0;i<16;i++){
  if(q->interface_name[i]!=name[i])return -1;
  if(!name[i])terminated=1;
 }
 if(!terminated||!name[0])return -1;
 if(q->request==13)value=state==NIWS_SCANNING?1u:0u;
 else if(q->request==15)value=1;
 else return -1;
 p[0]=(uint8_t)value;p[1]=0;p[2]=0;p[3]=0;*used=4;return 0;
}

static void niws_u32(uint8_t*p,uint32_t x){p[0]=(uint8_t)x;p[1]=(uint8_t)(x>>8);p[2]=(uint8_t)(x>>16);p[3]=(uint8_t)(x>>24);}
int niws_channels(const struct niw_request*q,const char name[16],
 const struct nss_info*info,void*output,size_t capacity,size_t*used){
 uint8_t*p=output;unsigned terminated=0;
 if(!q||!name||!info||!p||!used||capacity<NIWS_CHANNEL_BYTES||q->operation!=NIW_GET||
    (q->request!=27&&q->request!=254)||q->value||q->payload_bytes!=NIWS_CHANNEL_BYTES||
    !info->count||info->count>NSS_CHANNELS||info->band5>1)return -1;
 for(unsigned j=0;j<16;j++){if(q->interface_name[j]!=name[j])return -1;if(!name[j])terminated=1;}
 if(!name[0]||!terminated)return -1;
 for(unsigned j=0;j<info->count;j++){
  unsigned band=info->channels[j].band,ch=info->channels[j].channel;
  if((band!=1&&band!=2)||!ch||(band==1&&ch>14)||
     (band==2&&(!info->band5||ch<=14||ch>196)))return -1;
  for(unsigned k=0;k<j;k++)if(band==info->channels[k].band&&ch==info->channels[k].channel)return -1;
 }
 for(unsigned j=0;j<NIWS_CHANNEL_BYTES;j++)p[j]=0;
 niws_u32(p,1);niws_u32(p+4,info->count);
 for(unsigned j=0;j<info->count;j++){
  uint8_t*ch=p+8+12*j;niws_u32(ch,1);niws_u32(ch+4,info->channels[j].channel);
  niws_u32(ch+8,2u|(info->channels[j].band==1?8u:16u));
 }
 *used=NIWS_CHANNEL_BYTES;return 0;
}
