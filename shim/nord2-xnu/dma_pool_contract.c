#include "dma_pool_contract.h"

static int exact(const struct ndp_claim *c) {
    return c && c->valid == 1u && c->source == NDP_SOURCE_IOS_FIXED &&
           c->base == NDP_POOL_BASE && c->bytes == NDP_POOL_BYTES;
}
static uint32_t le32(const uint8_t *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
static uint64_t le64(const uint8_t *p) {
    return (uint64_t)le32(p) | ((uint64_t)le32(p+4) << 32);
}
static void put64(uint8_t *p,uint64_t v) {
    for(unsigned i=0;i<8;i++)p[i]=(uint8_t)(v>>(8u*i));
}
static void put32(uint8_t *p,uint32_t v) {
    for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(v>>(8u*i));
}
int ndp_claim_encode(const struct ndp_claim *c,uint8_t wire[24]) {
    if(!exact(c)||!wire)return -1;
    put64(wire,c->base);put64(wire+8,c->bytes);
    put32(wire+16,c->source);put32(wire+20,c->valid);
    return 0;
}
int ndp_claim_decode(const void *wire,size_t length,struct ndp_claim *out) {
    struct ndp_claim x;
    const uint8_t *p=wire;
    if(!wire||!out||length!=24)return -1;
    x.base=le64(p);x.bytes=le64(p+8);x.source=le32(p+16);x.valid=le32(p+20);
    if(!exact(&x))return -1;
    *out=x;return 0;
}
int ndp_region18_set_intent(const struct ndp_claim *c,struct ndp_smc_request out[2]) {
    if(!exact(c)||!out)return -1;
    /* mt6893 plat_priv.c:289-313: region18, AP0/CONN2 unrestricted,
     * all other domains forbidden=5, lock=true. 64KiB inclusive range. */
    out[0]=(struct ndp_smc_request){{UINT64_C(0xc200050b),0,UINT64_C(0x12009000),
        UINT64_C(0x010090f1),UINT64_C(0x00b6db6d),0,0,0}};
    out[1]=(struct ndp_smc_request){{UINT64_C(0xc200050b),0,UINT64_C(0x12009000),
        UINT64_C(0x000090f1),UINT64_C(0x80b6da28),0,0,0}};
    return 0;
}
int ndp_both_raw_zero(const uint64_t raw[2][4]) {
    if(!raw)return -1;
    return (raw[0][0] || raw[1][0]) ? -1 : 0;
}
int ndp_subpool_layout(const struct ndp_claim *c,struct ndp_subpool *out) {
    struct ndp_subpool x;
    if(!exact(c)||!out)return -1;
    /* 0x16000 occupied, rounded to two 64KiB MPU granules. */
    x.tx_desc=c->base+0x0000u; x.tx_desc_bytes=1024u*16u;
    x.rx_desc=c->base+0x4000u; x.rx_desc_bytes=16u*16u;
    x.tx_cmd=c->base+0x5000u; x.tx_cmd_bytes=4096u;
    x.rx_events=c->base+0x6000u; x.rx_event_stride=4096u;
    x.rx_event_count=16u;
    if(x.rx_events+((uint64_t)x.rx_event_stride*x.rx_event_count)>
       c->base+NDP_SUBPOOL_BYTES || NDP_SUBPOOL_BYTES>c->bytes)return -1;
    *out=x;return 0;
}
