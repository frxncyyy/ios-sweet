#ifndef NIW_RATE_COMPAT_H
#define NIW_RATE_COMPAT_H
#include "wifi_ios_mapping.h"
/* Driver-selected compatibility codes from the actual same-IPSW consumer.
 * These are categorical ABI codes, not measured physical Mbps or MCS rates.
 * Original management IEs remain unchanged in the scan result. */
const struct niwp_rate_map *niwr_production_map(size_t *count);
int niwr_rates(const uint8_t *ie_rates,size_t bytes,uint32_t *output,
    size_t capacity,size_t *count);
#endif
