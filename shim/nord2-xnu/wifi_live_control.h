#ifndef NORD2_WIFI_LIVE_CONTROL_H
#define NORD2_WIFI_LIVE_CONTROL_H
#include <stddef.h>
#include <stdint.h>
/* Development-only, fixed public words. No addresses, credentials or packet
 * contents cross this ABI. The USB decoder never takes a driver lock. */
#define NWL_MAGIC UINT32_C(0x4c57444e)
#define NWL_VERSION 1u
#define NWL_CAP_WORDS 16u
#define NWL_WORDS 64u
#define NWL_CAP_BYTES (NWL_CAP_WORDS * 4u)
#define NWL_BYTES (NWL_WORDS * 4u)
#define NWL_MAX_TOKEN 4095u
#define NWL_MAX_GENERATION 65535u
#define NWL_CAPTURE_WINDOWS 32u
#define NWL_USB_CAP 0x60u
#define NWL_USB_COMMAND 0x61u
#define NWL_USB_REPLY 0x62u
enum nwl_op {NWL_STATUS=1,NWL_CAPTURE_DHCP,NWL_CAPTURE_HEADERS,
 NWL_CAPTURE_STOP,NWL_OFLD_ALLOW,NWL_OFLD_DROP,NWL_DISCONNECT};
#define NWL_COMMANDS UINT32_C(0xfe)
enum nwl_result {NWL_OK,NWL_UNAVAILABLE,NWL_STALE,NWL_REFUSED,NWL_LIMIT};
enum nwl_field {
 NWL_F_MAGIC,NWL_F_VERSION,NWL_F_BYTES,NWL_F_BOOT_LO,NWL_F_BOOT_HI,
 NWL_F_TOKEN,NWL_F_OP,NWL_F_EXPECTED,NWL_F_RESULT,NWL_F_GENERATION_LO,
 NWL_F_GENERATION_HI,NWL_F_SERVICE,NWL_F_NATIVE_PHASE,NWL_F_ERROR,NWL_F_HALT,
 NWL_F_FLAGS,NWL_F_TX_PENDING,NWL_F_RX_PENDING,NWL_F_SUBMITTED,NWL_F_COMPLETED,
 NWL_F_RX_UNICAST,NWL_F_RX_GROUP,NWL_F_TX_DATA,NWL_F_TX_SECURITY,
 NWL_F_TX_DHCP,NWL_F_RX_DHCP,NWL_F_OFFER,NWL_F_ACK,NWL_F_NAK,
 NWL_F_RX_ERROR,NWL_F_RX_PEER,NWL_F_RX_KEY,NWL_F_RX_REPLAY,NWL_F_RX_PLAIN,
 NWL_F_TXS_OK,NWL_F_TXS_FAIL,NWL_F_TXS_AMBIGUOUS,NWL_F_TIME_LO,NWL_F_TIME_HI,
 NWL_F_CAPTURE,NWL_F_WINDOWS_LEFT,NWL_F_OFLD_DROP,NWL_F_DATA_CURSOR,
 NWL_F_COMMAND_CURSOR,NWL_F_RX0,NWL_F_RX1,NWL_F_RX2,NWL_F_RX3,NWL_F_RX4,
 NWL_F_DATA_PUBLISHED,NWL_F_DATA_REPORT,NWL_F_DATA_DDONE,NWL_F_AIR_PENDING,
 NWL_F_WIRE_BUSY,NWL_F_TX_ACCEPTED,NWL_F_TX_LOCAL,NWL_F_RX_DELIVERED,NWL_F_JOIN_READY,
 NWL_F_END
};
/* flags: active, linked, associated, PTK, GTK, controlled port, used,
 * callback retiring, backend failed. None is a DHCP lease assertion. */
struct nwl_request {uint32_t token,op,generation;};
struct nwl_snapshot {uint32_t word[NWL_WORDS];};
/* One-time NSS worker initialization; nonce is freshly generated per boot. */
int nwl_initialize(uint64_t nonce);
int nwl_enabled(void);
/* Sole USB producer. token must be last+1 (no wrap). Exact last-command
 * repeats are idempotent, including after completion. No payload stage.
 * STATUS requires generation0; mutations require an exact16bit generation.
 * ACK means queued only; execute outcome is obtained from REPLY. */
int nwl_submit(unsigned value,unsigned index);
/* Sole retained worker. A successful take owns the one slot until complete. */
int nwl_take(struct nwl_request*);
int nwl_complete(const struct nwl_request*,unsigned result,const struct nwl_snapshot*);
/* Bounded IRQ-safe copy: reply may return0 on a concurrent update; retry IN.
 * All shared words are atomic; never spin awaiting a preempted writer. */
size_t nwl_caps(uint8_t*,size_t);
size_t nwl_reply(uint8_t*,size_t);
/* Pure little-endian EP0 adapter: -1 unrecognized;0 STALL;1 IN;2 ACK. */
int nwl_usb(const uint8_t setup[8],uint8_t*,size_t,uint32_t*length);
#endif
