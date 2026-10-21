#ifndef PRIVATE_NIWB_BACKEND_BSS_H
#define PRIVATE_NIWB_BACKEND_BSS_H
#include <stdint.h>
#include <stddef.h>
#include "wifi_scan_bss.h"
struct niwb_bss {
 uint8_t bssid[6],ssid[32],ssid_bytes,ssid_present,hidden,subtype;
 uint8_t band,channel,rssi_valid,rcpi,rate_count,ie_rates[16];
 int16_t rssi_dbm;uint16_t interval,capabilities,ie_bytes;
 uint64_t observed_us;uint8_t ies[NSB_IE_BYTES];
};
/* Pure metadata from an actually decoded immutable nsb record, no authority
 * receipt. Source-selected non6GHz band1/2 and MAX(WF0,WF1) group3V2 RCPI.
 * Retains IE rates in500kbps units, basic bit stripped; no iOS rate guessing.
 * Noise/SNR/iOS flags/age conversions are intentionally absent. Atomic out. */
int niwb_bss_metadata(const struct nsb_record*,struct niwb_bss*);
#endif
