#ifndef WIFI_VCN33_H
#define WIFI_VCN33_H
#include <stdint.h>
#define NVC_REGS 13u
struct nvc_io {int(*owner)(void*);int(*read)(void*,uint16_t,uint16_t*);int(*write)(void*,uint16_t,uint16_t);int(*rc)(void*,uint32_t*);};
struct nvc_snapshot {uint16_t value[NVC_REGS];uint32_t valid,rc;};
struct nvc_state {unsigned attempted,mutated,complete,error,writes;struct nvc_snapshot before,after;};
extern const uint16_t nvc_addresses[NVC_REGS];
int nvc_capture(const struct nvc_io*,void*,struct nvc_snapshot*);
int nvc_run(const struct nvc_io*,void*,struct nvc_state*);
#endif
