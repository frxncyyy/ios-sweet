#define main original_full_main
#include "test-full.c"
#undef main
#include <stdarg.h>
static unsigned printed,seen_gate,seen_phase,seen_op,seen_stage,seen_failure,seen_pa,seen_uncertain;
static int capture_printf(const char *format,...)
{
 assert(strstr(format,"NORD2 AUDIO ERROR:"));
 va_list ap;va_start(ap,format);
 assert(!strcmp(va_arg(ap,const char*),"test"));assert(va_arg(ap,int)==NAH_E_IO);
 seen_gate=va_arg(ap,unsigned);seen_phase=va_arg(ap,unsigned);seen_op=va_arg(ap,unsigned);
 seen_stage=va_arg(ap,unsigned);seen_failure=va_arg(ap,unsigned);seen_pa=va_arg(ap,unsigned);
 (void)va_arg(ap,int);(void)va_arg(ap,int);(void)va_arg(ap,unsigned);seen_uncertain=va_arg(ap,unsigned);
 va_end(ap);printed++;return 0;
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
 unsigned before=cm.reads+cm.writes;
 diagnose("test",NAH_E_IO,0);
 assert(printed==1 && seen_gate==NAF_G_PINS && seen_phase==6 && seen_op==7);
 assert(seen_stage==12 && seen_failure==5 && seen_pa==NAF_AFE_I2S_CON3 && seen_uncertain==1);
 assert(cm.reads+cm.writes==before); /* Error logging reads saved state only. */
 full_prepare(0x0b74,0x0a74,0);
 *regp(&cm,NAF_SPM_PWR_STATUS)|=1u<<21;
 assert(nau_hw_init(&hw,&allio,0,0)==NAH_E_COLD);
 assert(hw.status.gate&NAF_G_AUDIO_ON);
 assert(!bm.starts && !cm.writes && hw.snapshot.snap_read==NAU_SNAP_COUNT);
 puts("PASS actual saved-state diagnose fields, partial-init uncertainty, early cold gate; zero extra MMIO");
}
