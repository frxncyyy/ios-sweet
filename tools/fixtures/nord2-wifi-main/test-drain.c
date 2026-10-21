#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <setjmp.h>
#define NSEC_PER_MSEC 1000000u
#define THREAD_UNINT 1
#define THREAD_WAITING 2
#define THREAD_CONTINUE_NULL 0
static uint64_t now,fixed;static unsigned waits,blocks,prints,enabled,early,wait_failure,lose_preemption;
static int preemption_enabled(void){return enabled;}
static void clock_interval_to_deadline(unsigned ms,unsigned scale,uint64_t *d){assert(ms==30000&&scale==NSEC_PER_MSEC);*d=fixed=now+30000;}
static void clock_get_uptime(uint64_t *n){*n=now;}
static int assert_wait_deadline(void *chan,int mode,uint64_t d){assert(chan&&mode==THREAD_UNINT&&d==fixed);waits++;return wait_failure?0:THREAD_WAITING;}
static void thread_block(void *cont){assert(!cont);blocks++;if(lose_preemption)enabled=0;if(early){early--;now++;}else now=fixed;}
static void capture(const char *format,...){(void)format;prints++;}
#define printf capture
#include "failure_drain.inc"
#undef printf
static void reset(void){now=100;fixed=0;waits=blocks=prints=early=wait_failure=lose_preemption=nwm_drain_entered=0;enabled=1;}
int main(void){
 reset();nwm_failure_drain();assert(waits==1&&blocks==1&&now==30100&&prints==2);
 nwm_failure_drain();assert(waits==1&&blocks==1);/* one use */
 reset();enabled=0;nwm_failure_drain();assert(!waits&&!blocks&&prints==1);
 reset();early=4;nwm_failure_drain();assert(waits==5&&blocks==5&&now==30100&&fixed==30100);
 reset();early=100;nwm_failure_drain();assert(waits==32&&blocks==32&&now==132&&fixed==30100);
 reset();wait_failure=1;nwm_failure_drain();assert(waits==1&&!blocks);
 reset();early=1;lose_preemption=1;nwm_failure_drain();assert(waits==1&&blocks==1&&now==101);
 puts("PASS actual drain helper model: fixed30s deadline, scheduler block, no radio/USB callbacks, one-use, unsafe-context refusal, early-wake/failed-wait budget. No runtime delivery guarantee.");
}
