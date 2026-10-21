#ifndef NORD2_WIFI_CAPABILITY_DISPATCHER_H
#define NORD2_WIFI_CAPABILITY_DISPATCHER_H
#include "wifi_capability.h"
#include "wifi_main_initializer.h"
#include "wifi_main_fw_xnu.h"
#define NCD_DEADLINE_US UINT64_C(3000000)
#define NCD_EVENT_BUDGET 16u
#define NCD_POLL_BUDGET 3000u
struct ncd_io {
 const struct nwt_io *bus;
 int (*now_us)(void *,uint64_t *); /* actual monotonic clock, not delay sum */
};
struct ncd_event {
 uint32_t bytes;uint16_t rxd_bytes,event_bytes,rx_class,debug_id,debug_bytes;
 uint8_t eid,sequence,kind,debug_type; /*1capability,2debug,4unsupported*/
};
struct ncd_state {
 struct nwi_state *init;const struct nfp_native *firmware;
 unsigned entered,phase,halt,error,mutated,submit_attempted,tx_done;
 unsigned events,rearmed,debug_events,sequence_echo,sequence_mismatches,matched,completed,polls;
 uint32_t sw_sync,irq_status[2];unsigned irq_valid;
 uint64_t started,last,deadline;
 uint8_t sequence,command[64],descriptor[16],frame[3840];
 size_t frame_bytes;
 struct ncd_event history[NCD_EVENT_BUDGET];struct ncp_result capability;
};
/* One actual first query after reviewed main receipt. Caller retains this
 * object/buffers. No heap allocation, global reset, MPU write or fake-ready
 * callback. Owner must bind exact same native init/firmware objects and real
 * current worker. A matched query increments shared completed3->4 once. */
int ncd_run(const struct ncd_io *,void *,struct nwi_state *,
 const struct nfp_native *,struct ncd_state *,unsigned);
#endif
