#include "wifi_dma_preflight.h"
#include <assert.h>
#include <stdio.h>
struct mock{unsigned calls,fail,failed,after,reads,mode;};
static int step(struct mock *m){if(m->failed)m->after++;if(++m->calls==m->fail){m->failed=1;return -1;}return 0;}
static int owner(void *p){return step(p);}
static int read32(void *p,uint32_t a,uint32_t *v){struct mock *m=p;if(step(m))return -1;m->reads++;*v=0;
 if(a==0x18024208||a==0x18025208){if(m->mode==1)*v=15;if(m->mode==3&&m->reads>2)*v=1;return 0;}
 assert((a>=0x18024500&&a<=0x1802457c)||(a>=0x18025300&&a<=0x1802535c)||(a>=0x18025380&&a<=0x180253dc)||(a>=0x18025400&&a<=0x1802543c)||(a>=0x18025500&&a<=0x1802552c));
 if(m->mode>=5&&((a&15)==4))*v=0x200;
 if(m->mode==6&&a==0x18025410)*v=0x90000000;
 if(m->mode==7&&a==0x18025414)*v=0x10200;
 if(m->mode==8&&a==0x18025418)*v=1;
 if(m->mode==9&&a==0x1802541c)*v=1;
 if(m->mode==10&&a==0x18025414)*v=0x201;
 if(m->mode==2&&a==0x18025414)*v=1024;
 if(m->mode==4){unsigned off=a&15;if(off==4)*v=0x8000f000;else if(off==8||off==12)*v=0xfffff000;}
 return 0;}
int main(void){const struct nwdp_io io={owner,read32};struct nwdp_state s={0};struct mock m={0};assert(nwdp_capture(&io,&m,&s,0)&&!m.calls);assert(!nwdp_capture(&io,&m,&s,1)&&s.complete&&s.observed_empty&&s.stable&&s.reads==112&&s.cfg_valid==15);unsigned total=m.calls;assert(nwdp_capture(&io,&m,&s,1)&&m.calls==total);
 for(unsigned f=1;f<=total;f++){s=(struct nwdp_state){0};m=(struct mock){.fail=f};assert(nwdp_capture(&io,&m,&s,1)&&!s.complete&&!m.after);}
 for(unsigned mode=1;mode<=4;mode++){s=(struct nwdp_state){0};m=(struct mock){.mode=mode};assert(nwdp_capture(&io,&m,&s,1)==(mode<4?1:0)&&s.complete&&s.reads==112);if(mode<4)assert(!s.observed_empty&&s.first_reason);else assert(s.observed_empty&&!s.first_reason);if(mode==2)assert(s.first_addr==0x18025414&&s.first_raw==1024&&s.first_mask==0xf0fff);if(mode==3)assert(!s.stable);}
 for(unsigned mode=5;mode<=10;mode++){s=(struct nwdp_state){0};m=(struct mock){.mode=mode};assert(nwdp_capture(&io,&m,&s,1)==(mode==5?0:1)&&s.complete&&s.reads==112);if(mode==5)for(unsigned n=0;n<NWDP_RINGS;n++)assert(s.rings[n].raw[1]==0x200);}
 printf("PASS read-onlysnapshot112reads/27definedrings/%ucallbackfaults/defaultoff/reentry/activefullcapture/firstmismatch/globalchange/reservedbitsignored; no writeAPI\n",total);}
