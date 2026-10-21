#ifndef PRIVATE_NIW_SCAN_MAPPING_H
#define PRIVATE_NIW_SCAN_MAPPING_H
#include <stdint.h>
#include <stddef.h>
struct niwp_channel_map {uint32_t ios_channel,ios_flags;uint8_t mtk_band,mtk_channel;};
struct niwp_rate_map {uint8_t ie_rate;uint32_t ios_rate;};
/* No default table: native must provide separately source-proven mappings.
 * Exact flag equality prevents dropping unsupported width/band attributes.
 * Pure mapping never establishes regulatory/worker ownership. */
int niwp_channel_to_mtk(const struct niwp_channel_map *,size_t,uint32_t,uint32_t,uint8_t*,uint8_t*);
int niwp_channel_from_mtk(const struct niwp_channel_map *,size_t,uint8_t,uint8_t,uint32_t*,uint32_t*);
/* Input IE rate has its basic-rate bit removed only after matching. Duplicate
 * values collapse; unknown rates refuse atomically rather than rounding5.5. */
int niwp_rates(const struct niwp_rate_map *,size_t,const uint8_t*,size_t,uint32_t*,size_t,size_t*);
#endif
