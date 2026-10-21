#ifndef NORD2_PRIVATE_ROM_POWERED_ADAPTER_H
#define NORD2_PRIVATE_ROM_POWERED_ADAPTER_H
#include "wifi_rom_powered.h"
struct nrpa_hw {
 int (*owner)(void *); /* retained common owner, fresh radio lifetime */
 int (*read32)(void *,uint32_t,uint32_t *);
 int (*write32)(void *,uint32_t,uint32_t);
 int (*delay_us)(void *,unsigned);
};
struct nrpa {
 const struct nrpa_hw *hw;void *ctx;struct nrb_native *rom;
 struct nrpc_state state;
 unsigned asleep,blocked,sem20owned,sem20pending,clock_step;
 unsigned clock_polls,clock_spent;
 uint32_t last_addr,last_value,clock_last,irq_last;
 unsigned last_valid;
};
/* `prepared` was produced by nrb_xnu_prepare in sleepable UFS context.
 * optin=1 only; reentry or disabled invocation performs zero callbacks. */
int nrpa_start(struct nrpa *,const struct nrpa_hw *,void *,struct nrb_native *,unsigned);
#endif
