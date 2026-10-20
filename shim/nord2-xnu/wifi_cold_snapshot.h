#ifndef NORD2_WFSYS_COLD_SNAPSHOT_H
#define NORD2_WFSYS_COLD_SNAPSHOT_H
#include <stdint.h>
/* Caller must already own the cold power probe, arm restoration, prove both
 * AP power acks, clock, bus protection/readability and CONN ID20010101. */
#define NWC_REQUIRED 0x3fu
struct nwc_ops {
 int (*check)(void *,unsigned); /*0 only while all prerequisites remain true*/
 int (*read)(void *,uint64_t,uint32_t *); /* exact one allowed read or error */
};
struct nwc_result {uint32_t raw[3],valid_mask;unsigned attempted;int error,cold;};
/* Explicit optin1. At most3new-register reads, no writes/maps/SMC/loops. An IO
 * callback itself can stall; software cannot interrupt a hung MMIO read.
 * Return0 all3valid+cold;1 valid but noncold;-1 invalid/guard/read failure.
 * Caller MUST always execute its existing restore path, regardless of result.
 * No effective EMI permission or firmware execution inference is made. */
int nwc_snapshot(const struct nwc_ops *,void *,unsigned,struct nwc_result *);
#endif
