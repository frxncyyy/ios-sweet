#ifndef WIFI_EMI_LEGACY_H
#define WIFI_EMI_LEGACY_H
#include <stdint.h>
#include <stddef.h>
#define WIFI_EMI_LEGACY_COUNT 128u
#define WIFI_EMI_LEGACY_FID UINT64_C(0x82000261)
enum wifi_emi_legacy_status { WIFI_EMI_LEGACY_NOT_CALLED=0,
 WIFI_EMI_LEGACY_VALUE, WIFI_EMI_LEGACY_ERROR,
 WIFI_EMI_LEGACY_INVALID, WIFI_EMI_LEGACY_TRANSPORT };
struct wifi_emi_legacy_field { uint64_t raw[4]; uint32_t offset;
 enum wifi_emi_legacy_status status; int32_t error; };
struct wifi_emi_legacy_snapshot { struct wifi_emi_legacy_field field[128];
 uint32_t valid[4]; unsigned issued; };
typedef int (*wifi_emi_legacy_call_fn)(void *,const uint64_t [8],uint64_t [4]);
/* Exact128 offsets only. No MMIO, decoding or effective-policy claim. */
int wifi_emi_legacy_offset(unsigned,uint32_t *);
int wifi_emi_legacy_args_valid(const uint64_t [8]);
int wifi_emi_legacy_native_call(void *,const uint64_t [8],uint64_t [4]);
/* Deterministic region0..31, fieldsSA/EA/APC0/APC1; stop first nonvalue.
 * All missing entries retain NOT_CALLED, never substitute their zero raw as data.
 * No software deadline can interrupt a stalled EL3 call. Caller serializes. */
int wifi_emi_legacy_collect(wifi_emi_legacy_call_fn,void *,struct wifi_emi_legacy_snapshot *);
int wifi_emi_legacy_probe(void);
#endif
