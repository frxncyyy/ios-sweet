/* Opt-in MT6893 common-configuration diagnostic, included by sweet_touch.c.
 * Existing sole probe thread; never a worker. No address outside allowlists. */
struct ncc_pred {uint32_t address,mask;};
static const struct ncc_pred ncc_predicates[14]={
 {0x18000010,1},{0x18000000,0x80},{0x180602cc,4},
 {0x18000008,0x80},{0x18000014,1},{0x18000114,0x03000000},
 {0x18000124,0x01000003},{0x18001874,1},{0x18001878,1},
 {0x18008040,1},{0x180601a8,1},{0x180601ac,1},{0x180601b0,1},
 {0x18004000,0x3e}
};
static struct {
 vm_offset_t rgu,host,cfg,spi,table,fm;
 uint32_t id,predicate_valid,predicate_raw[14];unsigned failed_field;
 struct ncc_result result;
} ncc_native;
#ifndef NCC_DEVICE_BARRIER
#define NCC_DEVICE_BARRIER() __asm__ volatile("dsb sy" ::: "memory")
#endif
static vm_offset_t ncc_aperture(uint32_t address)
{
 switch(address&~0xfffu){
 case 0x18000000:return ncc_native.rgu;
 case 0x18001000:return ncc_native.cfg;
 case 0x18004000:return ncc_native.spi;
 case 0x18005000:return ncc_native.table;
 case 0x18008000:return ncc_native.fm;
 case 0x18060000:return ncc_native.host;
 default:return 0;
 }
}
static int ncc_ap_check(void)
{
 if(!ncc_fresh_owned||ncc_latched_dirty||!nw.saved||!nw.domain_touched||
  ncc_native.id!=0x20010101||!nw.spm||!nw.infra||!nw.gals||!ncc_native.host)return -1;
 if((ni_rd(nw.spm,0x304)&0x1f)!=0xd||!(ni_rd(nw.spm,0x16c)&2)||
  !(ni_rd(nw.spm,0x170)&2)||!(ni_rd(nw.gals,0x504)&1)||
  (ni_rd(nw.infra,0x228)&0x6000)||(ni_rd(nw.infra,0x724)&4))return -1;
 return (ni_rd(ncc_native.host,0x184)&0x20)?-1:0;
}
static int ncc_native_check(void *opaque,unsigned required)
{
 (void)opaque;
 ncc_native.predicate_valid=0;ncc_native.failed_field=14;
 if(required!=NCC_REQUIRED||ncc_ap_check())return -1;
 for(unsigned i=0;i<14;i++){
  vm_offset_t base=ncc_aperture(ncc_predicates[i].address);
  /* Do not access CONN after an AP prerequisite disappears. */
  if(!base||ncc_ap_check()){ncc_native.failed_field=i;return -1;}
  uint32_t raw=ni_rd(base,ncc_predicates[i].address&0xfff);
  ncc_native.predicate_raw[i]=raw;ncc_native.predicate_valid|=1u<<i;
  if(raw&ncc_predicates[i].mask){ncc_native.failed_field=i;return -1;}
 }
 return 0;
}
static int ncc_native_index(uint32_t address)
{
 for(unsigned i=0;i<NCC_COUNT;i++)if(ncc_table[i].address==address)return (int)i;
 return -1;
}
static int ncc_native_read(void *opaque,uint32_t address,uint32_t *value)
{
 (void)opaque;
 if(!value||ncc_native_index(address)<0||ncc_ap_check())return -1;
 vm_offset_t base=ncc_aperture(address);if(!base)return -1;
 *value=ni_rd(base,address&0xfff);return 0;
}
static int ncc_native_write(void *opaque,uint32_t address,uint32_t value)
{
 (void)opaque;int i=ncc_native_index(address);
 if(i<0||ncc_native.result.snapshots!=NCC_COUNT||ncc_ap_check())return -1;
 const struct ncc_reg *t=&ncc_table[i];uint32_t before=ncc_native.result.before[i];
 uint32_t programmed=(before&~t->mask)|(t->value&t->mask);
 if(value!=before&&value!=programmed)return -1;
 vm_offset_t base=ncc_aperture(address);if(!base)return -1;
 ni_wr(base,address&0xfff,value);return 0;
}
static int ncc_native_barrier(void *opaque)
{(void)opaque;if(ncc_ap_check())return -1;NCC_DEVICE_BARRIER();return 0;}
static int ncc_cold_read(void *opaque,uint64_t address,uint32_t *value)
{
 (void)opaque;
 if(!value||ncc_ap_check())return -1;
 if(address!=0x18000010&&address!=0x18000000&&address!=0x180602cc)return -1;
 vm_offset_t base=ncc_aperture((uint32_t)address);if(!base)return -1;
 *value=ni_rd(base,(unsigned)address&0xfff);return 0;
}
static int ncc_cold_check(void *opaque,unsigned required)
{(void)opaque;return required==NWC_REQUIRED?ncc_ap_check():-1;}
#ifndef NCC_TICKS
static uint64_t ncc_ticks(void)
{ uint64_t value; __asm__ volatile("mrs %0, cntvct_el0" : "=r" (value)); return value; }
#define NCC_TICKS() ncc_ticks()
#endif
#ifndef NCC_FREQUENCY
static uint64_t ncc_frequency(void)
{ uint64_t value; __asm__ volatile("mrs %0, cntfrq_el0" : "=r" (value)); return value; }
#define NCC_FREQUENCY() ncc_frequency()
#endif
static void ncc_power_window(vm_offset_t host,vm_offset_t cfg,uint32_t id)
{
 unsigned enabled=0;PE_parse_boot_argn("nord2wificommon",&enabled,sizeof(enabled));
 if(enabled!=1)return; /* zero additional MMIO/map/log by default */
 ncc_native.host=host;ncc_native.cfg=cfg;ncc_native.id=id;
 if(ncc_ap_check()){
  printf("NORD2 WIFI COMMON: skipped=1 reason=owner-or-AP\n");return;
 }
 ncc_native.rgu=ml_io_map(0x18000000,0x1000);
 if(!ncc_native.rgu){printf("NORD2 WIFI COMMON: skipped=1 reason=rgu-map\n");return;}
 const struct nwc_ops cold_ops={ncc_cold_check,ncc_cold_read};struct nwc_result cold;
 int status=nwc_snapshot(&cold_ops,0,1,&cold);
 printf("NORD2 WIFI COMMON COLD: status=%d valid=%#x cpu=%#x top=%#x power=%#x\n",
  status,cold.valid_mask,cold.raw[0],cold.raw[1],cold.raw[2]);
 if(status)return;
 ncc_native.spi=ml_io_map(0x18004000,0x1000);
 ncc_native.table=ml_io_map(0x18005000,0x1000);
 ncc_native.fm=ml_io_map(0x18008000,0x1000);
 if(!ncc_native.spi||!ncc_native.table||!ncc_native.fm){
  printf("NORD2 WIFI COMMON: skipped=1 reason=table-map\n");return;
 }
 /* Two fresh predicate passes. No polling, sleep, selector write or retry. */
 if(ncc_native_check(0,NCC_REQUIRED)||ncc_native_check(0,NCC_REQUIRED)){
  unsigned i=ncc_native.failed_field;
  printf("NORD2 WIFI COMMON: skipped=1 valid=%#x field=%u raw=%#x\n",
   ncc_native.predicate_valid,i,(i<14&&(ncc_native.predicate_valid&(1u<<i)))?ncc_native.predicate_raw[i]:0);return;
 }
 const struct ncc_ops ops={ncc_native_check,ncc_native_read,ncc_native_write,ncc_native_barrier};
 uint64_t begin=NCC_TICKS();
 status=ncc_run(&ops,0,1,2,&ncc_native.result);
 uint64_t elapsed=NCC_TICKS()-begin;
 struct ncc_result *r=&ncc_native.result;
 printf("NORD2 WIFI COMMON: status=%d phase=%u snapshots=%u verified=%u error=%d restore_error=%d clean=%d\n",
  status,r->phase,r->snapshots,r->verified,r->error,r->restore_error,r->clean);
 printf("NORD2 WIFI COMMON RESTORE: touched=%#llx restored=%#llx valid=%#x field=%u\n",
  (unsigned long long)r->touched,(unsigned long long)r->restored,
  ncc_native.predicate_valid,ncc_native.failed_field);
 printf("NORD2 WIFI COMMON TIME: ticks=%llu frequency=%llu\n",
  (unsigned long long)elapsed,(unsigned long long)NCC_FREQUENCY());
 if(!r->clean){
  /* Persistent latch is checked by nw_restore and the Linux-return path.
   * Do NOT power down, clear saved ownership or discard snapshots. */
  ncc_latched_dirty=true;
  panic("NORD2 WIFI COMMON: dirty restoration; retain power/ownership; refuse handoff");
 }
}
