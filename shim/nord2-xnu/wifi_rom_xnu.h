#ifndef PRIVATE_ROM_XNU_H
#define PRIVATE_ROM_XNU_H
#include "wifi_rom_place.h"
#include "wifi_firmware.h"
struct nrb_native {
 unsigned attempted,enabled;struct nrc_claim claim;struct nrb_state state;
 unsigned char *output,*backup,*scratch;
 struct nwf_blob blobs[NWF_FILE_COUNT];uintptr_t io;
 int (*fresh_owner)(void *,enum nrb_owner_stage);void *owner_cookie;
 int supply_error;
};
/* Run in sleepable context AFTER UFSready BEFORE newWFSYScriticalownership.
 * Newexactoptin nord2wifirom1, currentchosenclaim24LEsource3, pinnedhighRAM;
 * realvendorreader verifiesall4 files; keepsauthenticatedsource+backups.
 * No native mapping/SMC/DRAM access is called by prepare. */
int nrb_xnu_prepare(struct nrb_native *);
/* Bind real fresh parent-owned WFSYS context, never old asleepnwsa object.
 * Coldbeforepower predicate for SET; poweredCPUheld predicate for placement.
 * Missing hook refuses; hook cannot grant MPUpermission (realSMCinternal). */
int nrb_xnu_policy(struct nrb_native *,int (*)(void *,enum nrb_owner_stage),void *);
int nrb_xnu_place(struct nrb_native *);
#endif
