#ifndef PRIVATE_NORMAL_RESOURCES_H
#define PRIVATE_NORMAL_RESOURCES_H
#include <stdint.h>
#include <stddef.h>
#include "wifi_capability.h"
struct nnr_plan {
 uint32_t version,pse_data,pse_cmd,ple_data,ple_cmd;
 uint16_t pse_page_bytes,ple_page_bytes;
 uint8_t extra;
 unsigned tc,pse_cost,ple_cost,wire_bytes;
};
struct nnr_budget { struct nnr_plan plan; unsigned reserved; uint32_t pse_remaining; };
/* Pure derivation only. Native must bind cap to its actual same-session receipt.
 * AXI source counts one credit per command; TC4 excludes PLE. */
int nnr_plan(const struct ncp_result *,size_t,struct nnr_plan *);
int nnr_begin(const struct nnr_plan *,struct nnr_budget *);
/* One-way debit before publication, never automatically refunded on failure,
 * DDONE, or absent response. No hardware authority is granted by this object. */
int nnr_reserve_once(struct nnr_budget *);
#endif
