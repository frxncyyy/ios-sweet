#ifndef NORD2_WIFI_STATION_TX_H
#define NORD2_WIFI_STATION_TX_H
#include <stdint.h>
#include <stddef.h>
#include "wifi_station_wire.h"
#define NSTX_PAGE_BYTES 4096u
#define NSTX_TOKEN_COUNT 2048u
#define NSTX_RING_COUNT 1024u
#define NSTX_REPORT_IDS 64u
/* One normal host frame at a time. Firmware token is unrelated to TXS PID.
 * A returned token is storage retirement, never an air ACK. Token reuse has
 * the selected source's wire limitation: no generation tag in TX-free reports. */
struct nstx_report {uint16_t ids[NSTX_REPORT_IDS];unsigned count,version;};
struct nstx {
 uint64_t buffer,ring;unsigned next,token_next,token,published,dma_done,report_done;
 unsigned submitted,dma_completed,completed,reports,invalid_reports;
 unsigned credit_available,credit_reserved,credit_acquired,credit_released;uint8_t descriptor[16],page[NSTX_PAGE_BYTES];
};
/* Output is atomic on failure. Complete source host-V2 object, zero tail.
 * Caller supplies actual AP/WLAN/ownMAC and source-derived basic rate. */
int nstx_build(uint8_t *,size_t,size_t *,const struct sw_txd *,const uint8_t *,size_t,unsigned security,unsigned token,uint64_t buffer);
/* Entire RX type6 packet, bounded source version0..3. No ledger mutation. */
int nstx_decode_report(struct nstx_report *,const uint8_t *,size_t);
/* A foreign, repeated or malformed report never releases an owned token.
 *0 unrelated/invalid,1 newly matched lifetime receipt. No air-status result. */
int nstx_report(struct nstx *,const uint8_t *,size_t);
int nstx_idle(const struct nstx *);
#endif
