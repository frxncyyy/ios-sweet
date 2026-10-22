#ifndef WIFI_STATION_DIAG_H
#define WIFI_STATION_DIAG_H
#include <stdint.h>
#include <stddef.h>
/* Observational only. No frame bytes, identities, replay counters or secrets. */
struct nstd_record {uint64_t generation,ticket;uint32_t kind,v[8];};
void nstd_tx(struct nstd_record*,uint64_t,uint64_t,const uint8_t*,size_t,unsigned,unsigned);
void nstd_txs(struct nstd_record*,uint64_t,uint64_t,const uint8_t*,size_t,unsigned,unsigned,unsigned,unsigned);
void nstd_eapol(struct nstd_record*,uint64_t,uint64_t,const uint8_t*,size_t);
/* Sole retained radio worker only. First96 records globally; no effect on IO. */
void nstd_rx(uint64_t,uint64_t,unsigned,const uint8_t*,size_t);
void nstd_summary(struct nstd_record*,uint64_t,uint64_t);
void nstd_log(const struct nstd_record*);
#endif
