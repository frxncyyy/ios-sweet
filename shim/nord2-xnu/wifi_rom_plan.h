/* Pure metadata planner: no destination memory is accessed. */
#ifndef NORD2_WIFI_ROM_PLAN_H
#define NORD2_WIFI_ROM_PLAN_H
#include <stdint.h>
#include <stddef.h>
#define NWF_ROM_EMI_BASE UINT64_C(0x8fb00000)
#define NWF_ROM_EMI_BYTES UINT64_C(0x450000)
#define NWF_ROM_SPANS 4u
struct nwf_rom_span {
 uint32_t blob; /* 0=WMMCU ROM, 1=Wi-Fi ROM; input buffers stay caller-owned. */
 uint32_t source_offset,bytes,emi_offset;
 uint64_t physical;
};
struct nwf_rom_plan { struct nwf_rom_span spans[NWF_ROM_SPANS]; };
/* Rechecks authoritative complete-file SHA256, then validates the exact pinned
 * header sizes/types/addresses and all source/destination span bounds.
 * Output is cleared on rejection. Inputs and plan must not overlap; buffers
 * must remain unchanged during the call. Does not validate the undocumented
 * ROM header CRC words, infer firmware behavior, map or copy destination EMI.
 * Input order and exact sizes: WMMCU49868, Wi-Fi296916 bytes.
 */
int nwf_rom_make_plan(const unsigned char *,size_t,const unsigned char *,size_t,
    struct nwf_rom_plan *);
#endif
