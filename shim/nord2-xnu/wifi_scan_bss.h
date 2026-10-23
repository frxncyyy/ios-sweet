#ifndef PRIVATE_NORD2_SCAN_BSS_H
#define PRIVATE_NORD2_SCAN_BSS_H
#include <stdint.h>
#include <stddef.h>
#define NSB_IE_BYTES 1024u /* Exact maximum IE room of the Apple1164-byte scan result. */
#define NSB_RESULTS 64u /* matches existing bounded Apple cursor capacity; owner storage is static */
#define NSB_SCAN_DONE_BYTES 476u
struct nsb_record {
 uint8_t bssid[6],transmitter[6],ssid[32],ssid_len,ssid_present,hidden;
 uint8_t subtype,hw_channel,ds_channel,ht_channel,channel,channel_mismatch;
 uint8_t group_valid,rxv_valid,rxv[8],rsn_present,wpa_present,privacy;
 uint16_t interval,capability,ie_bytes,ie_total;
 unsigned ies_truncated;uint64_t observed_us;
 uint8_t ies[NSB_IE_BYTES];
};
struct nsb_done {
 uint8_t sequence,state,version,completed_channels,sparse_valid,sparse_band;
 uint8_t sparse_channel,channel_count,pno;
 uint32_t beacons,beacons_2g,beacons_5g;
 uint16_t rxd_bytes,event_bytes;uint8_t event_sequence;
 uint8_t channels[64],beacon_probe_counts[64],mdrdy_counts[64];
 uint16_t idle[64],scan_time[64];
};
/* Pure copied-buffer decoder; caller must establish descriptor completion,
 * exact ring/buffer ownership, coherent status and visibility before entry.
 * Never rearms DMA or grants permission to transmit/scan. Outputs atomic. */
int nsb_decode(const uint8_t *,size_t,uint64_t,struct nsb_record *);
int nsb_decode_done(const uint8_t *,size_t,struct nsb_done *);
struct nsb_scan {
 unsigned entered,active,done,timed_out,partial,count,dropped,frames;
 uint8_t sequence;uint64_t started,last,deadline;
 struct nsb_done completion;struct nsb_record records[NSB_RESULTS];
};
/* Records an externally issued request, not readiness or regulatory approval. */
int nsb_begin(struct nsb_scan *,uint8_t,uint64_t,uint64_t);
int nsb_add(struct nsb_scan *,const uint8_t *,size_t,uint64_t);
/* 0 matched completion, 1 unrelated sequence, -1 malformed/expired/state. */
int nsb_complete(struct nsb_scan *,const uint8_t *,size_t,uint64_t);
/* Full copied RXD24 + normal WIFI_EVENT12 + selected completion body. */
int nsb_complete_event(struct nsb_scan *,const uint8_t *,size_t,uint64_t);
int nsb_expire(struct nsb_scan *,uint64_t);
#endif
