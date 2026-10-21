#ifndef PRIVATE_NORD2_POOL_ADAPTER_H
#define PRIVATE_NORD2_POOL_ADAPTER_H
#include "pool_probe.h"
struct nppa_hw {
 int (*owner)(void *); /* persistent phase10, ROM receipt, pool claim */
 int (*read32)(void *,uint32_t,uint32_t *);
 int (*write32)(void *,uint32_t,uint32_t);
 int (*delay_us)(void *,unsigned);
 int (*smc)(void *,const uint64_t[8],uint64_t[4]);
 int (*map)(void *,uint64_t,uint64_t);
 int (*word)(void *,uint32_t,uint32_t *,unsigned);
 int (*barrier)(void *);
};
struct nppa {
 const struct nppa_hw *hw;void *ctx;struct npp_state state;
 unsigned blocked,asleep,clock_polls,clock_spent;
 uint32_t clock_last,irq_last,last_addr,last_value;unsigned last_valid;
};
int nppa_start(struct nppa *,const struct nppa_hw *,void *,
               const struct ndp_claim *,unsigned);
#endif
