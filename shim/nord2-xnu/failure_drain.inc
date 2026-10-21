/* Normal-thread-only drain before the existing terminal
 * panic. No radio/USB MMIO callback, recovery, retry or return to input init. */
static int nwm_drain_channel;
static unsigned nwm_drain_entered;
static void nwm_failure_drain(void)
{
 uint64_t deadline,now;unsigned waits=0;
 if(nwm_drain_entered)return;
 nwm_drain_entered=1;
 /* sched_prim.c preemption_enabled includes both preemption level and IRQs. */
 if(!preemption_enabled()){
  printf("NORD2 WIFI MAIN DRAIN: sleepable=0 attempted=0; panic next\n");return;
 }
 printf("NORD2 WIFI MAIN DRAIN: sleepable=1 retained_halt=1 ms=30000; panic pending\n");
 clock_interval_to_deadline(30000,NSEC_PER_MSEC,&deadline);
 /* Fixed absolute deadline: early wakeups cannot extend the drain. The
  * wake bound avoids an accidental busy loop if a wait unexpectedly fails. */
 while(waits<32){
  clock_get_uptime(&now);if(now>=deadline)break;
  if(!preemption_enabled())break;
  if(assert_wait_deadline(&nwm_drain_channel,THREAD_UNINT,deadline)!=THREAD_WAITING)break;
  waits++;thread_block(THREAD_CONTINUE_NULL);
 }
 printf("NORD2 WIFI MAIN DRAIN: waits=%u; panic now\n",waits);
}
