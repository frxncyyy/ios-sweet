#ifndef NORD2_WIFI_STATION_DATA_DIAG_H
#define NORD2_WIFI_STATION_DATA_DIAG_H
#include <stddef.h>
#include <stdint.h>
/* Diagnostic-only public header classification. Never an admission, ACK,
 * authenticated DHCP exchange, lease, key or networking decision. No pointers,
 * key bytes or packet payload are retained. The bounded DHCP correlation ledger
 * retains only request XID/client identities until terminal summary, never logs
 * them and never authorizes a lease or influences packet admission. */
enum nsdd_parse {NSDD_OTHER,NSDD_DHCP,NSDD_IP_SHORT,NSDD_IP_HEADER,
 NSDD_IP_EXTENT,NSDD_FRAGMENT,NSDD_UDP_EXTENT,NSDD_BOOTP_SHORT,
 NSDD_BOOTP_HEADER,NSDD_COOKIE,NSDD_OPTIONS,NSDD_NO_TYPE,
 NSDD_DUP_TYPE,NSDD_TYPE,NSDD_OPTION_LIMIT,NSDD_OVERLOAD};
struct nsdd_frame {uint32_t ether,bytes,destination,dhcp,parse;};
struct nsdd_txs {uint32_t bytes,pid,wlan,status,sequence,tx_count,rate,flags,advanced,tid,flush;};
enum nsdd_rx_reason {NSDD_RX_EXTENT=1,NSDD_RX_ERROR,NSDD_RX_GROUP_EXTENT,
 NSDD_RX_RAW_EAPOL,NSDD_RX_RAW_HEADER,NSDD_RX_PEER,NSDD_RX_FC,
 NSDD_RX_EAPOL,NSDD_RX_PORT,NSDD_RX_CIPHER,NSDD_RX_KEY,
 NSDD_RX_REPLAY,NSDD_RX_PLAINTEXT,NSDD_RX_REASON_END};
struct nsdd_identity {uint32_t xid;uint8_t client[6];};
struct nsdd {
 uint32_t tx_security,tx_data,rx_unicast,rx_group,tx_dhcp,rx_dhcp;
 uint32_t tx_unicast,tx_group,tx_ether[5],rx_ether[5];
 uint32_t tx_type[9],rx_type[9],txs_ok,txs_fail,ambiguous,last_kind,last_pub;
 uint32_t rejected[NSDD_RX_REASON_END],reject_logged[NSDD_RX_REASON_END];
 uint32_t identities,identity_next,reply_match[9],reply_other[9],match_logs,rx_samples;
 struct nsdd_identity identity[4];
 uint32_t rx_error_logs;
 /* Independent live capture budget. Original counters/identities/quotas and
  * the boot PID ledger are never reset by a USB capture request. */
 uint32_t live_mode,live_epoch,live_tx,live_rx;
 uint32_t headers_tx,headers_rx,dhcp_tx,dhcp_rx,txs_early,txs_live,records,summary;
};
/* Fixed RXD metadata only; no rejected payload access. dst:0 unknown,
 * 1 hardware UC2ME,2 hardware MC/BC. Never an authenticated destination. */
struct nsdd_rx_error {uint64_t generation;uint32_t n,d1,d2,d4,wlan,sec,key,ht,groups,type,a1,dst,tid;};
int nsdd_rx_error_take(struct nsdd*,struct nsdd_rx_error*,uint64_t,const uint8_t*,size_t);
void nsdd_rx_error(struct nsdd*,uint64_t,const uint8_t*,size_t);
/* Serialized retained owner only. Boot-lifetime diagnostic identities never
 * reset with NST/nsdd/generation/scan state.127 is permanently ambiguous. */
#define NSDD_PID_MANAGEMENT_MIN 1u
#define NSDD_PID_MANAGEMENT_MAX 31u
#define NSDD_PID_SECURITY_MIN 32u
#define NSDD_PID_SECURITY_MAX 63u
#define NSDD_PID_DATA_MIN 64u
#define NSDD_PID_DATA_MAX 126u
#define NSDD_PID_SHARED 127u
struct nsdd_pid_record {uint64_t generation;uint32_t pid,wlan,kind,protected_frame,ether,dhcp,parse,bytes,eapol_class,attempted,published,publication,local,seen,status,closed;};
unsigned nsdd_pid_reserve(uint64_t,unsigned wlan,unsigned kind,unsigned protect,const uint8_t*,size_t);
void nsdd_pid_attempt(uint64_t,unsigned pid,unsigned wlan);
void nsdd_pid_publish(uint64_t,unsigned pid,unsigned wlan,unsigned publication);
void nsdd_pid_local(uint64_t,unsigned pid,unsigned wlan);
void nsdd_pid_status(uint64_t,const struct nsdd_txs*);
void nsdd_pid_finish(uint64_t);
#if !defined(KERNEL) && !defined(_KERNEL)
/* Host hardware models explicitly represent a fresh kernel boot. Never kernel. */
void nsdd_test_boot_reset(void);
const struct nsdd_pid_record *nsdd_test_pid(unsigned);
#endif
int nsdd_classify(struct nsdd_frame*,const uint8_t*,size_t);
int nsdd_decode_txs(struct nsdd_txs*,const uint8_t*,size_t);
/* Serialized native owner only. type2 is security, type3 ordinary data.
 * TX logs actual publication; RX logs native validated/rearmed Ethernet only.
 * Last kind/pub is context, NEVER correlation of reused host PID127 TXS. */
void nsdd_publish(struct nsdd*,uint64_t generation,uint32_t submitted,unsigned type,const uint8_t*,size_t);
void nsdd_receive(struct nsdd*,uint64_t generation,const uint8_t*,size_t);
void nsdd_event(struct nsdd*,uint64_t generation,const uint8_t *event_body,size_t,unsigned authorized);
void nsdd_reject(struct nsdd*,uint64_t generation,unsigned reason);
void nsdd_rx_snapshot(struct nsdd*,uint64_t generation);
void nsdd_summary(struct nsdd*,uint64_t generation);
void nsdd_capture(struct nsdd*,unsigned mode,unsigned epoch);
#endif
