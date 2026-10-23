/* One same-worker lease; the opt-in flag itself never grants MMIO authority. */
#include "wifi_basic_config.h"
static struct {struct nbc_state state;unsigned attempted,enabled;}nbc_native;
#ifdef NCT_AFTER_BASIC
#include "wifi_normal_transport_bind.inc"
#define NCT_OWNER_WINDOW(n) (nct_native_owner_window(n)||nst_native_owner_window(n)||nst_native_probe_window(n))
#else
#define NCT_AFTER_BASIC() 0
#define NCT_OWNER_WINDOW(n) 0
#endif
static int ncd_normal_owner_window(const struct nwfn_native*n)
{
 return n==&nwfn_native&&nbc_native.enabled==1&&nbc_native.attempted==1&&
  ncd_native.active==1&&!ncd_native.parked&&!ncd_native.halt&&
  ncd_native.worker==current_thread()&&
  (nbc_lease_window(&nbc_native.state,&ncd_native.diag,&nwfn_native.init,&nfp_native)||
   NCT_OWNER_WINDOW(n));
}
static void nbc_report(int result)
{
 const struct nbc_state*s=&nbc_native.state;const struct ncp_result*c=&s->verified_cap;
 printf("NORD2 WIFI BASIC: status=%d phase=%u complete=%u skipped=%u reason=%u halt=%u error=%u submit=%u txdone=%u polls=%u debug=%u rearmed=%u callbacks=%u publications=%u\n",
  result,s->phase,s->completed,s->skipped,s->skip_reason,s->halt,s->error,s->submit_attempted,s->tx_done,s->polls,s->debug_events,s->rearmed,s->callbacks,s->publications);
 printf("NORD2 WIFI BASIC ASYNC: events=%u sleepy=%u state_valid=%u state=%u\n",
  s->debug_events+s->sleepy_events,s->sleepy_events,!!s->sleepy_events,s->last_sleepy);
 if(s->prepared)printf("NORD2 WIFI BASIC TX: cid=%#x seq=%u bytes=%u from=%u to=%u completed=%u didx=%u control=%#x consumed=%u ack_expected=0 firmware_ack=0\n",
  2,s->sequence,NNI_BASIC_BYTES,s->tx_index,(s->tx_index+1)%1024,nwfn_native.init.transport.completed,s->tx_didx,s->tx_control,s->tx_consumed);
 if(s->prepared)printf("NORD2 WIFI BASIC TXDESC: observed=%u address=%#llx expected=%#llx software_used=%u raw=%#x/%#x/%#x/%#x\n",
  s->tx_observed,(unsigned long long)s->tx_descriptor_address,(unsigned long long)s->tx_expected_buffer,NNI_BASIC_BYTES,
  s->tx_raw[0],s->tx_raw[1],s->tx_raw[2],s->tx_raw[3]);
 printf("NORD2 WIFI BASIC RESOURCE: valid=%u version=%#x pse=%u/%u ple=%u/%u pages=%u/%u extra=%u debit=%u/%u reserved=%u remaining=%u\n",
  c->decoded&&c->resources_supported,c->resource_version,c->resources[0],c->resources[1],c->resources[2],c->resources[3],
  (unsigned)(uint16_t)c->resources[4],(unsigned)(uint16_t)(c->resources[4]>>16),c->resources[5],
  s->budget.reserved?s->plan.pse_cost:0,s->budget.reserved?s->plan.ple_cost:0,s->budget.reserved,s->budget.pse_remaining);
 printf("NORD2 WIFI BASIC CAPS: valid=%u sku=%u efuse=%u country=%#x phy=%u band5=%u bw=%u nss=%u path=%#x known=%u unknown=%u\n",
  s->metadata_valid,s->caps.sku_present,s->caps.efuse_valid,s->caps.efuse_country,s->caps.phy_present,s->caps.band5,s->caps.max_bw,s->caps.nss,s->caps.path,c->known,c->unknown);
 const struct ncd_event*h=&s->last_debug;
 printf("NORD2 WIFI BASIC DEBUG: valid=%u index=%u kind=%u type=%#x eid=%#x seq=%u bytes=%u declared=%u/%u inner=%#x id=%u format=%u length=%u\n",
  h->header_valid,s->debug_index,h->kind,h->rx_class,h->eid,h->sequence,h->bytes,h->rxd_bytes,h->event_bytes,h->inner_type,h->debug_id,h->debug_type,h->debug_bytes);
 printf("NORD2 WIFI BASIC DEBUGHEADER: valid=%u raw=%#x/%#x/%#x/%#x/%#x/%#x/%#x/%#x/%#x\n",
  h->header_valid,ncd_header_word(h,0),ncd_header_word(h,1),ncd_header_word(h,2),ncd_header_word(h,3),ncd_header_word(h,4),ncd_header_word(h,5),ncd_header_word(h,6),ncd_header_word(h,7),ncd_header_word(h,8));
 printf("NORD2 WIFI BASIC OWNER: worker=%u native_error=%u native_halt=%u init_halt=%u transport_halt=%u firmware_halt=%u completed=%u lease=%u\n",
  ncd_native.worker==current_thread(),nwfn_native.error,nwfn_native.halt,nwfn_native.init.halt,nwfn_native.init.transport.halt,nfp_native.halt,nwfn_native.init.transport.completed,ncd_normal_owner_window(&nwfn_native));
}
static int nbc_after_cap(void)
{
 unsigned enabled=0;PE_parse_boot_argn("nord2wifibasic",&enabled,sizeof(enabled));
 if(enabled!=1)return 0;
 if(!__sync_bool_compare_and_swap(&nbc_native.attempted,0,1))return 0;
 nbc_native.enabled=1;
 int result=nbc_prepare(&ncd_native.diag,&nwfn_native.init,&nfp_native,&nbc_native.state);
 if(!result){
  printf("NORD2 WIFI BASIC ARM: cid=%#x seq=%u bytes=%u deadline_us=%u rx_budget=%u\n",
   2,nbc_native.state.sequence,NNI_BASIC_BYTES,3000000u,NBC_RX_BUDGET);
  result=nbc_run(&ncd_io,&ncd_native,&nbc_native.state);
 }
 if(result<0){
  nbc_native.state.halt=1;ncd_native.halt=1;nwfn_native.halt=1;
  nwfn_native.init.halt=1;nwfn_native.init.transport.halt=1;nfp_native.halt=1;
 }
 nbc_report(result); /* captured-only after failure, no backend reads */
 if(result<0)return -1;
 if(!result&&NCT_AFTER_BASIC()<0)return -1;
 unsigned ios=0;PE_parse_boot_argn("nord2wifiios",&ios,sizeof(ios));
 printf("NORD2 WIFI BASIC WORKER: parking=%u retained=1 callbacks_complete=1\n",ios==1?0u:1u);
 return 0;
}
