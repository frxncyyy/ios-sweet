#ifndef NORD2_NORMAL_RX_H
#define NORD2_NORMAL_RX_H
#include "wifi_normal_transport.h"
#include "wifi_scan_bss.h"
#include "wifi_rx_readback.h"
#define NCR_QUEUES 5u
#define NCR_HISTORY 64u
enum ncr_phase {NCR_IDLE,NCR_DESC_READ,NCR_FRAME_READ,NCR_DESC_WRITE,NCR_PUBLISH};
struct ncr_queue {
 struct nwi_ring ring;
 unsigned next,segmented,packets,rearmed,wait_ddone,dropped;
 uint32_t didx,raw[4];
};
struct ncr_event {uint16_t bytes,outer,inner;uint8_t eid,sequence,kind,status;};
struct ncr_state {
 struct nwi_state *init;struct nct_session *session;
 unsigned prepared,entered,active,phase,current,halt,error,scan_command;
 unsigned passes,packets,rearmed,dropped,malformed,debug,sleepy,unknown,msdu_reports,rx_reports;
 unsigned events,history_count,done_pending,done_unrelated,done_duplicates;
 unsigned scan_requested,scan_expired,drain_passes;
 uint8_t scan_sequence;uint64_t scan_timeout,last;
 struct ncr_queue queue[NCR_QUEUES];
 struct ncr_event history[NCR_HISTORY];
 struct nrd_snapshot readback[2];
 struct nsb_scan scan;uint8_t done_body[NSB_SCAN_DONE_BYTES];
 uint8_t descriptor[16],frame[3840];uint32_t frame_bytes;
};
/* Actual retained initializer, not a replacement DMA allocation. Caller keeps
 * this object and its owning command session alive through the bounded batch. */
int ncr_prepare(struct ncr_state *,struct nwi_state *,struct nct_session *,unsigned,uint8_t,uint64_t);
int ncr_continue_prepare(struct ncr_state *,struct nwi_state *,struct nct_session *,const struct ncr_state *,uint8_t,uint64_t);
int ncr_pump(void *,const struct ncd_io *,void *,unsigned,unsigned *);
/* Native admission adds these exact per-operation slots to the existing
 * command-only allowlist; it also checks the current native worker/session. */
int ncr_memory_range(const struct ncr_state *,uint64_t,uint32_t,unsigned);
int ncr_register_write(const struct ncr_state *,uint32_t,uint32_t);
#endif
