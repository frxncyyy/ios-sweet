#define main unused_service_main
#include "service-test.c"
#undef main
static unsigned failed_notices,retired_notices,expected_result;static uint64_t expected_ticket;
static void pending_complete(void*c,uint64_t epoch,uint64_t ticket,enum njs_notice notice){
 assert(c==(void*)2&&epoch==9&&ticket==expected_ticket);struct njs_snapshot out;assert(!nord2_wifi_join_copy_state(&out)&&out.ticket==ticket&&out.phase==NJS_FAILED&&out.result==expected_result&&!out.associated&&!out.data_ready);struct niwj_request zero={0};assert(!memcmp(&njs_handover.request,&zero,sizeof zero));
 assert(!pthread_mutex_trylock(&nss_native.lock->mutex));pthread_mutex_unlock(&nss_native.lock->mutex);
 if(notice==NJS_JOIN_FAILED)failed_notices++;else{assert(notice==NJS_RETIRED);retired_notices++;struct niwj_request q=request();uint64_t t=123;int r=nord2_wifi_join_enqueue(&q,(void*)2,9,pending_complete,&t);assert((r==1||r==2)&&t==123&&njs_handover.state==2);}
}
int main(void){nss_native.lock=IOLockAlloc();nss_native.powered=1;nss_native.power_epoch=1;njs_initialize_locked();
 for(unsigned mode=0;mode<8;mode++){
  memset(&nst_native,0,sizeof nst_native);memset(&nsr_native,0,sizeof nsr_native);njs_native.used=njs_native.active=njs_native.leave=1;njs_native.queued=njs_native.retiring=0;njs_native.native_generation=njs_native.ticket=1;njs_native.serial=1;clock_us=mode==7?UINT64_MAX-NJS_HANDOVER_BUDGET_US-1000:10000;nss_native.powered=1;nss_native.failed=0;
  struct niwj_request q=request();uint64_t t=123;unsigned prepared=prepared_calls,prior=failed_notices;
  njs_native.leave=0;assert(nord2_wifi_join_enqueue(&q,(void*)2,9,pending_complete,&t)==1&&t==123);njs_native.leave=1;
  if(mode==6){njs_native.serial=255;assert(nord2_wifi_join_enqueue(&q,(void*)2,9,pending_complete,&t)==1&&t==123&&!njs_handover.state);continue;}
  assert(!nord2_wifi_join_enqueue(&q,(void*)2,9,pending_complete,&t)&&t==2&&njs_handover.state==1&&njs_native.ticket==1&&njs_native.native_generation==1);expected_ticket=t;
  memset(&q,0,sizeof q);assert(njs_handover.request.pmk[0]==0x42);struct njs_snapshot out;assert(!nord2_wifi_join_copy_state(&out)&&out.ticket==2&&out.phase==NJS_QUEUED&&!out.result&&!out.associated&&!out.data_ready);
  q=request();t=123;assert(nord2_wifi_join_enqueue(&q,(void*)2,9,pending_complete,&t)==2&&t==123&&njs_native.serial==2);
  expected_result=2;if(mode==0){assert(!nord2_wifi_join_cancel(2));struct niwj_request z={0};assert(!memcmp(&z,&njs_handover.request,sizeof z));}if(mode==1){clock_us=njs_handover.deadline;expected_result=9;}if(mode==2)clock_us=njs_handover.admitted-1;if(mode==3){assert(!nord2_wifi_power_set(0));struct niwj_request z={0};assert(!memcmp(&z,&njs_handover.request,sizeof z));}if(mode==4)nst_native.state.halt=1;if(mode==5)nsr_native.bad=1;if(mode==7)clock_us=njs_handover.deadline-1;
  assert(njs_owner_work()==1&&failed_notices==prior+1&&retired_notices==prior+1&&!njs_handover.state&&prepared_calls==prepared&&njs_native.ticket==1&&njs_native.native_generation==1);
 }
 /* Inherited uncertain-DMA fatal path keeps callback ownership until panic,
  * but must not retain the next network secret while it drains. */
 memset(&nst_native,0,sizeof nst_native);memset(&nsr_native,0,sizeof nsr_native);nss_native.powered=1;njs_native.used=njs_native.active=njs_native.leave=1;njs_native.serial=njs_native.ticket=njs_native.native_generation=1;clock_us=20000;struct niwj_request q=request();uint64_t t=0;assert(!nord2_wifi_join_enqueue(&q,(void*)2,9,pending_complete,&t));unsigned before=failed_notices;assert(njs_fatal_owner(5)==-1&&njs_handover.state==1&&njs_handover.canceled&&njs_handover.cookie==(void*)2&&njs_handover.completion==pending_complete&&failed_notices==before);struct niwj_request zero={0};assert(!memcmp(&zero,&njs_handover.request,sizeof zero));niwj_clear(&njs_handover,sizeof njs_handover);
 assert(!scan_build_calls&&!scan_native_calls&&!scan_delivery_calls);IOLock*notice=njs_native.notice_lock;assert(!pthread_mutex_destroy(&notice->mutex));free(notice);assert(!pthread_mutex_destroy(&nss_native.lock->mutex));free(nss_native.lock);
 puts("PASS pending handover admission/busy/secret copy+wiping/cancel/deadline/rollback/OFF/uncertain halt/exhaustion/callback-reentry; no prepare or firmware success synthesized");return 0;}
