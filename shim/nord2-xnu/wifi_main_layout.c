#include "wifi_main_layout.h"
#include <string.h>

#define NMT_MAIN_BYTES 973648u
#define NMT_PATCH_BYTES 5726u
#define NMT_MAIN_TAIL 36u
#define NMT_REGION_TAIL 40u
#define NMT_REGIONS 5u
#define NMT_PATCH_HEADER 30u
#define NMT_CONN_BASE UINT32_C(0x8fb00000)
#define NMT_CONN_BYTES UINT32_C(0x00450000)
#define NMT_FW_EMI_BASE UINT32_C(0xf0000000)
#define NMT_WINDOW_BYTES UINT32_C(0x00010000)

static uint32_t le32(const uint8_t *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
static int zero(const uint8_t *p, size_t n) {
    while (n--) if (*p++) return 0;
    return 1;
}
static int set_emi(struct nmt_span *s, const uint8_t *data,
                   uint32_t len, uint32_t target) {
    if (target < NMT_FW_EMI_BASE || !len ||
        target - NMT_FW_EMI_BASE > NMT_CONN_BYTES ||
        len > NMT_CONN_BYTES - (target - NMT_FW_EMI_BASE)) return -1;
    s->data = data; s->length = len; s->firmware_target = target;
    s->host_physical = NMT_CONN_BASE + target - NMT_FW_EMI_BASE;
    s->space = NMT_CONN_EMI;
    return 0;
}
static int set_window(struct nmt_span *s, const uint8_t *data,
                      uint32_t len, uint32_t target) {
    if (!len || len > NMT_WINDOW_BYTES || (target & 3u)) return -1;
    s->data = data; s->length = len; s->firmware_target = target;
    s->host_physical = 0; s->space = NMT_AP2WF_WINDOW;
    return 0;
}
int nmt_parse_fixed_layout(const uint8_t *main_image, size_t main_length,
                           const uint8_t *patch_image, size_t patch_length,
                           struct nmt_layout *out) {
    static const uint32_t target[5] = {
        0x00104000u, 0x00201c00u, 0x00404000u, 0xf031e000u, 0xf03c3800u
    };
    static const uint32_t length[5] = { 5744u, 5168u, 29408u, 675456u, 257520u };
    const uint8_t *common, *regions;
    struct nmt_layout x;
    uint32_t offset = 0;
    if (!main_image || !patch_image || !out ||
        main_length != NMT_MAIN_BYTES || patch_length != NMT_PATCH_BYTES) return -1;
    memset(&x, 0, sizeof(x));
    /* Selected v1 patch, exact header. The final date byte in this artifact
     * is LF and is copied as part of the source's 16-byte date code. */
    if (memcmp(patch_image + 16, "ALPS", 4) ||
        le32(patch_image + 20) != 0x108a108au ||
        le32(patch_image + 24) != 0x00100000u ||
        le32(patch_image + 24) == UINT32_MAX) return -1;
    if (set_window(&x.patch, patch_image + NMT_PATCH_HEADER,
                   NMT_PATCH_BYTES - NMT_PATCH_HEADER, 0x00100000u) ||
        set_emi(&x.patch_date, patch_image, 16u, 0xf027f0d0u)) return -1;
    common = main_image + main_length - NMT_MAIN_TAIL;
    regions = common - NMT_REGION_TAIL * NMT_REGIONS;
    if (common[0] != 12u || common[1] != 0u || common[2] != NMT_REGIONS ||
        common[3] != 2u || common[4] != 1u || !zero(common + 5, 2)) return -1;
    for (unsigned i = 0; i < NMT_REGIONS; ++i) {
        const uint8_t *r = regions + i * NMT_REGION_TAIL;
        uint8_t feature = i < 3 ? 0u : 0x80u;
        if (!zero(r, 16) || le32(r + 16) != target[i] ||
            le32(r + 20) != length[i] || r[24] != feature ||
            !zero(r + 25, 15) || offset > (uint32_t)(regions - main_image) ||
            length[i] > (uint32_t)(regions - main_image) - offset) return -1;
        if (i < 3) {
            if (set_window(&x.main[i], main_image + offset, length[i], target[i])) return -1;
        } else if (set_emi(&x.main[i], main_image + offset, length[i], target[i])) return -1;
        offset += length[i];
    }
    /* The remaining 116 bytes are release info, not a sixth region. */
    if ((uint32_t)(regions - main_image) - offset != 116u) return -1;
    x.main_entry_override = 0;
    *out = x;
    return 0;
}
int nmt_validate_init_result(const uint8_t *frame, size_t frame_length,
                             uint8_t expected_sequence, uint8_t expected_cid) {
    const uint8_t *e;
    if (!frame || frame_length != 24u + 8u + 4u ||
        (expected_cid != 0x40u && expected_cid != 0x41u)) return -1;
    e = frame + 24u;
    if (e[2] != 0x00u || e[3] != 0xe0u || e[4] != 1u ||
        e[5] != expected_sequence || e[8] != 0u ||
        e[9] != expected_cid) return -1;
    return 0;
}
