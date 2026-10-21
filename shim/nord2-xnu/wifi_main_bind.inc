#include "failure_drain.inc"
/* PRIVATE coordinator. Include after wfdma_bind.inc; external firmware/PHY
 * implementations are separate TUs. Hook immediately after nppa_after_rom. */
#include "wifi_main_fw_xnu.h"
#include "wifi_main_phy_xnu.h"
static struct {unsigned attempted,stage,ready,halt;int error;unsigned original_native_error,original_native_halt;}nwfn_main;
static void nwfn_main_failure(int error)
{
 nwfn_main.original_native_error=nwfn_native.error;
 nwfn_main.original_native_halt=nwfn_native.halt;
 nwfn_main.error=error;nwfn_main.halt=1;
 nwfn_stop(&nwfn_native,21);
 nfp_native.halt=1;nfp_native.state.halt=1;
 nfa_native.exchange.halt=1;
}
static void nwfn_main_arm(unsigned stage,const char *action)
{
 nwfn_main.stage=stage;
 printf("NORD2 WIFI MAIN ARM: stage=%u action=%s events=%u finish=%u ap=%u wf=%u driver=%u\n",
  stage,action,nwfn_native.init.transport.completed,
  nwfn_native.init.transport.finish_phase,nwfn_native.ap_owned,
  nwfn_native.wf_owned,nwfn_native.init.driver_owned);
}
static void nwfn_main_diagnostics(void)
{
 printf("NORD2 WIFI MAIN PRESPAN: reason=%u same=%#x sync=%#x valid=%u native_before=%u/%u span_native=%u\n",
  nfp_native.pre_span_reason,nfp_native.same_bits,nfp_native.pre_sync,
  nfp_native.pre_sync_valid,nwfn_main.original_native_error,
  nwfn_main.original_native_halt,nfp_native.original_native_error);
 printf("NORD2 WIFI MAIN PREORDER: index=%u events=%u finish=%u coex=%u prepared=%u\n",
  nfp_native.pre_span_index,nwfn_native.init.transport.completed,
  nwfn_native.init.transport.finish_phase,nfp_native.coex_attempted,nfp_native.prepared);
 const struct nwi_state *i=&nwfn_native.init;
 const struct nwt_state *t=&i->transport;
 const struct nfp_state *f=&nfp_native.state;
 const struct nfa_exchange_state *p=&nfa_native.exchange;
 printf("NORD2 WIFI MAIN NATIVE: stage=%u error=%u halt=%u live=%#x clock=%#x irq=%#x gic=%#x ap=%u wf=%u driver=%u\n",
  nwfn_native.stage,nwfn_native.error,nwfn_native.halt,nwfn_native.live_valid,
  nwfn_native.clock_raw,nwfn_native.irq_raw,nwfn_native.gic_enabled,
  nwfn_native.ap_owned,nwfn_native.wf_owned,i->driver_owned);
 printf("NORD2 WIFI MAIN INIT: phase=%u error=%u halt=%u dirty=%u sched=%u/%u/%u reg=%u memory=%u idle=%u\n",
  i->phase,i->error,i->halt,i->dirty,i->scheduler.complete,
  i->scheduler.operations,i->scheduler.error,i->reg_writes,i->memory_writes,i->idle_polls);
 printf("NORD2 WIFI MAIN IRQ: ena0=%#x/%#x ena1=%#x/%#x stat0=%#x/%#x stat1=%#x/%#x valid=%#x/%#x\n",
  i->irq_initial[0],i->irq_final[0],i->irq_initial[1],i->irq_final[1],
  i->irq_status_initial[0],i->irq_status_final[0],i->irq_status_initial[1],
  i->irq_status_final[1],i->irq_status_initial_valid,i->irq_status_final_valid);
 printf("NORD2 WIFI MAIN TRANSPORT: error=%u halt=%u submit=%u events=%u finish=%u polls=%u indices=%u/%u bytes=%u/%u/%u\n",
  t->error,t->halt,t->submit_attempted,t->completed,t->finish_phase,t->polls,
  t->tx_index,t->rx_index,t->rx_descriptor_bytes,t->rx_mac_bytes,t->rx_init_bytes);
 printf("NORD2 WIFI MAIN TXDESC: addr=%#llx used=%u didx=%u valid=%#x mismatch=%#x wait_ddone=%u consumed_zero=%u\n",
  (unsigned long long)t->tx_descriptor_address,t->tx_used,t->tx_didx,
  t->tx_descriptor_valid,t->tx_mismatch,t->tx_ddone_waits,t->tx_consumed_zero);
 printf("NORD2 WIFI MAIN TXWB: constructed=%u observed=%u ddone=%u consumed_zero=%u software_used=%u\n",
  !!(t->tx_descriptor_valid&1u),!!(t->tx_descriptor_valid&2u),
  !!((t->tx_descriptor_valid&2u)&&(t->tx_observed[1]&(1u<<31))),
  t->tx_consumed_zero,t->tx_used);
 printf("NORD2 WIFI MAIN TXSUBMIT: raw=%#x/%#x/%#x/%#x\n",
  t->tx_submitted[0],t->tx_submitted[1],t->tx_submitted[2],t->tx_submitted[3]);
 printf("NORD2 WIFI MAIN TXOBSERVED: raw=%#x/%#x/%#x/%#x\n",
  t->tx_observed[0],t->tx_observed[1],t->tx_observed[2],t->tx_observed[3]);
 printf("NORD2 WIFI MAIN DMASTATUS: raw=%#x/%#x valid=%#x\n",
  t->irq_status[0],t->irq_status[1],t->irq_status_valid);
 printf("NORD2 WIFI MAIN SPANS: stage=%u completed=%u next=%u dirty=%u halt=%u words=%u/%u remaps=%u barriers=%u error=%d\n",
  f->stage,f->completed,f->next,f->dirty,f->halt,f->words_written,
  f->words_read,f->remaps,f->barriers,f->error);
 printf("NORD2 WIFI MAIN SPANOBS: target=%#x address=%#x expected=%#x observed=%#x ready=%u raw=%#x polls=%u\n",
  f->last_target,f->last_address,f->last_expected,f->last_observed,
  nfp_native.ready,nfp_native.ready_raw,nfp_native.ready_polls);
 printf("NORD2 WIFI MAIN PHY: attempted=%u event=%u backup=%u halt=%u rx=%u fields=%u eid=%#x response=%#x status=%#x\n",
  p->attempted,p->event_valid,p->backup_valid,p->halt,(unsigned)p->actual_rx_bytes,
  p->actual_rx_bytes>=46,p->rx[28],p->rx[44],p->rx[45]);
 printf("NORD2 WIFI MAIN PHYBACKUP: valid=%u offset=%#x bytes=%u copied=%u reads=%u halt=%u error=%d\n",
  p->backup.valid,p->backup.emi_offset,p->backup.emi_bytes,p->backup.bytes_copied,
  p->backup.reads,p->backup.halt,p->backup.error);
}
static void nwfn_after_pool(void)
{
 unsigned opt=0;int result=0;
 PE_parse_boot_argn("nord2wifimain",&opt,sizeof(opt));
 if(opt!=1||nwfn_main.attempted)return;
 nwfn_main.attempted=1;
 /* Refuse any prior uncertain lifetime, rather than making a new vote. */
 if(nppa_native.active||nppa_native.probe.state.stage!=NPP_DONE||
    !nppa_native.probe.state.verified||nppa_native.probe.state.halt||
    nppa_native.probe.state.uncertain||nppa_native.probe.state.error||
    nwfn_native.halt||nfp_native.halt||nfp_native.state.halt||
    nfa_native.exchange.halt){result=-1;goto failure;}
 nwfn_main_arm(1,"prepare");
 if((result=nwfn_prepare())||
    (result=nfa_xnu_prepare(&nfa_native))||
    (result=nfp_xnu_prepare(npa_native.cfg,npa_native.host)))goto failure;
 nwfn_main_arm(2,"fresh-wake-and-wfdma");
 result=nwfn_start();
 if(result==1)goto report; /* complete inherited refusal, already asleep */
 if(result)goto failure;
 nwfn_main_arm(3,"patch-copy");
 if((result=nfp_xnu_span(0))||(result=nfp_xnu_span(1)))goto failure;
 nwfn_main_arm(4,"patch-finish");
 if((result=nwfn_finish_command(0x40)))goto failure;
 nwfn_main_arm(5,"force-calibration");
 if((result=nfa_xnu_run()))goto failure;
 nwfn_main_arm(6,"pccif");
 if((result=nfp_xnu_coex()))goto failure;
 nwfn_main_arm(7,"n9-copy");
 for(unsigned span=2;span<7;span++)if((result=nfp_xnu_span(span)))goto failure;
 nwfn_main_arm(8,"n9-finish-and-ready");
 if((result=nfp_xnu_finish())||!nfp_native.ready||
    nwfn_native.init.transport.completed!=3||
    nwfn_native.init.transport.finish_phase!=2){if(!result)result=-2;goto failure;}
 nwfn_main.stage=9;nwfn_main.ready=1;goto report;
failure:
 nwfn_main_failure(result?result:-3);
report:
 /* Captured software only. Never read off-domain state after refusal sleep. */
 printf("NORD2 WIFI MAIN: status=%d stage=%u ready=%u halt=%u error=%d events=%u finish=%u\n",
  result,nwfn_main.stage,nwfn_main.ready,nwfn_main.halt,nwfn_main.error,
  nwfn_native.init.transport.completed,nwfn_native.init.transport.finish_phase);
 printf("NORD2 WIFI MAIN OWNER: ap=%u wf=%u driver=%u dma=%u asleep=%u balanced=%u native_halt=%u transport_halt=%u\n",
  nwfn_native.ap_owned,nwfn_native.wf_owned,nwfn_native.init.driver_owned,
  nwfn_native.init.initialized,nwfn_native.asleep,nwfn_native.balanced,
  nwfn_native.halt,nwfn_native.init.transport.halt);
 nwfn_main_diagnostics();
 if(nwfn_main.halt&&(nwfn_native.ap_owned||nwfn_native.wf_owned||
    nwfn_native.init.dirty||nwfn_native.init.initialized||nfp_native.state.dirty||
    nfp_native.coex_attempted||(nppa_native.probe.state.halt&&
    (nppa_native.probe.state.ap_owned||nppa_native.probe.state.wf_owned||
     nppa_native.probe.state.set_attempted||nppa_native.probe.state.mapped||
     nppa_native.probe.state.zero_attempted))))
 {
  nwm_failure_drain();
  panic("NORD2 WIFI MAIN: uncertain retained radio state; manual reset required");
 }
}
