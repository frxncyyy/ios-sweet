#ifndef WIFI_WPA_DIAG_H
#define WIFI_WPA_DIAG_H
#include <stddef.h>
#include <stdint.h>
/* Single retained worker only; no packet/identity/crypto material retained. */
struct nwpd_budget {unsigned early,late,failure,serial;};
struct nwpd_record {uint64_t generation,ticket;unsigned n,bucket,state,bytes,version,type,declared,info,class_id,result,failure;};
/* Called AFTER actual engine evaluation, with state captured BEFORE evaluation.
 * Separate8 early /24 authorized /8 failure reservations; data never uses them.
 * Does not classify an unverified public header as cryptographic success. */
int nwpd_take(struct nwpd_budget*,struct nwpd_record*,uint64_t,uint64_t,unsigned,const uint8_t*,size_t,int,unsigned);
void nwpd_observe(uint64_t,uint64_t,unsigned,const uint8_t*,size_t,int,unsigned);
/* Pre-engine extent rejection. No payload pointer accepted/read. One emitted
 * row for the boot's single connection epoch, independent of all other budgets. */
struct nwpd_gate {uint64_t generation,ticket,bytes;unsigned state,present,reason;};
int nwpd_gate_record(struct nwpd_gate*,uint64_t,uint64_t,unsigned,unsigned,size_t);
void nwpd_gate_observe(uint64_t,uint64_t,unsigned,unsigned,size_t);
#endif
