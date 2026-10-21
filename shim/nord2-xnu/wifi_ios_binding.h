#ifndef NIWB_BINDING_CONTRACT_H
#define NIWB_BINDING_CONTRACT_H
#include <stddef.h>
#include <stdint.h>
/* A private codec/view, not an IOUserClient, owner token, or readiness claim. */
enum niwb_phase { NIWB_OPEN=1, NIWB_DRAINING=2, NIWB_CLOSED=3 };
enum niwb_control { NIWB_CONTROL_0=1, NIWB_CONTROL_6=2, NIWB_CONTROL_ALL=3 };
enum niwb_pipe {
 NIWB_ALLOCATE_3=1, NIWB_MAP_DATA_1=2, NIWB_MAP_STATE_2=4,
 NIWB_REGISTER_ASYNC_2=8, NIWB_HALT_4=16, NIWB_QUIESCE=32,
 NIWB_PIPE_ALL=63
};
/* Construct only inside the serialized owning native object from installed,
 * reviewed handler tables and actual close/drain state. Never copy this view
 * from externalMethod input. Bit presence cannot prove a handler is correct.
 * Pipe capability precedes allocation: the framework queries 5 before 3/2. */
struct niwb_dispatch_view { unsigned phase,control_handlers,pipe_handlers; };
/* Exact selectors1/5 struct call: no scalar/async arguments, input0, output1.
 * Returns one capability byte, never backend/radio/association readiness.
 * Closed/draining or malformed input refuses atomically. */
int niwb_selector(const struct niwb_dispatch_view *,unsigned selector,
 size_t input_bytes,size_t output_capacity,unsigned scalar_inputs,
 unsigned scalar_outputs,unsigned async_refs,uint8_t *output,size_t *used);
/* Dedicated bind GET43 carrier only (not all general DRIVER_VERSION queries).
 * Driver-owned identity includes its final NUL, 2..256 bytes. No version word,
 * firmware-ready bit, feature list or hardware/firmware version is invented.
 * Atomic output on malformed request, identity, or insufficient capacity.
 * Native caller still binds the decoded interface to its real owning service. */
int niwb_driver_version(const void *carrier,size_t carrier_bytes,
 const char *driver_identity,size_t identity_bytes,void *output,
 size_t output_capacity,size_t *used);
#endif
