#ifndef PRIVATE_WFDMA_INITIALIZER_H
#define PRIVATE_WFDMA_INITIALIZER_H
#include "wifi_main_dmashdl.h"
#include "pool_probe.h"
struct nwi_ring {uint64_t descriptors,buffers;uint32_t base,count,ext;unsigned rx;};
struct nwi_state {
 unsigned prepared,entered,phase,halt,dirty,error,driver_requested,driver_owned;
 unsigned memory_writes,reg_writes,idle_polls,initialized;
 unsigned irq_status_initial_valid,irq_status_final_valid;
 uint32_t irq_initial[2],irq_final[2],irq_status_initial[2],irq_status_final[2];
 struct nwi_ring rings[9];uint64_t command,end;
 const struct npp_state *pool;struct ndp_claim claim;
 struct nwdp_state preflight;struct nwdm_state scheduler;
 struct nwt_state transport;
};
/* Consume actual retained pool receipt, not a permission callback. Pure plan.
 * Four source-configured TX rings0,2,17,16; RX DMA0 0..3 +DMA1event0.
 * Caller must retain same current claim/ROM/pool and fresh AP/WF ownership. */
int nwi_prepare(struct nwi_state *,const struct ndp_claim *,const struct npp_state *);
/* Actual source init derivative through enabled host WFDMA. Private only.
 * Owner callback must prove fresh AP/WF/readability/clock/ROM/pool lifetime and
 * platform IRQ disabled. Internal driver-own readback gates configuration.
 * Read-only inherited-state refusal before writes returns1; all uncertain
 * errors retain resources. No native invocation is wired. */
int nwi_initialize(const struct nwt_io *,void *,struct nwi_state *,unsigned);
#endif
