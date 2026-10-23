#ifndef NORD2_STATION_TRANSPORT_H
#define NORD2_STATION_TRANSPORT_H
#include <stddef.h>
#include <stdint.h>
#include "wifi_normal_backend.h"
#include "wifi_station_wire.h"
#include "wifi_station_data_diag.h"
#include "wifi_station_tx.h"
/* Selected gen4m ENUM_CMD_ID: 0x04 is NIC_POWER_CTRL, never scan cancel.
 * Internal_wire4 is a host role and must not be used as a wire CID. */
#define NST_CID_SCAN_CANCEL 0x1bu
#define NST_MAX_ETHERNET 1514u
#define NST_RX_BYTES 3840u
#define NST_FRAME_BYTES 1600u
/* FRAME_RESULT status0..255 is firmware status. This disjoint local value
 * is negative-only for cleanup management after proved DDONE; it never means air success/ACK. */
#define NST_TX_LOCAL_TIMEOUT 0x100u
/* Exact cleanup deauth DDONE+wipe without an observed air status. Negative
 * only, distinct from the legacy bounded-wait timeout; no authorization. */
#define NST_TX_LOCAL_RETIRE 0x101u
/* Source PID range1..127. Disjoint boot-lifetime diagnostic ranges never
 * alias management. Security/data fallback127 remains ambiguous and local
 * receipts remain independent of TXS. Management exhaustion refuses IO. */
#define NST_PID_HOST 127u
#define NST_PID_MANAGEMENT_MAX NSDD_PID_MANAGEMENT_MAX
/* Only the actual CAP worker may prepare/run this object. The native binder
 * authenticates exact addresses/thread; these public structs alone grant no IO. */
enum nst_data_profile {NST_DATA_ETHERNET=NSTX_PROFILE_ETHERNET,NST_DATA_TODS=NSTX_PROFILE_TODS};
enum nst_kind {NST_NONE,NST_COMMAND_DMA,NST_FRAME_RESULT,NST_CHANNEL_GRANT,
 NST_STA_ACTIVE,NST_KEY_READY,NST_MANAGEMENT,NST_EAPOL,NST_ETHERNET,NST_PEER_PREPARED,NST_PEER_CONFIGURED,NST_PEER_RETIRED,NST_CHANNEL_RELEASED,NST_LINK_LOST,NST_FATAL,NST_SECURITY_SUBMITTED,NST_DATA_SUBMITTED};
enum nst_phase {NST_IDLE,NST_TX_READ,NST_TX_WRITE,NST_TX_PUBLISH,NST_TX_WAIT,
 NST_RX_READ,NST_RX_FRAME,NST_RX_WRITE,NST_RX_PUBLISH,NST_HALTED,
 NST_DATA_READ,NST_DATA_WRITE,NST_DATA_VERIFY,NST_DATA_PUBLISH,NST_DATA_WAIT,NST_DATA_RETIRE};
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
 struct nsb_scan *associated_scan;unsigned associated_scan_published,associated_scan_cancel,associated_scan_canceled;
 unsigned prepared,entered,active,phase,halt,error,mutated,wire_busy,air_pending;
 unsigned credit_capacity,credit_available,credit_reserved,credit_acquired,credit_released;
 unsigned tx_next,base_completed,submitted,dma_completed,rx_current,next_pid;
 /* Optional development comparison with the former OFLD drop policy.
  * This can only reject additional frames; all secure RX checks still run. */
 unsigned ofld_drop;
 /* Set only by pure prepare. Ordinary data may use explicit legacy ToDS;
  * EAPOL and management retain their existing descriptor/header contracts. */
 unsigned data_profile,security; /* SW_SECURITY_*; explicit immutable prepare */
 unsigned wire_bytes,wire_cid,wire_sequence,wire_pid,wire_wlan,wire_type;
 unsigned published,dma_done,air_done,air_status,callback_active;
 uint64_t generation,ticket,last,deadline,operation_deadline;
 struct nst_peer peer;struct sw_txd txd;
 struct ncr_queue queue[NCR_QUEUES];struct nst_replay ptk,gtk;
 /* GTK slots retain independent replay lanes during source key-index overlap. */
 struct nst_replay gtk_keys[4];uint8_t gtk_wlan[4],group_spare[3];unsigned gtk_mapped;
 uint32_t gtk_submission_serial,gtk_replay_serial;unsigned gtk_command_added;
 unsigned controlled_port,sta_active,ptk_ready,ptk_command,ptk_submitted,gtk_submitted;
 unsigned activation_pending,activation_sequence,peer_kind,peer_step,internal_wire;
 uint64_t peer_ticket,grant_deadline;uint8_t channel_token,channel_held;
 struct nst_profile profile;uint8_t ssid[32],rates[12];uint16_t aid,negotiated_rates,basic_rates;
 unsigned link_lost;
 /* Numeric only; raw-data seen/accepted EAPOL/unsupported-or-invalid. */
 unsigned rx_untranslated,rx_untranslated_eapol,rx_untranslated_drop,rx_untranslated_data;
 unsigned pm_scheduled,pm_connected;
 unsigned pm_pending,dtim_period,bss_absent;uint8_t gtk_command_index;
 uint8_t descriptor[16],wire[4096],rx_descriptor[16],rx_frame[NST_RX_BYTES];
 uint32_t rx_bytes;struct nst_event deferred;struct nsdd data_diag;
 struct nstx normal;uint8_t data_verify[NSTX_PAGE_BYTES];
};
/* Adopt exact terminal scan transport and preserved RX cursors; no allocation,
 * no guessed capability indices. peer indices must match retained scan profile. */
/* Worker-only borrowed slot remains owned until detach after actual scan completion.
 * Other RX and security/data dispatch keep their normal owner and replay path. */
int nst_associated_scan(struct nst *,struct nsb_scan *,const uint8_t *,size_t);
int nst_associated_scan_cancel(struct nst *);
int nst_associated_scan_detach(struct nst *,struct nsb_scan *);
int nst_scan_admissible(const struct nsb_scan *);
int nst_prepare(struct nst *,struct nwi_state *,const struct nct_session *,
 const struct ncr_state *,const struct ncs_cap *,const struct nst_peer *,
 const struct sw_txd *,uint64_t generation,uint64_t now_us,uint64_t deadline_us);
/* Copy complete CID payload or frame into owned storage.0 accepted,1busy,-1bad.
 * CID ticket completion means DMA/source software submission, not firmware ACK.
 * Management waits for actual PID+WLAN EID0f result. SECURITY_SUBMITTED and
 * DATA_SUBMITTED mean only local DDONE + matching TX-free token/storage retirement, never air success. */
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
/* Read-only idle owner proof; grants neither RX memory nor register writes. */
int nst_idle_window(const struct nst *,const struct nwi_state *);
int nst_lease_window(const struct nst *,const struct nwi_state *);
int nst_memory_range(const struct nst *,uint64_t,uint32_t,unsigned write);
int nst_register_write(const struct nst *,uint32_t,uint32_t);
/* Install replay state only once after corresponding actual CID07 submission.
 * GTK RSC6 is little endian (EAPOL RSC bytes0..5); bytes6..7 must be0. */
int nst_replay_install(struct nst *,unsigned pairwise,uint8_t key_id,const uint8_t rsc[8]);
/* WPA2 authorizes after actual STA+PTK/GTK replay; OPEN requires explicit
 * no-privacy association/STA activation, released channel and zero key state.
 * No external boolean supplies station or keyed readiness. */
int nst_replay_replace_group(struct nst *,uint8_t key_id,const uint8_t rsc[8]);
int nst_authorize(struct nst *);
/* Once per attempt, install the owner's fresh <=30s teardown deadline. Existing
 * published-operation and RF grant deadlines remain binding. No IO or retirement
 * claim; may be called under the serialized event callback. */
int nst_cleanup_begin(struct nst *,uint64_t now_us,uint64_t deadline_us);
void nst_close_port(struct nst *);
#endif
