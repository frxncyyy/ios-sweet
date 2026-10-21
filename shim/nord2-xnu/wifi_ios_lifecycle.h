#ifndef PRIVATE_LIFECYCLE_SCAN_H
#define PRIVATE_LIFECYCLE_SCAN_H
#include "wifi_ios_age.h"
/* All state operations require the owning lifecycle gate. Preparation alone
 * runs outside that gate on an immutable retained scan and unique output. */
struct nils_state {uint64_t generation,serial;unsigned open,inflight;struct niwa_snapshot *cache,*pending;};
struct nils_ticket {uint64_t generation,serial;struct niwa_snapshot *output;};
int nils_open(struct nils_state *,uint64_t);
int nils_begin(struct nils_state *,struct nils_ticket *,struct niwa_snapshot *);
int nils_prepare(const struct nils_ticket *,const struct nsb_scan *,uint64_t,uint64_t,
 const struct niwp_rate_map *,size_t,struct niwa_snapshot *);
/* Returns0 installed complete (including empty);1 truthful noncomplete;
 * 2 canceled generation, -1 invalid call. A matched ticket is consumed once.
 * Caller retains/frees output only after finish; cache never copied/relocated. */
int nils_finish(struct nils_state *,const struct nils_ticket *,struct niwa_snapshot *);
/* Close prevents admission and detaches cache. A pending preparation still owns
 * its buffers until matching finish returns2; close must not destroy state. */
/* Gate-held GET:1 row,0 proven complete EOF,-1 unavailable/stale. */
int nils_take(struct nils_state *,uint64_t,uint8_t *,size_t);
void nils_close(struct nils_state *);
#endif
