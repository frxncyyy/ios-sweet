#ifndef NORD2_PRIVATE_WFSYS_H
#define NORD2_PRIVATE_WFSYS_H
#include <stdint.h>

/* Source-selected SOC3_0 WLAN power transaction, stopped before ROM placement.
 * A future native adapter must prove the retained common owner in software and
 * apply AP-only guards before WF wake; off-domain checks begin after WF ack. */
enum nws_stage {
 NWS_PREWAKE=0, NWS_AP_ACK=1, NWS_AP_READY=2,
 NWS_WF_ACK=3, NWS_COMMON_READY=4, NWS_CPU_HELD=5,
 NWS_CLOCKS=6, NWS_POWERED=7, NWS_PROTECTION=8,
 NWS_LOCAL_ID=9, NWS_REVERSE=10, NWS_ASLEEP=11
};
struct nws_ops {
 int (*proof)(void *,unsigned); /* 0=source-owned; software/AP-only PREWAKE */
 int (*read)(void *,uint32_t,uint32_t *);
 int (*write)(void *,uint32_t,uint32_t); /* ordered 32-bit completion */
 int (*delay_us)(void *,unsigned);
 int (*readable)(void *); /* selected conninfra readability after each wake ack */
 int (*sema20_try)(void *); /* 1=acquired,0=busy,<0=uncertain */
 int (*sema20_release)(void *); /* 0=confirmed, else uncertain */
};
struct nws_state {
 unsigned entered,stage,halt,balanced,dirty,unverified;
 unsigned ap_vote,wf_vote,sem20,clock_vote,power,protection,local_id,cpu_held;
 unsigned ap_ack_polls,wf_ack_polls,cfg_polls,power_polls,prot_polls,id_polls,off_polls;
 unsigned sem_tries,reverse_sem_tries;
 unsigned cold_valid;uint32_t cold[5];
 uint32_t cfg_id,wfsys_id,initial_afe,final_power;
 int error;
};
/* optin must equal 1; disabled/reentry performs zero callbacks.
 * Success reverses the CPU-held WFSYS power path, WF vote, then AP sleep.
 * Any uncertain callback/timeout after WF request retains all owned state;
 * no speculative rollback/replay, CPU release, DRAM, SMC or ROM operation. */
int nws_run(const struct nws_ops *,void *,unsigned,struct nws_state *);
#endif
