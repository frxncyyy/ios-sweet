#ifndef NORD2_WIFI_ROM_TRANSACTION_H
#define NORD2_WIFI_ROM_TRANSACTION_H
#include "wifi_rom_plan.h"
#define NWRT_BACKUP_BYTES 346720u
#define NWRT_SCRATCH_BYTES 4096u
/* These are assertions supplied by a future audited adapter, NOT inferred by
 * this helper. All must be independently verified before enabling a device. */
#define NWRT_PREREQUISITES 0xffu
#define NWRT_PERMISSION 1u
#define NWRT_RESERVATION 2u
#define NWRT_NO_ALIAS 4u
#define NWRT_CPU_HELD_RESET 8u
#define NWRT_WFSYS_COLD 16u
#define NWRT_EXCLUSIVE 32u
#define NWRT_MAPPING_POLICY 64u
#define NWRT_RESTORE_OWNERSHIP 128u
struct nwrt_ops {
 /* Recheck current prerequisites before every IO/barrier. Zero only if all
  * requested facts remain proven; never changes hardware to satisfy them. */
 int (*check)(void *,unsigned);
 /* Exact complete operation or nonzero failure (which may follow a partial
  * write). Access only requested physical span, maximum4096bytes per call. */
 int (*read)(void *,uint64_t,void *,uint32_t);
 int (*write)(void *,uint64_t,const void *,uint32_t);
 int (*barrier)(void *); /* checked completion, correct mapping/cache policy */
};
struct nwrt_result {
 int error,restore_error;
 unsigned phase,reads,writes,barriers;
 unsigned write_attempted,dirty,restore_attempted,restore_verified,backup_retained;
};
/* No native IO implementation or activation hook. Rehashes exact ROM files;
 * snapshots all4 planned spans before first write; writes/readback compares;
 * always restores every attempted destination and verifies complete originals.
 * Returns0 only after placement/readback AND restoration verified. Dirty stays1
 * after any unverified restoration: caller MUST refuse CPU release/handoff.
 * Callbacks must be bounded/stable/exclusive, no concurrent buffer mutation.
 * Buffers, sources, result and callbacks/context storage must be disjoint.
 * Caller owns backup346720+scratch4096; after accepting buffers and source
 * identity, scratch is always erased. Argument/source rejection does not touch
 * buffers. Backup is
 * erased only when no unverified destination change remains; dirty failure
 * retains original bytes (backup_retained=1). Caller must preserve that backup
 * for controlled recovery and never hand off/release CPU while dirty.
 * No software deadline can interrupt a hung IO callback. No sourceCRC claim.
 */
int nwrt_run(const struct nwrt_ops *,void *,unsigned,
 const unsigned char *,size_t,const unsigned char *,size_t,
 unsigned char *,size_t,unsigned char *,size_t,struct nwrt_result *);
#endif
