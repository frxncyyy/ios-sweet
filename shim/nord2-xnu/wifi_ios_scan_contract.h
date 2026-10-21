#ifndef PRIVATE_IOS_WIFI_SCAN_CONTRACT_H
#define PRIVATE_IOS_WIFI_SCAN_CONTRACT_H
#include <stddef.h>
#include <stdint.h>
#define NIWS_SCAN_REQUEST_BYTES 5456u
#define NIWS_SCAN_RESULT_BYTES 1164u
#define NIWS_MAX_IE 1024u
#define NIWS_MAX_RATES 15u
struct niws_result {
 uint32_t channel,channel_flags,age;
 uint16_t noise_valid;int16_t noise,snr,rssi;
 uint16_t beacon_interval,capabilities;
 uint8_t bssid[6],rate_count,ssid_bytes,ssid[32],flags;
 uint32_t rates[NIWS_MAX_RATES];
 uint16_t ie_bytes;uint8_t ie[NIWS_MAX_IE];
};
/* Exact selected framework version1 carrier, pure bytes only. Caller supplies
 * actual provenance and explicit iOS channel flags; no RF/readiness authority. */
int niws_decode_result(const uint8_t *,size_t,struct niws_result *);
int niws_encode_result(const struct niws_result *,uint8_t *,size_t);
/* Request10 subset: proves extent/version and captures scan_type only. Other
 * fields remain opaque; this MUST NOT authorize a backend scan. */
int niws_scan_request_header(const uint8_t *,size_t,uint32_t *);
struct niws_scan_done {uint32_t id,status;};
/* Owned copied kernel-event form only: header24 + ifname16 + status4.
 * Caller still must establish event stream ownership/session and cancellation. */
int niws_decode_scan_done(const uint8_t *,size_t,const char[16],struct niws_scan_done *);
/* Exact copied pipe record87: fixed header48, primary status4, secondary0.
 * This does not read or publish a shared ring or synthesize a notification. */
int niws_decode_pipe_scan_done(const uint8_t *,size_t,const char[16],struct niws_scan_done *);
/* Exact SET0x802869c8/request10 envelope; metadata classification only. */
int niws_scan_envelope(unsigned,const uint8_t *,size_t,const char[16],uint32_t *);
#endif
