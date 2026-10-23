/* Include in the parent's audio-worker TU after kernel thread/vm headers.
 * Mapping ownership belongs to parent. This bridge NEVER calls ml_io_map.
 * Populate exactly9 windows with existing16KiB mappings, base list below.
 * If top/infra or GPIO/SPM have separate old aliases, consolidate/reuse one
 * mapping before populating this context; do not establish another alias.
 * Audio worker calls nau_hw_* with &nah_native_io and &native as cookie.
 * I2C8 lease callback must serialize AP users and must not touch PMIF. */
#include "audio_hw.h"
#include "audio_native.h"
#ifndef NAH_NATIVE_BARRIER
#define NAH_NATIVE_BARRIER() __asm__ __volatile__("dsb sy" ::: "memory")
#endif
#define NAH_NATIVE_WINDOWS 9u
struct nah_native_window { uint32_t base; vm_offset_t va; };
struct nah_native_context {
 struct nah_native_window windows[NAH_NATIVE_WINDOWS];
 thread_t worker;
 int (*i2c8_lease)(unsigned);
};
static const uint32_t nah_native_bases[NAH_NATIVE_WINDOWS]={
 0x10000000, /* TOPCKGEN+INFRACFG_AO */
 0x10004000, /* GPIO+SPM */
 0x1000c000, /* APMIXED */
 0x11000000, /* APDMA I2C8 channel+0x780 */
 0x11210000, /* AFE and SRAM+0x2000; same mapping */
 0x11c20000, /* IOCFG_RM */
 0x11d04000, /* I2C8 shadow/AP/CCU+IMPS gates+0x1e00 */
 0x11d10000, /* IOCFG_BM */
 0x11f20000  /* IOCFG_LT */
};
static int nah_native_valid(const struct nah_native_context *c)
{
 if (!c || PAGE_SIZE!=0x4000u || !c->worker || !c->i2c8_lease) return 0;
 for (unsigned i=0;i<NAH_NATIVE_WINDOWS;i++)
  if (c->windows[i].base!=nah_native_bases[i] || !c->windows[i].va ||
      (c->windows[i].va&0x3fff)) return 0;
 return 1;
}
static uintptr_t nah_native_worker(void *cookie)
{
 struct nah_native_context *c=cookie;
 return c && c->worker && c->worker==current_thread() ? (uintptr_t)c->worker:0;
}
static vm_offset_t nah_native_va(struct nah_native_context *c,uint32_t pa,unsigned width)
{
 if (!nah_native_worker(c) || (width!=1 && width!=2 && width!=4) || (pa&(width-1))) return 0;
 uint32_t base=pa&~0x3fffu;
 for (unsigned i=0;i<NAH_NATIVE_WINDOWS;i++)
  if (c->windows[i].base==base) return c->windows[i].va+(pa-base);
 return 0;
}
/* Reuse the PE-owned distributor mapping; do not create a tenth alias. */
extern int nord2_audio_gic_read(uint32_t *);
static int nah_native_read(void *cookie,uint32_t pa,unsigned w,uint32_t *v)
{
 struct nah_native_context *c=cookie;
 if (!v || !nah_native_worker(c)) return -1;
 if (pa==0x0c000110) {
  if (w!=4) return -1;
  NAH_NATIVE_BARRIER();
  int rc=nord2_audio_gic_read(v);
  NAH_NATIVE_BARRIER();
  return rc;
 }
 vm_offset_t va=nah_native_va(c,pa,w);
 if (!va) return -1;
 /* FIFO_DATA is only popped by the polled transport; SRAM is write-only. */
 if (pa>=NAF_SRAM && pa<NAF_SRAM+NAF_SRAM_BYTES) return -1;
 NAH_NATIVE_BARRIER();
 if (w==1) *v=*(volatile uint8_t *)va;
 else if (w==2) *v=*(volatile uint16_t *)va;
 else *v=*(volatile uint32_t *)va;
 NAH_NATIVE_BARRIER();
 return 0;
}
static int nah_native_bus_write(uint32_t pa,unsigned w)
{
 /* No shadow, CCU, APDMA, reset, RESUME_ARBIT, clock or pad writes.
  * Slave/value validation is done by audio_i2c8.c before any callback.
  * All AP setup registers are saved/restored by transport. */
 if (pa==0x11d04100) return w==1;
 if (w!=2) return 0;
 switch (pa) {
 case 0x11d04104: case 0x11d04108: case 0x11d0410c:
 case 0x11d04110: case 0x11d04114: case 0x11d04118:
 case 0x11d0411c: case 0x11d04120: case 0x11d04124:
 case 0x11d04128: case 0x11d0412c: case 0x11d04130:
 case 0x11d04134: case 0x11d04138: case 0x11d04140:
 case 0x11d04144: case 0x11d04148: case 0x11d0414c: return 1;
 default: return 0;
 }
}
static int nah_native_write(void *cookie,uint32_t pa,unsigned w,uint32_t v)
{
 struct nah_native_context *c=cookie;
 unsigned clock=(w==4 &&
   (((pa==0x100000b8) && v==(1u<<15)) ||
    ((pa==0x11d05e04 || pa==0x11d05e08) && v==(1u<<4))));
 unsigned reset=(w==4 &&
  (((pa==0x10005034 || pa==0x10005038 || pa==0x10005134 || pa==0x10005138) && v==(1u<<15)) ||
   ((pa==0x10005044 || pa==0x10005048 || pa==0x10005144 || pa==0x10005148) && v==(1u<<20))));
 if (!((w==4 && naf_write_allowed(pa)) || nah_native_bus_write(pa,w) || reset || clock)) return -1;
 vm_offset_t va=nah_native_va(c,pa,w);
 if (!va) return -1;
 NAH_NATIVE_BARRIER();
 if (w==1) *(volatile uint8_t *)va=(uint8_t)v;
 else if (w==2) *(volatile uint16_t *)va=(uint16_t)v;
 else *(volatile uint32_t *)va=v;
 NAH_NATIVE_BARRIER();
 return 0;
}
static int nah_native_delay(void *cookie,unsigned us)
{ if (!nah_native_worker(cookie) || us>1000) return -1; delay(us); return 0; }
static int nah_native_lease(void *cookie,unsigned acquire)
{
 struct nah_native_context *c=cookie;
 if (!nah_native_worker(c) || !c->i2c8_lease || acquire>1) return -1;
 return c->i2c8_lease(acquire);
}
static const struct nah_io nah_native_io={
 nah_native_read,nah_native_write,nah_native_delay,nah_native_worker,nah_native_lease
};

#ifdef NAH_NATIVE_DEFINE_PREPARE
/* Include this bridge after ni_gpio/ni_lt declarations in sweet_touch.c.
 * Parent service calls this exported entry on its exclusive audio worker.
 * Native init is one attempt/boot, all maps are retained even on failure:
 * other iOS components remain usable; no panic/unmap of shared NI windows. */
extern vm_offset_t ml_io_map(vm_offset_t, vm_size_t);
static struct nah_native_context nah_native;
static unsigned nah_native_attempted;
/* The producer owns NI/NW globals. Audio never reads them while they are
 * being initialized: immutable retained VAs publish via release/acquire. */
static struct { vm_offset_t infra,gpio,lt; } nah_native_shared;
static int nah_native_publication=NAH_NATIVE_WAIT;
#define NAH_NATIVE_PUBLISHING 2
int nah_native_dependency_state(void)
{
 int state=__atomic_load_n(&nah_native_publication,__ATOMIC_ACQUIRE);
 return state==NAH_NATIVE_PUBLISHING ? NAH_NATIVE_WAIT:state;
}
int nah_native_publish_ready(void)
{
 int expected=NAH_NATIVE_WAIT;
 if (!__atomic_compare_exchange_n(&nah_native_publication,&expected,
     NAH_NATIVE_PUBLISHING,0,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE))
  return nah_native_dependency_state();
 int result=NAH_NATIVE_READY;
 /* Called only by NI after its initial writes. Capture once, never replace
  * or unmap these windows, even if audio subsequently fails to initialize. */
 if (PAGE_SIZE!=0x4000u) result=NAH_NATIVE_E_PAGE;
 else if (!nw.infra || !ni_gpio || !ni_lt) result=NAH_NATIVE_E_DEPENDENCIES;
 else if (((nw.infra-0x1000)&0x3fff) || ((ni_gpio-0x1000)&0x3fff) ||
          (ni_lt&0x3fff)) result=NAH_NATIVE_E_WINDOW;
 if (!result) {
  nah_native_shared.infra=nw.infra;
  nah_native_shared.gpio=ni_gpio;
  nah_native_shared.lt=ni_lt;
 }
 __atomic_store_n(&nah_native_publication,result,__ATOMIC_RELEASE);
 return result;
}
static volatile unsigned nah_native_bus_owned;
static int nah_native_bus_lease(unsigned acquire)
{
 if (current_thread()!=nah_native.worker) return -1;
 if (acquire) return __sync_bool_compare_and_swap(&nah_native_bus_owned,0,1)?0:-1;
 return __sync_bool_compare_and_swap(&nah_native_bus_owned,1,0)?0:-1;
}
int nah_native_prepare(struct nah_io *out,void **cookie)
{
 if (!out || !cookie) return NAH_NATIVE_E_ARGS;
 if (PAGE_SIZE!=0x4000u) return NAH_NATIVE_E_PAGE;
 if (!current_thread()) return NAH_NATIVE_E_CONTEXT;
 if (nah_native_attempted) return NAH_NATIVE_E_REENTRY;
 int state=nah_native_dependency_state();
 if (state!=NAH_NATIVE_READY) return state; /* WAIT has zero side effects. */
 nah_native_attempted=1;
 nah_native.worker=current_thread(); nah_native.i2c8_lease=nah_native_bus_lease;
 for (unsigned i=0;i<NAH_NATIVE_WINDOWS;i++) {
  uint32_t pa=nah_native_bases[i]; vm_offset_t va=0;
  /* ni_gpio maps logical10005000, offset1000 within16KiB page. */
  if (pa==0x10000000) va=nah_native_shared.infra-0x1000;
  else if (pa==0x10004000) va=nah_native_shared.gpio-0x1000;
  else if (pa==0x11f20000) va=nah_native_shared.lt;
  else va=ml_io_map(pa,0x4000);
  if (!va) return NAH_NATIVE_E_MAP;
  nah_native.windows[i]=(struct nah_native_window){pa,va};
 }
 if (!nah_native_valid(&nah_native)) return NAH_NATIVE_E_WINDOW;
 *out=nah_native_io; *cookie=&nah_native; return 0;
}
#endif
