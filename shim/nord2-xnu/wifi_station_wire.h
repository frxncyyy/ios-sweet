#ifndef STATION_WIRE_H
#define STATION_WIRE_H
#include <stddef.h>
#include <stdint.h>

/* Offline payload codec only. No allocation, logging, transport, or state changes.
 * All buffers must be disjoint from inputs. On failure: used=0, output unchanged.
 * Little endian firmware payloads; never cast these bytes to native structs.
 */
enum sw_result { SW_OK=0, SW_INVALID=-1, SW_CAPACITY=-2 };
enum sw_cid { SW_KEY=0x07, SW_BSS=0x12, SW_STA=0x13, SW_RLM=0x19,
              SW_CHANNEL=0x1c };
struct sw_peer { uint8_t self[6], ap[6]; };
struct sw_assoc {
    struct sw_peer peer;
    const uint8_t *ssid; size_t ssid_len;
    const uint8_t *rates; size_t rates_len; /* 500 kb/s, basic bit retained */
    uint16_t capability, listen_interval;
};
/* Open-system transaction 1, or legacy WPA2-PSK/CCMP association request.
 * Auth sequence-control is zero as selected source leaves HW to assign it.
 * Assoc emits SSID, supported/ext rates and exactly WPA2 PSK/CCMP RSN.
 * No HT/VHT/HE, WMM, PMF, reassociation, vendor IE or enterprise support.
 */
int sw_auth(uint8_t *, size_t, size_t *, const struct sw_peer *);
int sw_assoc(uint8_t *, size_t, size_t *, const struct sw_assoc *);

struct sw_key {
    uint8_t bss, wlan, pairwise, key_id;
    uint8_t ap[6];
    const uint8_t *key; size_t key_len; /* add requires exactly 16 */
};
/* RSC remains zero to reproduce wlanoidSetAddKey, which zeroes the payload
 * but never copies PARAM_KEY RSC. RX replay state is a separate host contract.
 * remove uses the separate source removal shape; caller retires secret bytes.
 */
int sw_key_add(uint8_t *, size_t, size_t *, const struct sw_key *);
int sw_key_remove(uint8_t *, size_t, size_t *, const struct sw_key *);
struct sw_channel {
    uint8_t bss, token, abort, band, channel, dbdc_band;
    uint32_t interval_ms;
};
int sw_channel(uint8_t *, size_t, size_t *, const struct sw_channel *);
struct sw_rlm {
    uint8_t bss, band, channel, erp_protect, short_preamble, short_slot;
    uint8_t tx_nss, rx_nss;
};
int sw_rlm(uint8_t *, size_t, size_t *, const struct sw_rlm *);
struct sw_bss {
    struct sw_rlm rlm;
    const uint8_t *ssid; size_t ssid_len;
    uint8_t ap[6], connected, ap_sta, bmc_wlan, nonht_phy, phy_set;
    uint8_t dbdc_band, wmm_set, disconnect_threshold;
    uint16_t rates, basic_rates, fixed_rate;
    uint32_t private_data;
};
int sw_bss(uint8_t *, size_t, size_t *, const struct sw_bss *);
/* Selected cnmStaSendUpdateCmd (cnm_mem.c1141-1142) sends the peer WLAN and
 * unconditionally WTBL_RESERVED_ENTRY(255) as STA-record ucBMCWlanIndex.
 * BSS-info ucBMCWlanIndex is a different field and keeps the real BMC slot. */
#define SW_WTBL_RESERVED_ENTRY 255
struct sw_sta {
    uint8_t bss, sta, wlan, bmc_wlan, sta_type, state, need_response; /* bmc_wlan==SW_WTBL_RESERVED_ENTRY */
    uint8_t ap[6], phy_set, rcpi, rts_policy, version;
    uint16_t aid, listen_interval, rates, basic_rates, fixed_rate;
};
/* AX layout enabled, BE disabled. version is 0 or 1, selected by real efuse
 * policy (cnmStaRecCmdHeContentFill sets1 when fgEfuseCtrlAxOn==1).
 * No aggregated/HT features; zero HE fields have no advertised HE meaning.
 * Caller supplies source-owned indices and source enum values; no allocator.
 */
int sw_sta(uint8_t *, size_t, size_t *, const struct sw_sta *);
struct sw_txd {
    uint8_t wlan, own_mac, queue, pid, retry_limit;
    uint16_t fixed_rate, pse_header_len;
    uint32_t lifetime_ms;
    uint8_t protected_frame, spe_index;
};
/* Long Connac2x TXD ONLY; frame is a separate scatter/gather payload.
 * management: auth/assoc only; security: complete Ethernet-II EAPOL frame.
 * Explicit queue/rate/PSE values must come from selected hardware owner.
 * PID requests EVENT_TX_DONE; neither bytes built nor DMA DDONE prove TX ACK.
 */
int sw_management_txd(uint8_t *,size_t,size_t *,const struct sw_txd *,
                      const uint8_t *,size_t);
int sw_security_txd(uint8_t *,size_t,size_t *,const struct sw_txd *,
                    const uint8_t *,size_t);
int sw_data_txd(uint8_t *,size_t,size_t *,const struct sw_txd *,const uint8_t *,size_t);
#endif
