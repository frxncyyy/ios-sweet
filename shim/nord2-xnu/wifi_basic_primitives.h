#ifndef NORD2_BASIC_PRIMITIVES_H
#define NORD2_BASIC_PRIMITIVES_H
#include <stdint.h>
#include <stddef.h>
#define NNI_BASIC_BYTES 76u
/* Selected CID2 set/no-response. No address, NVRAM or domain builder. */
int nni_basic(uint8_t *,size_t,uint8_t,unsigned,unsigned);
#endif
