#ifndef WIFI_WPA2_H
#define WIFI_WPA2_H
#include <stddef.h>
#include <stdint.h>
#define NWP_FRAME 640
/* A pure serialized engine. One native owner must serialize ALL calls. No IO,
 * timers, logging, installation or carrier changes happen here.
 * Inputs/output/context must be disjoint; all buffers are kernel-owned copies.
 * Any NWP_FAIL requires immediate controlled-port closure by the owner. */
enum nwp_state {NWP_OFF,NWP_M1,NWP_M3,NWP_M4_TX,NWP_KEYS,NWP_AUTHORIZED};
enum nwp_action {NWP_DROP=0,NWP_TX=1,NWP_INSTALL=2,NWP_AUTH=3,NWP_BUSY=4,NWP_FAIL=-1};
enum nwp_failure { NWP_F_NONE=0,NWP_F_INTERNAL=1,NWP_F_OFF=2,NWP_F_ENVELOPE=3,NWP_F_KEY_LENGTH=4,NWP_F_M1_DATA=5,NWP_F_M1_STATE_REPLAY_NONCE=6,NWP_F_KEYINFO_STATE=7,NWP_F_MIC=8,NWP_F_M3_REPLAY_LENGTH=9,NWP_F_M3_CONTENT=10,NWP_F_M3_STATE_REPLAY=11,NWP_F_UNWRAP=12,NWP_F_ANONCE=13,NWP_F_KDE=14 };
struct nwp_result {unsigned action,bytes,failure;uint64_t ticket;uint8_t frame[256];};
struct nwp {
 uint64_t generation,serial,pending,install_ticket;unsigned state,tx_kind,installed,ready_mask;
 uint8_t pmk[32],own[6],peer[6],snonce[32],anonce[32],kck[16],kek[16],tk[16],gtk[16],rsc[8];
 unsigned gtk_index;uint8_t rsn[22],ap_rsn[257],m1_replay[8];unsigned ap_rsn_bytes;
 uint8_t ap_rsnxe[257];unsigned ap_rsnxe_bytes;
 uint8_t m3[NWP_FRAME];unsigned m3_bytes;
};
/* RNG callback must use actual cryptographic RNG and return0 only after all32
 * bytes are filled. This private engine has no XNU adapter yet. */
typedef int (*nwp_rng)(void*,uint8_t*,size_t);
int nwp_begin_ap(struct nwp*,uint64_t,const uint8_t pmk[32],const uint8_t own[6],const uint8_t peer[6],const uint8_t rsn[22],const uint8_t *ap_rsn,size_t ap_bytes,nwp_rng,void*);
/* Full captured AP IE list, called before processing M1; exact RSNXE binding. */
int nwp_copy_ap_rsnxe(struct nwp*,const uint8_t*,size_t);
int nwp_ap_rsn(const uint8_t*,size_t);
int nwp_begin(struct nwp*,uint64_t,const uint8_t pmk[32],const uint8_t own[6],const uint8_t peer[6],const uint8_t rsn[22],const uint8_t ap_rsn[22],nwp_rng,void*);
int nwp_receive(struct nwp*,const uint8_t*,size_t,struct nwp_result*);
/* Typed local security submission/storage-retirement receipt, NOT air ACK.
 * Native must prove DDONE and retire its copy before this call. M4 requests
 * exactly one install sequence; actual key/data gates remain independent. */
int nwp_security_submitted(struct nwp*,uint64_t generation,uint64_t ticket,int success);
/* Native owner correlation boundary: only after actual PTK AND GTK readiness
 * and controlled data-path installation. GTK readiness here means actual source
 * no-response submission plus installed host replay state, not a firmware ACK.
 * EID24's unqualified8B is insufficient.
 * No key identity is derived from that event by this module. */
/* mask1 PTK event-qualified,2 GTK submitted/replay installed,4 enforced data path; each receipt must be owner-correlated
 * to install_ticket. Partial successes accumulate, duplicate receipts do not
 * authorize twice. A matching terminal failure clears the whole transaction. */
int nwp_keys_ready(struct nwp*,uint64_t generation,uint64_t install_ticket,unsigned mask,int success);
void nwp_cancel(struct nwp*);
/* Validated passphrase8..63 and SSID1..32. Output wiped on refusal. */
int nwp_pmk(const uint8_t*,size_t,const uint8_t*,size_t,uint8_t out[32]);
#endif
