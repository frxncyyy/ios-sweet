#include "wifi_rom_powered.h"

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
#define AP2WF 0x18001120u
#define MCU_ENTRY 0x185001dcu
#define WF_ENTRY 0x185001e0u
#define ROM_DONE 0x184c1604u
#define DEBUG_CTRL 0x1800f000u

static int fail(struct nrpc_state *s,int e) {
 s->error=e;s->halt=(s->dirty||s->ap_vote||s->wf_vote||s->sem20||s->power||s->policy_attempted);
 s->unverified=s->halt;return e;
}
static int proof(const struct nrpc_ops *o,void *c,struct nrpc_state *s) {
 return o->proof(c,s->stage) ? fail(s,-2):0;
}
static int rd(const struct nrpc_ops *o,void *c,struct nrpc_state *s,uint32_t a,uint32_t *v) {
 if(proof(o,c,s))return s->error;
 return o->read(c,a,v)?fail(s,-3):0;
}
static int wr(const struct nrpc_ops *o,void *c,struct nrpc_state *s,uint32_t a,uint32_t v) {
 if(proof(o,c,s))return s->error;
 s->dirty=1;return o->write(c,a,v)?fail(s,-4):0;
}
static int delay(const struct nrpc_ops *o,void *c,struct nrpc_state *s,unsigned us) {
 if(proof(o,c,s))return s->error;
 return o->delay_us(c,us)?fail(s,-5):0;
}
static int readable(const struct nrpc_ops *o,void *c,struct nrpc_state *s) {
 if(proof(o,c,s))return s->error;
 return o->readable(c)?fail(s,-6):0;
}
static int rmw(const struct nrpc_ops *o,void *c,struct nrpc_state *s,uint32_t a,uint32_t clr,uint32_t set) {
 uint32_t v;if(rd(o,c,s,a,&v))return s->error;
 return wr(o,c,s,a,(v&~clr)|set);
}
static int poll(const struct nrpc_ops *o,void *c,struct nrpc_state *s,
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
static int version(const struct nrpc_ops *o,void *c,struct nrpc_state *s,
 uint32_t a,uint32_t want,unsigned count,unsigned us,unsigned *seen,uint32_t *last) {
 unsigned i;for(i=0;i<count;i++) {
  if(i&&us&&delay(o,c,s,us))return s->error;
  if(rd(o,c,s,a,last))return s->error;
  ++*seen;if(*last==want)return 0;
 }
 return fail(s,-8);
}
static int sem_acquire(const struct nrpc_ops *o,void *c,struct nrpc_state *s,unsigned *tries) {
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
static int sem_release(const struct nrpc_ops *o,void *c,struct nrpc_state *s) {
 if(proof(o,c,s))return s->error;
 if(o->sema20_release(c))return fail(s,-11);
 s->sem20=0;return 0;
}
static int clocks_on(const struct nrpc_ops *o,void *c,struct nrpc_state *s) {
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
static int clocks_off(const struct nrpc_ops *o,void *c,struct nrpc_state *s) {
 uint32_t v;
 if(sem_acquire(o,c,s,&s->release_sem_tries))return s->error;
 if(rd(o,c,s,AFE,&v))return s->error;
 if((v&((1u<<20)|(1u<<21)))!=((1u<<20)|(1u<<21)))return fail(s,-13);
 /* Selected source releases WPLL then BPLL under semaphore 20. */
 if(rmw(o,c,s,AFE,1u<<20,0))return s->error;
 if(rmw(o,c,s,AFE,1u<<21,0))return s->error;
 if(sem_release(o,c,s))return s->error;
 s->clock_vote=0;return 0;
}

static int policy_receipt(const struct nrb_native *n) {
 const struct nrb_state *r=&n->state;
 return r->prepared&&r->policy_attempted==1&&
  r->issued==2&&r->accepted==2&&r->raw_valid==3&&
  !r->raw[0][0]&&!r->raw[1][0]&&!r->halt&&!r->error;
}
static int policy_ok(const struct nrb_native *n) {
 return policy_receipt(n)&&n->state.phase==3;
}
int nrpc_receipt_ok(const struct nrb_native *n) {
 if(!n)return 0;
 const struct nrb_state *r=&n->state;
 unsigned chunks=0,bytes=0;
 for(unsigned i=0;i<NWF_ROM_SPANS;i++){
  unsigned sz=r->plan.spans[i].bytes;
  if(!sz||sz>NRB_BACKUP_BYTES-bytes)return 0;
  bytes+=sz;chunks+=(sz+NRB_CHUNK-1u)/NRB_CHUNK;
 }
 if(bytes!=NRB_BACKUP_BYTES)return 0;
 return policy_receipt(n)&&r->phase==7&&r->placed==1&&r->mapped==1&&
  r->backup_complete==1&&r->write_attempted==1&&r->barriers==2&&
  r->writes==chunks&&r->reads==2u*chunks;
}
int nrpc_run(const struct nrpc_ops *o,void *c,unsigned optin,
 struct nrb_native *rom,struct nrpc_state *s) {
 uint32_t v;
 if(!s||!rom||!optin||optin!=1||s->entered)return -1;
 __builtin_memset(s,0,sizeof(*s));s->entered=1;s->stage=NRPC_PREWAKE;
 if(!o||!o->proof||!o->read||!o->write||!o->delay_us||
    !o->readable||!o->sema20_try||!o->sema20_release||
    !o->rom_policy||!o->rom_place)return fail(s,-1);
 if(rd(o,c,s,AP_WAKE,&v))return s->error;
 if(v)return fail(s,-14); /* no inherited AP vote or unknown bits */
 s->ap_vote=1;if(wr(o,c,s,AP_WAKE,1))return s->error;
 s->stage=NRPC_AP_ACK;
 if(poll(o,c,s,ACK,1u<<5,0,12,1000,&s->ap_ack_polls))return s->error;
 if(readable(o,c,s))return s->error;
 s->stage=NRPC_AP_READY;
 if(version(o,c,s,CFG_ID,0x20010101u,10,0,&s->cfg_polls,&s->cfg_id))return s->error;

 /* Fresh WLAN force-on lifetime; distinct from the AP 1a0 reference. */
 if(rd(o,c,s,WF_WAKE,&v))return s->error;
 if(v&1u)return fail(s,-15);
 s->wf_vote=1;if(wr(o,c,s,WF_WAKE,v|1u))return s->error;
 s->stage=NRPC_WF_ACK;
 if(poll(o,c,s,ACK,1u<<5,0,12,1000,&s->wf_ack_polls))return s->error;
 if(readable(o,c,s))return s->error;
 s->stage=NRPC_COMMON_READY;
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
 /* Cold hardware is now proven under fresh AP/WF votes. SET can lock
  * region27 before the first CPU reset/PLL/WFSYS power operation. */
 s->policy_attempted=1;s->dirty=1;
 if(proof(o,c,s)||o->rom_policy(c,rom)||!policy_ok(rom))return fail(s,-17);
 s->policy_accepted=1;
 /* Source asserts reset even though the cold proof already showed it held. */
 if(rd(o,c,s,CPU_RST,&v))return s->error;
 if(v&1u)return fail(s,-16);
 if(wr(o,c,s,CPU_RST,v&~1u))return s->error;
 s->cpu_held=1;
 s->stage=NRPC_CPU_HELD;
 if(readable(o,c,s))return s->error; /* source sem20 pre-acquire check */
 if(clocks_on(o,c,s))return s->error;
 s->stage=NRPC_CLOCKS;

 if(rd(o,c,s,TOP_PWR,&v))return s->error;
 s->power=1;
 if(wr(o,c,s,TOP_PWR,(v&0x0000ff7fu)|0x57460080u))return s->error;
 if(poll(o,c,s,PWR_ACK,1u<<2,1u<<2,12,500,&s->power_polls))return s->error;
 s->stage=NRPC_POWERED;
 if(readable(o,c,s))return s->error;
 if(rmw(o,c,s,WF_PROT,1u,0))return s->error;
 if(poll(o,c,s,WF_PROT,1u<<3,0,102,500,&s->prot_polls))return s->error;
 if(rmw(o,c,s,DMA_PROT,1u,0))return s->error;
 if(poll(o,c,s,DMA_PROT,1u<<3,0,102,500,&s->prot_polls))return s->error;
 s->protection=1;s->stage=NRPC_PROTECTION;
 if(rmw(o,c,s,VDNR,0,1u))return s->error;
 if(version(o,c,s,WF_ID,0x20010000u,12,500,&s->id_polls,&s->wfsys_id))return s->error;
 s->local_id=1;s->stage=NRPC_LOCAL_ID;

 /* The native placement callback must act on this exact prepared native owner.
  * Its complete receipt is checked before any AP2WF or CPU-release write. */
 s->stage=NRPC_ROM_PLACE;
 if(proof(o,c,s)||o->rom_place(c,rom)||!nrpc_receipt_ok(rom))return fail(s,-18);
 s->rom_placed=1;s->stage=NRPC_ROM_CONFIG;
 if(wr(o,c,s,AP2WF,0x89040000u))return s->error;
 if(wr(o,c,s,MCU_ENTRY,0))return s->error;
 if(wr(o,c,s,WF_ENTRY,0))return s->error;
 if(rmw(o,c,s,SEMA_RST,1u,0))return s->error;
 /* Receipt and reset-held state are checked again at the release boundary. */
 if(!nrpc_receipt_ok(rom)||!s->cpu_held||!s->ap_vote||!s->wf_vote||
    !s->clock_vote||!s->power||!s->protection||s->sem20)return fail(s,-19);
 s->stage=NRPC_CPU_RELEASE;
 if(rd(o,c,s,CPU_RST,&v))return s->error;
 if(v&1u)return fail(s,-20); /* still held at the final boundary */
 s->cpu_release_attempted=1;
 if(wr(o,c,s,CPU_RST,v|1u))return s->error;
 s->cpu_released=1;s->cpu_held=0;s->stage=NRPC_ROM_WAIT;
 if(version(o,c,s,ROM_DONE,0x1d1eu,102,1000,&s->rom_polls,&s->rom_done))return s->error;
 s->rom_ready=1;s->stage=NRPC_RUNNING_CONFIG;
 /* Selected conninfra_config_setup_mt6893 writes this literal after ROM_DONE. */
 if(wr(o,c,s,DEBUG_CTRL,0x32c8001cu))return s->error;
 s->stage=NRPC_RELEASE;
 if(readable(o,c,s))return s->error;
 if(clocks_off(o,c,s))return s->error;
 if(readable(o,c,s))return s->error;
 if(rmw(o,c,s,WF_WAKE,1u,0))return s->error;
 s->wf_vote=0;
 /* Final MMIO. The fresh AP vote ends after the selected WF vote release. */
 if(wr(o,c,s,AP_WAKE,0))return s->error;
 s->ap_vote=0;s->balanced=1;s->dirty=0;s->unverified=0;s->stage=NRPC_RUNNING;
 return 0;
}
