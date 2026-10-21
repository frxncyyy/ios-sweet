#ifndef PRIVATE_NIWE_PIPE_H
#define PRIVATE_NIWE_PIPE_H
#include <stdint.h>
#include <stddef.h>
#define NIWE_HEADER 48u
#define NIWE_MAX_PRIMARY 4080u
struct niwe_view {uint32_t consumer,producer,capacity;};
struct niwe_record {
 uint32_t kind,event,next,primary_offset,first_bytes,second_bytes,total;
 uint16_t primary,secondary;uint8_t interface_name[16];
};
/* Immutable copied ring+index snapshot only. 0 empty,1 pad,2 event,-1 invalid.
 * Stricter bounded subset than consumer: full published extent required;
 * primary<=4080, no truncated-copy compatibility claim. Atomic output. */
int niwe_peek(const uint8_t*,size_t,const struct niwe_view*,struct niwe_record*);
/* Copies primary data from the same immutable snapshot. Does not advance/CAS. */
int niwe_copy_primary(const uint8_t*,size_t,const struct niwe_view*,const struct niwe_record*,uint8_t*,size_t);
struct niwe_space {uint32_t padding,record_offset,next,total,free_bytes;};
/* Proposed private single-producer safety policy, NOT recovered Apple producer:
 * reserve4-byte empty/full distinction; pad with kind80 words when header
 * cannot fit contiguously. No actual shared writes/publication/notification. */
int niwe_plan(const struct niwe_view*,uint16_t,uint16_t,struct niwe_space*);
/* Offline52-byte consumer-compatible event10 formatter. Uninterpreted header
 * bytes4..23 zeroed; no monotonic ID/Apple producer identity is claimed.
 * Native must authenticate actual backend completion before any publication. */
int niwe_scan_done(uint8_t*,size_t,const uint8_t[16],uint32_t);
#endif
