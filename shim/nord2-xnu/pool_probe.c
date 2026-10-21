#include "pool_probe.h"
#define AP 0x180601a0u
#define WF 0x180601a4u
#define ACK 0x18060184u
#define CFG 0x18001000u
#define CPU 0x18000010u
#define TOP 0x18000000u
#define PWR 0x180602ccu
#define PROT 0x18001620u
#define DMA_PROT 0x18001624u
#define LOCAL 0x184b0010u
#define ROM 0x184c1604u

static int fail(struct npp_state *s,int e){s->error=e;s->halt=!!(s->ap_owned||s->wf_owned||s->set_attempted||s->mapped||s->zero_attempted);s->uncertain=s->halt;return e;}
static int check(const struct npp_ops *o,void *c,struct npp_state *s){return o->proof(c,s->stage)?fail(s,-2):0;}
static int rd(const struct npp_ops *o,void *c,struct npp_state *s,uint32_t a,uint32_t *v){return check(o,c,s)||o->read(c,a,v)?fail(s,-3):0;}
static int wr(const struct npp_ops *o,void *c,struct npp_state *s,uint32_t a,uint32_t v){return check(o,c,s)||o->write(c,a,v)?fail(s,-4):0;}
static int wait_us(const struct npp_ops *o,void *c,struct npp_state *s,unsigned us){return check(o,c,s)||o->delay_us(c,us)?fail(s,-5):0;}
static int poll(const struct npp_ops *o,void *c,struct npp_state *s,uint32_t a,uint32_t mask,uint32_t want,unsigned limit,unsigned us,unsigned *count,uint32_t *last){
 for(unsigned i=0;i<limit;i++){if(i&&wait_us(o,c,s,us))return s->error;if(rd(o,c,s,a,last))return s->error;++*count;if((*last&mask)==want)return 0;}return fail(s,-6);
}
static int version(const struct npp_ops *o,void *c,struct npp_state *s,uint32_t a,uint32_t want,unsigned limit,unsigned us,unsigned *count,uint32_t *last){
 for(unsigned i=0;i<limit;i++){if(i&&us&&wait_us(o,c,s,us))return s->error;if(rd(o,c,s,a,last))return s->error;++*count;if(*last==want)return 0;}return fail(s,-7);
}
static int sleep_votes(const struct npp_ops *o,void *c,struct npp_state *s){
 uint32_t v;s->stage=NPP_RELEASE;
 if(rd(o,c,s,WF,&v))return s->error;
 if(!(v&1u)||v!=(s->wf_initial|1u))return fail(s,-8);
 if(wr(o,c,s,WF,v&~1u))return s->error;
 s->wf_owned=0;
 /* AP literal zero is the final MMIO; no further proof/readable/log MMIO. */
 if(wr(o,c,s,AP,0))return s->error;
 s->ap_owned=0;s->asleep=1;s->balanced=1;s->uncertain=0;s->halt=0;s->stage=NPP_DONE;return 0;
}
int npp_run(const struct npp_ops *o,void *c,const struct ndp_claim *claim,struct npp_state *s,unsigned opt){
 uint32_t v;struct ndp_smc_request req[2];struct ndp_subpool sub;
 if(!s||!claim||opt!=1||s->entered)return -1;
 __builtin_memset(s,0,sizeof(*s));s->entered=1;s->stage=NPP_PREWAKE;
 if(!o||!o->ready||!o->proof||!o->read||!o->write||!o->delay_us||
    !o->readable||!o->preflight||!o->smc||!o->map||!o->word||!o->barrier||
    ndp_region18_set_intent(claim,req)||ndp_subpool_layout(claim,&sub)||o->ready(c,claim))return fail(s,-1);
 s->claim_ready=1;
 if(rd(o,c,s,AP,&v))return s->error;
 if(v)return fail(s,-9);
 s->ap_owned=1;if(wr(o,c,s,AP,1))return s->error;
 s->stage=NPP_AP_ACK;
 if(poll(o,c,s,ACK,1u<<5,0,12,1000,&s->ap_polls,&v))return s->error;
 if(check(o,c,s)||o->readable(c))return fail(s,-10);
 s->stage=NPP_AP_READY;
 if(version(o,c,s,CFG,0x20010101u,10,0,&s->cfg_polls,&s->cfg_id))return s->error;
 if(rd(o,c,s,WF,&v))return s->error;
 if(v&1u)return fail(s,-11);
 s->wf_initial=v;s->wf_owned=1;if(wr(o,c,s,WF,v|1u))return s->error;
 s->stage=NPP_WF_ACK;
 if(poll(o,c,s,ACK,1u<<5,0,12,1000,&s->wf_polls,&v))return s->error;
 if(check(o,c,s)||o->readable(c))return fail(s,-10);
 s->stage=NPP_LIVE;
 if(version(o,c,s,CFG,0x20010101u,12,1000,&s->cfg_polls,&s->cfg_id))return s->error;
 /* Running ROM remains untouched. Prove release, power, protection and both IDs. */
 const uint32_t addr[5]={CPU,TOP,PWR,PROT,DMA_PROT};
 for(unsigned i=0;i<5;i++){if(rd(o,c,s,addr[i],&s->live[i]))return s->error;s->live_valid|=1u<<i;}
 if(!(s->live[0]&1u)||!(s->live[1]&0x80u)||!(s->live[2]&4u)||
    (s->live[3]&9u)||(s->live[4]&9u))return fail(s,-12);
 if(version(o,c,s,LOCAL,0x20010000u,12,500,&s->local_polls,&s->local_id))return s->error;
 if(version(o,c,s,ROM,0x1d1eu,12,1000,&s->rom_polls,&s->rom_done))return s->error;
 s->stage=NPP_PREFLIGHT;
 if(check(o,c,s))return s->error;
 int pre=o->preflight(c,&s->dma);
 if(pre<0||!s->dma.complete)return fail(s,-13);
 s->preflight_complete=1;
 if(pre==1){s->inherited_dma=1;if(sleep_votes(o,c,s))return s->error;return 1;}
 if(pre!=0||!s->dma.observed_empty||!s->dma.stable)return fail(s,-13);
 for(unsigned g=0;g<2;g++){
  s->stage=g?NPP_SET0:NPP_SET1;s->set_attempted|=1u<<g;s->possibly_locked=1;
  if(check(o,c,s)||o->smc(c,req[g].x,s->raw[g]))return fail(s,-14);
  s->raw_valid|=1u<<g;if(s->raw[g][0])return fail(s,-15);s->set_accepted++;
 }
 if(s->set_accepted!=2||s->raw_valid!=3)return fail(s,-15);
 s->stage=NPP_MAP;
 if(check(o,c,s)||o->map(c,claim->base,claim->bytes))return fail(s,-16);
 s->mapped=1;s->stage=NPP_ZERO;s->zero_attempted=1;
 for(uint32_t off=0;off<NDP_SUBPOOL_BYTES;off+=4){v=0;
  if(check(o,c,s)||o->word(c,off,&v,1))return fail(s,-17);
  s->words_written++;
 }
 if(check(o,c,s)||o->barrier(c))return fail(s,-18);
 s->zero_complete=1;s->stage=NPP_VERIFY;
 for(uint32_t off=0;off<NDP_SUBPOOL_BYTES;off+=4){v=0;
  if(check(o,c,s)||o->word(c,off,&v,0))return fail(s,-19);
  s->words_read++;if(v)return fail(s,-20);
 }
 if(check(o,c,s)||o->barrier(c))return fail(s,-18);
 s->verified=1;return sleep_votes(o,c,s);
}
