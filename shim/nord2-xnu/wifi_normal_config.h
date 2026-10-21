#ifndef PRIVATE_NORMAL_CONFIG_H
#define PRIVATE_NORMAL_CONFIG_H
#include <stddef.h>
#include <stdint.h>
#include "wifi_regulatory_policy.h"
#include "wifi_normal_init.h"
#define NCFG_COMMANDS 20u
#define NCFG_MAX_WIRE 1564u
#define NCFG_POWER_WIRE 1536u
struct ncfg_plan {
 unsigned count,band5,active_index,passive_index,active_fallback,passive_fallback;
 unsigned initial_power_index,project_power_index;
 struct nni_nvram nvram;
};
struct ncfg_input {const uint8_t *board;size_t board_bytes;struct nrp_caps caps;uint32_t project;};
struct ncfg_entry {uint8_t cid,set_query;uint16_t bytes;};
struct ncfg_channel {uint8_t band,channel,allowed,dfs,passive_marked;};
/* Pure byte provenance only: caller must retain an immutable, owned same-boot
 * full board and actual CAP, and bind native BASIC/session ownership separately.
 * This selected profile accepts only SHA-pinned board/project20827 and SKU0
 * PHY(5GHz=1,BW2,NSS2,pathf). No caller ready/override-authority booleans. */
int ncfg_prepare(const struct ncfg_input *,struct ncfg_plan *);
int ncfg_address(const struct ncfg_input *,const struct ncfg_plan *,struct nni_address *);
int ncfg_entry(const struct ncfg_plan *,unsigned,struct ncfg_entry *);
/* Builds seq0 immutable template; native session supplies sequence at39. */
int ncfg_build(const struct ncfg_input *,const struct ncfg_plan *,unsigned,
 uint8_t *,size_t,size_t *);
/* Full39 hardware intersection; metadata only, never RF admission. */
int ncfg_channels(const struct ncfg_input *,const struct ncfg_plan *,
 struct ncfg_channel *,size_t,size_t *);
#endif
