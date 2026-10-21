#ifndef PRIVATE_WFDMA_COMMAND_H
#define PRIVATE_WFDMA_COMMAND_H
#include <stdint.h>
#include <stddef.h>
/* Pure selected SOC3_0 wire formats. No IO, DMAownership or permission proof. */
int nwt_finish(uint8_t *,size_t,uint8_t cid,uint8_t sequence,size_t *used);
int nwt_tx_desc(uint8_t out[16],uint64_t dma,size_t bytes);
int nwt_rx_desc(uint8_t out[16],uint64_t dma,size_t capacity);
/* Call only after actual owned RX completion/acquire barrier. This cannot
 * establish hardware completion; returns bounded descriptor bytecount. */
int nwt_rx_complete(const uint8_t desc[16],uint64_t expected_dma,size_t capacity,size_t *bytes);
#endif
