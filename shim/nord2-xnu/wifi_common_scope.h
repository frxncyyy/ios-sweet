#ifndef NORD2_WIFI_COMMON_SCOPE_H
#define NORD2_WIFI_COMMON_SCOPE_H
#include "wifi_common.h"
/* Scoped callback variant. Same40 snapshots/apply/reverse as canonical.
 * inner invoked ONLY after all40 sourcevalues verified, before anyrestore.
 * inner must return clean1 ONLY after its owned resources restored; clean0
 * leaves common snapshots/power ownership retained for mandatory dirtyhalt.
 * No callback on failed commonapply. optin1; allprereqs canonical. */
int ncc_scope(const struct ncc_ops *,void *,unsigned,struct ncc_result *,int (*)(void *,int *),int *);
#endif
