#if defined(KERNEL)
#include <pexpert/arm64/board_config.h>
#endif
#if defined(NORD2)
/* Selected MT6893 I2Cv2 PIO derivative, GPL-2.0 source contract documented. */
#include "mtk_i2c_read.h"
static int allowed(unsigned b,unsigned s,unsigned cmd,unsigned n){
 if(b==7&&s==0x55&&n==2)return cmd==0x2c||cmd==8||cmd==6||cmd==0x0c;
 return b==11&&s==0x5c&&n==1&&(cmd==0x13||cmd==0x14);
}
int nbg_i2c_read(const struct nbg_i2c_io *o,void *c,struct nbg_i2c_state *s,
 unsigned bus,unsigned slave,unsigned cmd,unsigned count,uint16_t *value){
 if(!o||!o->read||!o->write||!o->delay_us||!s||s->offline||s->inflight||!value||!allowed(bus,slave,cmd,count))return -1;
 uint32_t base=bus==7?0x11d03000:0x1101a000,gate=bus==7?0x11d05000:0x11008000;
 uint32_t bit=bus==7?8:4,v,oldgate=0;unsigned touched=0,gate_changed=0;
 const uint32_t offs[]={4,8,0x10,0x14,0x18,0x1c,0x20,0x28,0x2c,0x30,0x34,0x40,0x44,0x48};
 /* cnt_constraint1/div5/26M ->sample0/step26; v2timeout bit0. */
 uint32_t next[]={0,0,0x3a,1,2,10,0x1b,0x8001,0x1a,0,3,1,0,0x404};
 uint32_t old[14];s->entered=1;s->polls=0;s->error=-1;
 s->stage=1;s->failure=s->observed=s->last_valid=0;
 s->mux=s->parent_hz=s->pinmux=s->initial_gate=s->completion_irq=0;s->prior_irq=s->prior_ack=0;
#define R(pa,w,out) do{s->last_addr=(pa);s->last_width=(w);s->last_write=0;s->last_valid=0;s->last_value=0;if(o->read(c,pa,w,out)){s->failure=10;goto unknown;}s->last_value=*(out);s->last_valid=1;}while(0)
#define W(pa,w,val) do{s->last_addr=(pa);s->last_width=(w);s->last_write=1;s->last_value=(val);s->last_valid=1;if(o->write(c,pa,w,val)){s->failure=11;goto unknown;}}while(0)
 /* Read selected source PLL/mux, never retune it, before controller access. */
 uint32_t mux,mux_after,parent=26000000;
 R(0x100000b0,4,&mux);s->mux=mux;s->observed|=1;if(mux&0x8000){s->failure=1;return -1;}
 unsigned select=(mux>>8)&3;if(select==3){s->failure=1;return -1;}
 s->stage=2;
 if(select){
  uint32_t pll=select==1?0x1000c340:0x1000c308,con,pcw,again,power;
  R(pll,4,&con);R(pll+4,4,&pcw);R(pll+12,4,&power);R(pll+4,4,&again);
  if(pcw!=again||!(con&1)||!(con&0x800000)||(power&3)!=1){s->failure=2;return -1;}
  unsigned div=1u<<((pcw>>24)&7);uint64_t hz=((uint64_t)26000000*(pcw&0x3fffff)+16383)>>14;
  hz=(hz+div-1)/div;hz/=select==1?32:20;
  if(hz<1000000||hz>250000000){s->failure=2;return -1;}
  parent=(uint32_t)hz;
 }
 s->parent_hz=parent;s->observed|=2;
 R(0x100000b0,4,&mux_after);if(mux!=mux_after){s->failure=1;return -1;}
 s->stage=3;
 /* Selected source speed calculator,100kHz,div5,cnt_constraint1. */
 unsigned hclk=(parent/5)/1000,min_div=((hclk>>1)+99)/100,best=512,sample=0,step=0;
 for(unsigned a=1;a<=8;a++){
  unsigned b=(min_div+a-1)/a;if(!b||b>64)continue;
  if(a*b<best){best=a*b;sample=a;step=b;if(best==min_div)break;}
 }
 if(!sample||!step||hclk/(2*sample*step)>100){s->failure=3;return -1;}
 sample--;if(sample)step--;
 next[6]=(sample<<8)|step|1;next[8]=(sample<<6)|step;
 s->stage=4;R(0x100053f0,4,&v);s->pinmux=v;s->observed|=4;
 if(bus==7){if((v&0xff0000)!=0x110000){s->failure=4;return -1;}}
 else {if((v&0xff000000)!=0x55000000){s->failure=4;return -1;}} /*126/127sourceSCL11/SDA11*/
 s->stage=5;R(gate+0xe00,4,&oldgate);s->initial_gate=oldgate;s->observed|=8;
 if(oldgate&bit){gate_changed=1;W(gate+0xe04,4,bit);R(gate+0xe00,4,&v);if(v&bit){s->failure=5;goto unknown;}}
 s->stage=6;R(base+0x24,2,&v);if(v&3){s->failure=6;goto unknown;}
 R(base+0x10,2,&v);if(v&4){s->failure=7;goto unknown;} /* never touch inheritedDMA */
 R(base+0x0c,2,&v);s->prior_irq=v;
 if(v!=0&&v!=1){s->failure=8;goto unknown;} /* preserve errors/unknown status */
 R(base+0xf4,2,&v);if(v){s->failure=9;goto unknown;} /* conservative: no inherited FIFO state */
 /* Source clears latched TRANSAC_COMP before a new transfer. Adopt only
  * exact completion, with no inherited START/DMA/FIFO and stable status.
  * This is AP-local boot handoff; no concurrent bus owner is permitted. */
 if(s->prior_irq==1){
  R(base+0x24,2,&v);if(v&3){s->failure=6;goto unknown;}
  R(base+0x10,2,&v);if(v&4){s->failure=7;goto unknown;}
  R(base+0xf4,2,&v);if(v){s->failure=9;goto unknown;}
  R(base+0x0c,2,&v);if(v!=1){s->failure=8;goto unknown;}
  W(base+0x0c,2,1);s->prior_ack=1;
  R(base+0x0c,2,&v);if(v){s->failure=8;goto unknown;}
 }
 s->stage=7;for(unsigned i=0;i<14;i++)R(base+offs[i],2,&old[i]);
 s->stage=8;for(unsigned i=0;i<14;i++){
  uint32_t n=next[i];if(i==0)n=slave<<1;if(i==12)n=count;
  touched=i+1;W(base+offs[i],2,n);
 }
 s->stage=9;W(base+0x38,2,1); /*sourceFIFOclear; preconditionFIFOempty*/
 W(base,1,cmd); /* only target write is SMBus read command pointer */
 s->stage=10;s->inflight=1;W(base+0x24,2,1);
 s->stage=11;
 for(unsigned i=0;i<2000;i++){
  R(base+0x0c,2,&v);s->polls++;s->completion_irq=v;s->observed|=16;
  if(v&0x1ff)break;
  if(o->delay_us(c,10)){s->failure=12;goto unknown;}
 }
 if(!(v&1)||(v&0x1fe)){s->failure=(v&0x1fe)?14:13;goto unknown;}
 s->stage=12;R(base+0x24,2,&v);if(v&3){s->failure=6;goto unknown;}
 /* Known complete, no error: source reads exactly requested FIFO bytes. */
 uint32_t lo,hi=0;R(base,1,&lo);if(count==2)R(base,1,&hi);
 if(lo>255||hi>255){s->failure=15;goto unknown;}
 *value=(uint16_t)(lo|(hi<<8));s->error=0;s->inflight=0;
 s->stage=13;W(base+0x0c,2,1);W(base+0x38,2,1);
 /* No command replay, START restore, reset, DMA or forceclock access. */
 s->stage=14;while(touched){unsigned i=--touched;W(base+offs[i],2,old[i]);}
 s->stage=15;if(gate_changed){W(gate+0xe08,4,bit);}
 s->stage=16;return s->error;
unknown:
 /* Even failed write may have committed; retain setup/clock, never resetbus.
  * No future attempt on this bus until explicit independently reviewed reset. */
 s->offline=1;s->error=-2;return -1;
#undef R
#undef W
}

#endif /* NORD2 */
