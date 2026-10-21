#ifndef PRIVATE_IOS_WIFI_CONTROL_CONTRACT_H
#define PRIVATE_IOS_WIFI_CONTROL_CONTRACT_H
#include <stddef.h>
#include <stdint.h>
#define NIW_SMALL_BYTES 15408u
#define NIW_SMALL_PAYLOAD 15360u
#define NIW_LARGE_BYTES 245808u
#define NIW_LARGE_PAYLOAD 245760u
#define NIW_GET UINT32_C(0xc02869c9)
/* Exact observed carrier fields only; no private payload ABI invented. */
struct niw_request {uint32_t operation,payload_bytes,request,value;size_t payload_offset,payload_capacity;char interface_name[16];};
/* Pure parser; selector0/small and selector6/large. No user pointer dereference. */
int niw_decode(unsigned,const void *,size_t,struct niw_request *);
/* Returns only call SHAPE support, never a successful driver operation. */
int niw_call_shape(unsigned,size_t,size_t,unsigned);
/* Unbound scaffolding: all requests fail unavailable; no success/carrier/data. */
int niw_unbound_request(const struct niw_request *);
#endif
