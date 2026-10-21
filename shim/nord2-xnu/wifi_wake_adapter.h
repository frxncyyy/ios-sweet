#ifndef NORD2_WIFI_WAKE_ADAPTER_H
#define NORD2_WIFI_WAKE_ADAPTER_H
#include "wifi_wake.h"
struct nawa_hw {
 int (*owner)(void *); /*SOFTWAREonly: oldphase10terminal2retained + solewakeowner*/
 int (*read32)(void *,uint32_t,uint32_t *);
 int (*write32)(void *,uint32_t,uint32_t); /*ordered with dsb*/
 int (*delay_us)(void *,unsigned);
};
struct nawa {const struct nawa_hw *hw;void *c;struct naw_state result;unsigned asleep,clock_seen;uint32_t last_clock;};
int nawa_start(struct nawa *,const struct nawa_hw *,void *,unsigned);
#endif
