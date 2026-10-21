#ifndef PRIVATE_NORD2_PHY_EXCHANGE_H
#define PRIVATE_NORD2_PHY_EXCHANGE_H
#include "wifi_main_phy_backup.h"
#include "wifi_main_initializer.h"
/* Static/retained object: never allocate this on the input thread stack. */
struct nfa_exchange_state {
 unsigned attempted,event_valid,backup_valid,halt;
 size_t actual_rx_bytes;
 struct nfa_result event;
 struct nfa_backup_state backup;
 uint8_t tx[NFA_FRAME_BYTES],rx[NFA_RX_BYTES];
};
/* One actual WFDMA CID8 exchange in the already initialized fresh driver
 * lifetime, followed by source semantic validation and retained EMI backup.
 * The caller must have completed real patch CID40; no fabricated response or
 * permission flag is accepted. Any failure latches transport and this state. */
int nfa_run(const struct nwt_io *,void *,struct nwi_state *,
 const struct nfa_board *,const struct nfa_backup_io *,void *,
 void *,uint32_t,struct nfa_exchange_state *);
#endif
