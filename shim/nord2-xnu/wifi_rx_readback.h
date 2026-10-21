#ifndef NORD2_RX_READBACK_H
#define NORD2_RX_READBACK_H
#include <stdint.h>
#include "wifi_capability_dispatcher.h"
#define NRD_QUEUES 4u
enum nrd_reason {NRD_BASELINE=1,NRD_SCAN_DONE=2,NRD_SCAN_EXPIRED=3};
struct nrd_queue {
 unsigned queue,next,valid;uint32_t base,ext_address,control[4],ext;
 uint64_t expected_descriptors,descriptor;uint32_t raw[4];
};
struct nrd_snapshot {
 unsigned attempted,complete,error,reason,valid;uint64_t started,last;
 uint32_t cfg[2];struct nrd_queue queue[NRD_QUEUES];
};
struct ncr_state;
/* Read-only diagnostics, not admission or authority. Caller supplies actual
 * active RX state + checked IO/access in the same live native worker. All
 * four descriptors use software-owned current cells, never returned SDP.
 * One-use output; restore phase/current on every exit. No stack packet array. */
int nrd_capture(const struct ncd_io *,void *,struct ncr_state *,struct nrd_snapshot *,unsigned);
#endif
