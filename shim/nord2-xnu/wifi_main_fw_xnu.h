#ifndef PRIVATE_NORD2_MAIN_FW_XNU_H
#define PRIVATE_NORD2_MAIN_FW_XNU_H
#include "wifi_main_fw_place.h"
struct nfp_native {
 struct nfp_state state;
 struct nmt_layout layout;
 uintptr_t cfg_io,host_io,window_io,coex_io;
 unsigned attempted,prepared,halt,coex_attempted,coex_done;
 unsigned pre_sync_valid,finish_attempted,finish_done,ready,ready_polls;
 uint32_t pre_sync,ready_raw;
};
extern struct nfp_native nfp_native;
/* Sleepable source selection after nwfn_prepare, before fresh WFDMA start.
 * cfg_io is the already mapped npa_native.cfg Device page in the owning TU.
 * No firmware write or remap occurs here. */
int nfp_xnu_prepare(uintptr_t cfg_io,uintptr_t host_io);
/* One exact source span while the same nwfn_native remains fresh/initialized.
 * Native coordinator calls indices 0,1 then real CID40/FORCE_CAL/PCCIF,
 * then indices 2..6; it must not call all seven back-to-back. */
int nfp_xnu_span(unsigned);
/* Selected coant0 board path: exact PCCIF4 status to modem, after actual
 * FORCE_CAL backup and before first N9 span. One-way; no speculative undo. */
int nfp_xnu_coex(void);
/* Actual CID41 completion, then source-bounded SW_SYNC0 bit0/1 check.
 * Retains fresh driver/DMA resources on any failure; no sleep/teardown. */
int nfp_xnu_finish(void);
#endif
