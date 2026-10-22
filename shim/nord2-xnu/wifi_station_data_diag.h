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
 uint32_t headers_tx,headers_rx,dhcp_tx,dhcp_rx,txs_early,txs_live,records,summary;
};
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
#endif
