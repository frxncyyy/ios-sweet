#ifndef PRIVATE_WIFI_DMA_PREFLIGHT_H
#define PRIVATE_WIFI_DMA_PREFLIGHT_H
#include <stdint.h>
#define NWDP_RINGS 27u
struct nwdp_io {int (*owner)(void *);int (*read32)(void *,uint32_t,uint32_t *);};
struct nwdp_ring {uint32_t address,raw[4];unsigned valid;};
struct nwdp_state {
 unsigned entered,complete,observed_empty,stable,reads,error;
 unsigned cfg_valid,first_reason;uint32_t cfg[2],cfg_after[2];
 uint32_t first_addr,first_raw,first_mask;
 struct nwdp_ring rings[NWDP_RINGS];
};
/* Onlysource-definedaddresses/masks. Sourcefullinit manualprefetch affects
 * DMA0RX0..7, DMA1RX0..2/TX0..5/8..13/16..19. Never readsreservedringholes.
 * Zeroenabled/busy/configuredfields is observation,NOT ownershippermission.
 * Fullsnapshot continuesaftermismatch; callbackerror stops immediately.
 * Return0=complete+observedempty/stable;1=completeknowninherited/change
 * refusal (caller may balance awakevotes withoutSET);-1=unknown/callbackfailure
 * (caller retains/manualhalt). No write/memory/DMA callback exists. Freshparentwakeowner perread required. */
int nwdp_capture(const struct nwdp_io *,void *,struct nwdp_state *,unsigned);
#endif
