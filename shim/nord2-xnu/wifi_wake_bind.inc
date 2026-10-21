/* PRIVATE native binding: same input thread, separate AP-wake lifetime.
 * Include after wifi_persistent_bind.inc. No frozen persistent state mutation,
 * ncc_ap_check/npa callbacks, new mappings, or off-domain reads before wake.
 */
#include "wifi_wake_adapter.h"
static struct {struct nawa diag;bool active;} nawa_native;
static int nawa_xnu_owner(void *cookie)
{
 (void)cookie;
 /* Software-only proof, including all required retained owner handles. */
 if(!nawa_native.active||!npa_persistent_lifetime||!npa_native.outer_retained||
    !npa_native.state.entered||!npa_native.state.retained||npa_native.state.fatal||
    npa_native.state.hw_handoff!=2||npa_native.state.result.phase!=10||
    !npa_native.state.result.retained||npa_native.state.result.halt||
    npa_native.state.sema11||npa_native.state.sema19||npa_native.state.spi_inflight||
    !ni_saved||!ncc_fresh_owned||!nw.saved||!nw.domain_touched||ncc_latched_dirty)
  return -1;
 if(!nw.spm||!nw.infra||!nw.gals||!npa_native.host||!npa_native.cfg||!npa_native.rgu)return -1;
 return 0;
}
static vm_offset_t nawa_xnu_base(uint32_t p)
{
 switch(p){
 case 0x10006304:case 0x1000616c:case 0x10006170:return nw.spm;
 case 0x10001228:case 0x10001724:return nw.infra;
 case 0x1020e504:return nw.gals;
 case 0x180601a0:case 0x18060184:case 0x18060000:case 0x180602d4:case 0x180602cc:return npa_native.host;
 case 0x18001000:return npa_native.cfg;
 case 0x18000010:case 0x18000000:case 0x18000014:case 0x18000008:return npa_native.rgu;
 default:return 0;
 }
}
static int nawa_xnu_read(void *cookie,uint32_t p,uint32_t *v)
{
 (void)cookie;if(!v||nawa_native.diag.asleep)return -1;
 vm_offset_t b=nawa_xnu_base(p);if(!b)return -1;
 *v=ni_rd(b,p&0xfff);return 0;
}
static int nawa_xnu_write(void *cookie,uint32_t p,uint32_t v)
{
 (void)cookie;
 if(nawa_native.diag.asleep||(p!=0x180601a0&&p!=0x18060000))return -1;
 if(p==0x180601a0&&v>1)return -1;
 vm_offset_t b=nawa_xnu_base(p);if(!b)return -1;
 ni_wr(b,p&0xfff,v);NCC_DEVICE_BARRIER();return 0;
}
static int nawa_xnu_delay(void *cookie,unsigned us)
{(void)cookie;if(nawa_native.diag.asleep)return -1;delay(us);return 0;}
static const struct nawa_hw nawa_xnu_hw={nawa_xnu_owner,nawa_xnu_read,nawa_xnu_write,nawa_xnu_delay};
static void nawa_after_persistent(void)
{
 unsigned opt=0;PE_parse_boot_argn("nord2wifiwake",&opt,sizeof(opt));
 if(opt!=1)return; /*zero maps/MMIO/results by default*/
 if(nawa_native.active||nawa_native.diag.result.entered)
  panic("NORD2 WIFI WAKE: refuse reentry; keep persistent owner");
 nawa_native.active=true;
 if(nawa_xnu_owner(0)){
  nawa_native.active=false;
  printf("NORD2 WIFI WAKE: skipped=1 reason=persistent-owner\n");return;
 }
 int status=nawa_start(&nawa_native.diag,&nawa_xnu_hw,0,1);
 struct naw_state *r=&nawa_native.diag.result;
 /* ONLY software result reads, including after balanced source sleep. */
 printf("NORD2 WIFI WAKE: status=%d phase=%u balanced=%u ref=%u halt=%u\n",
  status,r->phase,r->balanced,r->reference,r->halt);
 printf("NORD2 WIFI WAKE ID: id=%#x clock=%#x irq=%#x ack_polls=%u clock_polls=%u id_polls=%u\n",
  r->chip,r->clock,r->irq,r->ack_polls,r->clock_polls,r->id_polls);
 printf("NORD2 WIFI WAKE COLD: valid=%#x cpu=%#x top=%#x power=%#x bgfcpu=%#x bgftop=%#x\n",
  r->cold_valid,r->cold[0],r->cold[1],r->cold[2],r->cold[3],r->cold[4]);
 if(r->halt||r->reference||r->retained)
  panic("NORD2 WIFI WAKE: uncertain phase%u; retain wake/common; manual reset",r->phase);
 nawa_native.active=false;
}
