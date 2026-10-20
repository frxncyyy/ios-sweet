#ifndef NORD2_COMMON_TRANSACTION_H
#define NORD2_COMMON_TRANSACTION_H
#include <stdint.h>
#define NCC_COUNT 40
/* All bits must be freshly checked by adapter: exclusive cold power-probe
 * ownership; restoration armed; ID20010101; AP power/clock/protection good;
 * WFSYS CPU reset asserted and power/status off; BT/GPS/FM and firmware idle;
 * Wi-Fi-only next_status; both apertures readable and writable by AP.
 * No concurrent agent (including firmware) may touch any of these registers. */
#define NCC_OWNED 1u
#define NCC_RESTORE_ARMED 2u
#define NCC_CHIP_ID 4u
#define NCC_POWER_READABLE 8u
#define NCC_WFSYS_COLD 16u
#define NCC_ALL_MASTERS_IDLE 32u
#define NCC_WIFI_ONLY 64u
#define NCC_AP_ACCESS 128u
#define NCC_REQUIRED 0xffu
struct ncc_reg {uint32_t address,mask,value;};
extern const struct ncc_reg ncc_table[NCC_COUNT];
struct ncc_ops {
 int (*check)(void *,unsigned);
 int (*read)(void *,uint32_t,uint32_t *);
 /* Failure may have partially written; helper marks touched beforehand. */
 int (*write)(void *,uint32_t,uint32_t);
 int (*barrier)(void *); /* completed device write ordering before readback */
};
struct ncc_result {
 uint32_t before[NCC_COUNT];
 uint64_t touched,restored;
 unsigned snapshots,verified,phase;
 int error,restore_error,clean;
};
/* optin must be exactly1. stage1 retention only; stage2 retention+whole
 * non-BT SPI table. No continuation into RF commands or firmware execution.
 * Every mutation is reversed even on success; clean means verified restoration
 * of EVERY touched full register. Caller must still restore outer power probe.
 * Return0 completed+clean; -1 failed (consult clean/restore_error).
 * Callbacks bounded by caller; hung MMIO cannot be software-time-limited.
 * Result retains snapshots on failure. No allocation/maps/native MMIO here. */
int ncc_run(const struct ncc_ops *,void *,unsigned,unsigned,struct ncc_result *);
#endif
