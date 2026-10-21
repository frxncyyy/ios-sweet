#include "wifi_wfsys.h"
#include <string.h>

#define AP_WAKE 0x180601a0u
#define WF_WAKE 0x180601a4u
#define ACK 0x18060184u
#define CFG_ID 0x18001000u
#define CPU_RST 0x18000010u
#define BGF_CPU_RST 0x18000014u
#define BGF_TOP_PWR 0x18000008u
#define SEMA_RST 0x18000018u
#define AFE 0x18003008u
#define TOP_PWR 0x18000000u
#define PWR_ACK 0x180602ccu
#define WF_PROT 0x18001620u
#define DMA_PROT 0x18001624u
#define VDNR 0x1800e06cu
#define WF_ID 0x184b0010u
#define IDLE 0x1800f004u
#define EMI_REQ 0x18001c14u

static int fail(struct nws_state *s,int e) {
 s->error=e;s->halt=(s->dirty||s->ap_vote||s->wf_vote||s->sem20||s->power);
 s->unverified=s->halt;return e;
}
static int proof(const struct nws_ops *o,void *c,struct nws_state *s) {
 return o->proof(c,s->stage) ? fail(s,-2):0;
}
static int rd(const struct nws_ops *o,void *c,struct nws_state *s,uint32_t a,uint32_t *v) {
 if(proof(o,c,s))return s->error;
 return o->read(c,a,v)?fail(s,-3):0;
}
static int wr(const struct nws_ops *o,void *c,struct nws_state *s,uint32_t a,uint32_t v) {
 if(proof(o,c,s))return s->error;
 s->dirty=1;return o->write(c,a,v)?fail(s,-4):0;
}
static int delay(const struct nws_ops *o,void *c,struct nws_state *s,unsigned us) {
 if(proof(o,c,s))return s->error;
 return o->delay_us(c,us)?fail(s,-5):0;
}
static int readable(const struct nws_ops *o,void *c,struct nws_state *s) {
 if(proof(o,c,s))return s->error;
 return o->readable(c)?fail(s,-6):0;
}
static int rmw(const struct nws_ops *o,void *c,struct nws_state *s,uint32_t a,uint32_t clr,uint32_t set) {
 uint32_t v;if(rd(o,c,s,a,&v))return s->error;
 return wr(o,c,s,a,(v&~clr)|set);
}
static int poll(const struct nws_ops *o,void *c,struct nws_state *s,
 uint32_t a,uint32_t mask,uint32_t want,unsigned count,unsigned us,unsigned *seen) {
 uint32_t v;unsigned i;
 for(i=0;i<count;i++) {
  if(i&&delay(o,c,s,us))return s->error;
  if(rd(o,c,s,a,&v))return s->error;
  ++*seen;
  if(a==PWR_ACK)s->final_power=v;
  if((v&mask)==want)return 0;
 }
 return fail(s,-7);
}
static int version(const struct nws_ops *o,void *c,struct nws_state *s,
 uint32_t a,uint32_t want,unsigned count,unsigned us,unsigned *seen,uint32_t *last) {
 unsigned i;for(i=0;i<count;i++) {
  if(i&&us&&delay(o,c,s,us))return s->error;
  if(rd(o,c,s,a,last))return s->error;
  ++*seen;if(*last==want)return 0;
 }
 return fail(s,-8);
}
static int sem_acquire(const struct nws_ops *o,void *c,struct nws_state *s,unsigned *tries) {
 unsigned i;int v;
 for(i=0;i<1000;i++) {
  if(proof(o,c,s))return s->error;
  v=o->sema20_try(c);++*tries;
  if(v==1){s->sem20=1;return 0;}
  if(v<0)return fail(s,-9);
  if(i<999&&delay(o,c,s,1))return s->error;
 }
 return fail(s,-10);
}
static int sem_release(const struct nws_ops *o,void *c,struct nws_state *s) {
 if(proof(o,c,s))return s->error;
 if(o->sema20_release(c))return fail(s,-11);
 s->sem20=0;return 0;
}
static int clocks_on(const struct nws_ops *o,void *c,struct nws_state *s) {
 uint32_t v;
 if(sem_acquire(o,c,s,&s->sem_tries))return s->error;
 if(rd(o,c,s,AFE,&v))return s->error;
 s->initial_afe=v;
 /* Static WIFI vote starts empty and this diagnostic owns no other radio. */
 if(v&((1u<<20)|(1u<<21)))return fail(s,-12);
 s->clock_vote=1;
 if(rmw(o,c,s,AFE,0,1u<<21))return s->error;
 if(delay(o,c,s,30))return s->error;
 if(rmw(o,c,s,AFE,0,1u<<20))return s->error;
 if(delay(o,c,s,50))return s->error;
 return sem_release(o,c,s);
}
static int clocks_off(const struct nws_ops *o,void *c,struct nws_state *s) {
 uint32_t v;
 if(sem_acquire(o,c,s,&s->reverse_sem_tries))return s->error;
 if(rd(o,c,s,AFE,&v))return s->error;
 if((v&((1u<<20)|(1u<<21)))!=((1u<<20)|(1u<<21)))return fail(s,-13);
 /* Selected source releases WPLL then BPLL under semaphore 20. */
 if(rmw(o,c,s,AFE,1u<<20,0))return s->error;
 if(rmw(o,c,s,AFE,1u<<21,0))return s->error;
 if(sem_release(o,c,s))return s->error;
 s->clock_vote=0;return 0;
}

int nws_run(const struct nws_ops *o,void *c,unsigned optin,struct nws_state *s) {
 uint32_t v;
 if(!s||!optin||optin!=1||s->entered)return -1;
 memset(s,0,sizeof(*s));s->entered=1;s->stage=NWS_PREWAKE;
 if(!o||!o->proof||!o->read||!o->write||!o->delay_us||
    !o->readable||!o->sema20_try||!o->sema20_release)return fail(s,-1);
 if(rd(o,c,s,AP_WAKE,&v))return s->error;
 if(v)return fail(s,-14); /* no inherited AP vote or unknown bits */
 s->ap_vote=1;if(wr(o,c,s,AP_WAKE,1))return s->error;
 s->stage=NWS_AP_ACK;
 if(poll(o,c,s,ACK,1u<<5,0,12,1000,&s->ap_ack_polls))return s->error;
 if(readable(o,c,s))return s->error;
 s->stage=NWS_AP_READY;
 if(version(o,c,s,CFG_ID,0x20010101u,10,0,&s->cfg_polls,&s->cfg_id))return s->error;

 /* Fresh WLAN force-on lifetime; distinct from the AP 1a0 reference. */
 if(rd(o,c,s,WF_WAKE,&v))return s->error;
 if(v&1u)return fail(s,-15);
 s->wf_vote=1;if(wr(o,c,s,WF_WAKE,v|1u))return s->error;
 s->stage=NWS_WF_ACK;
 if(poll(o,c,s,ACK,1u<<5,0,12,1000,&s->wf_ack_polls))return s->error;
 if(readable(o,c,s))return s->error;
 s->stage=NWS_COMMON_READY;
 if(version(o,c,s,CFG_ID,0x20010101u,12,1000,&s->cfg_polls,&s->cfg_id))return s->error;
 {
  const uint32_t a[5]={CPU_RST,TOP_PWR,PWR_ACK,BGF_CPU_RST,BGF_TOP_PWR};
  const uint32_t mask[5]={1u,0x80u,4u,1u,0x80u};unsigned i;
  for(i=0;i<5;i++){
   if(rd(o,c,s,a[i],&s->cold[i]))return s->error;
   s->cold_valid|=1u<<i;
   if(s->cold[i]&mask[i])return fail(s,-16);
  }
 }
 /* Source asserts reset even though the cold proof already showed it held. */
 if(rd(o,c,s,CPU_RST,&v))return s->error;
 if(v&1u)return fail(s,-16);
 if(wr(o,c,s,CPU_RST,v&~1u))return s->error;
 s->cpu_held=1;
 s->stage=NWS_CPU_HELD;
 if(readable(o,c,s))return s->error; /* source sem20 pre-acquire check */
 if(clocks_on(o,c,s))return s->error;
 s->stage=NWS_CLOCKS;

 if(rd(o,c,s,TOP_PWR,&v))return s->error;
 s->power=1;
 if(wr(o,c,s,TOP_PWR,(v&0x0000ff7fu)|0x57460080u))return s->error;
 if(poll(o,c,s,PWR_ACK,1u<<2,1u<<2,12,500,&s->power_polls))return s->error;
 s->stage=NWS_POWERED;
 if(readable(o,c,s))return s->error;
 if(rmw(o,c,s,WF_PROT,1u,0))return s->error;
 if(poll(o,c,s,WF_PROT,1u<<3,0,102,500,&s->prot_polls))return s->error;
 if(rmw(o,c,s,DMA_PROT,1u,0))return s->error;
 if(poll(o,c,s,DMA_PROT,1u<<3,0,102,500,&s->prot_polls))return s->error;
 s->protection=1;s->stage=NWS_PROTECTION;
 if(rmw(o,c,s,VDNR,0,1u))return s->error;
 if(version(o,c,s,WF_ID,0x20010000u,12,500,&s->id_polls,&s->wfsys_id))return s->error;
 s->local_id=1;s->stage=NWS_LOCAL_ID;

 /* CPU-held source-subset power-off. No FW_WIFI top-clock resource was
  * acquired, so selected FW-resource release is deliberately not called. */
 s->stage=NWS_REVERSE;
 if(rmw(o,c,s,IDLE,0,1u))return s->error;
 if(rmw(o,c,s,WF_PROT,0,1u))return s->error;
 if(poll(o,c,s,WF_PROT,1u<<3,1u<<3,102,500,&s->prot_polls))return s->error;
 if(rmw(o,c,s,DMA_PROT,0,1u))return s->error;
 if(poll(o,c,s,DMA_PROT,1u<<3,1u<<3,102,500,&s->prot_polls))return s->error;
 s->protection=0;
 if(rmw(o,c,s,SEMA_RST,1u,0))return s->error;
 if(readable(o,c,s))return s->error; /* source sem20 pre-acquire check */
 if(clocks_off(o,c,s))return s->error;
 if(readable(o,c,s))return s->error; /* selected post-release check */
 if(rd(o,c,s,TOP_PWR,&v))return s->error;
 if(wr(o,c,s,TOP_PWR,(v&0x0000ff7fu)|0x57460000u))return s->error;
 if(poll(o,c,s,PWR_ACK,1u<<2,0,12,500,&s->off_polls))return s->error;
 s->power=0;
 if(rmw(o,c,s,IDLE,1u,0))return s->error;
 if(rd(o,c,s,EMI_REQ,&v))return s->error;
 if(wr(o,c,s,EMI_REQ,v|1u))return s->error;
 if(wr(o,c,s,EMI_REQ,v&~1u))return s->error;
 if(rmw(o,c,s,WF_WAKE,1u,0))return s->error;
 s->wf_vote=0;
 /* Final MMIO. No proof/read/log callback may follow this AP sleep write. */
 if(wr(o,c,s,AP_WAKE,0))return s->error;
 s->ap_vote=0;s->dirty=0;s->unverified=0;s->balanced=1;s->stage=NWS_ASLEEP;
 return 0;
}
