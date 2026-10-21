#ifndef NORD2_DMA_POOL_CONTRACT_H
#define NORD2_DMA_POOL_CONTRACT_H
#include <stddef.h>
#include <stdint.h>

/* Explicit iOS fixed selection candidate, not a recovered LK allocation. */
#define NDP_POOL_BASE UINT64_C(0x90000000)
#define NDP_POOL_BYTES UINT64_C(0x00f20000)
#define NDP_POOL_ALIGN UINT64_C(0x01000000)
#define NDP_SUBPOOL_BYTES UINT64_C(0x00020000)
#define NDP_REGION 18u
#define NDP_SOURCE_IOS_FIXED 3u

struct ndp_claim { uint64_t base, bytes; uint32_t source, valid; };
struct ndp_smc_request { uint64_t x[8]; };
struct ndp_subpool {
    uint64_t tx_desc, rx_desc, tx_cmd, rx_events;
    uint32_t tx_desc_bytes, rx_desc_bytes, tx_cmd_bytes;
    uint32_t rx_event_stride, rx_event_count;
};

/* Includes the existing current-FDT CONN validator, then checks the pool's
 * distinct DRAM containment, Wi-Fi dynamic-node shape, phandle and exclusion
 * from all explicit reserved reg/memreserve spans. No memory is claimed or
 * mapped merely by returning zero. */
int ndp_fixed_claim_from_fdt(const void *fdt, size_t bytes,
                             struct ndp_claim *out);
/* Same 24-byte LE /chosen wire layout as nord2-conn-claim. */
int ndp_claim_encode(const struct ndp_claim *claim, uint8_t wire[24]);
int ndp_claim_decode(const void *wire, size_t length, struct ndp_claim *out);
/* Source-selected region18 group1 then group0 intent. Never issues SMC. */
int ndp_region18_set_intent(const struct ndp_claim *claim,
                            struct ndp_smc_request out[2]);
/* Actual native caller must retain both full raw returns; only x0 carries
 * the selected monitor status. The remaining registers are observations. */
int ndp_both_raw_zero(const uint64_t raw[2][4]);
/* Bounded first-stage suballocation inside the full source-sized pool.
 * Ring17 count1024, RX event count16; exactly one TX cmd in flight. */
int ndp_subpool_layout(const struct ndp_claim *claim, struct ndp_subpool *out);
#endif
