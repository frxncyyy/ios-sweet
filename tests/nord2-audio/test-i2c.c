#include <assert.h>
#include <stdio.h>
#include "audio_i2c8.h"
#include "audio_id.h"
#include "test-i2c-model.h"
static unsigned io_writes,start_reads,race_read;
static int readio(void *c,uint32_t pa,unsigned w,uint32_t *v) {
 if(pa==0x11d04124 && ++start_reads==race_read)((struct i2c8_model *)c)->ch[1][0x24/2]|=1u;
 *v=m_read(c,pa,w);return 0;
}
static int writeio(void *c,uint32_t pa,unsigned w,uint32_t v) { io_writes++;m_write(c,pa,w,v);return 0; }
static int delayio(void *c,unsigned us) { (void)c;assert(us<=10);return 0; }
static const struct na8_io io={readio,writeio,delayio};
static void fresh(struct i2c8_model *m,struct na8_state *s) {
 m_init(m);*s=(struct na8_state){0};io_writes=start_reads=race_read=0;
 m->ch[1][0x24/2]=2; /* Measured on Nord2 7d2ca8f9, not an idle-zero fixture. */
}
static void test_start_readback(void) {
 struct i2c8_model m;struct na8_state s;uint16_t v=0;
 fresh(&m,&s);
 for(unsigned i=0;i<3;i++) {
  assert(!na8_read16(&io,&m,&s,0x34,0x00,&v));
  assert(!na8_write16(&io,&m,&s,0x34,0x00,(uint16_t)(v^8u)));
  assert(!s.uncertain && m.ch[1][0x24/2]==2);m_check_restored(&m);
 }
 assert(m.starts==6 && !m.slave[5].accessed && !m.slave[6].accessed);
 /* Exact inherited completion1 is acknowledged, not a global reset. */
 fresh(&m,&s);m.ch[1][0x0c/2]=1;
 assert(!na8_read16(&io,&m,&s,0x34,0x03,&v) && v==0x0b74 && s.prior_ack);
 assert(m.ch[1][0x24/2]==2);m_check_restored(&m);
 /* AP changes to busy on the second check: refuse before W1C/setup. */
 fresh(&m,&s);m.ch[1][0x0c/2]=1;race_read=2;
 assert(na8_read16(&io,&m,&s,0x34,0x03,&v)==-1 && s.failure==6);
 assert(!io_writes && !m.starts && !s.uncertain);
 fresh(&m,&s);m.ch[1][0x0c/2]=1;m.irq_sticky=1;
 assert(na8_read16(&io,&m,&s,0x34,0x03,&v)==-2 && s.uncertain && !m.starts);
 /* Bit0 remains busy for both initial values1/3. Unowned channel guards
  * remain unchanged, including their conservative RESUME bit refusal. */
 for(unsigned busy=1;busy<=3;busy+=2) {
  fresh(&m,&s);m.ch[1][0x24/2]=(uint16_t)busy;
  assert(na8_read16(&io,&m,&s,0x34,0x03,&v)==-1 && s.failure==6);
  assert(!io_writes && !m.starts && !s.uncertain);
 }
 for(unsigned channel=0;channel<3;channel+=2)for(unsigned busy=1;busy<=3;busy++) {
  fresh(&m,&s);m.ch[channel][0x24/2]=(uint16_t)busy;
  assert(na8_read16(&io,&m,&s,0x34,0x03,&v)==-1);
  assert(s.failure==(channel==0?18u:17u) && !io_writes && !m.starts);
 }
 /* The later guards stay active with START2. No rail, reset, shadow or
  * arbitration write can pass the strict model's callback allowlist. */
 for(unsigned fault=0;fault<6;fault++) {
  fresh(&m,&s);
  if(fault==0)m.ch[1][0x10/2]|=4;
  if(fault==1)m.ch[1][0x0c/2]=2;
  if(fault==2)m.ch[1][0xf4/2]=1;
  if(fault==3)m.ch[1][0xec/2]=0x8000;
  if(fault==4){m.ifr1=m.ifr2=0;m.dma[8/4]=1;}
  if(fault==5)m.ch[2][0x10/2]=4;
  assert(na8_read16(&io,&m,&s,0x34,0x03,&v)==-1);
  assert(!io_writes && !m.starts && !s.uncertain);
 }
 fresh(&m,&s);m.slave[0].nack=1;
 assert(na8_read16(&io,&m,&s,0x34,0x03,&v)==1 && !s.uncertain);
 assert(m.ch[1][0x24/2]==2);m_check_restored(&m);
 assert(!na8_read16(&io,&m,&s,0x37,0x03,&v) && v==0x0a74);
 for(unsigned nack=0;nack<2;nack++) {
  fresh(&m,&s);m.start_stuck=1;m.slave[0].nack=(int)nack;
  assert(na8_read16(&io,&m,&s,0x34,0x03,&v)==-2 && s.failure==6 && s.uncertain);
  assert(m.ch[1][0x24/2]==3 && m.starts==1);
  assert(na8_read16(&io,&m,&s,0x37,0x03,&v)==-1 && m.starts==1);
 }
 fresh(&m,&s);m.behaviour=M_HANG;
 assert(na8_read16(&io,&m,&s,0x34,0x03,&v)==-2 && s.failure==13 && s.uncertain);
 assert(m.ch[1][0x24/2]==3);
 /* Pure readiness diagnostics must agree with the transport. */
 struct nau_state n={0};n.snap_read=NAU_SNAP_COUNT;n.i2c8_read=NAU_I2C8_COUNT;
 n.snap[NAU_S_MODE_120]=0x1100;n.snap[NAU_S_BM_IES122]=0x2008;n.snap[NAU_S_DIN_96]=0x0c000000;
 n.i2c8[NAU_I2C8_INDEX(NAU_CH_AP,NAU_C_START)]=2;
 assert(!nau_i2c8_ready(&n));
 n.i2c8[NAU_I2C8_INDEX(NAU_CH_AP,NAU_C_START)]=3;
 assert(nau_i2c8_ready(&n)==NAU_READY_START);
 puts("PASS AP START2 across read/write/NACK/adoption, START1/3 busy and races, sticky error, all unowned/DMA/FIFO/I3C guards");
}
int main(void)
{
 test_start_readback();race_read=0;
 struct i2c8_model m; struct na8_state s={0}; uint16_t v=0;
 m_init(&m);
 assert(na8_read16(&io,&m,&s,0x34,0x00,&v)==0);
 assert(na8_write16(&io,&m,&s,0x34,0x00,9)==0);
 assert(m.slave[0].reg[0x00]==9 && m.starts==2);
 m_check_restored(&m);
 assert(na8_read16(&io,&m,&s,0x34,0x00,&v)==0 && v==9);
 m_check_restored(&m);
 assert(m.slave[5].accessed==0 && m.slave[6].accessed==0);
 for (unsigned i=0;i<m.ntrans;i++) assert(m.trans[i].addr==0x34);
 /* Both DDR/UFS bucks, unknown slave, OTP programming and reset rejected
  * before even a clock/controller read or write. */
 unsigned starts=m.starts;
 assert(na8_write16(&io,&m,&s,0x51,0x00,1)==-1 && m.starts==starts);
 s=(struct na8_state){0}; assert(na8_write16(&io,&m,&s,0x56,0x00,1)==-1);
 s=(struct na8_state){0}; assert(na8_write16(&io,&m,&s,0x34,0xa3,0x10)==-1);
 s=(struct na8_state){0}; assert(na8_write16(&io,&m,&s,0x34,0x09,0xffff)==-1);
 assert(m.starts==starts);
 /* Clean write NACK restores controller; second amplifier is still usable. */
 m_init(&m);m.slave[0].nack=1;s=(struct na8_state){0};
 assert(na8_read16(&io,&m,&s,0x34,0x00,&v)==1);
 /* Use exact siliconinit write for clean-writeNACK test. */
 assert(na8_write16(&io,&m,&s,0x34,0x02,0x22a8)==1);
 assert(!s.uncertain && !s.offline && na8_return_ok(&s));m_check_restored(&m);
 assert(na8_read16(&io,&m,&s,0x37,0x01,&v)==0);
 assert(na8_write16(&io,&m,&s,0x37,0x01,(uint16_t)(v|4))==0);m_check_restored(&m);
 /* Bus-error after START is sticky; no retries, reset, next-slave transfer. */
 m_init(&m);m.behaviour=M_BUSERR;s=(struct na8_state){0};
 assert(na8_write16(&io,&m,&s,0x34,0x02,0x22a8)==-2 && s.uncertain);
 starts=m.starts;
 assert(na8_write16(&io,&m,&s,0x37,0x00,1)==-1 && m.starts==starts);
 /* Source V2 I3C mode gate is 0xec, not similarly named DEBUGSTAT0xe4. */
 m_init(&m);s=(struct na8_state){0};m.ch[1][0xec/2]=0x8000;
 assert(na8_write16(&io,&m,&s,0x34,0x02,0x22a8)==-1 && m.starts==0);
 puts("PASS: narrow three-byte amp writes, readback endian, bystanders untouched, configuration restore, clean NACK, sticky bus fault, V2 I3C gate");
}
