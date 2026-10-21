#ifndef NORD2_NORMAL_TRANSPORT_H
#define NORD2_NORMAL_TRANSPORT_H
#include "wifi_basic_config.h"
#define NCT_COMMAND_LIMIT 64u
#define NCT_WIRE_LIMIT 4096u
#define NCT_COMMAND_US UINT64_C(3000000)
#define NCT_BATCH_MAX_US UINT64_C(60000000)
#define NCT_SERVICE_MAX_US UINT64_C(15000000)
#define NCT_POLL_LIMIT 3000u
#define NCT_EVENT_LIMIT 8192u
struct nct_entry {uint16_t bytes;uint8_t cid,set_query;};
/* Pure builder. Transport inserts real sequence at full-wire byte39; source
 * bytes remain immutable. Recipe/source provenance is native caller owned. */
struct nct_recipe {
 const struct nct_entry *entry;unsigned count;
 int (*build)(void *,unsigned,uint8_t *,size_t,size_t *);void *context;
 uint64_t batch_us,service_us;
};
enum nct_phase {NCT_NEW,NCT_PREPARED,NCT_COMMAND,NCT_WRITING,NCT_SUBMITTED,NCT_BETWEEN,NCT_SERVICE,NCT_DONE,NCT_HALTED,NCT_DRAIN};
struct nct_session;
struct nct_access {struct nct_session *session;const struct ncd_io *hardware;void *context;};
struct nct_record {uint8_t cid,sequence;uint16_t bytes;unsigned from,to,didx,polls,observed;
 unsigned publication_attempted,published,credit_released,tx_done;uint32_t raw[4];};
struct nct_session {
 const struct nbc_state *basic;struct ncd_state *cap;struct nwi_state *init;const struct nfp_native *firmware;
 unsigned attempted,prepared,entered,phase,halt,error,refusal,mutated,cursor,count,completed;
 unsigned callbacks,pump_calls,events,scan_complete,service_timed_out,event_limit_reached;
 uint32_t pse_total,basic_spent,capacity,available,reserved,acquired,released;
 unsigned first_tx,base_completed;uint8_t first_sequence;
 uint64_t started,last,deadline,command_deadline,service_deadline,batch_us,service_us;
 struct nct_entry entry[NCT_COMMAND_LIMIT];uint64_t fingerprint[NCT_COMMAND_LIMIT];
 struct nct_record record[NCT_COMMAND_LIMIT];
 int (*build)(void *,unsigned,uint8_t *,size_t,size_t *);void *build_context;
 unsigned drain_quiescent,drain_refused;
 unsigned continuation;const struct nct_session *previous;
 struct nct_access access;uint8_t wire[NCT_WIRE_LIMIT],descriptor[16];
};
/* Pure one-use admission/seal. Return1 clean prerequisite refusal, -1 malformed
 * recipe/reuse,0 prepared. Plain copies cannot grant native authority. */
int nct_prepare(const struct nbc_state *,const struct nct_recipe *,struct nct_session *);
int nct_done_receipt(const struct nct_session *);
int nct_continue_prepare(const struct nbc_state *,const struct nct_session *,const struct nct_recipe *,struct nct_session *);
int nct_lease_window(const struct nct_session *,const struct nbc_state *,const struct ncd_state *,const struct nwi_state *,const struct nfp_native *);
/* Checked IO is usable only via exact session.access while actual lease active.
 * Root's RX parser can consume it with the unchanged ncd_io interface. */
extern const struct ncd_io nct_checked_io;
/* Pump uses checked IO. -1fatal,0continue,1matching scan complete; consumed<=budget.
 * Optional sleep callback is scheduler-only (no MMIO); transport checks lease/
 * time/owner before and after. Root supplies fixed bounded sleeps to keep USB live. */
struct nct_hooks {
 int (*pump)(void *,const struct ncd_io *,void *,unsigned,unsigned *);void *pump_context;
 int (*sleep)(void *,unsigned);void *sleep_context;
};
int nct_run(const struct ncd_io *,void *,const struct nct_hooks *,struct nct_session *);
#endif
