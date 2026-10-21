/* PRIVATE proposed XNU binding. Fresh ROM owner only; no old wake object reuse. */
#include "wifi_rom_powered_adapter.h"
static struct {
 struct nrpa diag;
 struct nrb_native rom;
 vm_offset_t brcm,local,dynamic,rom_done;
 bool active,prepare_attempted,prepared,run_attempted;
 int prepare_status;
} nrpa_native;

static int nrpa_source_owner(void)
{
 if(!nrpa_native.active||!npa_persistent_lifetime||!npa_native.outer_retained||
    !npa_native.state.entered||!npa_native.state.retained||npa_native.state.fatal||
    npa_native.state.hw_handoff!=2||npa_native.state.result.phase!=10||
    !npa_native.state.result.retained||npa_native.state.result.halt||
    npa_native.state.sema11||npa_native.state.sema19||npa_native.state.spi_inflight||
    !ni_saved||!ncc_fresh_owned||!nw.saved||!nw.domain_touched||ncc_latched_dirty||
    nawa_native.active||nawa_native.diag.result.entered||
    nwsa_native.active||nwsa_native.diag.state.entered)return -1;
 if(!nw.spm||!nw.infra||!nw.gals||!npa_native.host||!npa_native.cfg||
    !npa_native.rgu||!npa_native.afe||!npa_native.debug||!npa_native.sem)return -1;
 return 0;
}
static int nrpa_xnu_owner(void *cookie)
{
 (void)cookie;
 return nrpa_source_owner()||!nrpa_native.prepared||
  !nrpa_native.rom.enabled||!nrpa_native.brcm||!nrpa_native.local||
  !nrpa_native.dynamic||!nrpa_native.rom_done;
}
static vm_offset_t nrpa_xnu_base(uint32_t p)
{
 switch(p){
 case 0x10006304:case 0x1000616c:case 0x10006170:return nw.spm;
 case 0x10001228:case 0x10001724:return nw.infra;
 case 0x1020e504:return nw.gals;
 case 0x180601a0:case 0x180601a4:case 0x18060184:
 case 0x18060000:case 0x180602d4:case 0x180602cc:return npa_native.host;
 case 0x18001000:case 0x18001620:case 0x18001624:
 case 0x18001120:return npa_native.cfg;
 case 0x18000010:case 0x18000000:case 0x18000018:
 case 0x18000014:case 0x18000008:return npa_native.rgu;
 case 0x18003008:return npa_native.afe;
 case 0x1800f000:return npa_native.debug;
 case 0x18072050:case 0x18072250:return npa_native.sem;
 case 0x1800e06c:return nrpa_native.brcm;
 case 0x184b0010:return nrpa_native.local;
 case 0x185001dc:case 0x185001e0:return nrpa_native.dynamic;
 case 0x184c1604:return nrpa_native.rom_done;
 default:return 0;
 }
}
static int nrpa_xnu_read(void *cookie,uint32_t p,uint32_t *v)
{
 (void)cookie;
 if(!v||nrpa_native.diag.asleep)return -1;
 switch(p){
 case 0x18072250:case 0x18001120:case 0x185001dc:
 case 0x185001e0:case 0x1800f000:return -1;
 default:break;
 }
 vm_offset_t b=nrpa_xnu_base(p);if(!b)return -1;
 *v=ni_rd(b,p&0xfff);return 0;
}
static int nrpa_xnu_write(void *cookie,uint32_t p,uint32_t v)
{
 (void)cookie;if(nrpa_native.diag.asleep)return -1;
 struct nrpc_state *s=&nrpa_native.diag.state;
 switch(p){
 case 0x180601a0:
  if(!((s->stage==NRPC_PREWAKE&&v==1)||
       (s->stage==NRPC_RELEASE&&v==0&&s->rom_ready&&
        !s->wf_vote&&!s->clock_vote&&!s->sem20)))return -1;
  break;
 case 0x18000010:
  if(v&1u){
   if(s->stage!=NRPC_CPU_RELEASE||!s->cpu_release_attempted||
      !nrpc_receipt_ok(&nrpa_native.rom)||!s->cpu_held||s->cpu_released)return -1;
   /* Captured state only, before the irreversible CPU-release write. */
   printf("NORD2 WIFI ROM CPU ARM: stage=%u attempted=%u placed=%u policy=%u/%u raw=%#llx/%#llx writes=%u reads=%u barriers=%u\n",
    s->stage,s->cpu_release_attempted,nrpa_native.rom.state.placed,
    nrpa_native.rom.state.accepted,nrpa_native.rom.state.issued,
    (unsigned long long)nrpa_native.rom.state.raw[0][0],
    (unsigned long long)nrpa_native.rom.state.raw[1][0],
    nrpa_native.rom.state.writes,nrpa_native.rom.state.reads,
    nrpa_native.rom.state.barriers);
  }else if(s->stage!=NRPC_COMMON_READY)return -1;
  break;
 case 0x18000000:
  if(s->stage!=NRPC_CLOCKS||(v&0xffff0000)!=0x57460000||
     (v&0x80u)==0)return -1;
  break;
 case 0x18072250:
  if(v!=1||(s->stage!=NRPC_CPU_HELD&&s->stage!=NRPC_RELEASE))return -1;
  break;
 case 0x18001120:
  if(s->stage!=NRPC_ROM_CONFIG||!s->rom_placed||v!=0x89040000u)return -1;
  break;
 case 0x185001dc:case 0x185001e0:
  if(s->stage!=NRPC_ROM_CONFIG||!s->rom_placed||v)return -1;
  break;
 case 0x1800f000:
  if(s->stage!=NRPC_RUNNING_CONFIG||v!=0x32c8001cu||!s->rom_ready)return -1;
  break;
 case 0x180601a4:
  if(s->stage!=NRPC_AP_READY&&s->stage!=NRPC_RELEASE)return -1;
  break;
 case 0x18060000:
  if(s->stage!=NRPC_AP_ACK&&s->stage!=NRPC_WF_ACK&&
     s->stage!=NRPC_CPU_HELD&&s->stage!=NRPC_POWERED&&
     s->stage!=NRPC_RELEASE)return -1;
  break;
 case 0x18001620:case 0x18001624:
  if(s->stage!=NRPC_POWERED)return -1;
  break;
 case 0x18000018:
  if(s->stage!=NRPC_ROM_CONFIG||(v&1u))return -1;
  break;
 case 0x18003008:
  if(s->stage!=NRPC_CPU_HELD&&s->stage!=NRPC_RELEASE)return -1;
  break;
 case 0x1800e06c:
  if(s->stage!=NRPC_PROTECTION)return -1;
  break;
 default:return -1;
 }
 vm_offset_t b=nrpa_xnu_base(p);if(!b)return -1;
 ni_wr(b,p&0xfff,v);NCC_DEVICE_BARRIER();return 0;
}
static int nrpa_xnu_delay(void *cookie,unsigned us)
{(void)cookie;if(nrpa_native.diag.asleep)return -1;delay(us);return 0;}
static const struct nrpa_hw nrpa_xnu_hw={
 nrpa_xnu_owner,nrpa_xnu_read,nrpa_xnu_write,nrpa_xnu_delay
};
/* Proposed ni_thread insertion: AFTER ni_wifi_snapshot(), before the radio
 * power probe. This may sleep on UFS; no WFSYS ownership or SMC yet. */
static void nrpa_prepare_before_power(void)
{
 unsigned opt=0;PE_parse_boot_argn("nord2wifirom",&opt,sizeof(opt));
 if(opt!=1)return;
 if(nrpa_native.prepare_attempted)panic("NORD2 WIFI ROM: prepare reentry");
 nrpa_native.prepare_attempted=true;
 nrpa_native.prepare_status=nrb_xnu_prepare(&nrpa_native.rom);
 nrpa_native.prepared=nrpa_native.prepare_status==0&&nrpa_native.rom.enabled&&
  nrpa_native.rom.state.prepared&&nrpa_native.rom.state.phase==1;
 printf("NORD2 WIFI ROM PREP: status=%d prepared=%u supply_error=%d claim_source=%u\n",
  nrpa_native.prepare_status,nrpa_native.prepared,nrpa_native.rom.supply_error,
  nrpa_native.rom.claim.source);
}
/* Proposed retained phase-10 call. Old `nord2wifiwake` and
 * `nord2wifiwfsys` flags must be absent from the planned image. */
static void nrpa_after_persistent(void)
{
 unsigned opt=0;PE_parse_boot_argn("nord2wifirom",&opt,sizeof(opt));
 if(opt!=1)return;
 if(!nrpa_native.prepare_attempted||!nrpa_native.prepared){
  printf("NORD2 WIFI ROM: skipped=1 reason=prepare status=%d\n",nrpa_native.prepare_status);
  return;
 }
 if(nrpa_native.active||nrpa_native.run_attempted||nrpa_native.diag.state.entered)
  panic("NORD2 WIFI ROM: reentry; retain common owner");
 nrpa_native.run_attempted=true;
 nrpa_native.active=true;
 if(nrpa_source_owner()){
  nrpa_native.active=false;
  printf("NORD2 WIFI ROM: skipped=1 reason=persistent-owner-or-wake-conflict\n");
  return;
 }
 /* Mapping is metadata only; no powered-off WFSYS register is read. AP2WF
  * uses retained CFG map, debug uses retained debug map. */
 nrpa_native.brcm=ml_io_map(0x1800e000,0x1000);
 nrpa_native.local=ml_io_map(0x184b0000,0x1000);
 nrpa_native.dynamic=ml_io_map(0x18500000,0x1000);
 nrpa_native.rom_done=ml_io_map(0x184c1000,0x1000);
 if(!nrpa_native.brcm||!nrpa_native.local||!nrpa_native.dynamic||!nrpa_native.rom_done){
  nrpa_native.active=false;
  printf("NORD2 WIFI ROM: skipped=1 reason=map\n");return;
 }
 int status=nrpa_start(&nrpa_native.diag,&nrpa_xnu_hw,0,&nrpa_native.rom,1);
 const struct nrpc_state *s=&nrpa_native.diag.state;
 /* Captured software results only. Final AP sleep has already occurred. */
 printf("NORD2 WIFI ROM: status=%d stage=%u policy=%u placed=%u cpu_attempt=%u cpu_release=%u done=%#x halt=%u error=%d\n",
  status,s->stage,s->policy_accepted,s->rom_placed,s->cpu_release_attempted,
  s->cpu_released,s->rom_done,s->halt,s->error);
 printf("NORD2 WIFI ROM OWNER: ap=%u wf=%u sem=%u sem_hw=%u pending=%u clock=%u power=%u protection=%u asleep=%u balanced=%u\n",
  s->ap_vote,s->wf_vote,s->sem20,nrpa_native.diag.sem20owned,nrpa_native.diag.sem20pending,
  s->clock_vote,s->power,s->protection,nrpa_native.diag.asleep,s->balanced);
 printf("NORD2 WIFI ROM RECEIPT: phase=%u accepted=%u issued=%u raw_valid=%u raw0=%#llx raw1=%#llx backup=%u writes=%u reads=%u barriers=%u\n",
  nrpa_native.rom.state.phase,nrpa_native.rom.state.accepted,nrpa_native.rom.state.issued,
  nrpa_native.rom.state.raw_valid,
  (unsigned long long)nrpa_native.rom.state.raw[0][0],
  (unsigned long long)nrpa_native.rom.state.raw[1][0],
  nrpa_native.rom.state.backup_complete,nrpa_native.rom.state.writes,
  nrpa_native.rom.state.reads,nrpa_native.rom.state.barriers);
 printf("NORD2 WIFI ROM OBS: conn=%#x local=%#x clock=%#x irq=%#x cold=%#x pwr=%#x polls=%u/%u/%u/%u\n",
  s->cfg_id,s->wfsys_id,nrpa_native.diag.clock_last,nrpa_native.diag.irq_last,
  s->cold_valid,s->final_power,s->ap_ack_polls,s->wf_ack_polls,s->id_polls,s->rom_polls);
 if(status||s->halt||s->dirty||s->unverified||!s->balanced||!s->rom_ready||
    !s->cpu_released||s->ap_vote||s->wf_vote||s->sem20||s->clock_vote||
    !s->power||!s->protection||nrpa_native.diag.sem20owned||
    nrpa_native.diag.sem20pending||!nrpa_native.diag.asleep)
  panic("NORD2 WIFI ROM: uncertain stage%u; retain resources; manual reset",s->stage);
 nrpa_native.active=false;
}
