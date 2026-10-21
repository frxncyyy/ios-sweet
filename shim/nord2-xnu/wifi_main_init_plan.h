#ifndef PRIVATE_WFDMA_INIT_PLAN_H
#define PRIVATE_WFDMA_INIT_PLAN_H
#include <stdint.h>
#include <stddef.h>
struct nwt_rmw {uint32_t physical,clear,set;};
/* Exact selected source globalCFG transforms, not IO. Caller must firstown
 * ALL sourceaffectedglobals/rings+DMASHDL and pass standalonepreflight. */
int nwt_cfg_plan(unsigned dma,unsigned enable,struct nwt_rmw *);
/* Pure source manualprefetch ordered writes/RMW plan. Includesexactsourceindexresets:
 * source reset operations are represented as pure data; noexecutor exists.
 * reset/enable must remain gated until fullsourceinitownership implemented. */
struct nwt_plan_op {uint32_t physical,clear,set;unsigned rmw;};
int nwt_prefetch_plan(struct nwt_plan_op *,size_t,size_t *);
#endif
