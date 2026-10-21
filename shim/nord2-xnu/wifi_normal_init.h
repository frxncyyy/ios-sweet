#ifndef PRIVATE_NORD2_NORMAL_INIT_H
#define PRIVATE_NORD2_NORMAL_INIT_H
#include <stdint.h>
#include <stddef.h>
#include "wifi_capability.h"
#include "wifi_basic_primitives.h"
#define NNI_NVRAM_BYTES 1500u
#define NNI_FRAGMENT_BYTES 1564u
#define NNI_MAX_FRAGMENTS 32u
/* Source CID2/CID48 are set commands with fgNeedResp FALSE. TX completion
 * is not an invented firmware ACK or normal-init readiness receipt. */
struct nni_fragment {uint32_t offset,bytes;uint8_t tag;};
struct nni_nvram {
 unsigned fragments;uint32_t source_bytes,used_bytes,terminator,ignored_bytes;
 struct nni_fragment span[NNI_MAX_FRAGMENTS];
};
/* Structural planner only: caller must separately authenticate and retain
 * full selected board-file provenance; no boolean grants that authority. */
int nni_nvram_plan(const uint8_t *,size_t,struct nni_nvram *);
int nni_nvram_fragment(const uint8_t *,size_t,const struct nni_nvram *,
 unsigned,uint8_t,uint8_t *,size_t);
enum nni_address_source {NNI_ADDRESS_BOARD=1,NNI_ADDRESS_FIRMWARE=2};
struct nni_address {uint8_t mac[6];enum nni_address_source source;};
/* Host/NVRAM address precedence, then actual decoded firmware address.
 * Deliberately omits Linux's time-derived fallback: never fabricate one. */
int nni_address_select(const uint8_t *,size_t,const struct ncp_result *,struct nni_address *);
#endif
