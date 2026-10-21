#include <assert.h>
#include <string.h>
#include "mtk_i2c_read.h"
struct mock {unsigned calls,fail,failed,after,start,data,gate,gated,started,timeout,error,selector,prior,acked,start_reads,dma_reads,fifo_reads,irq_reads,busy_second,dma_second,fifo_second,prior_fifo,changing,sticky;uint32_t regs[1024];};
static int tick(struct mock *m){if(m->failed)m->after++;if(++m->calls==m->fail){m->failed=1;return -1;}return 0;}
static int read(void *p,uint32_t a,unsigned width,uint32_t *v){struct mock*m=p;if(tick(m))return -1;
 *v=0;
 if(a==0x100000b0)*v=m->selector<<8;
 else if(a==0x1000c308)*v=0x800001;
 else if(a==0x1000c30c)*v=96u<<14; /*26M*96=2496M*/
 else if(a==0x1000c314)*v=1;
 else if(a==0x100053f0)*v=0x55110000;
 else if(a==0x11d05e00||a==0x11008e00)*v=m->gate;
 else if((a&0xfff)==0xc){m->irq_reads++;*v=m->started?(m->timeout?0:m->error?2:1):((m->acked&&!m->sticky)?0:((m->changing&&m->irq_reads==2)?2:m->prior));}
 else if((a&0xfff)==0x24)*v=(++m->start_reads==2&&m->busy_second)?1:0;
 else if((a&0xfff)==0x10)*v=(++m->dma_reads==2&&m->dma_second)?4:m->regs[4];
 else if((a&0xfff)==0xf4)*v=m->prior_fifo|((++m->fifo_reads==2&&m->fifo_second)?1:0);
 else if(a==0x11d03000||a==0x1101a000){assert(width==1);*v=(m->data++==0)?43:0;}
 else *v=m->regs[(a&0xfff)/4];
 return 0;
}
static int write(void *p,uint32_t a,unsigned width,uint32_t v){struct mock*m=p;
 /* Model potential commit before failing callback return. */
 if(a==0x11d05e04||a==0x11008e04){m->gate&=~v;m->gated++;}
 else if(a==0x11d05e08||a==0x11008e08)m->gate|=v;
 else if((a&0xfff)==0xc){assert(v==1);m->acked++;}
 else if((a&0xfff)==0x24){assert(v==1);m->started=1;m->start++;}
 else if(a==0x11d03000||a==0x1101a000){assert(width==1&&v==0x2c);}
 else if((a&0xfff)!=0xc&&(a&0xfff)!=0x38)m->regs[(a&0xfff)/4]=v;
 return tick(m);
}
static int delay(void*p,unsigned n){assert(n==10);return tick(p);}
static const struct nbg_i2c_io io={read,write,delay};
int main(void){struct nbg_i2c_state s={0};struct mock m={.gate=8};uint16_t value=0;
 assert(!nbg_i2c_read(&io,&m,&s,7,0x55,0x2c,2,&value));assert(value==43&&m.start==1&&m.gate==8&&!s.inflight&&!s.offline);
 unsigned total=m.calls;assert(total==62);
 assert(s.stage==16&&!s.failure&&s.observed==31&&s.parent_hz==26000000&&s.pinmux==0x55110000&&s.initial_gate==8&&s.completion_irq==1&&s.last_write&&s.last_addr==0x11d05e08);
 for(unsigned f=1;f<=total;f++){
  s=(struct nbg_i2c_state){0};m=(struct mock){.gate=8,.fail=f};
  assert(nbg_i2c_read(&io,&m,&s,7,0x55,0x2c,2,&value)&&s.offline&&!m.after);
  unsigned count=m.calls;assert(nbg_i2c_read(&io,&m,&s,7,0x55,0x2c,2,&value)&&m.calls==count);
 }
 s=(struct nbg_i2c_state){0};m=(struct mock){.gate=8,.selector=2};assert(!nbg_i2c_read(&io,&m,&s,7,0x55,0x2c,2,&value));
 s=(struct nbg_i2c_state){0};m=(struct mock){.gate=8,.timeout=1};assert(nbg_i2c_read(&io,&m,&s,7,0x55,0x2c,2,&value)&&s.offline&&s.inflight&&s.polls==2000&&m.gate==0&&s.failure==13&&s.stage==11&&s.completion_irq==0);
 s=(struct nbg_i2c_state){0};m=(struct mock){.gate=8,.error=1};assert(nbg_i2c_read(&io,&m,&s,7,0x55,0x2c,2,&value)&&s.offline);
 s=(struct nbg_i2c_state){0};m=(struct mock){0};assert(nbg_i2c_read(&io,&m,&s,7,0x55,0x3e,2,&value)&&!m.calls);
 /* Boot-inherited pure completion is acknowledged once only after full idle proof. */
 s=(struct nbg_i2c_state){0};m=(struct mock){.prior=1,.gate=8};
 assert(!nbg_i2c_read(&io,&m,&s,7,0x55,0x2c,2,&value));
 assert(m.calls==68&&s.prior_irq==1&&s.prior_ack==1&&m.acked==2&&m.start==1&&value==43);
 total=m.calls;
 for(unsigned f=1;f<=total;f++){
  s=(struct nbg_i2c_state){0};m=(struct mock){.prior=1,.gate=8,.fail=f};
  assert(nbg_i2c_read(&io,&m,&s,7,0x55,0x2c,2,&value)&&s.offline&&!m.after);
  unsigned calls=m.calls;assert(nbg_i2c_read(&io,&m,&s,7,0x55,0x2c,2,&value)&&m.calls==calls);
 }
 for(unsigned prior=2;prior<1024;prior++){
  s=(struct nbg_i2c_state){0};m=(struct mock){.prior=prior};
  assert(nbg_i2c_read(&io,&m,&s,7,0x55,0x2c,2,&value)&&!m.acked&&!m.start&&s.failure==8);
 }
 for(unsigned kind=0;kind<5;kind++){
  s=(struct nbg_i2c_state){0};m=(struct mock){.prior=1,.busy_second=kind==0,.dma_second=kind==1,.prior_fifo=kind==2,.fifo_second=kind==3,.changing=kind==4};
  assert(nbg_i2c_read(&io,&m,&s,7,0x55,0x2c,2,&value)&&s.offline&&!m.acked&&!m.start);
 }
 s=(struct nbg_i2c_state){0};m=(struct mock){.prior=1,.sticky=1};
 assert(nbg_i2c_read(&io,&m,&s,7,0x55,0x2c,2,&value)&&s.offline&&m.acked==1&&!m.start&&s.failure==8);
 /* Actual captured raw status and all nonzero count mutations. */
 for(unsigned irq=0;irq<=1;irq++){
  s=(struct nbg_i2c_state){0};m=(struct mock){.prior=irq,.prior_fifo=0x840};
  assert(!nbg_i2c_read(&io,&m,&s,7,0x55,0x2c,2,&value)&&m.start==1&&value==43);
  for(unsigned count=1;count<=31;count++){
   s=(struct nbg_i2c_state){0};m=(struct mock){.prior=irq,.prior_fifo=0x840|count};
   assert(nbg_i2c_read(&io,&m,&s,7,0x55,0x2c,2,&value)&&s.failure==9&&!m.start&&!m.acked);
  }
 }
 return 0;
}
