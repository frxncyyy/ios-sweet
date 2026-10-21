#ifndef NORD2_PRIVATE_WFSYS_ADAPTER_H
#define NORD2_PRIVATE_WFSYS_ADAPTER_H
#include "wifi_wfsys.h"
struct nwsa_hw {
 int (*owner)(void *); /* software only: phase10 retained/terminal2, sole new owner */
 int (*read32)(void *,uint32_t,uint32_t *);
 int (*write32)(void *,uint32_t,uint32_t); /* ordered, synchronous */
 int (*delay_us)(void *,unsigned);
};
struct nwsa {
 const struct nwsa_hw *hw;void *ctx;
 struct nws_state state;
 unsigned asleep,blocked,sem20owned,sem20pending;
 unsigned clock_step,emi_pending;
 unsigned clock_polls,clock_spent;
 uint32_t last_addr,last_value,clock_last,irq_last,emi_original;
 unsigned last_valid;
};
/* Separate fresh one-shot state; never reuse old naw sleep or npa terminal2.
 * optin0 and reentry perform no callback. No native backend supplied here. */
int nwsa_start(struct nwsa *,const struct nwsa_hw *,void *,unsigned);
#endif
