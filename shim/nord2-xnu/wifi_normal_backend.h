#ifndef NORD2_NORMAL_BACKEND_H
#define NORD2_NORMAL_BACKEND_H
#include "wifi_normal_config.h"
#include "wifi_scan_plan.h"
#include "wifi_normal_rx.h"
#define NCB_COMMANDS (NCFG_COMMANDS+6u)
struct ncb_plan {
 unsigned attempted,prepared,channel_count;
 struct ncfg_input input;struct ncfg_plan config;
 struct ncs_frames scan_frames;
 struct nct_entry entry[NCB_COMMANDS];
};
/* Caller retains authenticated board and actual full capability frame.
 * This plan is one-use, including failed preparation. All26 frames are planned before the first batch MMIO operation. */
int ncb_prepare(struct ncb_plan *,const uint8_t *,size_t,const uint8_t *,size_t,uint8_t,uint8_t);
int ncb_build(void *,unsigned,uint8_t *,size_t,size_t *);
#endif
