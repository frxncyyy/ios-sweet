/* Private bounded derivative of selected MT6893 GPL-2.0 wake path,
 * source04d00c25 mt6893_pos.c468 / consys_reg_util.h90. */
#include "wifi_wake.h"
struct ctx {const struct naw_ops *o;void *c;struct naw_state *s;};
static int proof(struct ctx *x){return x->o->proof(x->c);}
static int read(struct ctx *x,uint32_t a,uint32_t *v){return proof(x)||x->o->read(x->c,a,v);}
static int write(struct ctx *x,uint32_t a,uint32_t v){return proof(x)||x->o->write(x->c,a,v);}
static int wait(struct ctx *x,unsigned us){return proof(x)||x->o->delay_us(x->c,us);}
#define TRY(call) do{if(call){s->error=-(int)s->phase;goto fail;}}while(0)
int naw_run(const struct naw_ops *o,void *c,unsigned opt,struct naw_state *s){
 if(opt!=1||!s||s->entered)return -1;
 if(!o||!o->proof||!o->read||!o->write||!o->delay_us)return -1;
 struct ctx x={o,c,s};uint32_t v;unsigned count=0;
 s->entered=1;s->phase=1;
 TRY(read(&x,0x180601a0,&v));
 /* Unknown/another AP vote is not ours. Clean refusal, no write. */
 if(v){s->error=-100;return -1;}
 s->phase=2;
 TRY(proof(&x));
 s->reference=s->retained=1; /* BEFORE write could commit */
 if(o->write(c,0x180601a0,1)){s->error=-2;goto fail;}
 /* ACKpoll source macro: initialread + max11 reread/delay1000 steps.
  * Diagnostic stops on first timeout instead of source retry/debug dumps. */
 TRY(read(&x,0x18060184,&v));
 while(v&0x20){
  if(count>10){s->error=-101;goto fail;}
  TRY(read(&x,0x18060184,&v));TRY(wait(&x,1000));
  count++;s->ack_polls++;
 }
 s->phase=3;
 /* Exact source readability clock measurement: reset on EACH retry, source
  * spent<16000us/retry1600. Deterministic10us retry replaces randomjitter. */
 unsigned spent=0,delay_us=1;
 while(s->clock_polls<1600&&spent<16000){
  TRY(read(&x,0x18060000,&v));TRY(write(&x,0x18060000,v|1));
  TRY(wait(&x,delay_us));spent+=delay_us;
  TRY(read(&x,0x18060000,&s->clock));s->clock_polls++;
  if((s->clock&6)==6)break;
  delay_us=10;
 }
 if((s->clock&6)!=6){s->error=-102;goto fail;}
 TRY(read(&x,0x180602d4,&s->irq));
 if(!(s->irq&1)){s->error=-103;goto fail;}
 TRY(read(&x,0x18060184,&v));if(v&0x20){s->error=-104;goto fail;}
 s->awake=1;s->phase=4;
 for(unsigned i=0;i<10;i++){
  TRY(read(&x,0x18001000,&s->chip));s->id_polls++;
  if(s->chip==0x20010101)break;
 }
 if(s->chip!=0x20010101){s->error=-105;goto fail;}
 s->phase=5;
 const uint32_t address[5]={0x18000010,0x18000000,0x180602cc,0x18000014,0x18000008};
 const uint32_t mask[5]={1,0x80,4,1,0x80};
 for(unsigned i=0;i<5;i++){
  TRY(read(&x,address[i],&s->cold[i]));s->cold_valid|=1u<<i;
  if(s->cold[i]&mask[i]){s->error=-106;goto fail;}
 }
 s->phase=6;
 TRY(proof(&x));
 /* Last source command. No callbacks/readback/proof after sleep write.
  * Keep reference until ordered callback confirms command completed. */
 if(o->write(c,0x180601a0,0)){s->error=-6;goto fail;}
 s->reference=s->retained=s->awake=0;s->balanced=1;s->phase=7;
 return 0;
fail:
 s->halt=s->retained||s->reference;return -1;
}
