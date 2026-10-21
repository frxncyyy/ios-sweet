#ifndef NORD2_MAIN_FW_LAYOUT_H
#define NORD2_MAIN_FW_LAYOUT_H
#include <stddef.h>
#include <stdint.h>

/* Structural description only. Call after nwf_load has authenticated and
 * retained blobs[0] (main) and blobs[1] (patch); this does not grant IO. */
enum nmt_space { NMT_AP2WF_WINDOW = 1, NMT_CONN_EMI = 2 };
struct nmt_span {
    const uint8_t *data;
    uint32_t length;
    uint32_t firmware_target;
    uint32_t host_physical; /* zero for dynamic AP2WF window */
    enum nmt_space space;
};
struct nmt_layout {
    struct nmt_span patch, patch_date, main[5];
    uint32_t main_entry_override; /* zero: source firmware default */
};
int nmt_parse_fixed_layout(const uint8_t *main_image, size_t main_length,
                           const uint8_t *patch_image, size_t patch_length,
                           struct nmt_layout *out);

/* Pure 24-byte RXD + 8-byte INIT_WIFI_EVENT + 4-byte CMD_RESULT validator.
 * This checks a supplied frame; it does not establish DMA completion. */
int nmt_validate_init_result(const uint8_t *frame, size_t frame_length,
                             uint8_t expected_sequence, uint8_t expected_cid);
#endif
