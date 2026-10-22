/* Include after actual nbc_native definition in the owning CAP worker TU.
 * Root coordinator supplies the immutable reviewed config+scan recipe/hooks;
 * no flag, caller receipt clone or pure plan grants this native authority. */
#include "wifi_normal_transport.h"
#ifndef NCT_SCAN_RETURN_VALID
#define NCT_SCAN_RETURN_VALID(s) (!(s)->scan_return)
#endif
static struct {struct nct_session session;unsigned attempted,active;}nct_native;
static int nct_native_owner_window(const struct nwfn_native*n)
{
 return n==&nwfn_native&&nct_native.attempted==1&&nct_native.active==1&&
  ncd_native.active==1&&!ncd_native.parked&&!ncd_native.halt&&
  ncd_native.worker==current_thread()&&
  NCT_SCAN_RETURN_VALID(&nct_native.session)&&
  nct_native.session.access.session==&nct_native.session&&
  nct_native.session.access.context==&ncd_native&&nct_native.session.access.hardware==&ncd_io&&
  nct_lease_window(&nct_native.session,&nbc_native.state,&ncd_native.diag,&nwfn_native.init,&nfp_native);
}
/* Captured-only terminal report; raw words are metadata, never addresses used
 * for reads/writes. No firmware acceptance/ACK is inferred from DDONE. */
static void nct_native_report(int result)
{
 const struct nct_session*s=&nct_native.session;
 printf("NORD2 WIFI NORMAL: status=%d phase=%u prepared=%u halt=%u error=%u refusal=%u cursor=%u count=%u complete=%u current_cid=%#x events=%u scan_complete=%u service_timed_out=%u event_limit=%u callbacks=%u pumps=%u\n",
  result,s->phase,s->prepared,s->halt,s->error,s->refusal,s->cursor,s->count,s->completed,
  s->cursor<s->count?s->entry[s->cursor].cid:0,s->events,s->scan_complete,s->service_timed_out,s->event_limit_reached,s->callbacks,s->pump_calls);
 printf("NORD2 WIFI NORMAL CREDIT: pse_total=%u basic_spent=%u capacity=%u available=%u reserved=%u acquired=%u released=%u ple_debit=0 firmware_ack=0\n",
  s->pse_total,s->basic_spent,s->capacity,s->available,s->reserved,s->acquired,s->released);
 printf("NORD2 WIFI NORMAL BOUNDS: started=%llu last=%llu deadline=%llu command_deadline=%llu service_deadline=%llu command_us=%u event_limit=%u pump_limit=16\n",
  (unsigned long long)s->started,(unsigned long long)s->last,(unsigned long long)s->deadline,
  (unsigned long long)s->command_deadline,(unsigned long long)s->service_deadline,3000000u,NCT_EVENT_LIMIT);
 for(unsigned k=0;k<s->count&&k<NCT_COMMAND_LIMIT;k++){
  const struct nct_record*q=&s->record[k];
  printf("NORD2 WIFI NORMAL COMMAND: n=%u cid=%#x seq=%u bytes=%u from=%u to=%u didx=%u polls=%u attempted=%u published=%u released=%u txdone=%u observed=%u raw=%#x/%#x/%#x/%#x\n",
   k,q->cid,q->sequence,q->bytes,q->from,q->to,q->didx,q->polls,q->publication_attempted,q->published,q->credit_released,q->tx_done,q->observed,q->raw[0],q->raw[1],q->raw[2],q->raw[3]);
 }
}
static int nct_native_run(const struct nct_recipe*recipe,const struct nct_hooks*hooks)
{
 if(!ncd_native.active||ncd_native.parked||ncd_native.halt||ncd_native.worker!=current_thread()||
    !nbc_native.enabled||nbc_native.attempted!=1)return -1;
 if(!__sync_bool_compare_and_swap(&nct_native.attempted,0,1))return -1;
 int result=nct_prepare(&nbc_native.state,recipe,&nct_native.session);
 if(result){nct_native_report(result);return result;} /* pure refusal: no IO */
 nct_native.active=1;
 result=nct_run(&ncd_io,&ncd_native,hooks,&nct_native.session);
 if(result<0){
  ncd_native.halt=1;nwfn_native.halt=1;nwfn_native.init.halt=1;
  nwfn_native.init.transport.halt=1;nfp_native.halt=1;
 }
 nct_native.active=0;
 nct_native_report(result);
 /* Root caller performs captured-only report; on error uses existing actual
  *30s scheduler drain/panic. Success must not resume an unrelated old callback. */
 return result;
}
