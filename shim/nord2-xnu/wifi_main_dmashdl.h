#ifndef PRIVATE_DMASHDL_H
#define PRIVATE_DMASHDL_H
#include "wifi_main_transport.h"
#include "wifi_main_init_plan.h"
#include "wifi_dma_preflight.h"
struct nwdm_state {unsigned entered,complete,dirty,halt,error,operations;struct nwdp_state snapshot;};
/* Actual selected99RMW initialization subset. Freshsourceowner required, and
 * internalread-onlypreflight runs immediately beforefirstmutation. DoesNOT
 * enableDMA, resetindices, or minttransportphase2. Failureafterwrite retains. */
int nwdm_apply(const struct nwt_io *,void *,struct nwdm_state *,unsigned);
int nwdm_plan(struct nwt_plan_op *,size_t,size_t *);
#endif
