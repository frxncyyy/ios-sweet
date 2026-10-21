#ifndef PRIVATE_WFDMA_TRANSPORT_H
#define PRIVATE_WFDMA_TRANSPORT_H
#include "wifi_main_command.h"
#define NWT_TX_COUNT 1024u
#define NWT_RX_COUNT 16u
#define NWT_RX_BYTES 3840u
#define NWT_REQUIRED_FULL_INIT 0x3fu
/* Legacy unsupported-initializer model flags; actual initializer is separate. */
struct nwt_io {
 int (*owner)(void *);
 int (*reg_read)(void *,uint32_t,uint32_t *);
 int (*reg_write)(void *,uint32_t,uint32_t);
 int (*mem_read)(void *,uint64_t,void *,uint32_t);
 int (*mem_write)(void *,uint64_t,const void *,uint32_t);
 int (*sync)(void *,uint64_t,uint32_t,unsigned); /*1toDevice,2fromDevice*/
 int (*delay_us)(void *,unsigned);
};
struct nwt_state {
 unsigned entered,preflight_complete,phase,halt,dirty,missing_init,error;
 unsigned cfg_reads,ring_reads,callback_count,polls,submit_attempted,tx_done,rx_done,completed;
 uint32_t cfg[2],ring[2][28][4];
 uint64_t tx_ring,rx_ring,rx_buffers,command;
 unsigned tx_index,rx_index,finish_phase;uint8_t next_sequence;
 uint32_t irq_status[2];unsigned irq_status_valid;
 uint32_t rx_descriptor_bytes,rx_mac_bytes,rx_init_bytes;
 uint64_t tx_descriptor_address;
 uint32_t tx_submitted[4],tx_observed[4],tx_didx,tx_used,tx_mismatch;
 unsigned tx_descriptor_valid,tx_ddone_waits;
 unsigned tx_consumed_zero;
 uint64_t rx_descriptor_address,rx_expected_buffer;
 uint32_t rx_descriptor_raw[4],rx_didx;
 unsigned rx_diag_valid,rx_failure_reason,rx_expected_eid,rx_expected_sequence;
 unsigned rx_packet_type,rx_event_eid,rx_event_sequence,rx_status,rx_cid;
};
/* Read-only exactaffected-domain preflight. ALWAYS refuses initialization
 * until finite full-source initport is completed. Never resets/enables DMA.
 * All ringsmust have zero base/count/index, bothGLOenable/busybitszero. */
int nwt_preflight(const struct nwt_io *,void *,struct nwt_state *,unsigned);
/* Capture a bounded owned RX event after one command. The source EID/sequence
 * checks apply; caller validates command-specific payload before another
 * command. Raw counts are captured, without inventing equality constraints.
 * Caller must retain the actual initializer and pool lifetime. On semantic
 * payload failure caller must latch halt before any further IO. */
int nwt_capture(const struct nwt_io *,void *,struct nwt_state *,
 const uint8_t *,size_t,uint8_t,uint8_t *,size_t,size_t *);
/* Source finish-command status/CID wrapper. */
int nwt_exchange(const struct nwt_io *,void *,struct nwt_state *,uint8_t cid);
#endif
