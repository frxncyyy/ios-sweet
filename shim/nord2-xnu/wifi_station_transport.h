#ifndef NORD2_STATION_TRANSPORT_H
#define NORD2_STATION_TRANSPORT_H
#include <stddef.h>
#include <stdint.h>
#include "wifi_normal_backend.h"
#include "wifi_station_wire.h"
#define NST_MAX_ETHERNET 1514u
#define NST_RX_BYTES 3840u
#define NST_FRAME_BYTES 1600u
/* FRAME_RESULT status0..255 is firmware status. This disjoint local value
 * is negative-only, after proved DDONE; it never means air success/ACK. */
#define NST_TX_LOCAL_TIMEOUT 0x100u
/* Host-only optional TXS can never alias an air-correlated management PID.
 * Source driver PID range is1..127. This first-attempt epoch never reuses a
 * management PID; exhaustion refuses publication without touching DMA. */
#define NST_PID_HOST 127u
#define NST_PID_MANAGEMENT_MAX 126u
/* Only the actual CAP worker may prepare/run this object. The native binder
 * authenticates exact addresses/thread; these public structs alone grant no IO. */
enum nst_kind {NST_NONE,NST_COMMAND_DMA,NST_FRAME_RESULT,NST_CHANNEL_GRANT,
 NST_STA_ACTIVE,NST_KEY_READY,NST_MANAGEMENT,NST_EAPOL,NST_ETHERNET,NST_PEER_PREPARED,NST_PEER_CONFIGURED,NST_PEER_RETIRED,NST_CHANNEL_RELEASED,NST_LINK_LOST,NST_FATAL,NST_SECURITY_SUBMITTED,NST_DATA_SUBMITTED};
enum nst_phase {NST_IDLE,NST_TX_READ,NST_TX_WRITE,NST_TX_PUBLISH,NST_TX_WAIT,
 NST_RX_READ,NST_RX_FRAME,NST_RX_WRITE,NST_RX_PUBLISH,NST_HALTED};
struct nst_event {
 unsigned kind,status;uint64_t generation,ticket;
 uint8_t cid,sequence,pid,wlan,bss,sta,token,band,channel,key_id;
 uint32_t interval_ms;const uint8_t *body;size_t bytes;
};
/* Callback consumes/copies borrowed bytes synchronously; never retain them or
 * submit recursively. It is called only after the corresponding RX slot rearm. */
typedef int (*nst_event_fn)(void *,const struct nst_event *);
struct nst_peer {uint8_t self[6],ap[6],bss,own_mac,sta,wlan,group_wlan,band,channel;};
enum nst_peer_kind {NST_PREPARE_PEER=1,NST_CONFIGURE_PEER,NST_RETIRE_PEER};
struct nst_profile {struct sw_assoc assoc;uint16_t beacon_interval;uint8_t rcpi,sta_version,erp_protect;};
struct nst_replay {unsigned installed;uint8_t key_id;uint64_t pn[17];};
struct nst {
 struct nwi_state *init;const struct nct_session *previous;const struct ncr_state *previous_rx;
 unsigned cleanup_started;
 unsigned prepared,entered,active,phase,halt,error,mutated,wire_busy,air_pending;
 unsigned credit_capacity,credit_available,credit_reserved,credit_acquired,credit_released;
 unsigned tx_next,base_completed,submitted,dma_completed,rx_current,next_pid;
 unsigned wire_bytes,wire_cid,wire_sequence,wire_pid,wire_wlan,wire_type;
 unsigned published,dma_done,air_done,air_status,callback_active;
 uint64_t generation,ticket,last,deadline,operation_deadline;
 struct nst_peer peer;struct sw_txd txd;
 struct ncr_queue queue[NCR_QUEUES];struct nst_replay ptk,gtk;
 unsigned controlled_port,sta_active,ptk_ready,ptk_command,ptk_submitted,gtk_submitted;
 unsigned activation_pending,activation_sequence,peer_kind,peer_step,internal_wire;
 uint64_t peer_ticket,grant_deadline;uint8_t channel_token,channel_held;
 struct nst_profile profile;uint8_t ssid[32],rates[12];uint16_t aid,negotiated_rates,basic_rates;
 unsigned link_lost;
 /* Numeric only; raw-data seen/accepted EAPOL/unsupported-or-invalid. */
 unsigned rx_untranslated,rx_untranslated_eapol,rx_untranslated_drop;
 unsigned pm_scheduled,pm_connected;
 unsigned pm_pending,dtim_period,bss_absent;uint8_t gtk_command_index;
 uint8_t descriptor[16],wire[4096],rx_descriptor[16],rx_frame[NST_RX_BYTES];
 uint32_t rx_bytes;struct nst_event deferred;
};
/* Adopt exact terminal scan transport and preserved RX cursors; no allocation,
 * no guessed capability indices. peer indices must match retained scan profile. */
int nst_scan_admissible(const struct nsb_scan *);
int nst_prepare(struct nst *,struct nwi_state *,const struct nct_session *,
 const struct ncr_state *,const struct ncs_cap *,const struct nst_peer *,
 const struct sw_txd *,uint64_t generation,uint64_t now_us,uint64_t deadline_us);
/* Copy complete CID payload or frame into owned storage.0 accepted,1busy,-1bad.
 * CID ticket completion means DMA/source software submission, not firmware ACK.
 * Management waits for actual PID+WLAN EID0f result. SECURITY_SUBMITTED and
 * DATA_SUBMITTED mean only local DDONE/storage retirement, never air success. */
int nst_command(struct nst *,uint64_t ticket,uint8_t cid,const uint8_t *,size_t);
int nst_management(struct nst *,uint64_t ticket,const uint8_t *,size_t);
int nst_eapol(struct nst *,uint64_t ticket,const uint8_t *ethernet,size_t,unsigned protected_frame);
int nst_legacy_rates(const uint8_t *,size_t,uint16_t*,uint16_t*,uint16_t*);
int nst_peer_begin(struct nst *,uint64_t ticket,unsigned kind,const struct nst_profile *,const uint8_t *response,size_t);
int nst_data(struct nst *,uint64_t ticket,const uint8_t *ethernet,size_t);
/* Actual bounded IO pass: one TX progression and <=budget RX frames, budget1..16.
 * Uses real ncd_io with native exact-worker/lease checks, not nct_checked_io. */
int nst_step(struct nst *,const struct ncd_io *,void *,unsigned budget,
 nst_event_fn,void *);
/* Precise slots for the retained owner's normal-stage allowlist extension. */
int nst_lease_window(const struct nst *,const struct nwi_state *);
int nst_memory_range(const struct nst *,uint64_t,uint32_t,unsigned write);
int nst_register_write(const struct nst *,uint32_t,uint32_t);
/* Install replay state only once after corresponding actual CID07 submission.
 * GTK RSC6 is little endian (EAPOL RSC bytes0..5); bytes6..7 must be0. */
int nst_replay_install(struct nst *,unsigned pairwise,uint8_t key_id,const uint8_t rsc[8]);
/* Authorize only after actual STA+PTK ready, GTK replay installed, and no pending
 * key command. No external boolean can supply STA/PTK readiness. */
int nst_authorize(struct nst *);
/* Once per attempt, install the owner's fresh <=30s teardown deadline. Existing
 * published-operation and RF grant deadlines remain binding. No IO or retirement
 * claim; may be called under the serialized event callback. */
int nst_cleanup_begin(struct nst *,uint64_t now_us,uint64_t deadline_us);
void nst_close_port(struct nst *);
#endif
