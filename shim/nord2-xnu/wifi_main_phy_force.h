#ifndef PRIVATE_NORD2_PHY_FORCE_H
#define PRIVATE_NORD2_PHY_FORCE_H
#include <stdint.h>
#include <stddef.h>
#define NFA_WIFI_BYTES 6146u
#define NFA_NVRAM_BYTES 228u
#define NFA_TLV_BYTES 400u
#define NFA_FRAME_BYTES 464u
#define NFA_RX_BYTES 60u
struct nfa_board {uint8_t nvram[NFA_NVRAM_BYTES];unsigned verified,connfem_absent,coant;};
struct nfa_result {unsigned valid,force_success,backup_needed;uint32_t emi_offset,emi_bytes,temperature_raw;};
/* Saved board WIFI hash plus current ORIGINAL FDT no-ConnFem-compatible
 * proof. The native caller must acquire the WIFI bytes from a real owned
 * source and current FDT from loader's retained original PA. No fabricated
 * permission/context boolean is accepted. */
int nfa_prepare(const uint8_t *wifi,size_t wifi_bytes,const void *current_fdt,
                size_t fdt_bytes,struct nfa_board *out);
/* Source CONNAC2X MAC32 + INIT32 + FORCE_CAL TLV400. No DMA submission. */
int nfa_force_frame(const struct nfa_board *,uint8_t seq,
                    uint8_t out[NFA_FRAME_BYTES]);
/* Call only on actual owned 60-byte RX capture returned by WFDMA transport.
 * INIT byte count is intentionally not gated: selected source ignores it;
 * full DMA frame length and packet type/EID/seq/TLV are still required. */
int nfa_force_response(const uint8_t *frame,size_t bytes,uint8_t expected_seq,
                       uint32_t conn_bytes,struct nfa_result *out);
#endif
