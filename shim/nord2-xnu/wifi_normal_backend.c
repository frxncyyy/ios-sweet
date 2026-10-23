#include "wifi_normal_backend.h"
#include "wifi_default_feature.h"
static void copy(void*d,const void*s,size_t n){uint8_t*a=d;const uint8_t*b=s;while(n--)*a++=*b++;}
int ncb_prepare(struct ncb_plan*p,const uint8_t*board,size_t bytes,const uint8_t*cap,size_t cap_bytes,uint8_t cap_seq,uint8_t first_seq){
 struct ncfg_channel all[39];struct ncs_channel channels[39];struct nni_address address;size_t count=0;
 if(!p||p->attempted)return -1;
 p->attempted=1;
 p->input.board=board;p->input.board_bytes=bytes;p->input.project=20827;
 if(nrp_caps_decode(cap,cap_bytes,&p->input.caps)||p->input.caps.sequence!=cap_seq||
    ncfg_prepare(&p->input,&p->config)||ncfg_address(&p->input,&p->config,&address)||
    address.source!=NNI_ADDRESS_BOARD||ncfg_channels(&p->input,&p->config,all,39,&count)||count!=39)return -1;
 for(unsigned k=0;k<count;k++)if(all[k].allowed)channels[p->channel_count++]=(struct ncs_channel){all[k].band,all[k].channel};
 if(!p->channel_count||ncs_build(cap,cap_bytes,cap_seq,address.mac,channels,p->channel_count,
    (uint8_t)(first_seq+NCFG_COMMANDS+4),(uint8_t)(first_seq+NCFG_COMMANDS+5),1,1,0,0,&p->scan_frames))return -1;
 for(unsigned k=0;k<NCFG_COMMANDS;k++){
  struct ncfg_entry e;if(ncfg_entry(&p->config,k,&e))return -1;
  p->entry[k]=(struct nct_entry){e.bytes,e.cid,e.set_query};
 }
 for(unsigned k=0;k<3;k++)p->entry[NCFG_COMMANDS+k]=(struct nct_entry){348,0x70,1};
 p->entry[NCFG_COMMANDS+3]=(struct nct_entry){132,0x0a,1};
 p->entry[NCFG_COMMANDS+4]=(struct nct_entry){NCS_ACTIVATE_BYTES,0x11,1};
 p->entry[NCFG_COMMANDS+5]=(struct nct_entry){NCS_SCAN_BYTES,3,1};
 p->prepared=1;return 0;
}
int ncb_build(void*context,unsigned index,uint8_t*out,size_t capacity,size_t*used){
 struct ncb_plan*p=context;
 if(!p||!p->prepared||!out||!used||index>=NCB_COMMANDS)return -1;
 if(index<NCFG_COMMANDS)return ncfg_build(&p->input,&p->config,index,out,capacity,used);
 if(index<NCFG_COMMANDS+3){
  if(capacity<348)return -1;
  uint8_t frame[348]={0};frame[0]=92;frame[1]=1;frame[3]=1;frame[6]=1;
  frame[32]=60;frame[33]=1;frame[36]=0x70;frame[37]=0xa0;frame[38]=1;
  if(ndf_body(index-NCFG_COMMANDS,frame+64))return -1;
  copy(out,frame,sizeof(frame));*used=sizeof(frame);return 0;
 }
 if(index==NCFG_COMMANDS+3){
  if(capacity<132)return -1;
  uint8_t frame[132]={0};frame[0]=132;frame[2]=0;frame[3]=1;frame[6]=1;
  frame[32]=100;frame[36]=0x0a;frame[37]=0xa0;frame[38]=1;frame[64]=0x0c;
  copy(out,frame,sizeof(frame));*used=sizeof(frame);return 0;
 }
 size_t bytes=index==NCFG_COMMANDS+4?NCS_ACTIVATE_BYTES:NCS_SCAN_BYTES;
 if(capacity<bytes)return -1;
 copy(out,index==NCFG_COMMANDS+4?p->scan_frames.activate:p->scan_frames.scan,bytes);*used=bytes;return 0;
}
