#ifndef PRIVATE_NORD2_REGULATORY_H
#define PRIVATE_NORD2_REGULATORY_H
#include <stdint.h>
#include <stddef.h>
#define NRP_CHANNELS 39u /* selected source14+25, CFG_SUPPORT_WIFI_6G=0 */
struct nrp_caps {unsigned sku_present,phy_present;uint32_t efuse_country;uint8_t efuse_valid,band5,nss,max_bw,path,sequence;};
struct nrp_board {uint8_t country[2],support5,enable5;};
struct nrp_channel {uint16_t channel;uint8_t band;uint32_t flags;}; /*band0=2GHz,1=5GHz*/
struct nrp_passive {unsigned count;struct nrp_channel channel[NRP_CHANNELS];};
/* Pure byte validation, no ownership/provenance receipt. Caller must consume
 * actual retained EIDec and authenticated immutable full board file. */
int nrp_caps_decode(const uint8_t *,size_t,struct nrp_caps *);
int nrp_board_decode(const uint8_t *,size_t,struct nrp_board *);
/* SINGLE_SKU runtime en comes from actual tag3, not compile config/board.
 * Source defaults to country00 unless tag3 supplies a valid efuse country. */
int nrp_country(const struct nrp_caps *,uint32_t *);
/* Channel inputs MUST already be source regulatory-evaluated (wiphy-equivalent).
 * This helper never generates allowed channels or certifies legal operation.
 * Builds normal CID0f/set/no-response,20MHz-only, exact12+8*N V2 body. */
int nrp_domain_frame(const struct nrp_caps *,const struct nrp_board *,
 const struct nrp_channel *,unsigned,uint8_t,uint8_t *,size_t,size_t *);
/* Conservative bounded passive20MHz subset: omit disabled/no20/radar/indoor,
 * retain NO_IR (passive listening), do not manufacture a channel list. */
int nrp_passive_plan(const struct nrp_caps *,const struct nrp_board *,
 const struct nrp_channel *,unsigned,struct nrp_passive *);
#endif
