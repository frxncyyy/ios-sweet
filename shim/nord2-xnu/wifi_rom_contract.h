#ifndef NORD2_ROM_CONTRACT_H
#define NORD2_ROM_CONTRACT_H
#include <stdint.h>
#include <stddef.h>

/* Pure boot/MPU/CPU-release intent. No FDT walker, SMC, DRAM or MMIO call. */
#define NRC_CONN_BASE UINT64_C(0x8fb00000)
#define NRC_CONN_BYTES UINT64_C(0x450000)
#define NRC_REGION 27u
#define NRC_HANDOFF_STEPS 6u

enum nrc_claim_source { NRC_CLAIM_NONE, NRC_CLAIM_FIXED_REG,
    NRC_CLAIM_LK_TAG, NRC_CLAIM_IOS_FIXED };
struct nrc_claim { enum nrc_claim_source source; uint64_t base, bytes; };
/* Caller must obtain these exact properties from the ORIGINAL current-boot
 * FDT: /reserved-memory/consys-reserve-memory reg/no-map, or fallback
 * /consys@18000000 emi-addr/emi-size. This validates encoding/value only.
 * It does not assert exclusivity, absence of aliases, or MPU permission.
 * Pass both sources when present; disagreement is rejected. Missing fixed
 * data is rejected, even if the dynamic reservation has size/no-map.
 */
int nrc_boot_claim(const void *reg, size_t reg_len,
    int no_map_present, size_t no_map_len,
    const void *emi_addr, size_t emi_addr_len,
    const void *emi_size, size_t emi_size_len,
    struct nrc_claim *out);
/* Explicit iOS-owned fixed-selection candidate, against the current ORIGINAL
 * LK FDT bytes. Validates DRAM containment, dynamic CONN node shape, and all
 * fixed /reserved-memory reg + FDT memreserve non-overlap. Does not mutate
 * FDT, allocate from LK, or establish hidden-master/MPU/cache authority.
 * Integration must record this claim in the loader-to-XNU ADT, keep the low
 * range out of all AP allocators/aliases, and review the current boot image.
 */
int nrc_ios_fixed_claim_from_fdt(const void *fdt, size_t bytes,
    struct nrc_claim *out);

struct nrc_smc_request { uint64_t x[8]; };
/* Linux source request: group1 then group0, region27, AP0/CONN2/SCP3=0,
 * other domains=5. These are intended SET arguments, not permission proof.
 * Group0 can software-lock region27 on the backed-up monitor.
 */
int nrc_region27_set_intent(const struct nrc_claim *claim,
    struct nrc_smc_request out[2]);

enum nrc_op { NRC_WRITE_LITERAL, NRC_CLEAR_BIT0, NRC_SET_BIT0, NRC_POLL_EQUAL };
struct nrc_step { enum nrc_op op; uint64_t physical; uint32_t value, max_reads, delay_us; };
/* Source-order post-ROM script. Construct only; never executes. Caller must
 * prove successful persistent placement, barriers, CPU-held ownership, and
 * validated current-boot CONN claim before writing any step. */
void nrc_handoff_steps(struct nrc_step out[NRC_HANDOFF_STEPS]);
#endif
