#ifndef PRIVATE_BASIC_METADATA_H
#define PRIVATE_BASIC_METADATA_H
#include <stdint.h>
#include <stddef.h>
struct nbc_caps {unsigned sku_present,phy_present;uint32_t efuse_country;uint8_t efuse_valid,band5,nss,max_bw,path,sequence;};
int nbc_caps_decode(const uint8_t *,size_t,struct nbc_caps *);
#endif
