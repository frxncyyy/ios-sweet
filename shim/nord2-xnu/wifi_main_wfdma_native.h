#ifndef PRIVATE_NORD2_WFDMA_NATIVE_H
#define PRIVATE_NORD2_WFDMA_NATIVE_H
#include "wifi_main_initializer.h"
struct nrb_native;
enum nwfn_stage { NWFN_PREPARED, NWFN_AP_ACK, NWFN_AP_READY,
 NWFN_WF_ACK, NWFN_LIVE, NWFN_INITIALIZING, NWFN_RETAINED,
 NWFN_RELEASE, NWFN_ASLEEP };
struct nwfn_native {
 struct nwi_state init;
 const struct nrb_native *rom;
 const struct npp_state *pool;
 struct ndp_claim claim;
 struct nwi_ring expected_rings[9];
 uintptr_t pool_va,scheduler_va,dummy_va,gic_va;
 unsigned prepared,attempted,active,halt,error,stage;
 unsigned ap_owned,wf_owned,asleep,balanced;
 unsigned ap_polls,wf_polls,clock_polls,clock_spent;
 uint32_t gic_enabled;
 uint32_t wf_initial,clock_raw,irq_raw,cfg_id,local_id,rom_done;
 uint32_t live[5],live_valid;
 uint32_t last_address,last_value;unsigned last_valid;
 uint8_t finish_tx[72],finish_rx[3840];size_t finish_received;
 unsigned failure_stage,guard_reason,guard_source,guard_valid;
 uint32_t guard_address,guard_raw,guard_mask,guard_expected;
};
/* One native object in the owning sweet-touch TU. New flag nord2wifimain=1
 * additionally requires the existing ROM and DMA flags. It cannot repurpose
 * the read-only nord2wififw flag. */
extern struct nwfn_native nwfn_native;
extern const struct nwt_io nwfn_io;
/* Fresh source lifetime; never old sleeping nppa/nrpa callback objects.
 * Owner proves exact retained receipts and immutable prepared ring identities,
 * AP/WF/readability/clock/IRQ policy and driver-own after acquisition. */
int nwfn_owner(void *);
/* Prepare is sleepable before fresh votes. Start either retains fully enabled
 * DMA/driver ownership, balances a clean inherited refusal before mutation,
 * or halts with owned resources retained. No teardown after DMA enable. */
int nwfn_prepare(void);
int nwfn_start(void);
int nwfn_finish_command(uint8_t);
#endif
