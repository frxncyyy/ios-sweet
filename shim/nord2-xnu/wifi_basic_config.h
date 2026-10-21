#ifndef PRIVATE_BASIC_CONFIG_H
#define PRIVATE_BASIC_CONFIG_H
#include "wifi_capability_dispatcher.h"
#include "wifi_basic_resources.h"
#include "wifi_basic_metadata.h"
#include "wifi_basic_primitives.h"
#define NBC_DEADLINE_US UINT64_C(3000000)
#define NBC_POLL_BUDGET 3000u
#define NBC_RX_BUDGET 16u
enum nbc_phase {NBC_NEW,NBC_LEASED,NBC_WRITING,NBC_SUBMITTED,NBC_DRAINING,NBC_DONE,NBC_SKIPPED};
struct nbc_state {
 struct ncd_state *cap;struct nwi_state *init;const struct nfp_native *firmware;
 unsigned attempted,prepared,entered,phase,halt,error,mutated,submit_attempted,tx_done,completed;
 unsigned skipped,skip_reason,polls,debug_events,rearmed,metadata_valid;
 unsigned sleepy_events,last_sleepy;
 unsigned tx_index,rx_index,callbacks,publications,tx_didx,tx_consumed;uint32_t tx_control,tx_raw[4];unsigned tx_observed;
 uint64_t tx_descriptor_address,tx_expected_buffer;uint8_t sequence;
 struct ncd_event last_debug;unsigned debug_index;
 uint64_t started,last,deadline;uint32_t irq[2];unsigned irq_valid;
 struct ncp_result verified_cap;struct nbc_caps caps;struct nnr_plan plan;struct nnr_budget budget;
 uint8_t command[76],descriptor[16],rx[3840];size_t rx_bytes;
};
/* Pure preparation only: optional refusal is a skip, with no backend callback.
 * MAC/regulatory metadata cannot grant TX ownership and are not CID2 inputs. */
int nbc_prepare(struct ncd_state *,struct nwi_state *,const struct nfp_native *,struct nbc_state *);
/* Pure captured predicates used by the actual native one-command lease hook. */
int nbc_lease_window(const struct nbc_state *,const struct ncd_state *,const struct nwi_state *,const struct nfp_native *);
/* One source no-response CID2 TX, no invented ACK or resource refund. */
int nbc_run(const struct ncd_io *,void *,struct nbc_state *);
#endif
