/* Scoped MT6893 A-die CHIP_ID probe, called only inside the owned common
 * transaction. Reuses its access proofs and persistent dirty latch. */
#include "wifi_common_scope.h"
#include "wifi_adie_strap.h"
static struct {vm_offset_t sem;bool retained;struct nis_result result;
 uint32_t clock_raw,irq_raw;unsigned clock_polls;bool clock_attempted,clock_ok;} nis_native;
static int nis_native_check(void *c)
{
 (void)c;if(ncc_ap_check())return -1;
 if(nis_native.retained&&ncc_native.result.verified!=NCC_COUNT)return -1;
 /* Same cold request/power proof, EXCEPT SPIbusy may be ours in-flight. */
 for(unsigned i=0;i<13;i++){
  if(ncc_ap_check())return -1;
  vm_offset_t b=ncc_aperture(ncc_predicates[i].address);if(!b)return -1;
  if(ni_rd(b,ncc_predicates[i].address&0xfff)&ncc_predicates[i].mask)return -1;
 }
 return 0;
}
static int nis_read(void *c,uint32_t a,uint32_t *v)
{
 (void)c;if(!v||ncc_ap_check())return -1;
 if(a==0x10005050||a==0x10005150||a==0x10005460){*v=ni_rd(ni_gpio,a-0x10005000);return 0;}
 if(a==0x18001900){*v=ni_rd(ncc_native.cfg,0x900);return 0;}
 return -1;
}
static int nis_write_mask(void *c,uint32_t a,uint32_t mask,uint32_t v)
{
 (void)c;unsigned i;
 for(i=0;i<4;i++)if(nis_fields[i].address==a)break;
 if(i==4||mask!=nis_fields[i].mask||(v&~mask)||ncc_ap_check())return -1;
 if(v!=nis_fields[i].value&&v!=(nis_native.result.before[i]&mask))return -1;
 if(a==0x10005050||a==0x10005150){
  /* SET/CLR aliases prevent writes to unrelated GPIO fields. */
  ni_wr(ni_gpio,a-0x10005000+(v?4:8),mask);return 0;
 }
 vm_offset_t b=a==0x10005460?ni_gpio:ncc_native.cfg;
 unsigned off=a==0x10005460?0x460:0x900;
 uint32_t old=ni_rd(b,off);ni_wr(b,off,(old&~mask)|v);return 0;
}
static int nis_barrier(void *c){(void)c;if(ncc_ap_check())return -1;NCC_DEVICE_BARRIER();return 0;}
static const struct nis_ops nis_ops={nis_native_check,nis_read,nis_write_mask,nis_barrier};
static int nis_id_check(void *c,unsigned required)
{
 if(required!=NAI_REQUIRED||!nis_native.retained||nis_native_check(c))return -1;
 for(unsigned i=0;i<4;i++){
  uint32_t v;if(nis_read(c,nis_fields[i].address,&v))return -1;
  if(v!=((nis_native.result.before[i]&~nis_fields[i].mask)|nis_fields[i].value))return -1;
 }
 return 0;
}
/* Selected consys_check_reg_readable clockdetect/IRQ portion. Source resets
 * detector by settingHOST0bit0; bits1/2 report offbus+osc clocks. This is a
 * measurement trigger, not a restorable power/clockconfiguration register. */
static int nis_clock_probe(void *c)
{
 nis_native.clock_attempted=true;nis_native.clock_ok=false;nis_native.clock_polls=0;
 nis_native.clock_raw=nis_native.irq_raw=0;
 if(nis_id_check(c,NAI_REQUIRED))return -1;
 uint32_t old=ni_rd(ncc_native.host,0);
 ni_wr(ncc_native.host,0,old|1u); /* same source-preserving SET_BIT */
 for(unsigned i=0;i<1600;i++){
  if(nis_id_check(c,NAI_REQUIRED))return -1;
  delay(i?10:1);
  nis_native.clock_raw=ni_rd(ncc_native.host,0);nis_native.clock_polls++;
  if((nis_native.clock_raw&6)==6)break;
 }
 if((nis_native.clock_raw&6)!=6||nis_id_check(c,NAI_REQUIRED))return -1;
 nis_native.irq_raw=ni_rd(ncc_native.host,0x2d4);
 if(!(nis_native.irq_raw&1))return -1;
 nis_native.clock_ok=true;return 0;
}
static int nis_acquire(void *c,unsigned index)
{
 if(index!=11||!nis_native.sem)return -1;
 if(nis_clock_probe(c))return 1; /* no semaphore acquisition attempted */
 /* Source semaphore acquisition is a side-effecting READ, not a plain
  * ownership observation. Do not read it during snapshots or cleanup. */
 for(unsigned i=0;i<1000;i++){
  if(nis_id_check(c,NAI_REQUIRED))return 1; /* no acquired status observed */
  if(ni_rd(nis_native.sem,0x2c)&1)return 0;
  delay(1);
 }
 return 1;
}
static int nis_release(void *c,unsigned index)
{
 if(index!=11||!nis_native.sem||nis_id_check(c,NAI_REQUIRED))return -1;
 ni_wr(nis_native.sem,0x22c,1);return 0;
}
static int nis_id_read(void *c,uint32_t a,uint32_t *v)
{
 (void)c;if(!v||ncc_ap_check())return -1;
 if(a!=0x18004000&&a!=0x18004050&&a!=0x18004058)return -1;
 *v=ni_rd(ncc_native.spi,a&0xfff);return 0;
}
static int nis_id_write(void *c,uint32_t a,uint32_t v)
{
 (void)c;if(ncc_ap_check())return -1;
 if(a==0x18004050){if(v!=0xb02c&&v!=nis_native.result.id.original_address)return -1;}
 else if(a!=0x18004054||v)return -1;
 ni_wr(ncc_native.spi,a&0xfff,v);return 0;
}
static int nis_delay(void *c,unsigned usec){(void)c;if(usec!=50||ncc_ap_check())return -1;delay(usec);return 0;}
static const struct nai_ops nis_id_ops={nis_id_check,nis_acquire,nis_release,nis_id_read,nis_id_write,nis_barrier,nis_delay};
static int nis_inner(void *c,int *clean)
{
 nis_native.retained=true;
 int e=nis_execute(&nis_ops,&nis_id_ops,c,&nis_native.result);
 *clean=nis_native.result.clean;
 if(*clean)nis_native.retained=false; /* keep true for dirty evidence */
 return e;
}
static int nis_scope_or_common(const struct ncc_ops *ops,struct ncc_result *r)
{
 unsigned enabled=0;PE_parse_boot_argn("nord2wifiadieid",&enabled,sizeof(enabled));
 if(enabled!=1)return ncc_run(ops,0,1,2,r);
 nis_native.retained=false;nis_native.clock_attempted=nis_native.clock_ok=false;
 nis_native.clock_polls=0;nis_native.clock_raw=nis_native.irq_raw=0;
 nis_native.result=(struct nis_result){0};nis_native.result.clean=1;
 nis_native.sem=ml_io_map(0x18072000,0x1000);
 if(!nis_native.sem)nis_native.result.error=-9;
 if(!nis_native.sem||nis_prepare(&nis_ops,0,&nis_native.result)){
  *r=(struct ncc_result){0};r->clean=1;r->error=-12;
  printf("NORD2 WIFI ADIE: skipped=1 prepare_error=%d snapshots=%u\n",nis_native.result.error,nis_native.result.snapshots);return -1;
 }
 int inner_clean=1;
 int e=ncc_scope(ops,0,1,r,nis_inner,&inner_clean);
 struct nis_result *s=&nis_native.result;struct nai_result *id=&s->id;
 printf("NORD2 WIFI ADIE: attempted=%d id=%#x valid=%d known=%d complete=%d clean=%d error=%d restore_error=%d\n",id->phase!=0,id->id,id->id_valid,id->known_chip,id->command_complete,id->clean,id->error,id->restore_error);
 printf("NORD2 WIFI ADIE RESTORE: strap_touched=%#x restored=%#x clean=%d error=%d sem_owned=%d inner_clean=%d\n",s->touched,s->restored,s->clean,s->error,id->semaphore_owned,inner_clean);
 printf("NORD2 WIFI ADIE CLOCK: attempted=%u ok=%u raw=%#x irq=%#x polls=%u\n",
  (unsigned)nis_native.clock_attempted,(unsigned)nis_native.clock_ok,
  nis_native.clock_raw,nis_native.irq_raw,nis_native.clock_polls);
 return e; /* enclosing existing common wrapper latches+panics if !r.clean */
}
