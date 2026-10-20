/* Memory-only, opt-in verified Nord 2 firmware reader. No hardware writes. */
#ifndef NORD2_WIFI_FIRMWARE_H
#define NORD2_WIFI_FIRMWARE_H
#include <stdint.h>
#include <stddef.h>
#define NWF_VENDOR_BYTES UINT64_C(544354304)
#define NWF_SCRATCH_BYTES 65536u
#define NWF_OUTPUT_BYTES 1326158u
#define NWF_FILE_COUNT 4u
/* Reader returns zero only after filling the entire vendor-relative range.
 * Adapter must validate GPT/super geometry and serialize with the root UFS
 * driver without changing its partition binding. No device adapter is wired. */
typedef int (*nwf_read_fn)(void *, uint64_t, void *, uint32_t);
struct nwf_blob { const char *name; unsigned char *data; uint32_t size; };
/* Caller owns all memory. Only success publishes blob pointers; failure wipes
 * all accepted output and scratch buffers after extraction starts. Invalid
 * argument rejection does not touch caller buffers. The blobs array must be
 * separate from both buffers. Caller serializes calls and does not consume output
 * until success. This function has no boot hook, allocation or MMIO access. */
int nwf_load(nwf_read_fn, void *, unsigned char *, size_t,
    unsigned char *, size_t, struct nwf_blob [NWF_FILE_COUNT]);
/* Standalone decoder requires non-overlapping source/destination ranges.
 * Destination must contain at least the advertised capacity bytes. */
int nwf_lz4(const unsigned char *, size_t, unsigned char *, size_t);
#endif
