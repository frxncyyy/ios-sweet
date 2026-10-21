/* Opt-in XNU binding for the persistent radio initialization.
 * Include after wifi_common_adapter.inc so AP proof/ni_rd/ni_wr are available.
 * Raw32 access is allowlisted by wifi_persistent_adapter.c, not these maps.
 * Bind the *reviewed* external owner proof at the outer call. Passing NULL
 * refuses, never weakens proof. No reserved-RAM physical mapping is created.
 */
#include "wifi_persistent_adapter.h"
static struct {
 struct npa state;
 vm_offset_t rgu,cfg,thermal,afe,spi,table,fm,debug,sram,host,sem;
 int (*proof)(void *);void *proof_cookie;
 bool outer_retained;
} npa_native;
static vm_offset_t npa_base(uint32_t a)
{
 if(a>=0x18050000&&a<0x18054000)return npa_native.sram;
 switch(a&~0xfffu){
 case 0x10005000:return ni_gpio;
 case 0x10006000:return nw.spm;
 case 0x18000000:return npa_native.rgu;
 case 0x18001000:return npa_native.cfg;
 case 0x18002000:return npa_native.thermal;
 case 0x18003000:return npa_native.afe;
 case 0x18004000:return npa_native.spi;
 case 0x18005000:return npa_native.table;
 case 0x18008000:return npa_native.fm;
 case 0x1800f000:return npa_native.debug;
 case 0x18060000:return npa_native.host;
 case 0x18072000:return npa_native.sem;
 default:return 0;
 }
}
static unsigned npa_off(uint32_t a)
{return a>=0x18050000&&a<0x18054000?a-0x18050000:a&0xfff;}
static int npa_xnu_read(void *c,uint32_t a,uint32_t *v)
{
 (void)c;vm_offset_t b=npa_base(a);if(!b||!v)return -1;
 *v=ni_rd(b,npa_off(a));return 0;
}
static int npa_xnu_write(void *c,uint32_t a,uint32_t v)
{
 (void)c;vm_offset_t b=npa_base(a);if(!b)return -1;
 /* Before even a remap/clockdetector write: outer lifetime must retain.
  * nw_restore/suspend/Linux-return hooks must honor this separate latch.
  * DO NOT set ncc_latched_dirty here: existing AP guard requires it false. */
 npa_native.outer_retained=true;
 npa_persistent_lifetime=true;
 ni_wr(b,npa_off(a),v);NCC_DEVICE_BARRIER();return 0;
}
static int npa_xnu_ap(void *c){(void)c;return ncc_ap_check();}
static int npa_xnu_proof(void *c){(void)c;return npa_native.proof?npa_native.proof(npa_native.proof_cookie):-1;}
static int npa_xnu_delay(void *c,unsigned us){(void)c;delay(us);return 0;}
static const struct npa_hw npa_xnu_ops={npa_xnu_ap,npa_xnu_proof,npa_xnu_read,npa_xnu_write,npa_xnu_delay};
/* Hook at same owned-power point as ncc_power_window, selected BEFORE the
 * legacy common diagnostic. Tri-state:0notselected/-1premutationrefusal/
 *1retainedsuccess. Fatal after mutation panics without any cleanup. */
static int npa_persistent_window(vm_offset_t host,vm_offset_t cfg,uint32_t id,
 int (*proof)(void *),void *cookie,uint32_t conn,uint32_t mcif,unsigned build)
{
 unsigned opt=0;PE_parse_boot_argn("nord2wifipersist",&opt,sizeof(opt));
 if(opt!=1)return 0;
 if(npa_native.state.entered||npa_native.outer_retained)
  panic("NORD2 WIFI PERSIST: reentry; manual reset required");
 if(!proof)return -1;
 npa_native.proof=proof;npa_native.proof_cookie=cookie;
 ncc_native.host=host;ncc_native.cfg=cfg;ncc_native.id=id;
 if(ncc_ap_check())return -1;
 npa_native.cfg=cfg;npa_native.host=host;
#define NPA_MAP(field,pa,sz) do{npa_native.field=ml_io_map(pa,sz);if(!npa_native.field)return -1;}while(0)
 NPA_MAP(rgu,0x18000000,0x1000);NPA_MAP(thermal,0x18002000,0x1000);
 NPA_MAP(afe,0x18003000,0x1000);NPA_MAP(spi,0x18004000,0x1000);
 NPA_MAP(table,0x18005000,0x1000);NPA_MAP(fm,0x18008000,0x1000);
 NPA_MAP(debug,0x1800f000,0x1000);NPA_MAP(sram,0x18050000,0x4000);
 NPA_MAP(sem,0x18072000,0x1000);
#undef NPA_MAP
 int e=npa_start(&npa_native.state,&npa_xnu_ops,0,1,conn,mcif,build);
 if(e){
  if(npa_native.state.fatal||npa_native.outer_retained)
   panic("NORD2 WIFI PERSIST: fatal phase%u; retain owner/power; manual reset",npa_native.state.result.phase);
  return -1;
 }
 /* No MMIO/guard after source HW handoff. These are software result fields. */
 printf("NORD2 WIFI PERSIST: retained=1 phase=%u id=%#x handoff=%u\n",
  npa_native.state.result.phase,npa_native.state.result.chip,npa_native.state.hw_handoff);
 return 1;
}
