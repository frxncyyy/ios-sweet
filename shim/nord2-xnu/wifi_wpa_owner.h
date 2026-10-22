#ifndef WIFI_WPA_OWNER_H
#define WIFI_WPA_OWNER_H
#include "wifi_station_transaction.h"
/* Serialized retained-radio-worker API. No client-thread calls or inline
 * callbacks from submit. Not an installed native transport by itself. */
enum nwo_receipt { NWO_PREPARED=1,NWO_COMMAND,NWO_AIR_TX,NWO_CONFIGURED,
 NWO_GTK_REPLAY,NWO_DATA_PATH,NWO_RETIRED };
struct nwo_install { uint8_t gtk_index,rsc[8]; };
struct nwo_ops {
 /* Must COPY action and metadata before returning0. Return1 BUSY without mutation or callbacks; -1 is a
  * terminal refusal with no later callback. Never log their contents. */
 int (*submit)(void*,const struct stc_action*,const struct nwo_install*);
 /* Synchronous controlled-port closure, including queued ordinary data.
  * Return0 only when enforced. Does not imply firmware key retirement. */
 int (*close_port)(void*);
};
struct nwo {
 struct stc transaction;struct nwo_ops ops;void *context;
 uint64_t in_generation,in_ticket;unsigned in_kind,issuing,halted,closed;
};
int nwo_begin(struct nwo*,struct stc_arbiter*,const struct stc_config*,const struct nwo_ops*,void*,uint64_t);
int nwo_pump(struct nwo*,uint64_t);
/* Typed completion: DMA DDONE has deliberately no receipt type. Exactly one
 * completion retires each submitted ticket. Failed receipt always closes port. */
int nwo_complete(struct nwo*,uint64_t,uint64_t,unsigned receipt,int success,uint64_t);
int nwo_eapol(struct nwo*,uint64_t,const uint8_t*,size_t,uint64_t);
int nwo_management(struct nwo*,uint64_t,const uint8_t*,size_t,uint64_t);
int nwo_grant(struct nwo*,uint64_t,uint8_t,uint8_t,uint8_t,uint8_t,int,uint64_t);
int nwo_key24(struct nwo*,uint64_t,const uint8_t*,size_t,uint64_t);
int nwo_link_lost(struct nwo*,uint64_t,uint64_t);
int nwo_cancel(struct nwo*,uint64_t,uint64_t);
int nwo_authorized(const struct nwo*);
#endif
