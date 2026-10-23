#include <assert.h>
#include <stdio.h>
#include "audio_i2c8.h"
#include "test-i2c-model.h"
static int readio(void *c,uint32_t pa,unsigned w,uint32_t *v) { *v=m_read(c,pa,w);return 0; }
static int writeio(void *c,uint32_t pa,unsigned w,uint32_t v) { m_write(c,pa,w,v);return 0; }
static int delayio(void *c,unsigned us) { (void)c;assert(us<=10);return 0; }
static const struct na8_io io={readio,writeio,delayio};
int main(void)
{
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
