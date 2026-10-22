#ifndef STC_TRANSACTION_H
#define STC_TRANSACTION_H
#include <stdint.h>
#include <stddef.h>
#include "wifi_station_wire.h"
#include "wifi_wpa2.h"
/* Pure serialized model, NOT a native lease/adapter. Zero-initialize once.
 * Caller-owned scan arbiter must be shared with the actual scan owner. */
enum stc_phase {STC_NEW,STC_PREPARE,STC_CHANNEL,STC_GRANT,STC_AUTH_TX,STC_AUTH_RX,STC_ASSOC_TX,STC_ASSOC_RX,STC_CONFIGURE,STC_EAPOL,STC_EAPOL_TX,STC_PTK,STC_PTK_READY,STC_GTK,STC_DATA,STC_CONNECTED,STC_DEAUTH,STC_REMOVE_PTK,STC_REMOVE_GTK,STC_RETIRE,STC_RELEASE,STC_DONE,STC_HALTED};
enum stc_kind {STC_NONE,STC_PREPARE_PEER,STC_CID_CHANNEL,STC_TX_AUTH,STC_TX_ASSOC,STC_CONFIGURE_PEER,STC_TX_EAPOL,STC_CID_PTK,STC_CID_GTK,STC_INSTALL_DATA_PATH,STC_TX_DEAUTH,STC_DELETE_PTK,STC_DELETE_GTK,STC_RETIRE_PEER,STC_ABORT_CHANNEL};
enum stc_outcome {STC_PENDING,STC_AUTHORIZED,STC_FAILED,STC_CANCELED,STC_TIMEOUT,STC_DISCONNECTED};
struct stc_arbiter {unsigned mode,connection_used;uint64_t generation;}; /* 0idle,1scan,2connection */
int stc_scan_acquire(struct stc_arbiter*,uint64_t);
int stc_scan_release(struct stc_arbiter*,uint64_t);
struct stc_config {
 uint64_t generation,deadline_ms;uint32_t cleanup_timeout_ms;
 struct sw_assoc assoc;struct sw_channel channel;
 uint8_t sta_wlan,group_wlan; /* source-owned distinct WTBL allocations */
 const uint8_t *pmk,*station_rsn,*ap_rsn; /*32/22/full AP IE */
 size_t ap_rsn_bytes; /*0 retains legacy22; otherwise exact captured IE extent */
 nwp_rng rng;void*rng_context;
};
struct stc_action {uint64_t generation,ticket;unsigned kind,bytes;uint8_t payload[640];};
struct stc {
 unsigned attempted,phase,outcome,authenticated,associated,channel_held,peer_prepared,ptk_attempted,gtk_attempted;
 unsigned grant_seen,key_event_seen,response_seen,response_bytes;uint16_t aid;uint8_t response[512];
 unsigned cleanup_abort_first,resume_phase,eapol_pending,eapol_bytes;uint8_t queued_eapol[640];
 uint64_t generation,serial,last_ms,deadline,cleanup_deadline,wpa_tx_ticket;uint32_t cleanup_timeout_ms;
 struct stc_arbiter *arbiter;struct sw_peer peer;struct sw_channel channel;
 uint8_t sta_wlan,group_wlan,gtk_index,ssid[32],rates[12];
 struct sw_assoc assoc;struct nwp wpa;struct stc_action action;
};
/* Adapter must COPY actions into its own DMA lifetime storage; never DMA this
 * model's memory. Cancel wipes secrets immediately and asks bounded cleanup.
 * command receipt success means actual source submission/software completion;
 * TX_* receipt success means EID0f PID+WLAN correlated actual frame status.
 * RETIRE means software deletion/queue cleanup, NOT a proven firmware barrier.
 * PREPARE/CONFIGURE/RETIRE/DATA receipts are explicit future-adapter contracts,
 * not evidence that these operations exist in the current installed runtime. */
int stc_begin(struct stc*,struct stc_arbiter*,const struct stc_config*,uint64_t now_ms);
const struct stc_action*stc_next(const struct stc*);
int stc_receipt(struct stc*,uint64_t generation,uint64_t ticket,int success,uint64_t now_ms);
int stc_grant(struct stc*,uint64_t generation,uint8_t bss,uint8_t token,uint8_t band,uint8_t channel,int granted,uint64_t now_ms);
int stc_management(struct stc*,uint64_t generation,const uint8_t*,size_t,uint64_t now_ms);
int stc_eapol(struct stc*,uint64_t generation,const uint8_t*,size_t,uint64_t now_ms);
/* Full validated EID24 payload; body[1] opaque. No GTK ACK path exists. */
int stc_key_event(struct stc*,uint64_t generation,const uint8_t*,size_t,uint64_t now_ms);
/* Native owner records its actual source channel-release submission; no FW ACK. */
int stc_channel_released(struct stc*,uint64_t generation,uint8_t token,uint64_t now_ms);
int stc_link_lost(struct stc*,uint64_t generation,uint64_t now_ms);
int stc_cancel(struct stc*,uint64_t generation,uint64_t now_ms);
int stc_tick(struct stc*,uint64_t now_ms);
#endif
