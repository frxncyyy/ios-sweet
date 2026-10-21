#ifndef WIFI_DEFAULT_FEATURE_H
#define WIFI_DEFAULT_FEATURE_H
#include <stdint.h>
#define NDF_COMMANDS 3u
#define NDF_BODY_BYTES 284u
#define NDF_CID 0x70u
/* SET, no response. Source-default profile only; not optional cfg overrides.
 * out must hold 284 bytes. Invalid index/null leaves output untouched. */
int ndf_body(unsigned index, uint8_t out[NDF_BODY_BYTES]);
#endif
