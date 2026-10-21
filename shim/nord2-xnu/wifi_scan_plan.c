#include "wifi_scan_plan.h"
#include "wifi_capability.h"
static uint16_t g16(const uint8_t*p){return p[0]|(uint16_t)p[1]<<8;}
static uint32_t g32(const uint8_t*p){return g16(p)|(uint32_t)g16(p+2)<<16;}
static void p16(uint8_t*p,uint16_t x){p[0]=(uint8_t)x;p[1]=(uint8_t)(x>>8);}
static void p32(uint8_t*p,uint32_t x){p16(p,(uint16_t)x);p16(p+2,(uint16_t)(x>>16));}
int ncs_cap_mac(const uint8_t*p,size_t n,uint8_t seq,struct ncs_cap*out){
 struct ncp_result cap;struct ncs_cap r={4,32,4,0,1,4,0,0,0,0};size_t off=40;
 if(!out||ncp_parse(p,n,seq,&cap))return -1;
 for(unsigned i=0;i<cap.elements;i++){uint32_t tag=g32(p+off),z=g32(p+off+4);off+=8;
  if(tag==9){r.tag9=1;if(p[off]>0&&p[off]<=4){r.bss=p[off];r.bss_override=1;}
   if(p[off+2]>0&&p[off+2]<=32){r.wtbl=p[off+2];r.wtbl_override=1;}
   r.wmm=p[off+1]?p[off+1]:1;r.wmm_override=1;
  }off+=z;
 }
 if(r.wtbl<6)return -1;r.own_mac=r.bss==1?0:1;*out=r;return 0;
}
static void header(uint8_t*p,unsigned bytes,uint8_t cid,uint8_t seq){
 p32(p,bytes|(2u<<23));p32(p+4,1u<<16);p16(p+32,(uint16_t)(bytes-32));p[36]=cid;p[37]=0xa0;p[38]=1;p[39]=seq;
}
int ncs_build(const uint8_t*cap,size_t n,uint8_t capseq,const uint8_t mac[6],const struct ncs_channel*c,size_t count,uint8_t aseq,uint8_t seq,uint8_t scanseq,unsigned dbdc_disabled,uint16_t dwell,uint16_t minimum,struct ncs_frames*out){
 struct ncs_frames r={0};unsigned any=0;
 if(!out||!mac||!c||!count||count>64||!aseq||!seq||aseq==seq||dbdc_disabled>1||!((dwell==0&&minimum==0)||(dwell==40&&minimum==20))||ncs_cap_mac(cap,n,capseq,&r.cap))return -1;
 for(unsigned i=0;i<6;i++)any|=mac[i];if(!any||(mac[0]&1))return -1;
 for(size_t i=0;i<count;i++){if((c[i].band!=1&&c[i].band!=2)||!c[i].channel||(c[i].band==1&&c[i].channel>14)||(c[i].band==2&&c[i].channel<=14))return -1;
  for(size_t j=0;j<i;j++)if(c[i].band==c[j].band&&c[i].channel==c[j].channel)return -1;
 }
 header(r.activate,76,0x11,aseq);uint8_t*a=r.activate+64;a[0]=0;a[1]=1;a[2]=0;a[3]=r.cap.own_mac;for(unsigned i=0;i<6;i++)a[4+i]=mac[i];a[10]=r.cap.bmc;
 header(r.scan,1252,3,seq);uint8_t*s=r.scan+64;s[0]=scanseq;s[1]=0;s[2]=0;s[3]=0;s[4]=0;s[5]=0;s[6]=dbdc_disabled?2:0;s[7]=1;
 p16(s+154,dwell);p16(s+828,minimum);s[158]=4;s[159]=(uint8_t)(count>32?32:count);s[826]=(uint8_t)(count>32?count-32:0);
 for(size_t i=0;i<count;i++){size_t o=i<32?160+2*i:830+2*(i-32);s[o]=c[i].band;s[o+1]=c[i].channel;}
 for(unsigned i=0;i<6;i++)s[1112+i]=255;
 r.command[0]=(struct ncs_command){0x11,aseq,1,0,76};r.command[1]=(struct ncs_command){3,seq,1,0,1252};*out=r;return 0;
}
int ncs_done(const uint8_t*p,size_t n,uint8_t seq,struct ncs_event*out){
 struct ncs_event r;if(!p||!out||n<512||n>3840||(g16(p+2)&0x380f)!=0x3800||p[28]!=0x0d||p[52]>64)return -1;
 r.inner_type=g16(p+26);r.rxd_bytes=g16(p);r.event_bytes=g16(p+24);r.event_sequence=p[29];r.scan_sequence=p[36];r.state=p[41];r.version=p[42];if(r.scan_sequence!=seq)return 1;*out=r;return 0;
}
