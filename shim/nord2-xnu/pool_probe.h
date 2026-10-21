#ifndef PRIVATE_NORD2_POOL_PROBE_H
#define PRIVATE_NORD2_POOL_PROBE_H
#include <stdint.h>
#include "dma_pool_contract.h"
#include "wifi_dma_preflight.h"

enum npp_stage { NPP_PREWAKE, NPP_AP_ACK, NPP_AP_READY, NPP_WF_ACK,
 NPP_LIVE, NPP_PREFLIGHT, NPP_SET1, NPP_SET0, NPP_MAP,
 NPP_ZERO, NPP_VERIFY, NPP_RELEASE, NPP_DONE };
struct npp_state {
 unsigned entered,stage,ap_owned,wf_owned,asleep,balanced,halt,uncertain;
 unsigned inherited_dma,claim_ready,live_valid,preflight_complete;
 unsigned set_attempted,set_accepted,raw_valid,possibly_locked;
 unsigned mapped,zero_attempted,zero_complete,verified,words_written,words_read;
 unsigned ap_polls,wf_polls,cfg_polls,local_polls,rom_polls;
 uint32_t cfg_id,local_id,rom_done,live[5],wf_initial;
 uint64_t raw[2][4];struct nwdp_state dma;
 int error;
};
struct npp_ops {
 /* ready ties the exact current-boot claim and completed nrpa/nrb receipt to
  * a new persistent common owner; proof rechecks that owner before every IO. */
 int (*ready)(void *,const struct ndp_claim *);
 int (*proof)(void *,unsigned);
 int (*read)(void *,uint32_t,uint32_t *);
 int (*write)(void *,uint32_t,uint32_t);
 int (*delay_us)(void *,unsigned);
 int (*readable)(void *);
 int (*preflight)(void *,struct nwdp_state *); /* 0 idle, 1 inherited, -1 unknown */
 int (*smc)(void *,const uint64_t[8],uint64_t[4]);
 int (*map)(void *,uint64_t,uint64_t);
 int (*word)(void *,uint32_t,uint32_t *,unsigned); /* offset, value, write? */
 int (*barrier)(void *);
};
/* One-shot. Complete inherited WFDMA preflight returns 1 after balanced
 * sleep and performs no SET. Any uncertainty after a vote/SET retains all
 * owned votes and requires manual reset. Success keeps the full pool mapped
 * and protected; this probe neither starts DMA nor copies firmware. */
int npp_run(const struct npp_ops *,void *,const struct ndp_claim *,
            struct npp_state *,unsigned);
#endif
