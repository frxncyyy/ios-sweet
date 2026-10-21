#include "wifi_rom_contract.h"

static uint32_t be32(const unsigned char *p)
{ return ((uint32_t)p[0]<<24)|((uint32_t)p[1]<<16)|((uint32_t)p[2]<<8)|p[3]; }
static uint64_t be64(const unsigned char *p)
{ return ((uint64_t)be32(p)<<32)|be32(p+4); }

int nrc_boot_claim(const void *reg, size_t reg_len,
    int no_map_present, size_t no_map_len,
    const void *emi_addr, size_t emi_addr_len,
    const void *emi_size, size_t emi_size_len,
    struct nrc_claim *out)
{
    if (!out) return -1;
    *out=(struct nrc_claim){0};
    int fixed=reg!=0 || reg_len!=0;
    int tag=emi_addr!=0 || emi_addr_len!=0 || emi_size!=0 || emi_size_len!=0;
    if (fixed && (!reg || reg_len!=16 || !no_map_present || no_map_len!=0)) return -1;
    if (tag && (!emi_addr || emi_addr_len!=8 || !emi_size || emi_size_len!=4)) return -1;
    if (!fixed && !tag) return -1;
    if (fixed) {
        const unsigned char *p=reg;
        if (be64(p)!=NRC_CONN_BASE || be64(p+8)!=NRC_CONN_BYTES) return -1;
    }
    if (tag) {
        if (be64(emi_addr)!=NRC_CONN_BASE || be32(emi_size)!=NRC_CONN_BYTES) return -1;
    }
    /* Both sources, if present, must name the same pinned range. A tag alone
     * is a current-boot LK allocation claim; it still needs ownership review. */
    *out=(struct nrc_claim){fixed?NRC_CLAIM_FIXED_REG:NRC_CLAIM_LK_TAG,
        NRC_CONN_BASE,NRC_CONN_BYTES};
    return 0;
}

int nrc_region27_set_intent(const struct nrc_claim *claim,
    struct nrc_smc_request out[2])
{
    if (!claim || !out || claim->source==NRC_CLAIM_NONE ||
        claim->base!=NRC_CONN_BASE || claim->bytes!=NRC_CONN_BYTES) return -1;
    /* Inclusive end 8ff4ffff. 64KiB words are packed in bits23:0. */
    uint64_t start_word=(claim->base>>16)|((uint64_t)NRC_REGION<<24);
    uint64_t end_word=(claim->base+claim->bytes-1)>>16;
    if (start_word!=UINT64_C(0x1b008fb0) || end_word!=UINT64_C(0x8ff4)) return -1;
    struct nrc_smc_request group1={{UINT64_C(0xc200050b),0,start_word,
        end_word|UINT64_C(0x01000000),UINT64_C(0x00b6db6d),0,0,0}};
    struct nrc_smc_request group0={{UINT64_C(0xc200050b),0,start_word,
        end_word,UINT64_C(0x00b6d028),0,0,0}};
    out[0]=group1; out[1]=group0;
    return 0;
}

void nrc_handoff_steps(struct nrc_step out[NRC_HANDOFF_STEPS])
{
    if (!out) return;
    /* soc3_0.c:2317-2359: initial read, then up to101 delayed reads
     * before polling_count>100 halts. No MMIO callback here. */
    out[0]=(struct nrc_step){NRC_WRITE_LITERAL,UINT64_C(0x18001120),0x89040000,0,0};
    out[1]=(struct nrc_step){NRC_WRITE_LITERAL,UINT64_C(0x185001dc),0,0,0};
    out[2]=(struct nrc_step){NRC_WRITE_LITERAL,UINT64_C(0x185001e0),0,0,0};
    out[3]=(struct nrc_step){NRC_CLEAR_BIT0,UINT64_C(0x18000018),0,0,0};
    out[4]=(struct nrc_step){NRC_SET_BIT0,UINT64_C(0x18000010),1,0,0};
    out[5]=(struct nrc_step){NRC_POLL_EQUAL,UINT64_C(0x184c1604),0x1d1e,102,1000};
}
