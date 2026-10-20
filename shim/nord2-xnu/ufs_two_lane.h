#ifndef NORD2_UFS_TWO_LANE_DIAGNOSTIC_H
#define NORD2_UFS_TWO_LANE_DIAGNOSTIC_H
#include <stdint.h>
#include <stddef.h>
/* Opt-in pre-root diagnostic: linked through the Nord2 UFS adapter. No reset,
 * PHY/clock/AHIT writes, peer commands, allocation or root publication. */
#define N2L_READ_COMPLETED_ERROR 1
#define N2L_READ_UNCERTAIN 2
struct n2l_ops {
 uint32_t (*read32)(void *,uint32_t);
 void (*write32)(void *,uint32_t,uint32_t);
 uint64_t (*ticks)(void *);
 void (*delay_us)(void *,unsigned);
 void (*barrier)(void *); /* DSB SY, including compiler memory barrier */
 /* Must recheck actual exclusive mutex/controller ownership, pre-root state,
  * fixed LUN2/descriptor ownership, no writable publication or concurrent I/O.
  * Called before each UIC transaction and each validation read. */
 int (*owned_preroot)(void *);
 /* Read ONLY fixed LUN2 LBA1,count5 (primary GPT header+array) through owned
  * descriptor/bounce and full DMA maintenance; SHA256 all20480bytes. Must not
  * rebind root, write media, publish, clear unknown IRQs, or hide timeouts.
  * Return0 success;1 ONLY a known-completed protocol/read error; any other
  * nonzero means ambiguous/timeout and forbids rollback or handoff. */
 int (*read_digest)(void *,uint64_t,uint32_t,unsigned char [32]);
 const unsigned char *expected_gpt_digest; /* pinned SHA256 of all20480bytes */
 uint64_t frequency;
};
struct n2l_result {
 uint32_t phase,commands,sets,reads,last_attr,last_result,last_is,last_hcs;
 uint32_t snapshot[18],limits[6],snapshot_mask,limits_mask;
 int error,uncertain,rollback_attempted,original_verified,root_allowed,handoff_allowed;
};
/* optin must equal1. Return0 ONLY after x2 negotiated/read verified AND original
 * x1 negotiated/all attributes/read verified. Every failure blocks root writes
 * and publication; uncertain state also blocks Linux handoff. Caller MUST obey
 * both flags. No software deadline can bound a hung MMIO access/callback. */
int n2l_diagnostic(const struct n2l_ops *,void *,unsigned,struct n2l_result *);
#endif
