#include "wifi_station_diag.h"
#if defined(KERNEL) || defined(_KERNEL)
#include <sys/systm.h>
#else
#include <stdio.h>
#endif
static uint32_t le16(const uint8_t*p){return p[0]|(uint32_t)p[1]<<8;}
static uint32_t le32(const uint8_t*p){return le16(p)|le16(p+2)<<16;}
static void begin(struct nstd_record*r,uint64_t g,uint64_t t,unsigned k){*r=(struct nstd_record){0};r->generation=g;r->ticket=t;r->kind=k;}
void nstd_tx(struct nstd_record*r,uint64_t g,uint64_t t,const uint8_t*p,size_t n,unsigned type,unsigned stage){begin(r,g,t,stage==2?3:stage==1?2:1);r->v[0]=type;r->v[1]=(uint32_t)n;if(!p||n<32){r->v[7]=1;return;}uint32_t d0=le32(p),d1=le32(p+4),d3=le32(p+12),d5=le32(p+20),d6=le32(p+24);if(stage==0){r->v[2]=d5&255;r->v[3]=d1&255;r->v[4]=d0>>25;r->v[5]=(d6>>16)&16383;r->v[6]=d0&65535;r->v[7]=0;}else{r->v[2]=(d0>>23)&3;r->v[3]=(d1>>16)&3;r->v[4]=(d1>>15)&1;r->v[5]=(d5>>9)&3;r->v[6]=(d3>>1)&1;r->v[7]=(d0>>16)&127;}}
void nstd_txs(struct nstd_record*r,uint64_t g,uint64_t t,const uint8_t*p,size_t n,unsigned pending,unsigned pid,unsigned wlan,unsigned done){begin(r,g,t,4);r->v[0]=(uint32_t)n;r->v[3]=r->v[4]=r->v[5]=256;r->v[6]=1;if(!p)return;if(n>=2)r->v[1]=le16(p);if(n>=26)r->v[2]=le16(p+24);if(n>=30)r->v[7]=p[29];if(n<36)return;size_t z=n-36;if(z){r->v[3]=p[36];}if(z>1)r->v[5]=p[37];if(z>4)r->v[4]=p[40];r->v[6]=z<88?2:!pending?3:p[36]!=pid?4:p[40]!=wlan?5:done?6:7;}
void nstd_eapol(struct nstd_record*r,uint64_t g,uint64_t t,const uint8_t*p,size_t n){begin(r,g,t,5);r->v[0]=(uint32_t)n;if(!p||n<4)return;r->v[1]=p[0];r->v[2]=p[1];r->v[5]=(uint32_t)p[2]<<8|p[3];if(n<7||p[1]!=3)return;r->v[3]=(uint32_t)p[5]<<8|p[6];r->v[4]=r->v[3]==0x008a?1:r->v[3]==0x13ca?3:0;r->v[6]=r->v[5]==n-4;}
static uint32_t queue_count[5],type_count[5][33];
static void increment(uint32_t*p){if(*p!=UINT32_MAX)(*p)++;}
void nstd_rx(uint64_t g,uint64_t t,unsigned q,const uint8_t*p,size_t n){if(q>=5||!p)return;unsigned type=n>=4?(le16(p+2)>>11)&31:32;increment(&queue_count[q]);increment(&type_count[q][type]);if(queue_count[q]<=2||(type==0&&type_count[q][0]<=8)){struct nstd_record r;begin(&r,g,t,7);r.v[0]=q;r.v[1]=type;r.v[2]=queue_count[q];r.v[3]=type_count[q][type];r.v[4]=(uint32_t)n;r.v[5]=n>=2?le16(p):0;nstd_log(&r);}}
void nstd_summary(struct nstd_record*r,uint64_t g,uint64_t t){begin(r,g,t,8);for(unsigned q=0;q<5;q++){r->v[q]=queue_count[q];for(unsigned k=0;k<3;k++){unsigned type=k==0?0:k==1?7:2;uint32_t a=r->v[5+k],b=type_count[q][type];r->v[5+k]=UINT32_MAX-a<b?UINT32_MAX:a+b;}}}
void nstd_log(const struct nstd_record*r){static unsigned count,summary;if(!r||count>=96||(r->kind!=8&&count>=95)||(r->kind==8&&summary))return;if(r->kind==8)summary=1;unsigned n=++count;printf("NORD2 WIFI NSTDIAG: n=%u g=%llu t=%llu k=%u a=%u b=%u c=%u d=%u e=%u f=%u h=%u i=%u\n",n,(unsigned long long)r->generation,(unsigned long long)r->ticket,r->kind,r->v[0],r->v[1],r->v[2],r->v[3],r->v[4],r->v[5],r->v[6],r->v[7]);}
