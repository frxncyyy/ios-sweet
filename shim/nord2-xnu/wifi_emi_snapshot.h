/* Isolated read-only EMI observation. No boot hook or automatic activation. */
#ifndef NORD2_WIFI_EMI_SNAPSHOT_H
#define NORD2_WIFI_EMI_SNAPSHOT_H
#include <stdint.h>
#include <stddef.h>
#define NWES_REGIONS 32u
#define NWES_FIELDS 4u
#define NWES_SMC_ID UINT64_C(0xc200050b)
enum nwes_status { NWES_NOT_CALLED=0, NWES_VALUE, NWES_KNOWN_ERROR,
 NWES_INVALID_VALUE, NWES_TRANSPORT_ERROR };
struct nwes_field { uint64_t raw[4]; uint32_t value; enum nwes_status status;
 int32_t known_error; };
/* Fields: SA, EA, APC group0, APC group1. Raw observations do not establish
 * effective permissions, region priority, ownership, or authority to write. */
struct nwes_region { uint32_t region; struct nwes_field field[NWES_FIELDS]; };
/* Callback returns0 with all four result registers filled, or nonzero without
 * a usable result. Never retry. Arguments are freshly constructed read-only
 * calls. Callback/context, input region list and output must be separate and
 * stable for the duration; caller owns/serializes all memory. */
typedef int (*nwes_call_fn)(void *, const uint64_t [8], uint64_t [4]);
/* Regions may be27/31 initially, or any unique selection0..31. Count1..32.
 * All arguments validate before output modification/callbacks. Outputs are
 * initialized first; unissued fields retain NOT_CALLED. Return0 means every
 * query returned (inspect field statuses);1 means stopped on unsupported or
 * transport failure;-1 means bad arguments. Unsupported(-1) stops immediately.
 * There is no deadline: a hung firmware SMC cannot be interrupted here. */
int nwes_collect(nwes_call_fn,void *,const uint32_t *,size_t,
 struct nwes_region *,size_t);
/* Explicit callback only; never invoked automatically. Host returns failure.
 * Rejects all IDs/actions/selectors except the exact structured READ ABI.
 * No SET/CLEAR/debug/MMIO, firmware placement, allocation or power changes. */
int nwes_native_call(void *,const uint64_t [8],uint64_t [4]);
/* Pure decode using captured values and caller-provided live-DT geometry.
 * Returns0 only for valid SA<=EA, checked shifts/additions and DRAM bounds.
 * Inclusive end; does not imply AP access. No use of APC/priority policy. */
int nwes_decode_range(const struct nwes_region *,uint64_t,uint64_t,unsigned,
 uint64_t *,uint64_t *);
int nwes_domain(const struct nwes_region *,unsigned,uint32_t *);
#endif
