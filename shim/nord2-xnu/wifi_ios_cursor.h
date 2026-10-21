#ifndef PRIVATE_IOS_WIFI_CACHE_CURSOR_H
#define PRIVATE_IOS_WIFI_CACHE_CURSOR_H
#include <stddef.h>
#include <stdint.h>
#define NIWC_MAX_RECORDS 64u
#define NIWC_RECORD_BYTES 1164u
enum niwc_phase {NIWC_NEW,NIWC_WAITING,NIWC_READY,NIWC_CANCELLED};
struct niwc_cursor {enum niwc_phase phase;uint64_t generation;const uint8_t *records;unsigned count,next;};
/* Pure local state only. The real backend owner must authenticate generation,
 * actual SCAN_DONE, immutable records and lifetime, under its gate. */
int niwc_begin(struct niwc_cursor *,uint64_t);
int niwc_complete(struct niwc_cursor *,uint64_t,const uint8_t *,size_t,unsigned);
/* Returns1 copied row,0 real completed snapshot exhausted,-1 refusal. */
int niwc_take(struct niwc_cursor *,uint64_t,uint8_t *,size_t);
int niwc_cancel(struct niwc_cursor *,uint64_t);
#endif
