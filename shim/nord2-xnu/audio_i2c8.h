#ifndef NORD2_AUDIO_I2C8_H
#define NORD2_AUDIO_I2C8_H
#include <stdint.h>

/* MT6893 I2C8 multi-channel I2Cv2 PIO transport. Only amp slaves34/37.
 * Read16 uses registerpointer + WRRD(two BE bytes); write16 uses exactly
 * three bytes (register, MSB, LSB), no controller reset/bitbang/APDMA writes.
 * Board-profile RMW requires an immediately preceding same-register read
 * and a source-backed masked change from the SHA-validated runtime plan.
 * Silicon errata writes and volatile key latches use exact selected-source
 * values. No MTP programming commands or rail slaves51/56 are admitted.
 * AP configuration is saved/restored on clean completion/cleanNACK; failure
 * after mutation is sticky uncertain, with no reset/retry/next-slave access.
 * Shared clock enabling, amp reset and the AP lease belong to audio_hw.c.
 * Every transfer rechecks clocks, pinmux, CAM/CCU/shadow/AP idle, DMA/FIFO,
 * and V2 I3C_EN at0xec. Source references: SOURCE-AUDIT.md.
 */
struct na8_io {
	int (*read)(void *, uint32_t, unsigned, uint32_t *);   /* width 1/2/4 */
	int (*write)(void *, uint32_t, unsigned, uint32_t);
	int (*delay_us)(void *, unsigned);
};

/* failure: 1 mux, 2 PLL, 3 timing, 4 pinmux, 5 controller gated, 6 AP START,
 * 7 DMA, 8 inherited IRQ, 9 FIFO, 10 read callback, 11 write callback,
 * 12 delay, 13 timeout, 14 completion error, 15 byte bounds,
 * 16 CAM power (CCU may run), 17 CCU channel busy, 18 shadow START,
 * 19 argument not allowed, 20 existing GPIO input-enable/pads not high,
 * 21 AP-channel I3C mode enabled. */
struct na8_state {
	unsigned entered, inflight, offline, mutated, uncertain, polls, calls;
 unsigned poll_limit; /* 0=2000, streaming monitor uses200 (2ms); maximum2000 */
	unsigned nack, nacks, i3c, restoring;
 unsigned prior_amp_valid, prior_amp_slave, prior_amp_reg;
 uint16_t prior_amp_value;
	int error;
	unsigned stage, failure, last_width, last_write, last_valid;
	uint32_t last_addr, last_value, mux, parent_hz, pinmux, gate, pwr, pwr2;
	uint32_t ifr1, ifr2, dma_en, sh_start, ccu_start, ccu_ctrl, prior_irq, prior_ack;
	uint32_t completion_irq;
	unsigned dma_checked;
};

/* One read of a 16-bit big-endian TFA register (tfa98xx.c:4705-4709).
 *  0 success (value valid)
 *  1 clean NACK: transaction completed, status/FIFO cleared, config restored,
 *    no data byte was read, nothing is uncertain, Linux return may proceed
 * -1 refused before any write (offline, hardware untouched)
 * -2 uncertain after a write (offline, Linux return must be refused) */
int na8_read16(const struct na8_io *, void *, struct na8_state *,
    unsigned slave, unsigned reg, uint16_t *value);

/* 1 if Linux return may proceed: nothing in flight and no write was
 * followed by a non-clean outcome. Pure: performs no callback. A clean NACK
 * does not refuse return. */
int na8_return_ok(const struct na8_state *);

enum na8_class {
	NA8_UNPROBED = 0,
	NA8_FAILED = 1,        /* transport refused/uncertain; sequence stopped */
	NA8_UNKNOWN = 2,       /* REV low byte not handled by tfa98xx.c probe */
	NA8_OTHER_FAMILY = 3,  /* handled by the driver but not a 9873/9874 */
	NA8_UNLISTED_REV = 4,  /* 0x73/0x74 family, revision not in tfa_init.c */
	NA8_SUPPORTED = 5,     /* exact revision with a source init table */
	NA8_NACK = 6,          /* clean address/data NACK; no REV value read */
	NA8_PARTIAL = 7,       /* revision5: supported REV read, then a clean NACK on
	                        * 0x00/0x14; nothing further on that amp */
};

struct na8_amp {
	uint8_t addr;
	unsigned cls, reads;
	uint16_t rev, sys0, status14;
	unsigned pwdn, ampe, manstate;
	int rc;
};

/* REV (0x03) first. Only an exact supported revision may be followed by
 * 0x00 and 0x14 on that slave. A clean NACK (on REV, or revision5: on 0x00/
 * 0x14 after a supported REV -> NA8_PARTIAL) stops only that amplifier; no
 * register pointer is invented or retried; the other amplifier is still
 * probed because the controller is verified idle and restored again. Any
 * uncertain/refused transport outcome stops the whole sequence; the second
 * amplifier is probed only after clean completions. */
int na8_probe(const struct na8_io *, void *, struct na8_state *, struct na8_amp amp[2]);
unsigned na8_classify(uint16_t rev);
int na8_write16(const struct na8_io *, void *, struct na8_state *,
    unsigned slave, unsigned reg, uint16_t value);
#endif
