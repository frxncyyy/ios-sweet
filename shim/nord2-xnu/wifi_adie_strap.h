#ifndef NORD2_WIFI_ADIE_STRAP_H
#define NORD2_WIFI_ADIE_STRAP_H
#include "wifi_adie_id.h"
struct nis_field {uint32_t address,mask,value;};
extern const struct nis_field nis_fields[4];
struct nis_ops {
 int (*check)(void *); /*owned AP/power/readability/cold CPUs,not SPIbusy*/
 int (*read)(void *,uint32_t,uint32_t *);
 /* must change ONLY given source-ownedmask; failure may partialwrite */
 int (*write_mask)(void *,uint32_t,uint32_t,uint32_t);
 int (*barrier)(void *);
};
struct nis_result {uint32_t before[4];unsigned snapshots,touched,restored;int prepared,error,clean;struct nai_result id;};
/* prepare called BEFORE common mutation; snapshots all4fields and requires
 * A-die reset0. execute is scoped commoncallback; retains common ifdirty.
 * No SYSRAM,calibration,CPUrelease; GPIO180strap+ADIEreset only. */
int nis_prepare(const struct nis_ops *,void *,struct nis_result *);
int nis_execute(const struct nis_ops *,const struct nai_ops *,void *,struct nis_result *);
#endif
