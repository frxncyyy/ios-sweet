/* Actual pure transport included unchanged. live reset/odd-sequence injection
 * below are host fixture seams only, not exported production functionality. */
#include <assert.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <string.h>
#include "wifi_live_control.c"
static unsigned long checks;
#define C(x) do{__atomic_add_fetch(&checks,1,__ATOMIC_RELAXED);assert(x);}while(0)
static uint32_t word(const uint8_t*p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static void fresh(void){memset(&live,0,sizeof live);}
static void setup(uint8_t*s,unsigned type,unsigned code,unsigned v,unsigned i,unsigned n){s[0]=type;s[1]=code;s[2]=v;s[3]=v>>8;s[4]=i;s[5]=i>>8;s[6]=n;s[7]=n>>8;}
static void fill(struct nwl_snapshot*s,unsigned token){for(unsigned k=0;k<NWL_WORDS;k++)s->word[k]=token*1000u+k;}
static void basics(void){uint8_t s[8],out[300],before[300];uint32_t n=999;struct nwl_request q;struct nwl_snapshot snap={0};
 fresh();C(!nwl_enabled());C(nwl_initialize(0)<0);C(!nwl_submit(0x1001,0));C(!nwl_take(&q));C(!nwl_reply(out,sizeof out));
 memset(out,0xa5,sizeof out);C(nwl_caps(out,63)==0&&out[0]==0xa5);C(nwl_caps(out,sizeof out)==64);C(word(out)==NWL_MAGIC&&word(out+36)==0);
 C(!nwl_initialize(UINT64_C(0x1234567887654321)));C(nwl_enabled());C(nwl_initialize(2)<0);
 C(!nwl_submit(0,0));C(!nwl_submit(65536,0));C(!nwl_submit(0x1001,1));C(!nwl_submit(0x1001,65536));C(!nwl_submit(0xf001,0));C(!nwl_submit(0x1002,0));
 for(unsigned type=0;type<256;type++)if(type!=0x40){setup(s,type,NWL_USB_COMMAND,0x1001,0,0);C(nwl_usb(s,out,sizeof out,&n)==0);}
 for(unsigned len=1;len<=65535;len++){setup(s,0x40,NWL_USB_COMMAND,0x1001,0,len);C(nwl_usb(s,out,sizeof out,&n)==0);}
 for(unsigned code=0;code<256;code++)if(code<0x60||code>0x62){setup(s,0xc0,code,0,0,64);C(nwl_usb(s,out,sizeof out,&n)==-1);}
 for(unsigned k=0;k<2;k++){unsigned code=k?NWL_USB_REPLY:NWL_USB_CAP;setup(s,0xc0,code,1,0,256);C(!nwl_usb(s,out,sizeof out,&n));setup(s,0xc0,code,0,1,256);C(!nwl_usb(s,out,sizeof out,&n));setup(s,0xc0,code,0,0,0);C(!nwl_usb(s,out,sizeof out,&n));}
 setup(s,0xc0,NWL_USB_CAP,0,0,1);C(nwl_usb(s,out,sizeof out,&n)==1&&n==1);setup(s,0xc0,NWL_USB_CAP,0,0,65535);C(nwl_usb(s,out,sizeof out,&n)==1&&n==64);
 C(!nwl_usb(NULL,out,sizeof out,&n));C(!nwl_usb(s,NULL,0,&n));C(!nwl_usb(s,out,sizeof out,NULL));
 setup(s,0x40,NWL_USB_COMMAND,0x1001,0,0);C(nwl_usb(s,out,sizeof out,&n)==2&&n==0);C(nwl_submit(0x1001,0));C(!nwl_submit(0x2001,0));C(!nwl_submit(0x2002,0));
 C(nwl_take(&q));C(!nwl_take(&q));struct nwl_request bad=q;bad.token++;C(nwl_complete(&bad,0,&snap)<0);C(nwl_complete(&q,NWL_LIMIT+1,&snap)<0);C(nwl_complete(&q,0,NULL)<0);
 fill(&snap,1);C(!nwl_complete(&q,NWL_OK,&snap));C(nwl_complete(&q,0,&snap)<0);C(nwl_submit(0x1001,0));C(!nwl_take(&q));
 C(nwl_reply(out,255)==0);C(nwl_reply(out,sizeof out)==256);C(word(out)==NWL_MAGIC&&word(out+4)==1&&word(out+8)==256&&word(out+20)==1&&word(out+24)==1&&word(out+32)==NWL_OK);
 C(word(out+12)==0x87654321&&word(out+16)==0x12345678);for(unsigned k=9;k<64;k++)C(word(out+k*4)==1000+k);
 /* IRQ preempts an odd writer: one bounded read returnsbusy, no partial copy. */
 memset(out,0xa5,sizeof out);memcpy(before,out,sizeof out);__atomic_store_n(&live.sequence,3,__ATOMIC_SEQ_CST);C(!nwl_reply(out,sizeof out));C(!memcmp(out,before,sizeof out));__atomic_store_n(&live.sequence,2,__ATOMIC_SEQ_CST);
 C(nwl_submit(0x7002,65535));C(nwl_reply(out,sizeof out)==256&&word(out+20)==1);C(nwl_take(&q)&&q.token==2&&q.op==7&&q.generation==65535);C(!nwl_complete(&q,NWL_STALE,&snap));C(nwl_reply(out,sizeof out)==256&&word(out+20)==2&&word(out+28)==65535&&word(out+32)==NWL_STALE);
}
static unsigned producer_done,owner_done;static const unsigned rounds=500;
static void*producer(void*x){(void)x;for(unsigned k=1;k<=rounds;k++){while(!nwl_submit(0x1000|k,0))sched_yield();C(nwl_submit(0x1000|k,0));}__atomic_store_n(&producer_done,1,__ATOMIC_RELEASE);return NULL;}
static void*owner(void*x){(void)x;unsigned seen=0;struct nwl_request q;struct nwl_snapshot s;
 while(seen<rounds){if(!nwl_take(&q)){sched_yield();continue;}C(q.token==++seen&&q.op==1&&q.generation==0);fill(&s,q.token);C(!nwl_complete(&q,0,&s));}
 __atomic_store_n(&owner_done,1,__ATOMIC_RELEASE);return NULL;}
static void*reader(void*x){(void)x;uint8_t out[256];unsigned successful=0;
 do{if(nwl_reply(out,sizeof out)){unsigned token=word(out+20);C(token>=1&&token<=rounds);C(word(out)==NWL_MAGIC&&word(out+4)==1&&word(out+8)==256&&word(out+24)==1&&word(out+28)==0&&word(out+32)==0);for(unsigned k=9;k<64;k++)C(word(out+4*k)==token*1000u+k);successful++;}else sched_yield();}while(!__atomic_load_n(&owner_done,__ATOMIC_ACQUIRE));
 C(successful>0);return NULL;}
static void concurrency(void){fresh();C(!nwl_initialize(3));producer_done=owner_done=0;pthread_t p,o,r;C(!pthread_create(&p,NULL,producer,NULL));C(!pthread_create(&o,NULL,owner,NULL));C(!pthread_create(&r,NULL,reader,NULL));C(!pthread_join(p,NULL));C(!pthread_join(o,NULL));C(!pthread_join(r,NULL));C(producer_done&&owner_done);}
static void exhaustion(void){fresh();C(!nwl_initialize(7));struct nwl_request q;struct nwl_snapshot s={0};for(unsigned t=1;t<=4095;t++){C(nwl_submit(0x1000|t,0));C(nwl_take(&q)&&q.token==t);C(!nwl_complete(&q,0,&s));}C(nwl_submit(0x1fff,0));C(!nwl_submit(0x2fff,0));C(!nwl_submit(0x1001,0));C(!nwl_submit(0x1000,0));C(!nwl_submit(0x2000,0));C(!nwl_take(&q));C(__atomic_load_n(&live.sequence,__ATOMIC_SEQ_CST)==8190);}
int main(void){basics();concurrency();exhaustion();printf("PASS %lu actual live transport assertions: shape/replay/startup/busy/odd-writer/concurrent snapshot/exhaustion\n",checks);return 0;}
