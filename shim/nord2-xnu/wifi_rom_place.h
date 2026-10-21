#ifndef PRIVATE_ROM_PLACE_H
#define PRIVATE_ROM_PLACE_H
#include <stddef.h>
#include <stdint.h>
#include "wifi_rom_contract.h"
#include "wifi_rom_plan.h"
#define NRB_BACKUP_BYTES 346720u
#define NRB_CHUNK 4096u
enum nrb_owner_stage {NRB_COLD_BEFORE_POWER=1,NRB_POWERED_CPU_HELD=2};
struct nrb_ops {
 /* Fresh real radio ownership/reset/readability proof, NOT MPU permission.
  * Cold stage before policy; poweredstage localID20010000/CPUreset0,
  * AP+WFwake/PLLowned and exclusive CONNDRAMclaim consumer. */
 int (*owner)(void *,enum nrb_owner_stage);
 int (*smc)(void *,const uint64_t [8],uint64_t [4]);
 int (*map)(void *,uint64_t,uint64_t);
 int (*read)(void *,uint64_t,void *,uint32_t);
 int (*write)(void *,uint64_t,const void *,uint32_t);
 int (*barrier)(void *);
};
struct nrb_state {
 unsigned prepared,policy_attempted,issued,accepted,raw_valid,mapped;
 unsigned phase,halt,backup_complete,write_attempted,placed,reads,writes,barriers;
 int error;uint64_t raw[2][4];
 struct nrc_claim claim;struct nwf_rom_plan plan;
 const unsigned char *mcu,*wifi;size_t mcu_bytes,wifi_bytes;
 unsigned char *backup,*scratch;
};
/* Decode ONLY currentboot loader ADT little-endian24B record. No historical
 * fallback/claim invented by this code. valid must1, source3(IOS_FIXED)only, exactrange. */
int nrb_claim_decode(const void *,size_t,struct nrc_claim *);
/* Pure setup/rehash. Buffer/source/state storage must be disjoint and sources
 * immutable for entire retained lifetime. No callback/mapping/SET here. */
int nrb_prepare(struct nrb_state *,unsigned,const struct nrc_claim *,
 const unsigned char *,size_t,const unsigned char *,size_t,
 unsigned char *,size_t,unsigned char *,size_t);
/* One-way SET before fresh WFSYS power. No retry evenafter firstfailure.
 * Raw0only accepted; stopbeforegroup0onnonzero/transporterror. SETcouldhang:
 * boundedcallcount is NOT a softwareinterruptible executiondeadline. */
int nrb_policy(const struct nrb_ops *,void *,struct nrb_state *);
/* Only after parent supplies fresh poweredCPUheld owner; this is NOT old
 * asleepnwsa_start. Snapshotsall4 first; bounded4096copy/readback/barriers.
 * Successful bytes persist. ANY error keeps policy/backup/powerowned/halt;
 * no implicit restore/unmap/free/CPUrelease. Caller handles manualreset. */
int nrb_place(const struct nrb_ops *,void *,struct nrb_state *);
#endif
