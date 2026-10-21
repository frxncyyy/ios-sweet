#ifndef NORD2_PRIVATE_ROM_POWERED_H
#define NORD2_PRIVATE_ROM_POWERED_H
#include <stdint.h>
#include "wifi_rom_xnu.h"

enum nrpc_stage {
 NRPC_PREWAKE=0, NRPC_AP_ACK=1, NRPC_AP_READY=2,
 NRPC_WF_ACK=3, NRPC_COMMON_READY=4, NRPC_CPU_HELD=5,
 NRPC_CLOCKS=6, NRPC_POWERED=7, NRPC_PROTECTION=8,
 NRPC_LOCAL_ID=9, NRPC_ROM_PLACE=10, NRPC_ROM_CONFIG=11,
 NRPC_CPU_RELEASE=12, NRPC_ROM_WAIT=13,
 NRPC_RUNNING_CONFIG=14, NRPC_RELEASE=15, NRPC_RUNNING=16
};
struct nrpc_ops {
 int (*proof)(void *,unsigned);
 int (*read)(void *,uint32_t,uint32_t *);
 int (*write)(void *,uint32_t,uint32_t);
 int (*delay_us)(void *,unsigned);
 int (*readable)(void *);
 int (*sema20_try)(void *);
 int (*sema20_release)(void *);
 /* These must execute native policy/placement on the exact `rom` object.
  * They are operations, not caller-supplied permission predicates. */
 int (*rom_policy)(void *,struct nrb_native *);
 int (*rom_place)(void *,struct nrb_native *);
};
struct nrpc_state {
 unsigned entered,stage,halt,balanced,dirty,unverified;
 unsigned ap_vote,wf_vote,sem20,clock_vote,power,protection,local_id,cpu_held;
 unsigned policy_attempted,policy_accepted,rom_placed;
 unsigned cpu_release_attempted,cpu_released,rom_ready;
 unsigned ap_ack_polls,wf_ack_polls,cfg_polls,power_polls,prot_polls,id_polls,rom_polls;
 unsigned sem_tries,release_sem_tries;
 unsigned cold_valid;uint32_t cold[5];
 uint32_t cfg_id,wfsys_id,initial_afe,final_power,rom_done;
 int error;
};
/* Fresh one-shot owner; prepared ROM native object is retained by caller.
 * Failure at/after policy leaves owned resources for manual reset. Success
 * leaves WFSYS powered and CPU running, with AP/WF/PLL votes released. */
int nrpc_run(const struct nrpc_ops *,void *,unsigned,struct nrb_native *,struct nrpc_state *);
/* Native placement receipt: actual four-span chunk counts, same owner object. */
int nrpc_receipt_ok(const struct nrb_native *);
#endif
