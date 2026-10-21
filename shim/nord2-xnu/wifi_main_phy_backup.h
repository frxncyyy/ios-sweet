#ifndef PRIVATE_NORD2_PHY_BACKUP_H
#define PRIVATE_NORD2_PHY_BACKUP_H
#include "wifi_main_phy_force.h"
/* Selected wlanAccessCalibrationEMI(TRUE): copy the returned CONN EMI
 * interval to retained driver memory. The caller supplies an actual mapped
 * CONN owner, already allocated destination, and a successfully validated
 * FORCE_CAL result from an owned WFDMA RX capture. No restoration is done. */
struct nfa_backup_io {
 int (*owner)(void *);
 int (*read_emi)(void *,uint32_t,void *,uint32_t);
 int (*barrier)(void *);
};
struct nfa_backup_state {
 unsigned attempted,valid,halt,reads,bytes_copied;
 uint32_t emi_offset,emi_bytes;
 int error;
};
int nfa_backup(const struct nfa_backup_io *,void *,const struct nfa_result *,
 void *,uint32_t,struct nfa_backup_state *);
#endif
