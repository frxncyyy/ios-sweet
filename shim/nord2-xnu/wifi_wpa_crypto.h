#ifndef NWP_CRYPTO_PROVIDER_H
#define NWP_CRYPTO_PROVIDER_H
#include <stddef.h>
#include <stdint.h>
/* Original project glue over existing MIT-licensed BearSSL primitives.
 * Separate nonaliasing bounded input/output, exclusive caller ownership. */
int nwpc_hmac(const uint8_t*,size_t,const uint8_t*const*,const size_t*,size_t,uint8_t out[20]);
int nwpc_pmk(const uint8_t*,size_t,const uint8_t*,size_t,uint8_t out[32]);
int nwpc_ptk(const uint8_t pmk[32],const uint8_t sorted_context[76],uint8_t out[48]);
int nwpc_unwrap(const uint8_t kek[16],const uint8_t*,size_t,uint8_t*,size_t,size_t*);
#endif
