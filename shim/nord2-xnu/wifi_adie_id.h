#ifndef NORD2_ADIE_ID_H
#define NORD2_ADIE_ID_H
#include <stdint.h>
/* Fresh owned cold CONN; common table APPLIED and retained; source-correct
 *26/52MHz strap GPIO ownership; A-die reset deasserted; SPI aperture readable;
 *WFSYS/BGF CPUs held; all other clients inactive. Do not test SPI busy here:
 *the command itself legitimately asserts it. No EMI permission prerequisite. */
#define NAI_REQUIRED 0x7fu
struct nai_ops {
 int (*check)(void *,unsigned);
 /* exact RF SPI semaphore11. acquire0=owned,1=known notowned; other=uncertain.
  * Adapter bounds acquisition and must not clear someone else's ownership. */
 int (*acquire)(void *,unsigned);
 int (*release)(void *,unsigned); /*0 confirmed complete,else ownership unknown*/
 int (*read)(void *,uint32_t,uint32_t *);
 int (*write)(void *,uint32_t,uint32_t); /*failure can be partial*/
 int (*barrier)(void *);
 int (*delay_us)(void *,unsigned);
};
struct nai_result {
 uint32_t id,original_address;
 unsigned polls,phase;
 int error,restore_error,semaphore_owned,address_touched,command_touched;
 int command_complete,id_valid,known_chip,clean;
};
/* optin1 only. At most100 status reads before+after trigger,50us betweenbusy
 *reads. Addresses restricted to18004000/50/54/58. NO TOPSPI analog write,
 *CPU release, SYSRAM/EMI access, strap/reset changes, or calibration.
 *Snapshots command-address register; restores it ONLY when own command is
 *known complete (or triggerneverattempted). NEVER restore WD/trigger register.
 *Unknown commandcompletion: preserve semaphore/outerpower; caller must HALT.
 *Return0 knownID/clean;1 unknownID/clean;-1 error (consult clean).
 *clean refers transport/address+semaphore, not strap/common outertransaction.
 *Caller must prove strap/reset/shared ownership before invoking this engine. */
int nai_read_id(const struct nai_ops *,void *,unsigned,struct nai_result *);
#endif
