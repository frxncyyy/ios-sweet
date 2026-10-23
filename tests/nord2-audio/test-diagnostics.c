#define main original_full_main
#include "test-full.c"
#undef main
#include <stdarg.h>
static unsigned printed,ap_printed,seen_gate,seen_phase,seen_op,seen_stage,seen_failure,seen_pa,seen_uncertain;
static unsigned seen_i2c_pa,seen_i2c_value,seen_ownership,seen_amp[2],seen_ap_valid,seen_ap[NA8_AP_FIELDS],unexpected_mmio;
static int diagnostic_read(void *c,uint32_t pa,unsigned w,uint32_t *v)
{(void)c;(void)pa;(void)w;(void)v;unexpected_mmio++;return -1;}
static int diagnostic_write(void *c,uint32_t pa,unsigned w,uint32_t v)
{(void)c;(void)pa;(void)w;(void)v;unexpected_mmio++;return -1;}
static int capture_printf(const char *format,...)
{
 va_list ap;va_start(ap,format);
 if(strstr(format,"NORD2 AUDIO ERROR:")) {
  assert(!strcmp(va_arg(ap,const char*),"test"));assert(va_arg(ap,int)==NAH_E_IO);
  seen_gate=va_arg(ap,unsigned);seen_phase=va_arg(ap,unsigned);seen_op=va_arg(ap,unsigned);
  seen_stage=va_arg(ap,unsigned);seen_failure=va_arg(ap,unsigned);seen_pa=va_arg(ap,unsigned);
  (void)va_arg(ap,int);(void)va_arg(ap,int);(void)va_arg(ap,unsigned);seen_uncertain=va_arg(ap,unsigned);
  seen_i2c_pa=va_arg(ap,unsigned);seen_i2c_value=va_arg(ap,unsigned);seen_ownership=va_arg(ap,unsigned);
  seen_amp[0]=va_arg(ap,unsigned);seen_amp[1]=va_arg(ap,unsigned);printed++;
 } else if(strstr(format,"NORD2 AUDIO AP:")) {
  seen_ap_valid=va_arg(ap,unsigned);
  for(unsigned i=0;i<NA8_AP_FIELDS;i++)seen_ap[i]=va_arg(ap,unsigned);
  ap_printed++;
 } else assert(!"unexpected diagnostic record");
 va_end(ap);return 0;
}
#define hardware hw
#define printf capture_printf
#include "ready-diagnose.inc"
#undef printf
#undef hardware
int main(void)
{
 setup();
 hw.status.gate=NAF_G_PINS;hw.status.uncertain=0;
 hw.afe.phase=6;hw.afe.op=7;hw.afe.last_pa=NAF_AFE_I2S_CON3;
 hw.bus.stage=12;hw.bus.failure=5;hw.bus.uncertain=1;
 hw.bus.last_addr=0x11d04110;hw.bus.last_value=0x3c;
 hw.status.ownership=0x55;hw.status.amp_status[0]=1;hw.status.amp_status[1]=9;
 const uint32_t ap_values[NA8_AP_FIELDS]={2,0x3c,2,1,0x8000};
 hw.bus.ap_valid=NA8_AP_VALID_ALL;
 for(unsigned i=0;i<NA8_AP_FIELDS;i++)hw.bus.ap_preflight[i]=ap_values[i];
 hw.io.read=diagnostic_read;hw.io.write=diagnostic_write;
 unsigned before=cm.reads+cm.writes;
 diagnose("test",NAH_E_IO,0);
 assert(printed==1 && seen_gate==NAF_G_PINS && seen_phase==6 && seen_op==7);
 assert(seen_stage==12 && seen_failure==5 && seen_pa==NAF_AFE_I2S_CON3 && seen_uncertain==1);
 assert(ap_printed==1 && seen_ap_valid==NA8_AP_VALID_ALL);
 assert(seen_i2c_pa==0x11d04110 && seen_i2c_value==0x3c && seen_ownership==0x55);
 assert(seen_amp[0]==1 && seen_amp[1]==9);
 for(unsigned i=0;i<NA8_AP_FIELDS;i++)assert(seen_ap[i]==ap_values[i]);
 assert(cm.reads+cm.writes==before && !unexpected_mmio); /* Saved data only. */
 /* Partial callback failure: valid mask identifies missing zero-valued slots. */
 hw.bus.ap_valid=3;
 for(unsigned i=2;i<NA8_AP_FIELDS;i++)hw.bus.ap_preflight[i]=0;
 diagnose("test",NAH_E_IO,0);
 assert(printed==2 && ap_printed==2 && seen_ap_valid==3);
 for(unsigned i=0;i<NA8_AP_FIELDS;i++)assert(seen_ap[i]==(i<2?ap_values[i]:0));
 assert(cm.reads+cm.writes==before && !unexpected_mmio);
 full_prepare(0x0b74,0x0a74,0);
 *regp(&cm,NAF_SPM_PWR_STATUS)|=1u<<21;
 assert(nau_hw_init(&hw,&allio,0,0)==NAH_E_COLD);
 assert(hw.status.gate&NAF_G_AUDIO_ON);
 assert(!bm.starts && !cm.writes && hw.snapshot.snap_read==NAU_SNAP_COUNT);
 puts("PASS actual saved-state diagnose+AP full/partial valid payload, exact refused PA/value, partial-init uncertainty, early cold gate; zero MMIO callbacks");
}
