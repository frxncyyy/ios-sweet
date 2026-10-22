#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wifi_station_data_diag.h"
static unsigned checks;
#define C(x) do{assert(x);checks++;}while(0)
static void put(unsigned char*p,unsigned v){for(unsigned k=0;k<4;k++)p[k]=(unsigned char)(v>>(8*k));}
int main(void){
 unsigned char*p=malloc(24);C(p!=NULL);memset(p,0,24);p[0]=24;p[3]=0x10;
 const unsigned masks[]={0x0f800000,0x0b800000,0x1803};
 for(unsigned bits=1;bits<8192;bits++){
  unsigned words[3]={0},n=0;for(unsigned w=0;w<3;w++)for(unsigned b=0;b<32;b++)if(masks[w]&(1u<<b)){if(bits&(1u<<n))words[w]|=1u<<b;n++;}
  put(p+4,words[0]|5);put(p+8,words[1]);put(p+16,words[2]);struct nsdd s={0};struct nsdd_rx_error r={0};
  C(nsdd_rx_error_take(&s,&r,1,p,24)==1);C(r.d1==words[0]&&r.d2==words[1]&&r.d4==words[2]);C(s.rx_error_logs==1);s.rx_error_logs=0;struct nsdd zero={0};C(!memcmp(&s,&zero,sizeof s));
 }
 for(unsigned a=0;a<4;a++)for(unsigned h=0;h<2;h++)for(unsigned sec=0;sec<32;sec++)for(unsigned key=0;key<4;key++)for(unsigned tid=0;tid<16;tid++){
  put(p+4,0x800000|1023|(31u<<11)|(sec<<16)|(key<<21));put(p+8,(h<<13)|(tid<<16));put(p+12,a<<16);put(p+16,0);
  struct nsdd s={0};struct nsdd_rx_error r={0};C(nsdd_rx_error_take(&s,&r,UINT64_MAX,p,3840)==1);
  C(r.wlan==1023&&r.groups==31&&r.sec==sec&&r.key==key&&r.ht==h&&r.tid==tid&&r.a1==a&&r.dst==(!a?0:a==1?1:2));
 }
 put(p+4,0x0f800000);put(p+8,0x0b800000);put(p+16,0x1803);struct nsdd s={0};struct nsdd_rx_error r,before;
 memset(&r,0xa5,sizeof r);before=r;
 for(unsigned n=0;n<24;n++){C(!nsdd_rx_error_take(&s,&r,1,p,n));C(!memcmp(&r,&before,sizeof r));}
 C(!nsdd_rx_error_take(&s,&r,1,(void*)1,23));C(!nsdd_rx_error_take(&s,&r,1,p,SIZE_MAX));C(!nsdd_rx_error_take(&s,&r,0,p,24));
 C(!nsdd_rx_error_take(NULL,&r,1,p,24));C(!nsdd_rx_error_take(&s,NULL,1,p,24));C(!nsdd_rx_error_take(&s,&r,1,NULL,24));
 p[0]=23;C(!nsdd_rx_error_take(&s,&r,1,p,24));p[0]=25;C(!nsdd_rx_error_take(&s,&r,1,p,24));p[0]=24;
 p[3]=0;C(!nsdd_rx_error_take(&s,&r,1,p,24));p[2]=1;p[3]=0x38;C(nsdd_rx_error_take(&s,&r,1,p,24));p[2]=0;C(!nsdd_rx_error_take(&s,&r,1,p,24));p[2]=0;p[3]=0x10;
 memset(&s,0,sizeof s);for(unsigned i=0;i<8;i++){C(nsdd_rx_error_take(&s,&r,1,p,24));C(r.n==i+1);}
 struct nsdd saved=s;before=r;for(unsigned i=0;i<100;i++){C(!nsdd_rx_error_take(&s,&r,1,p,24));C(!memcmp(&r,&before,sizeof r));C(!memcmp(&s,&saved,sizeof s));}
 s.rx_error_logs=0;s.summary=1;C(!nsdd_rx_error_take(&s,&r,1,p,24));s.summary=0;s.rx_error_logs=UINT32_MAX;C(!nsdd_rx_error_take(&s,&r,1,p,24));
 s.rx_error_logs=0;put(p+4,0);put(p+8,0);put(p+16,0);C(!nsdd_rx_error_take(&s,&r,1,p,24));
 put(p+4,0x0fffffff);put(p+8,0x0b8fe000);put(p+12,0x30000);put(p+16,0x1803);
 for(unsigned i=0;i<12;i++)nsdd_rx_error(&s,UINT64_MAX,p,24);C(s.rx_error_logs==8);C(!s.rejected[NSDD_RX_ERROR]);
 free(p);printf("PASS %u fixed-header masks, metadata, bounds, atomic refusal, budget and coarse-counter invariance checks\n",checks);return 0;
}
