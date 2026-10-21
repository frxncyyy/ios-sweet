#ifndef NORD2_WIFI_WAKE_H
#define NORD2_WIFI_WAKE_H
#include <stdint.h>
/* Fresh proof callback: phase10 successful, old persistent owner retained,
 * terminal2 unchanged; this separate AP-wake transition exclusively owned.
 * Beforewake it must inspect SOFTWARE and always-on AP/host ONLY.
 * No ncc_ap_check or old npa/npc callback: those require awake off-domain. */
struct naw_ops {
 int (*proof)(void *);
 int (*read)(void *,uint32_t,uint32_t *);
 int (*write)(void *,uint32_t,uint32_t); /* synchronous ordered32-bit */
 int (*delay_us)(void *,unsigned);
};
struct naw_state {
 unsigned entered,phase,reference,awake,retained,halt,balanced;
 unsigned ack_polls,clock_polls,id_polls,cold_valid;
 uint32_t chip,clock,irq,cold[5];int error;
};
/* One-shot exact optin1; disabled/reentry does zero callbacks/state changes.
 * No allocation, firmware, DRAM, SMC, WFSYS write or oldowner-state mutation.
 * Owns HOST1a0 only from initial0; successful balanced sleep ends ALL MMIO.
 * After any issued wakewrite, uncertainty/timeout retains reference + halt.
 * balanced means our wake vote released, not common power restoration. */
int naw_run(const struct naw_ops *,void *,unsigned,struct naw_state *);
#endif
