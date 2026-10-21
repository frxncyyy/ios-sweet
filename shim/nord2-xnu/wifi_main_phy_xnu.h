#ifndef PRIVATE_NORD2_PHY_XNU_H
#define PRIVATE_NORD2_PHY_XNU_H
#include "wifi_main_phy_exchange.h"
struct nfa_native {
 unsigned attempted,prepared;
 uintptr_t original_fdt_io;
 uint64_t original_fdt_pa;
 uint32_t original_fdt_bytes;
 struct nfa_board board;
 struct nfa_exchange_state exchange;
 void *backup_memory;
 uint32_t backup_capacity;
};
extern struct nfa_native nfa_native;
/* Sleepable, read-only prepare before fresh radio ownership. Requires the
 * exact future opt-in and the private, SHA-pinned same-image ADT property.
 * Rechecks the original FDT bytes from loader's current stock-dtb PA. */
int nfa_xnu_prepare(struct nfa_native *);
/* Consume actual nwi initialized transport after patch CID40. The returned
 * FORCE_CAL event and EMI backup must succeed before any N9 placement. */
int nfa_xnu_run(void);
#endif
