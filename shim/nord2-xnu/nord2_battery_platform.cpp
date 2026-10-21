#if defined(KERNEL)
#include <pexpert/arm64/board_config.h>
#endif
#if defined(NORD2)
/* PRIVATE native polling provider. No touch/PMIF/DMA/charger/gauge controls.
 * Explicit nord2battery=1 activates class polling, this file supplies I2C PIO.
 * Read-only SMBus commands; chip/charger configurations are never written. */
#include <stdint.h>
#include <mach/mach_types.h>
#include <kern/clock.h>
extern "C" {
#include "nord2_battery.h"
#include "mtk_i2c_read.h"
#include "nbg_telemetry.h"
extern void IOLog(const char *,...) __attribute__((format(printf,1,2)));
extern vm_offset_t ml_io_map(vm_offset_t,vm_size_t);
extern void delay(int);
}
struct nbg_map {uint32_t pa;vm_size_t size;vm_offset_t va;};
static struct nbg_map maps[]={{0x10000000,0x1000,0},{0x1000c000,0x1000,0},{0x10005000,0x1000,0},
 {0x11d05000,0x1000,0},{0x11008000,0x1000,0},{0x11d03000,0x1000,0},{0x1101a000,0x1000,0}};
static unsigned busy;
static struct nbg_i2c_state buses[2];
static unsigned bus_logged[2],bus_last_stage[2],bus_last_failure[2];
static int bus_last_rc[2];
static unsigned sample_logged,sample_last_valid;
static int sample_last_rc;
static void report_bus(unsigned bus,int rc,const struct nbg_i2c_state *s){
 unsigned i=bus==7?0:1;
 if(bus_logged[i]&&bus_last_stage[i]==s->stage&&bus_last_failure[i]==s->failure&&bus_last_rc[i]==rc)return;
 bus_logged[i]=1;bus_last_stage[i]=s->stage;bus_last_failure[i]=s->failure;bus_last_rc[i]=rc;
 IOLog(NBG_I2C_LOG,bus,rc,s->stage,s->failure,s->observed,s->offline,s->inflight,
  s->last_addr,s->last_value,s->last_valid,s->last_write,s->last_width);
 IOLog(NBG_CLOCK_LOG,bus,s->mux,s->parent_hz,s->pinmux,s->initial_gate,s->completion_irq,s->polls,s->observed);
}
static vm_offset_t address(uint32_t p,unsigned bytes){
 for(unsigned i=0;i<7;i++)if(p>=maps[i].pa&&(uint64_t)p+bytes<=(uint64_t)maps[i].pa+maps[i].size){
  if(!maps[i].va)maps[i].va=ml_io_map(maps[i].pa,maps[i].size);
  return maps[i].va?maps[i].va+p-maps[i].pa:0;
 }
 return 0;
}
static int rd(void *,uint32_t p,unsigned width,uint32_t *out){
 vm_offset_t v=address(p,width);if(!v||!out||(p&(width-1)))return -1;
 if(width==1)*out=*(volatile uint8_t *)v;
 else if(width==2)*out=*(volatile uint16_t *)v;
 else if(width==4)*out=*(volatile uint32_t *)v;else return -1;
 __asm__ volatile("dmb oshld" ::: "memory");return 0;
}
static int wr(void *,uint32_t p,unsigned width,uint32_t value){
 vm_offset_t v=address(p,width);if(!v||(p&(width-1)))return -1;
 __asm__ volatile("dmb oshst" ::: "memory");
 if(width==1)*(volatile uint8_t *)v=(uint8_t)value;
 else if(width==2)*(volatile uint16_t *)v=(uint16_t)value;
 else if(width==4)*(volatile uint32_t *)v=value;else return -1;
 __asm__ volatile("dsb osh" ::: "memory");return 0;
}
static int us(void *,unsigned n){if(n>10000)return -1;delay((int)n);return 0;}
static const struct nbg_i2c_io io={rd,wr,us};
static int owned(void *){return busy==1?0:-1;}
static int read(void *,unsigned bus,unsigned slave,unsigned cmd,unsigned bytes,uint16_t *out){
 if(bus!=7&&bus!=11)return -1;
 struct nbg_i2c_state *s=&buses[bus==7?0:1];
 int rc=nbg_i2c_read(&io,0,s,bus,slave,cmd,bytes,out);
 report_bus(bus,rc,s);return rc;
}
static int ms(void *,unsigned n){if(n!=10)return -1;delay(10000);return 0;}
static const struct nbg_ops ops={owned,read,ms};
extern "C" int nord2_battery_platform_sample(struct nbg_sample *s){
 if(!s)return -1;
 if(!__sync_bool_compare_and_swap(&busy,0u,1u)){*s=(struct nbg_sample){};s->error=-1;return -1;}
 /* Nocontrol-device-type query yet: SOC standardread available, current0c
  * deliberately unavailable rather than guessing BQ/ZY installed identity. */
 int e=nbg_collect(&ops,0,1,0,s);
 unsigned valid=s->raw_valid|(s->soc_valid<<8)|(s->charger_valid<<9);
 if(!sample_logged||sample_last_valid!=valid||sample_last_rc!=e){
  sample_logged=1;sample_last_valid=valid;sample_last_rc=e;
  IOLog(NBG_RAW_LOG,e,s->raw_valid,(unsigned)s->raw_soc[0],(unsigned)s->raw_soc[1],
   (unsigned)s->raw_voltage,(unsigned)s->raw_temperature,(unsigned)s->raw_current,
   (unsigned)s->raw_status[0],(unsigned)s->raw_status[1],(unsigned)s->raw_fault,s->soc_valid,s->charger_valid);
 }
 __sync_lock_release(&busy);return e;
}

#endif /* NORD2 */
