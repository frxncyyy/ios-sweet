#ifndef PRIVATE_NORD2_MAIN_FW_PLACE_H
#define PRIVATE_NORD2_MAIN_FW_PLACE_H
#include <stdint.h>
#include "wifi_main_layout.h"
/* A primitive used only by a source-ordered native coordinator. The layout
 * parser is structural; its inputs must be the same retained nwf_load blobs
 * held by the accepted ROM native owner. This primitive grants no runtime
 * firmware, calibration, or transport permission on its own. */
enum nfp_stage {NFP_NEW, NFP_PATCH_PAYLOAD, NFP_PATCH_DATE,
 NFP_N9_0, NFP_N9_1, NFP_N9_2, NFP_N9_3, NFP_N9_4, NFP_COPIED};
struct nfp_state {
 unsigned entered,next,stage,dirty,halt,attempted,completed;
 unsigned remaps,words_written,words_read,barriers;
 uint32_t last_target,last_address,last_expected,last_observed;
 int error;
};
struct nfp_io {
 int (*owner)(void *); /* actual retained same-object ROM+driver owner */
 int (*remap)(void *,uint32_t); /* 0x18001120 exact literal target */
 int (*write32)(void *,enum nmt_space,uint32_t,uint32_t);
 int (*read32)(void *,enum nmt_space,uint32_t,uint32_t *);
 int (*barrier)(void *);
};
/* index 0=patch RAM,1=patch date,2..6=N9 regions. Each call performs one
 * complete source span, write barrier, and bounded readback. Fresh owner is
 * rechecked at every 4-KiB boundary; raw callbacks must enforce the exact
 * stage/address allowlist and no concurrent driver lifetime transition.
 * Native caller
 * must interleave actual patch result+FORCE_CAL/coexistence before index2 and
 * actual N9 finish after index6. No retry after attempted write/remap. */
int nfp_span(const struct nfp_io *,void *,const struct nmt_layout *,
             struct nfp_state *,unsigned index);
#endif
