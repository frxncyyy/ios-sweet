#ifndef NORD2_WIFI_SCAN_PLAN_H
#define NORD2_WIFI_SCAN_PLAN_H
#include <stdint.h>
#include <stddef.h>
#define NCS_ACTIVATE_BYTES 76u
#define NCS_SCAN_BODY_BYTES 1188u
#define NCS_SCAN_BYTES 1252u
struct ncs_cap {uint8_t bss,wtbl,wmm,bss_index,own_mac,bmc;unsigned tag9,bss_override,wtbl_override,wmm_override;};
struct ncs_channel {uint8_t band,channel;};
struct ncs_command {uint8_t cid,sequence,set,need_response;uint16_t bytes;};
struct ncs_frames {struct ncs_cap cap;struct ncs_command command[2];uint8_t activate[76],scan[1252];};
/* Fullactual owned CAPframe, matchingseq; source defaults4/32/4 whenTAG9
 * absent/invalid overrides. Native must authenticate snapshot andboardMAC. */
int ncs_cap_mac(const uint8_t*,size_t,uint8_t,struct ncs_cap*);
/* Timing subset: source ordinarydefaults0/0/0 or LOW_SPAN40/20/0. Host
 * transaction deadline separatelyrequired. Channels fromactual config policy,
 * notlegalizedhere; no random/probes/SSIDs. dbdc_disabled is checkedconfiginput.
 * Pure only: no software reservation, publication, credits or ACK fabricated. */
int ncs_build(const uint8_t*,size_t,uint8_t,const uint8_t[6],const struct ncs_channel*,size_t,uint8_t,uint8_t,uint8_t,unsigned,uint16_t,uint16_t,struct ncs_frames*);
struct ncs_event {uint16_t inner_type,rxd_bytes,event_bytes;uint8_t event_sequence,scan_sequence,state,version;};
/* 0 matchingcompletion,1 unrelatedscanbodyseq,-1 malformed. Headersequence
 * andinner typecaptured only; sourceeventdispatch usesouter+EID. */
int ncs_done(const uint8_t*,size_t,uint8_t,struct ncs_event*);
#endif
