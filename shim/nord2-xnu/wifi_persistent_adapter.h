#ifndef NORD2_WIFI_PERSISTENT_ADAPTER_H
#define NORD2_WIFI_PERSISTENT_ADAPTER_H
#include "wifi_persistent.h"
/* Backend is bound only by the new persistent outer owner. AP check reuses
 * ncc_ap_check before HW handoff; proof validates cold subsystem SRAM ownership and reviewed historical remap policy,
 * initial source rail/clock setup and exclusive software power lifetime.
 * No claim that this software proof establishes effective EMI permission. */
struct npa_hw {
 int (*ap)(void *);
 int (*proof)(void *); /*fresh external owner/26MHz/RC/reservation proof*/
 int (*read32)(void *,uint32_t,uint32_t *);
 int (*write32)(void *,uint32_t,uint32_t); /* includes dsb sy */
 int (*delay_us)(void *,unsigned);
};
struct npa {
 const struct npa_hw *hw;void *cookie;struct npc_result result;
 uint32_t conn_base,mcif_base,last_address,last_value;
 unsigned entered,retained,fatal,hw_handoff,remap_done,sema11,sema19;
 unsigned spi_inflight,clock_polls,mmio_writes;
};
/* initialize storage before first call; never memset after entered/retained.
 * Return0 only source-common retained; -1 failure (inspect fatal/retained).
 * optin!=1 performs ZERO callbacks and leaves owner untouched.
 * conn/mcif must match the parent-approved historical Android/source address policy;
 * hard pins reject source Android expectation drift, not prove current DRAM allocation.
 * No MMIO is allowed after result.halt/fatal. Manualreset remains sole recovery.
 */
int npa_start(struct npa *,const struct npa_hw *,void *,unsigned,uint32_t,uint32_t,unsigned);
#endif
