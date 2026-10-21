/* Private CPU-held WFSYS binding. Same input thread and retained common owner;
 * a new object owns AP/WF/clock/semaphore requests. No old terminal callbacks. */
#include "wifi_wfsys_adapter.h"
static struct {
 struct nwsa diag;
 vm_offset_t brcm,local;
 bool active;
} nwsa_native;

static int nwsa_source_owner(void)
{
 if(!nwsa_native.active||!npa_persistent_lifetime||!npa_native.outer_retained||
    !npa_native.state.entered||!npa_native.state.retained||npa_native.state.fatal||
    npa_native.state.hw_handoff!=2||npa_native.state.result.phase!=10||
    !npa_native.state.result.retained||npa_native.state.result.halt||
    npa_native.state.sema11||npa_native.state.sema19||npa_native.state.spi_inflight||
    !ni_saved||!ncc_fresh_owned||!nw.saved||!nw.domain_touched||ncc_latched_dirty||
    nawa_native.active||nawa_native.diag.result.entered)return -1;
 if(!nw.spm||!nw.infra||!nw.gals||!npa_native.host||!npa_native.cfg||
    !npa_native.rgu||!npa_native.afe||!npa_native.debug||!npa_native.sem)return -1;
 return 0;
}
static int nwsa_xnu_owner(void *cookie)
{
 (void)cookie;
 return nwsa_source_owner()||!nwsa_native.brcm||!nwsa_native.local;
}
static vm_offset_t nwsa_xnu_base(uint32_t p)
{
 switch(p){
 case 0x10006304:case 0x1000616c:case 0x10006170:return nw.spm;
 case 0x10001228:case 0x10001724:return nw.infra;
 case 0x1020e504:return nw.gals;
 case 0x180601a0:case 0x180601a4:case 0x18060184:
 case 0x18060000:case 0x180602d4:case 0x180602cc:return npa_native.host;
 case 0x18001000:case 0x18001620:case 0x18001624:case 0x18001c14:return npa_native.cfg;
 case 0x18000010:case 0x18000000:case 0x18000018:
 case 0x18000014:case 0x18000008:return npa_native.rgu;
 case 0x18003008:return npa_native.afe;
 case 0x1800f004:return npa_native.debug;
 case 0x18072050:case 0x18072250:return npa_native.sem;
 case 0x1800e06c:return nwsa_native.brcm;
 case 0x184b0010:return nwsa_native.local;
 default:return 0;
 }
}
static int nwsa_xnu_read(void *cookie,uint32_t p,uint32_t *v)
{
 (void)cookie;
 if(!v||nwsa_native.diag.asleep||p==0x18072250)return -1;
 vm_offset_t b=nwsa_xnu_base(p);if(!b)return -1;
 *v=ni_rd(b,p&0xfff);return 0;
}
static int nwsa_xnu_write(void *cookie,uint32_t p,uint32_t v)
{
 (void)cookie;if(nwsa_native.diag.asleep)return -1;
 switch(p){
 case 0x180601a0:if(v>1)return -1;break;
 case 0x18000010:if(v&1)return -1;break; /* CPU release is never allowed. */
 case 0x18000000:if((v&0xffff0000)!=0x57460000)return -1;break;
 case 0x18072250:if(v!=1)return -1;break;
 case 0x180601a4:case 0x18060000:case 0x18001620:case 0x18001624:
 case 0x18001c14:case 0x18000018:case 0x18003008:case 0x1800f004:
 case 0x1800e06c:break;
 default:return -1;
 }
 vm_offset_t b=nwsa_xnu_base(p);if(!b)return -1;
 ni_wr(b,p&0xfff,v);NCC_DEVICE_BARRIER();return 0;
}
static int nwsa_xnu_delay(void *cookie,unsigned us)
{(void)cookie;if(nwsa_native.diag.asleep)return -1;delay(us);return 0;}
static const struct nwsa_hw nwsa_xnu_hw={
 nwsa_xnu_owner,nwsa_xnu_read,nwsa_xnu_write,nwsa_xnu_delay
};
static void nwsa_after_persistent(void)
{
 unsigned opt=0;PE_parse_boot_argn("nord2wifiwfsys",&opt,sizeof(opt));
 if(opt!=1)return;
 if(nwsa_native.active||nwsa_native.diag.state.entered)
  panic("NORD2 WIFI WFSYS: refuse reentry; retain common owner");
 nwsa_native.active=true;
 if(nwsa_source_owner()){
  nwsa_native.active=false;
  printf("NORD2 WIFI WFSYS: skipped=1 reason=persistent-owner-or-wake-conflict\n");
  return;
 }
 /* Mapping alone does not access the powered-off WFSYS register aperture. */
 nwsa_native.brcm=ml_io_map(0x1800e000,0x1000);
 nwsa_native.local=ml_io_map(0x184b0000,0x1000);
 if(!nwsa_native.brcm||!nwsa_native.local){
  nwsa_native.active=false;
  printf("NORD2 WIFI WFSYS: skipped=1 reason=map\n");return;
 }
 int status=nwsa_start(&nwsa_native.diag,&nwsa_xnu_hw,0,1);
 struct nws_state *s=&nwsa_native.diag.state;
 /* Only software result fields after the final AP sleep write. */
 printf("NORD2 WIFI WFSYS: status=%d stage=%u local_id=%u cpu_held=%u balanced=%u halt=%u dirty=%u unverified=%u error=%d\n",
  status,s->stage,s->local_id,s->cpu_held,s->balanced,s->halt,s->dirty,s->unverified,s->error);
 printf("NORD2 WIFI WFSYS OWNER: ap=%u wf=%u sem=%u sem_hw=%u sem_pending=%u clock=%u power=%u protection=%u asleep=%u\n",
  s->ap_vote,s->wf_vote,s->sem20,nwsa_native.diag.sem20owned,nwsa_native.diag.sem20pending,
  s->clock_vote,s->power,s->protection,nwsa_native.diag.asleep);
 printf("NORD2 WIFI WFSYS ID: conn=%#x local=%#x clock=%#x irq=%#x initial_afe=%#x final_power=%#x\n",
  s->cfg_id,s->wfsys_id,nwsa_native.diag.clock_last,nwsa_native.diag.irq_last,s->initial_afe,s->final_power);
 printf("NORD2 WIFI WFSYS COLD: valid=%#x cpu=%#x top=%#x power=%#x bgfcpu=%#x bgftop=%#x\n",
  s->cold_valid,s->cold[0],s->cold[1],s->cold[2],s->cold[3],s->cold[4]);
 printf("NORD2 WIFI WFSYS POLLS: ap=%u wf=%u cfg=%u power=%u protection=%u id=%u off=%u sem=%u sem_off=%u clock=%u\n",
  s->ap_ack_polls,s->wf_ack_polls,s->cfg_polls,s->power_polls,s->prot_polls,
  s->id_polls,s->off_polls,s->sem_tries,s->reverse_sem_tries,nwsa_native.diag.clock_polls);
 if(s->halt||s->dirty||s->unverified||s->ap_vote||s->wf_vote||s->sem20||
    s->clock_vote||s->power||nwsa_native.diag.sem20owned||nwsa_native.diag.sem20pending)
  panic("NORD2 WIFI WFSYS: uncertain stage%u; retain resources; manual reset",s->stage);
 nwsa_native.active=false;
}
