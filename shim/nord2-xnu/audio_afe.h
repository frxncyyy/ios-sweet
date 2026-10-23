#ifndef NORD2_AUDIO_AFE_H
#define NORD2_AUDIO_AFE_H
#include <stdint.h>
#include "audio_id.h"

/* MT6893 continuous speaker AFE backend, derived from reviewed revision5.
 * Selected source Android04d00c2; register/order audit in SOURCE-AUDIT.md.
 * Cold SPM AUDIO domain is powered up; APLL2 is actually programmed to
 * 196.608MHz (PCW/postdivider/tuner), not assumed to inherit that rate.
 * DL1 uses one4KiB SRAM block, stereoS16; I2S3 uses32-bit serial slots as
 * required by selected Android BE fixup. SRAM must reuse AFE16KiB mapping.
 * naf_stream_open/close retain per-object plans; worker feeds SRAM while
 * open. audio_hw.c owns continuous refill, amplifiers, watchdogs and errors.
 * RMW preserves unrelated bits; polling is bounded. Uncertain MMIO failure
 * latches component offline, never panics iOS. Legacy naf_cycle is retained
 * only as a source-model compatibility entry; parent must use nau_hw_*.
 */

enum naf_kind { NAF_RMW = 1, NAF_W = 2, NAF_POLL = 3, NAF_DELAY = 4, NAF_FILL = 5, NAF_OBSERVE = 6 };

enum naf_phase {
	NAF_PH_POWER = 1, NAF_PH_AFECLK = 2, NAF_PH_AFE = 3, NAF_PH_I2S_CFG = 4,
	NAF_PH_DL1_CFG = 5, NAF_PH_APLL2 = 6, NAF_PH_I2S_ON = 7, NAF_PH_STREAM = 8,
	NAF_PH_OBSERVE = 9,
	NAF_PH_STOP = 10, NAF_PH_I2S_OFF = 11, NAF_PH_APLL2_OFF = 12, NAF_PH_AFE_OFF = 13,
	NAF_PH_POWER_OFF = 14,
};

struct naf_op {
	uint8_t kind, phase;
	uint16_t step_us;      /* POLL/OBSERVE interval */
	uint32_t pa, mask, value;
	uint32_t arg;          /* POLL/OBSERVE iterations, DELAY us, FILL bytes */
};

/* Physical addresses (live DT + selected sources). */
#define NAF_SPM_POWERON      0x10006000u
#define NAF_SPM_PWR_STATUS   0x1000616cu
#define NAF_SPM_PWR_STATUS2  0x10006170u
#define NAF_SPM_AUDIO_PWR    0x10006354u
#define NAF_IFR_CG1_SET      0x10001088u
#define NAF_IFR_CG1_CLR      0x1000108cu
#define NAF_IFR_CG1_STA      0x10001094u
#define NAF_IFR_CG3_SET      0x100010a4u
#define NAF_IFR_CG3_CLR      0x100010a8u
#define NAF_IFR_CG3_STA      0x100010acu
#define NAF_IFR_PROT2_SET    0x10001714u
#define NAF_IFR_PROT2_CLR    0x10001718u
#define NAF_IFR_PROT2_STA1   0x10001724u
#define NAF_TOP_UPDATE1      0x10000008u
#define NAF_TOP_CFG7         0x10000080u
#define NAF_TOP_CFG8         0x10000090u
#define NAF_TOP_CFG12        0x100000d0u
#define NAF_TOP_CFG13        0x100000e0u
#define NAF_TOP_SET(cfg)     ((cfg) + 4u)
#define NAF_TOP_CLR(cfg)     ((cfg) + 8u)
#define NAF_APM_AP_PLL_CON3  0x1000c00cu
#define NAF_APM_APLL2_TUNER  0x1000c044u
#define NAF_APM_APLL2_CON0   0x1000c32cu
#define NAF_APM_APLL2_CON1   0x1000c330u
#define NAF_APM_APLL2_CON2   0x1000c334u
#define NAF_APM_APLL2_CON4   0x1000c33cu
#define NAF_GPIO_MODE_32     0x10005340u
#define NAF_LT_PD            0x11f20060u
#define NAF_LT_PU            0x11f20080u
#define NAF_AFE              0x11210000u
#define NAF_AFE_TOP_CON0     (NAF_AFE + 0x000u)
#define NAF_AFE_DAC_CON0     (NAF_AFE + 0x010u)
#define NAF_AFE_I2S_CON3     (NAF_AFE + 0x040u)
#define NAF_AFE_CONN_24BIT   (NAF_AFE + 0x048u)
#define NAF_AFE_DL1_CON0     (NAF_AFE + 0x04cu)
#define NAF_AFE_DL1_BASE_MSB (NAF_AFE + 0x050u)
#define NAF_AFE_DL1_BASE     (NAF_AFE + 0x054u)
#define NAF_AFE_DL1_CUR      (NAF_AFE + 0x05cu)
#define NAF_AFE_DL1_END_MSB  (NAF_AFE + 0x060u)
#define NAF_AFE_DL1_END      (NAF_AFE + 0x064u)
#define NAF_AFE_DAC_MON      (NAF_AFE + 0x2ecu)
#define NAF_AFE_APLL2_TCFG   (NAF_AFE + 0x3f4u)
#define NAF_AFE_CONN28       (NAF_AFE + 0x4bcu)
#define NAF_AFE_CONN29       (NAF_AFE + 0x4c0u)
#define NAF_AFE_MEMIF_CON0   (NAF_AFE + 0x8ccu)
#define NAF_AFE_CONN_24BIT_1 (NAF_AFE + 0x994u)
#define NAF_AFE_HD_ENGEN     (NAF_AFE + 0xdd0u)
#define NAF_SRAM             0x11212000u
#define NAF_SRAM_BYTES       0x1000u   /* one DT block_size, prefer_mode 0 */

/* Source-derived constants. */
#define NAF_SPM_PROJECT_CODE 0xb16u      /* clk-mt6885-pg.c:263 */
#define NAF_APLL2_HZ         196608000u  /* mt6885-afe-clk.c:481 */
#define NAF_RATE_48K         10u         /* mt6885-afe-control.c:36,78-79 */

/* Measured inputs the gate requires; anything missing is a blocker. */
struct naf_amp_evidence {
	unsigned measured;   /* 1 only from a clean milestone-1b probe */
	unsigned supported;  /* exact REV in the tfa_init.c tables */
	unsigned pwdn, ampe, manstate;
};

enum {
	NAF_G_SNAPSHOT = 1u << 0,   /* milestone-1a snapshot incomplete/failed */
	NAF_G_SPM_CFG = 1u << 1,    /* POWERON_CONFIG_EN != selected keyed write */
	NAF_G_AUDIO_ON = 1u << 2,   /* AUDIO MTCMOS not in the exact cold state */
	NAF_G_ADSP_ON = 1u << 3,    /* ADSP domain on or not in selected cold state */
	NAF_G_PROTECT = 1u << 4,    /* AUDIO bus protection not set while cold */
	NAF_G_INFRA = 1u << 5,      /* ifrao_audio/audio26m already ungated */
	NAF_G_MUX = 1u << 6,        /* an audio mux ungated or not on 26 MHz */
	NAF_G_APLL2_ON = 1u << 7,   /* APLL2 enabled/powered or its tuner enabled */
	NAF_G_APLL2_RATE = 1u << 8, /* measured postdiv/PCW != 196.608 MHz source calc */
	NAF_G_APLL2_TUNER = 1u << 9,/* tuner != PCW + 1 (clk-pll.c:271-272) */
	NAF_G_PINS = 1u << 10,      /* GPIO32/33/35 not idleGPIOinputs or exactI2S3 */
	NAF_G_AMPS = 1u << 11,      /* no measured power-down for both amplifiers */
	NAF_G_AFE_SEEN = 1u << 12,  /* snapshot found the AFE readable (not cold) */
};

/* mtk_pll_calc_values() for the MT6893 PLL() parameters (fin 26 MHz,
 * fmin 1500 MHz, fmax 3800 MHz, pcwbits 32, integer bits 8). Returns the
 * PCW and writes the postdiv register code (log2 of the divider). */
uint32_t naf_pll_pcw(uint32_t freq, uint32_t *postdiv_code);
/* __mtk_pll_recalc_rate() inverse check, clk-pll.c:118-142. */
uint32_t naf_pll_rate(uint32_t pcw, uint32_t postdiv_code);

unsigned naf_gate(const struct nau_state *, const struct naf_amp_evidence amp[2]);

#define NAF_MAX_OPS 128
struct naf_plan {
	unsigned n_up, n_down;
	struct naf_op up[NAF_MAX_OPS];
	struct naf_op down[NAF_MAX_OPS];
};
/* Builds the bring-up and teardown programmes from a gate-passing snapshot.
 * 0 on success, -1 if the gate is not clean (plan left empty). */
int naf_build(const struct nau_state *, const struct naf_amp_evidence amp[2], struct naf_plan *);
/* 1 if pa may ever be written by this component. */
int naf_write_allowed(uint32_t pa);

struct naf_io {
	int (*read)(void *, uint32_t, uint32_t *);
	int (*write)(void *, uint32_t, uint32_t);
	int (*delay_us)(void *, unsigned);
};

enum { NAF_FAIL_NONE, NAF_FAIL_ARG, NAF_FAIL_GATE, NAF_FAIL_LIVE, NAF_FAIL_PLAN, NAF_FAIL_READ,
	NAF_FAIL_WRITE, NAF_FAIL_DELAY, NAF_FAIL_POLL, NAF_FAIL_OBSERVE, NAF_FAIL_VERIFY };

struct naf_state {
	unsigned entered, gate, live_gate, failure, phase, op, polls;
	unsigned up_done, down_done, observed, cur_changes, verify_mismatch;
	unsigned active, uncertain, writes, reads;
	uint32_t cur_first, cur_last, last_pa, last_value;
	int error;
};

/* Magic the caller must pass; no revision5 call site passes it. */
#define NAF_AUTHORIZE 0x4e414632u

/*
 * One silent-stream cycle: live re-read of every gate register (reads only),
 * gate, plan validation against the write allowlist, bring-up, bounded
 * DL1_CUR observation, source teardown, final read-back verify.
 *   0  streamed, observed, torn down and verified cold
 *  -1  refused before any write (arguments, authorization, gate, live drift, plan)
 *  -2  uncertain: a callback failed or a bring-up/teardown poll timed out; no
 *      cleanup is attempted, state->active/uncertain stay set (Linux return
 *      must be refused, no further call)
 *  -3  bring-up finished but DL1_CUR did not advance in range; teardown and
 *      verify still ran cleanly (functional failure, hardware back to cold)
 *  -4  teardown finished but read-back differs from the snapshot (uncertain)
 * A second call on the same state makes no callback.
 */
int naf_cycle(const struct naf_io *, void *, const struct nau_state *,
    const struct naf_amp_evidence amp[2], uint32_t authorize, struct naf_state *);
/* 1 if Linux return may proceed (pure, no callback). */
int naf_return_ok(const struct naf_state *);
int naf_stream_open(const struct naf_io *, void *, const struct nau_state *,
    const struct naf_amp_evidence[2], struct naf_plan *, struct naf_state *);
int naf_stream_close(const struct naf_io *, void *, const struct nau_state *,
    const struct naf_plan *, struct naf_state *);
#endif
