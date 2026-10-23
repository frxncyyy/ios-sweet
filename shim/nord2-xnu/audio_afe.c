#if defined(KERNEL)
#include <pexpert/arm64/board_config.h>
#endif
#if defined(NORD2)
/* Nord2 milestone 2 AFE silent-stream programme (revision5). GPL-2.0 source
 * contract and line references in audio_afe.h and AUDIO-RESUME-REPORT.md. */
#include "audio_afe.h"

/* ---- PLL arithmetic: clk-pll.c:118-142, 289-325; clk-mt6885.c:2407-2442 ---- */
#define NAF_FIN 26000000u
#define NAF_FMIN 1500000000ull
#define NAF_FMAX 3800000000ull
#define NAF_PCWBITS 32u
#define NAF_IBITS 8u

uint32_t
naf_pll_pcw(uint32_t freq, uint32_t *postdiv_code)
{
	uint64_t f = freq;
	unsigned val;
	if (f > NAF_FMAX) { f = NAF_FMAX; }
	/* Exactly the source loop, including its val==5 fall-through quirk. */
	uint32_t postdiv = 1;
	for (val = 0; val < 5; val++) {
		postdiv = 1u << val;
		if (f * postdiv >= NAF_FMIN) { break; }
	}
	uint64_t pcw = (f << val) << (NAF_PCWBITS - NAF_IBITS);
	pcw /= NAF_FIN;
	if (postdiv_code) {
		unsigned code = 0;
		while ((1u << code) < postdiv) { code++; } /* ffs(postdiv) - 1 */
		*postdiv_code = code;
	}
	return (uint32_t)pcw;
}

uint32_t
naf_pll_rate(uint32_t pcw, uint32_t postdiv_code)
{
	unsigned fbits = NAF_PCWBITS - NAF_IBITS;
	uint64_t vco = (uint64_t)NAF_FIN * pcw;
	unsigned c = (vco & ((1ull << fbits) - 1)) ? 1 : 0;
	vco >>= fbits;
	vco += c;
	uint64_t postdiv = 1ull << (postdiv_code & 7);
	return (uint32_t)((vco + postdiv - 1) / postdiv);
}

/* ---- gate ---- */
/* Pins 32, 33, 35 (I2S3 BCK/LRCK/DO; GPIO34 = I2S0_DI is not touched). */
#define NAF_PIN_MODE_MASK 0x0000f0ffu
#define NAF_PIN_MODE_I2S3 0x00001011u
#define NAF_PIN_DIR_MASK ((1u << 0) | (1u << 1) | (1u << 3))
#define NAF_PIN_LT_MASK ((1u << 3) | (1u << 5) | (1u << 4)) /* pinctrl-mt6885.c:299-302,395-398 */

static int
naf_amp_ok(const struct naf_amp_evidence *a)
{
	return a->measured && a->supported && a->pwdn == 1 && a->ampe == 0 && a->manstate == 0;
}

static unsigned
naf_gate_words(const uint32_t *s, const struct naf_amp_evidence amp[2])
{
	unsigned g = 0;
	/* The PG path writes the entire keyed register. Refuse if that would
	 * replace a distinct inherited high half. */
	if (s[NAU_S_SPM_POWERON] != ((NAF_SPM_PROJECT_CODE << 16) | 1u)) {
		/* Source keyed write is performed for each power transition. */
	}
	/* Cold MTCMOS: status bit21 clear; ON/ON_2ND/RST_B clear; ISO, CLK_DIS,
	 * SRAM_PDN and its ack set (the power-down end state, pg.c:3282-3311). */
	if (((s[NAU_S_PWR_STATUS] | s[NAU_S_PWR_STATUS_2ND]) & (1u << 21)) ||
	    (s[NAU_S_AUDIO_PWR_CON] & 0x111fu) != 0x1112u) {
		g |= NAF_G_AUDIO_ON;
	}
	/* pg.c:3370-3416 finishes ADSP off with CKISO/ISO/CLK_DIS set,
	 * ISOINT_B/SLEEP_B/ACK/ON/ON_2ND/RST_B clear. Status alone can still
	 * be zero while a power transition is in progress. */
	if (((s[NAU_S_PWR_STATUS] | s[NAU_S_PWR_STATUS_2ND]) & (1u << 22)) ||
	    (s[NAU_S_ADSP_PWR_CON] & 0x227fu) != 0x0032u) {
		g |= NAF_G_ADSP_ON;
	}
	if (!(s[NAU_S_PROT2_STA1] & (1u << 4))) { g |= NAF_G_PROTECT; }
	if (!(s[NAU_S_IFR1_CG] & (1u << 25)) || !(s[NAU_S_IFR3_CG] & (1u << 4))) { g |= NAF_G_INFRA; }
	/* audio_sel pdn31 sel[25:24]; aud_intbus pdn7 [1:0]; engen2 pdn7 [1:0];
	 * aud_2 pdn7 [0]: all gated and on parent 0 (26 MHz). */
	if ((s[NAU_S_CLK_CFG_7] & ((1u << 31) | (3u << 24))) != (1u << 31) ||
	    (s[NAU_S_CLK_CFG_8] & ((1u << 7) | 3u)) != (1u << 7) ||
	    (s[NAU_S_CLK_CFG_12] & ((1u << 7) | 3u)) != (1u << 7) ||
	    (s[NAU_S_CLK_CFG_13] & ((1u << 7) | 1u)) != (1u << 7)) {
		g |= NAF_G_MUX;
	}
	/* APLL2 unprepared end state: CON0 en clear, CON4 ISO set/PWR_ON clear,
	 * tuner enable (AP_PLL_CON3 bit5) clear (clk-pll.c:524-551). */
	if ((s[NAU_S_APLL2_CON0] & 1u) || (s[NAU_S_APLL2_CON4] & 3u) != 2u ||
	    (s[NAU_S_AP_PLL_CON3] & (1u << 5))) {
		g |= NAF_G_APLL2_ON;
	}
	uint32_t code = 0, pcw = naf_pll_pcw(NAF_APLL2_HZ, &code);
	if (((s[NAU_S_APLL2_CON1] >> 24) & 7u) != code || s[NAU_S_APLL2_CON2] != pcw) {
		/* Program cold APLL2 rather than requiring bootloader rate. */
	}
	if (s[NAU_S_APLL2_TUNER] != s[NAU_S_APLL2_CON2] + 1u) { /* Reprogram tuner along with PCW. */ }
	/* Cold AUDIO/ADSP and gated clocks were checked above. GPIO mode0
	 * inputs are idle; function1 is exactly our quiescent I2S3 peripheral
	 * (pinctrl-mtk-mt6885.h377-418). GPIO DIR is irrelevant in function1.
	 * Keep rejecting GPIO outputs and every alternate peripheral function;
	 * this does not authorize takeover of I2S1/I2S5/PCM/I2S0. */
	static const unsigned pins[3]={0,1,3};
	for (unsigned i=0;i<3;i++) {
		unsigned mode=(s[NAU_S_MODE_32]>>(4*pins[i]))&15u;
		unsigned output=(s[NAU_S_DIR_32]>>pins[i])&1u;
		if ((mode==0 && output) || (mode!=0 && mode!=1)) g |= NAF_G_PINS;
	}
	if (!amp || !naf_amp_ok(&amp[0]) || !naf_amp_ok(&amp[1])) { g |= NAF_G_AMPS; }
	return g;
}

unsigned
naf_gate(const struct nau_state *st, const struct naf_amp_evidence amp[2])
{
	if (!st || st->failed || st->snap_read != NAU_SNAP_COUNT) {
		return NAF_G_SNAPSHOT | ((!amp || !naf_amp_ok(&amp[0]) || !naf_amp_ok(&amp[1])) ? NAF_G_AMPS : 0);
	}
	unsigned g = naf_gate_words(st->snap, amp);
	if (st->afe_read) { g |= NAF_G_AFE_SEEN; }
	return g;
}

/* Gate/verify registers and the fields this component may change. */
static const struct {
	uint8_t idx;
	uint32_t mask;
} naf_watch[] = {
	{NAU_S_SPM_POWERON, 1u},
	{NAU_S_PWR_STATUS, (1u << 21) | (1u << 22)},
	{NAU_S_PWR_STATUS_2ND, (1u << 21) | (1u << 22)},
	{NAU_S_ADSP_PWR_CON, 0x227fu},
	{NAU_S_AUDIO_PWR_CON, 0x111fu},
	{NAU_S_IFR1_CG, 1u << 25},
	{NAU_S_IFR3_CG, 1u << 4},
	{NAU_S_PROT2_STA1, 1u << 4},
	{NAU_S_CLK_CFG_7, (1u << 31) | (3u << 24)},
	{NAU_S_CLK_CFG_8, (1u << 7) | 3u},
	{NAU_S_CLK_CFG_12, (1u << 7) | 3u},
	{NAU_S_CLK_CFG_13, (1u << 7) | 1u},
	{NAU_S_AP_PLL_CON3, 1u << 5},
	{NAU_S_APLL2_TUNER, 0xffffffffu},
	{NAU_S_APLL2_CON0, 1u},
	{NAU_S_APLL2_CON1, 7u << 24},
	{NAU_S_APLL2_CON2, 0xffffffffu},
	{NAU_S_APLL2_CON4, 3u},
	{NAU_S_MODE_32, NAF_PIN_MODE_MASK},
	{NAU_S_DIR_32, NAF_PIN_DIR_MASK},
	{NAU_S_LT_PD32, NAF_PIN_LT_MASK},
	{NAU_S_LT_PU32, NAF_PIN_LT_MASK},
};
#define NAF_WATCH (sizeof(naf_watch) / sizeof(naf_watch[0]))

/* ---- allowlists ---- */
int
naf_write_allowed(uint32_t pa)
{
	switch (pa) {
	case NAF_SPM_POWERON: case NAF_SPM_AUDIO_PWR:
	case NAF_IFR_CG1_SET: case NAF_IFR_CG1_CLR: case NAF_IFR_CG3_SET: case NAF_IFR_CG3_CLR:
	case NAF_IFR_PROT2_SET: case NAF_IFR_PROT2_CLR:
	case NAF_TOP_UPDATE1:
	case NAF_TOP_SET(NAF_TOP_CFG7): case NAF_TOP_CLR(NAF_TOP_CFG7):
	case NAF_TOP_SET(NAF_TOP_CFG8): case NAF_TOP_CLR(NAF_TOP_CFG8):
	case NAF_TOP_SET(NAF_TOP_CFG12): case NAF_TOP_CLR(NAF_TOP_CFG12):
	case NAF_TOP_SET(NAF_TOP_CFG13): case NAF_TOP_CLR(NAF_TOP_CFG13):
	case NAF_APM_AP_PLL_CON3: case NAF_APM_APLL2_CON0: case NAF_APM_APLL2_CON4:
	case NAF_APM_APLL2_CON1: case NAF_APM_APLL2_CON2: case NAF_APM_APLL2_TUNER:
	case NAF_GPIO_MODE_32: case NAF_LT_PD: case NAF_LT_PU:
	case NAF_AFE_TOP_CON0: case NAF_AFE_DAC_CON0: case NAF_AFE_I2S_CON3:
	case NAF_AFE_CONN_24BIT: case NAF_AFE_DL1_CON0: case NAF_AFE_DL1_BASE_MSB:
	case NAF_AFE_DL1_BASE: case NAF_AFE_DL1_END_MSB: case NAF_AFE_DL1_END:
	case NAF_AFE_APLL2_TCFG: case NAF_AFE_CONN28: case NAF_AFE_CONN29:
	case NAF_AFE_MEMIF_CON0: case NAF_AFE_CONN_24BIT_1: case NAF_AFE_HD_ENGEN:
		return 1;
	default:
		return pa >= NAF_SRAM && pa < NAF_SRAM + NAF_SRAM_BYTES && !(pa & 3u);
	}
}

static int naf_write_only(uint32_t pa);

static int
naf_read_allowed(uint32_t pa)
{
	if (pa == NAF_SPM_PWR_STATUS || pa == NAF_SPM_PWR_STATUS2 || pa == NAF_IFR_PROT2_STA1 ||
	    pa == NAF_AFE_DAC_MON || pa == NAF_AFE_DL1_CUR) {
		return 1;
	}
	for (unsigned i = 0; i < NAF_WATCH; i++) {
		if (nau_snapshot_pa[naf_watch[i].idx] == pa) { return 1; }
	}
	/* RMW targets are read before they are written; aliases are never read. */
	return naf_write_allowed(pa) && !naf_write_only(pa) &&
	    (pa < NAF_SRAM || pa >= NAF_SRAM + NAF_SRAM_BYTES);
}

/* ---- plan ---- */
struct naf_builder {
	struct naf_op *list;
	unsigned n, overflow;
};

static void
naf_emit(struct naf_builder *b, unsigned kind, unsigned phase, uint32_t pa, uint32_t mask,
    uint32_t value, uint32_t arg, unsigned step)
{
	if (b->n >= NAF_MAX_OPS) { b->overflow = 1; return; }
	struct naf_op *o = &b->list[b->n++];
	o->kind = (uint8_t)kind;
	o->phase = (uint8_t)phase;
	o->step_us = (uint16_t)step;
	o->pa = pa;
	o->mask = mask;
	o->value = value;
	o->arg = arg;
}
#define RMW(ph, pa, m, v) naf_emit(b, NAF_RMW, (ph), (pa), (m), (v), 0, 0)
#define WR(ph, pa, v) naf_emit(b, NAF_W, (ph), (pa), 0xffffffffu, (v), 0, 0)
#define POLL(ph, pa, m, v, n, us) naf_emit(b, NAF_POLL, (ph), (pa), (m), (v), (n), (us))
#define DLY(ph, us) naf_emit(b, NAF_DELAY, (ph), 0, 0, 0, (us), 0)

static void
naf_build_up(struct naf_builder *b)
{
	const uint32_t poweron = (NAF_SPM_PROJECT_CODE << 16) | 1u;
	/* 1. PG_AUDIO parents (pg.c:5001-5003), then MTCMOS on (pg.c:3314-3355).
	 * Mux enable writes the CLR alias (clk-mux.c:78-98); infra CG enable
	 * writes the CLR alias (clk-mt6885.c:2115-2131). */
	WR(NAF_PH_POWER, NAF_TOP_CLR(NAF_TOP_CFG8), 1u << 7);   /* aud_intbus_sel */
	WR(NAF_PH_POWER, NAF_IFR_CG3_CLR, 1u << 4);             /* ifrao_audio26m */
	WR(NAF_PH_POWER, NAF_IFR_CG1_CLR, 1u << 25);            /* ifrao_audio */
	WR(NAF_PH_POWER, NAF_SPM_POWERON, poweron);
	RMW(NAF_PH_POWER, NAF_SPM_AUDIO_PWR, 1u << 2, 1u << 2); /* PWR_ON */
	RMW(NAF_PH_POWER, NAF_SPM_AUDIO_PWR, 1u << 3, 1u << 3); /* PWR_ON_2ND */
	POLL(NAF_PH_POWER, NAF_SPM_PWR_STATUS, 1u << 21, 1u << 21, 1000, 10);
	POLL(NAF_PH_POWER, NAF_SPM_PWR_STATUS2, 1u << 21, 1u << 21, 1000, 10);
	RMW(NAF_PH_POWER, NAF_SPM_AUDIO_PWR, 1u << 4, 0);        /* CLK_DIS */
	RMW(NAF_PH_POWER, NAF_SPM_AUDIO_PWR, 1u << 1, 0);        /* ISO */
	RMW(NAF_PH_POWER, NAF_SPM_AUDIO_PWR, 1u << 0, 1u << 0);  /* RST_B */
	RMW(NAF_PH_POWER, NAF_SPM_AUDIO_PWR, 1u << 8, 0);        /* SRAM_PDN */
	POLL(NAF_PH_POWER, NAF_SPM_AUDIO_PWR, 1u << 12, 0, 1000, 10);
	WR(NAF_PH_POWER, NAF_IFR_PROT2_CLR, 1u << 4);
	/* Source ignores this ack (pg.c:3351-3353); waiting is strictly safer. */
	POLL(NAF_PH_POWER, NAF_IFR_PROT2_STA1, 1u << 4, 0, 1000, 10);

	/* 2. mt6885_afe_enable_clock (afe-clk.c:212-279): audio_sel on (parent
	 * already 26 MHz per gate), aud_afe gate (audsys no_setclr, bit2). */
	WR(NAF_PH_AFECLK, NAF_TOP_CLR(NAF_TOP_CFG7), 1u << 31);
	RMW(NAF_PH_AFECLK, NAF_AFE_TOP_CON0, 1u << 2, 0);
	/* 3. runtime resume (pcm.c:3466-3474): CPU_HD_ALIGN 0, 24-bit ports, AFE_ON. */
	RMW(NAF_PH_AFE, NAF_AFE_MEMIF_CON0, 1u << 1, 0);
	WR(NAF_PH_AFE, NAF_AFE_CONN_24BIT, 0xffffffffu);
	WR(NAF_PH_AFE, NAF_AFE_CONN_24BIT_1, 0xffffffffu);
	RMW(NAF_PH_AFE, NAF_AFE_DAC_CON0, 1u << 0, 1u << 0);

	/* 4. I2S3 BE hw_params: 48 kHz, I2S format, 16-bit (dai-i2s.c:1803-1808). */
	RMW(NAF_PH_I2S_CFG, NAF_AFE_I2S_CON3, 0xffffeffeu,
	    (NAF_RATE_48K << 8) | (1u << 3) | (1u << 1));

	/* 5. DL1 FE: disabled check (fe-dai.c:789-795), addr (:802-823), stereo,
	 * 48 kHz, S16 (hd 0), pbuf/minlen 32 B (pcm.c:300-308), SRAM normal mode
	 * (pcm.c:3500-3508, 3591-3595), then zero the ring. */
	POLL(NAF_PH_DL1_CFG, NAF_AFE_DAC_CON0, 1u << 4, 0, 1, 0);
	WR(NAF_PH_DL1_CFG, NAF_AFE_DL1_BASE, NAF_SRAM);
	WR(NAF_PH_DL1_CFG, NAF_AFE_DL1_END, NAF_SRAM + NAF_SRAM_BYTES - 1u);
	WR(NAF_PH_DL1_CFG, NAF_AFE_DL1_BASE_MSB, 0);
	WR(NAF_PH_DL1_CFG, NAF_AFE_DL1_END_MSB, 0);
	RMW(NAF_PH_DL1_CFG, NAF_AFE_DL1_CON0, 1u << 8, 0);
	RMW(NAF_PH_DL1_CFG, NAF_AFE_DL1_CON0, 0xfu << 24, NAF_RATE_48K << 24);
	RMW(NAF_PH_DL1_CFG, NAF_AFE_DL1_CON0, 3u, 0);
	RMW(NAF_PH_DL1_CFG, NAF_AFE_DL1_CON0, 3u << 12, 0);
	RMW(NAF_PH_DL1_CFG, NAF_AFE_DL1_CON0, 0xfu << 20, 0);
	RMW(NAF_PH_DL1_CFG, NAF_AFE_DL1_CON0, 1u << 5, 1u << 5);
	RMW(NAF_PH_DL1_CFG, NAF_AFE_MEMIF_CON0, 1u << 2, 0);
	naf_emit(b, NAF_FILL, NAF_PH_DL1_CFG, NAF_SRAM, 0, 0, NAF_SRAM_BYTES, 0);

	/* 6. APLL2 supply = mt6885_apll2_enable (afe-clk.c:421-451):
	 * aud_2_sel enable, PLL prepare (clk-pll.c:426-459: CON4 PWR_ON, 1 us,
	 * ISO off, 1 us, CON0 en + EN_BIT_CTRL bit0, tuner en AP_PLL_CON3 bit5,
	 * 20 us; no RST_BAR flag), reparent to apll2_ck (clk-mux.c:178-216),
	 * aud_engen2 on apll2_d4, aud_24m/aud_apll2_tuner gates, AFE tuner cfg,
	 * 24M engine. No PCW/postdiv/tuner-value write: gate checked them. */
	/* clk-pll.c:245-279 set_rate_regs, cold ownership required by gate.
	 * APLL2 pcw_chg_addr defaults to base+CON1 (line613), bit31.
	 * Tuner is disabled during all three writes and enabled at prepare. */
	uint32_t pd = 0, pcw = naf_pll_pcw(NAF_APLL2_HZ, &pd);
	RMW(NAF_PH_APLL2, NAF_APM_AP_PLL_CON3, 1u << 5, 0);
	RMW(NAF_PH_APLL2, NAF_APM_APLL2_CON1, 7u << 24, pd << 24);
	WR(NAF_PH_APLL2, NAF_APM_APLL2_CON2, pcw);
	RMW(NAF_PH_APLL2, NAF_APM_APLL2_CON1, 1u << 31, 1u << 31);
	WR(NAF_PH_APLL2, NAF_APM_APLL2_TUNER, pcw + 1u);
	DLY(NAF_PH_APLL2, 20);
	POLL(NAF_PH_APLL2, NAF_APM_APLL2_CON1, 7u << 24, pd << 24, 1, 0);
	POLL(NAF_PH_APLL2, NAF_APM_APLL2_CON2, 0xffffffffu, pcw, 1, 0);
	POLL(NAF_PH_APLL2, NAF_APM_APLL2_TUNER, 0xffffffffu, pcw + 1u, 1, 0);
	WR(NAF_PH_APLL2, NAF_TOP_CLR(NAF_TOP_CFG13), 1u << 7);
	RMW(NAF_PH_APLL2, NAF_APM_APLL2_CON4, 1u << 0, 1u << 0);
	DLY(NAF_PH_APLL2, 1);
	RMW(NAF_PH_APLL2, NAF_APM_APLL2_CON4, 1u << 1, 0);
	DLY(NAF_PH_APLL2, 1);
	RMW(NAF_PH_APLL2, NAF_APM_APLL2_CON0, 1u << 0, 1u << 0);
	RMW(NAF_PH_APLL2, NAF_APM_AP_PLL_CON3, 1u << 5, 1u << 5);
	DLY(NAF_PH_APLL2, 20);
	WR(NAF_PH_APLL2, NAF_TOP_CLR(NAF_TOP_CFG13), 1u << 0);
	WR(NAF_PH_APLL2, NAF_TOP_SET(NAF_TOP_CFG13), 1u << 0); /* index 1 apll2_ck */
	WR(NAF_PH_APLL2, NAF_TOP_UPDATE1, 1u << 21);           /* TOP_MUX_AUD_2_SHIFT */
	WR(NAF_PH_APLL2, NAF_TOP_CLR(NAF_TOP_CFG12), 1u << 7);
	WR(NAF_PH_APLL2, NAF_TOP_CLR(NAF_TOP_CFG12), 3u);
	WR(NAF_PH_APLL2, NAF_TOP_SET(NAF_TOP_CFG12), 2u);      /* index 2 apll2_d4 */
	WR(NAF_PH_APLL2, NAF_TOP_UPDATE1, 1u << 17);           /* TOP_MUX_AUD_ENGEN2_SHIFT */
	RMW(NAF_PH_APLL2, NAF_AFE_TOP_CON0, 1u << 9, 0);        /* aud_24m */
	RMW(NAF_PH_APLL2, NAF_AFE_TOP_CON0, 1u << 18, 0);       /* aud_apll2_tuner */
	RMW(NAF_PH_APLL2, NAF_AFE_APLL2_TCFG, 0x0000fff7u, 0x00000374u);
	RMW(NAF_PH_APLL2, NAF_AFE_APLL2_TCFG, 1u, 1u);
	RMW(NAF_PH_APLL2, NAF_AFE_HD_ENGEN, 1u << 1, 1u << 1);  /* AFE_24M_ON */

	/* 7. I2S3_HD_EN (Oplus connects it: dai-i2s.c:1036-1056, 908-911), pins
	 * to function 1 with bias disabled (DT:1737-1757; SMT group skipped),
	 * then the I2S3_EN supply bit (dai-i2s.c:871-874, PRE_PMU gpio first). */
	RMW(NAF_PH_I2S_ON, NAF_AFE_I2S_CON3, 1u << 12, 1u << 12);
	RMW(NAF_PH_I2S_ON, NAF_GPIO_MODE_32, NAF_PIN_MODE_MASK, NAF_PIN_MODE_I2S3);
	RMW(NAF_PH_I2S_ON, NAF_LT_PU, NAF_PIN_LT_MASK, 0);
	RMW(NAF_PH_I2S_ON, NAF_LT_PD, NAF_PIN_LT_MASK, 0);
	RMW(NAF_PH_I2S_ON, NAF_AFE_I2S_CON3, 1u << 0, 1u << 0);

	/* 8. DL1_CH1/2 -> I2S3_CH1/2 (I_DL1_CH1 5, I_DL1_CH2 6), DL1 on. */
	RMW(NAF_PH_STREAM, NAF_AFE_CONN28, 1u << 5, 1u << 5);
	RMW(NAF_PH_STREAM, NAF_AFE_CONN29, 1u << 6, 1u << 6);
	RMW(NAF_PH_STREAM, NAF_AFE_DAC_CON0, 1u << 4, 1u << 4);

	/* 9. Observe the DMA pointer only: <= 400 x 50 us. */
	naf_emit(b, NAF_OBSERVE, NAF_PH_OBSERVE, NAF_AFE_DL1_CUR, NAF_SRAM,
	    NAF_SRAM + NAF_SRAM_BYTES - 1u, 400, 50);
}

static void
naf_build_down(struct naf_builder *b, const uint32_t *s)
{
	const uint32_t poweron = (NAF_SPM_PROJECT_CODE << 16) | 1u;
	/* DL1 off (fe-dai.c:631-644), interconnect off. */
	RMW(NAF_PH_STOP, NAF_AFE_DAC_CON0, 1u << 4, 0);
	RMW(NAF_PH_STOP, NAF_AFE_CONN28, 1u << 5, 0);
	RMW(NAF_PH_STOP, NAF_AFE_CONN29, 1u << 6, 0);
	/* DAPM POST_PMD in reverse supply order: I2S_EN, gpio, HD_EN, APLL. Pins
	 * return to the measured snapshot fields, not to an assumed value. */
	RMW(NAF_PH_I2S_OFF, NAF_AFE_I2S_CON3, 1u << 0, 0);
	RMW(NAF_PH_I2S_OFF, NAF_GPIO_MODE_32, NAF_PIN_MODE_MASK, s[NAU_S_MODE_32]);
	RMW(NAF_PH_I2S_OFF, NAF_LT_PU, NAF_PIN_LT_MASK, s[NAU_S_LT_PU32]);
	RMW(NAF_PH_I2S_OFF, NAF_LT_PD, NAF_PIN_LT_MASK, s[NAU_S_LT_PD32]);
	RMW(NAF_PH_I2S_OFF, NAF_AFE_I2S_CON3, 1u << 12, 0);
	/* mt6885_apll2_disable (afe-clk.c:463-477), apll2_mux_setting(false)
	 * (:185-205), PLL unprepare (clk-pll.c:524-551). */
	RMW(NAF_PH_APLL2_OFF, NAF_AFE_HD_ENGEN, 1u << 1, 0);
	RMW(NAF_PH_APLL2_OFF, NAF_AFE_APLL2_TCFG, 1u, 0);
	RMW(NAF_PH_APLL2_OFF, NAF_AFE_TOP_CON0, 1u << 18, 1u << 18);
	RMW(NAF_PH_APLL2_OFF, NAF_AFE_TOP_CON0, 1u << 9, 1u << 9);
	WR(NAF_PH_APLL2_OFF, NAF_TOP_CLR(NAF_TOP_CFG12), 3u);
	WR(NAF_PH_APLL2_OFF, NAF_TOP_SET(NAF_TOP_CFG12), 0);    /* index 0, as source writes */
	WR(NAF_PH_APLL2_OFF, NAF_TOP_UPDATE1, 1u << 17);
	WR(NAF_PH_APLL2_OFF, NAF_TOP_SET(NAF_TOP_CFG12), 1u << 7);
	WR(NAF_PH_APLL2_OFF, NAF_TOP_CLR(NAF_TOP_CFG13), 1u << 0);
	WR(NAF_PH_APLL2_OFF, NAF_TOP_SET(NAF_TOP_CFG13), 0);
	WR(NAF_PH_APLL2_OFF, NAF_TOP_UPDATE1, 1u << 21);
	RMW(NAF_PH_APLL2_OFF, NAF_APM_AP_PLL_CON3, 1u << 5, 0);
	RMW(NAF_PH_APLL2_OFF, NAF_APM_APLL2_CON0, 1u << 0, 0);
	RMW(NAF_PH_APLL2_OFF, NAF_APM_APLL2_CON4, 1u << 1, 1u << 1);
	RMW(NAF_PH_APLL2_OFF, NAF_APM_APLL2_CON4, 1u << 0, 0);
	RMW(NAF_PH_APLL2_OFF, NAF_APM_APLL2_CON1, 7u << 24, s[NAU_S_APLL2_CON1]);
	WR(NAF_PH_APLL2_OFF, NAF_APM_APLL2_CON2, s[NAU_S_APLL2_CON2]);
	RMW(NAF_PH_APLL2_OFF, NAF_APM_APLL2_CON1, 1u << 31, 1u << 31);
	WR(NAF_PH_APLL2_OFF, NAF_APM_APLL2_TUNER, s[NAU_S_APLL2_TUNER]);
	DLY(NAF_PH_APLL2_OFF, 20);
	WR(NAF_PH_APLL2_OFF, NAF_TOP_SET(NAF_TOP_CFG13), 1u << 7);
	/* runtime suspend (pcm.c:3411-3421), then disable_clock (afe-clk.c:300-314). */
	RMW(NAF_PH_AFE_OFF, NAF_AFE_DAC_CON0, 1u << 0, 0);
	POLL(NAF_PH_AFE_OFF, NAF_AFE_DAC_MON, 1u << 0, 0, 500, 20);
	RMW(NAF_PH_AFE_OFF, NAF_AFE_TOP_CON0, 1u << 2, 1u << 2);
	WR(NAF_PH_AFE_OFF, NAF_TOP_SET(NAF_TOP_CFG7), 1u << 31);
	/* MTCMOS off (pg.c:3267-3313), then the PG parents in reverse. */
	WR(NAF_PH_POWER_OFF, NAF_SPM_POWERON, poweron);
	WR(NAF_PH_POWER_OFF, NAF_IFR_PROT2_SET, 1u << 4);
	POLL(NAF_PH_POWER_OFF, NAF_IFR_PROT2_STA1, 1u << 4, 1u << 4, 1000, 10);
	RMW(NAF_PH_POWER_OFF, NAF_SPM_AUDIO_PWR, 1u << 8, 1u << 8);
	POLL(NAF_PH_POWER_OFF, NAF_SPM_AUDIO_PWR, 1u << 12, 1u << 12, 1000, 10);
	RMW(NAF_PH_POWER_OFF, NAF_SPM_AUDIO_PWR, 1u << 1, 1u << 1);
	RMW(NAF_PH_POWER_OFF, NAF_SPM_AUDIO_PWR, 1u << 4, 1u << 4);
	RMW(NAF_PH_POWER_OFF, NAF_SPM_AUDIO_PWR, 1u << 0, 0);
	RMW(NAF_PH_POWER_OFF, NAF_SPM_AUDIO_PWR, 1u << 2, 0);
	RMW(NAF_PH_POWER_OFF, NAF_SPM_AUDIO_PWR, 1u << 3, 0);
	POLL(NAF_PH_POWER_OFF, NAF_SPM_PWR_STATUS, 1u << 21, 0, 1000, 10);
	POLL(NAF_PH_POWER_OFF, NAF_SPM_PWR_STATUS2, 1u << 21, 0, 1000, 10);
	WR(NAF_PH_POWER_OFF, NAF_IFR_CG1_SET, 1u << 25);
	WR(NAF_PH_POWER_OFF, NAF_IFR_CG3_SET, 1u << 4);
	WR(NAF_PH_POWER_OFF, NAF_TOP_SET(NAF_TOP_CFG8), 1u << 7);
	WR(NAF_PH_POWER_OFF, NAF_SPM_POWERON, (NAF_SPM_PROJECT_CODE << 16) | (s[NAU_S_SPM_POWERON] & 1u));
}
#undef RMW
#undef WR
#undef POLL
#undef DLY

/* SET/CLR/UPDATE aliases and command registers: explicit writes only, never
 * read-modify-write (a read of an alias is not the target state). */
static int
naf_write_only(uint32_t pa)
{
	switch (pa) {
	case NAF_SPM_POWERON:
	case NAF_IFR_CG1_SET: case NAF_IFR_CG1_CLR: case NAF_IFR_CG3_SET: case NAF_IFR_CG3_CLR:
	case NAF_IFR_PROT2_SET: case NAF_IFR_PROT2_CLR: case NAF_TOP_UPDATE1:
	case NAF_TOP_SET(NAF_TOP_CFG7): case NAF_TOP_CLR(NAF_TOP_CFG7):
	case NAF_TOP_SET(NAF_TOP_CFG8): case NAF_TOP_CLR(NAF_TOP_CFG8):
	case NAF_TOP_SET(NAF_TOP_CFG12): case NAF_TOP_CLR(NAF_TOP_CFG12):
	case NAF_TOP_SET(NAF_TOP_CFG13): case NAF_TOP_CLR(NAF_TOP_CFG13):
		return 1;
	default:
		return 0;
	}
}

static int
naf_plan_valid(const struct naf_op *ops, unsigned n)
{
	for (unsigned i = 0; i < n; i++) {
		const struct naf_op *o = &ops[i];
		switch (o->kind) {
		case NAF_RMW:
			if (!naf_write_allowed(o->pa) || !naf_read_allowed(o->pa) || naf_write_only(o->pa)) {
				return 0;
			}
			break;
		case NAF_W:
			if (!naf_write_allowed(o->pa) || (o->pa >= NAF_SRAM && o->pa < NAF_SRAM + NAF_SRAM_BYTES)) {
				return 0;
			}
			break;
		case NAF_POLL:
		case NAF_OBSERVE:
			if (!naf_read_allowed(o->pa) || !o->arg || o->arg > 50000) { return 0; }
			break;
		case NAF_DELAY:
			if (o->arg > 1000) { return 0; }
			break;
		case NAF_FILL:
			if (o->pa != NAF_SRAM || o->arg != NAF_SRAM_BYTES || o->value) { return 0; }
			break;
		default:
			return 0;
		}
	}
	return 1;
}

int
naf_build(const struct nau_state *st, const struct naf_amp_evidence amp[2], struct naf_plan *p)
{
	if (!p) { return -1; }
	p->n_up = p->n_down = 0;
	if (naf_gate(st, amp)) { return -1; }
	struct naf_builder up = {p->up, 0, 0}, down = {p->down, 0, 0};
	naf_build_up(&up);
	naf_build_down(&down, st->snap);
	if (up.overflow || down.overflow || !naf_plan_valid(p->up, up.n) ||
	    !naf_plan_valid(p->down, down.n)) {
		return -1;
	}
	p->n_up = up.n;
	p->n_down = down.n;
	return 0;
}

/* ---- executor ---- */
static struct naf_plan naf_plan_store; /* static: not on the kernel stack */

enum { NAF_X_OK = 0, NAF_X_UNCERTAIN = -2, NAF_X_OBSERVE = -3 };

static int
naf_exec(const struct naf_io *o, void *c, struct naf_state *s, const struct naf_op *ops,
    unsigned n, unsigned *done)
{
	for (unsigned i = 0; i < n; i++) {
		const struct naf_op *op = &ops[i];
		uint32_t v = 0;
		s->phase = op->phase;
		s->op = i;
		s->last_pa = op->pa;
		switch (op->kind) {
		case NAF_RMW:
			s->reads++;
			if (o->read(c, op->pa, &v)) { s->failure = NAF_FAIL_READ; return NAF_X_UNCERTAIN; }
			v = (v & ~op->mask) | (op->value & op->mask);
			s->last_value = v;
			s->writes++;
			s->active = 1;
			if (o->write(c, op->pa, v)) { s->failure = NAF_FAIL_WRITE; return NAF_X_UNCERTAIN; }
			break;
		case NAF_W:
			s->last_value = op->value;
			s->writes++;
			s->active = 1;
			if (o->write(c, op->pa, op->value)) { s->failure = NAF_FAIL_WRITE; return NAF_X_UNCERTAIN; }
			break;
		case NAF_POLL: {
			unsigned k;
			for (k = 0; k < op->arg; k++) {
				s->reads++;
				s->polls++;
				if (o->read(c, op->pa, &v)) { s->failure = NAF_FAIL_READ; return NAF_X_UNCERTAIN; }
				s->last_value = v;
				if ((v & op->mask) == op->value) { break; }
				if (op->step_us && o->delay_us(c, op->step_us)) {
					s->failure = NAF_FAIL_DELAY;
					return NAF_X_UNCERTAIN;
				}
			}
			if (k == op->arg) { s->failure = NAF_FAIL_POLL; return NAF_X_UNCERTAIN; }
			break;
		}
		case NAF_DELAY:
			if (o->delay_us(c, op->arg)) { s->failure = NAF_FAIL_DELAY; return NAF_X_UNCERTAIN; }
			break;
		case NAF_FILL:
			for (uint32_t off = 0; off < op->arg; off += 4) {
				s->writes++;
				s->active = 1;
				if (o->write(c, op->pa + off, op->value)) {
					s->failure = NAF_FAIL_WRITE;
					return NAF_X_UNCERTAIN;
				}
			}
			break;
		case NAF_OBSERVE: {
			unsigned seen = 0, bad = 0;
			for (unsigned k = 0; k < op->arg && seen < 3; k++) {
				s->reads++;
				if (o->read(c, op->pa, &v)) { s->failure = NAF_FAIL_READ; return NAF_X_UNCERTAIN; }
				if (v < op->mask || v > op->value) { bad = 1; break; }
				if (!s->observed) { s->cur_first = v; s->observed = 1; }
				else if (v != s->cur_last) { seen++; }
				s->cur_last = v;
				if (o->delay_us(c, op->step_us)) { s->failure = NAF_FAIL_DELAY; return NAF_X_UNCERTAIN; }
			}
			s->cur_changes = seen;
			(*done)++;
			if (bad || seen < 2) { s->failure = NAF_FAIL_OBSERVE; return NAF_X_OBSERVE; }
			continue;
		}
		default:
			s->failure = NAF_FAIL_PLAN;
			return NAF_X_UNCERTAIN;
		}
		(*done)++;
	}
	return NAF_X_OK;
}

static int
naf_live(const struct naf_io *o, void *c, struct naf_state *s, const uint32_t *snap,
    uint32_t *live, unsigned *mismatch)
{
	*mismatch = 0;
	for (unsigned i = 0; i < NAF_WATCH; i++) {
		uint32_t v = 0, idx = naf_watch[i].idx;
		s->reads++;
		if (o->read(c, nau_snapshot_pa[idx], &v)) { return -1; }
		live[idx] = v;
		if ((v ^ snap[idx]) & naf_watch[i].mask) { *mismatch |= 1u << i; }
	}
	return 0;
}

int
naf_return_ok(const struct naf_state *s)
{
	return s && !s->active && !s->uncertain;
}

int
naf_cycle(const struct naf_io *o, void *c, const struct nau_state *st,
    const struct naf_amp_evidence amp[2], uint32_t authorize, struct naf_state *s)
{
	if (!s || s->entered) { return -1; }
	*s = (struct naf_state){0};
	s->entered = 1;
	s->error = -1;
	if (!o || !o->read || !o->write || !o->delay_us || !st || authorize != NAF_AUTHORIZE) {
		s->failure = NAF_FAIL_ARG;
		return -1;
	}
	s->gate = naf_gate(st, amp);
	if (s->gate) { s->failure = NAF_FAIL_GATE; return -1; }
	/* Live re-read: the boot snapshot must still describe the hardware. */
	uint32_t live[NAU_SNAP_COUNT];
	for (unsigned i = 0; i < NAU_SNAP_COUNT; i++) { live[i] = st->snap[i]; }
	unsigned drift = 0;
	if (naf_live(o, c, s, st->snap, live, &drift)) {
		s->failure = NAF_FAIL_READ; /* reads only so far: hardware untouched */
		return -1;
	}
	s->live_gate = naf_gate_words(live, amp);
	if (drift || s->live_gate) {
		s->verify_mismatch = drift;
		s->failure = NAF_FAIL_LIVE;
		return -1;
	}
	struct naf_plan *p = &naf_plan_store;
	if (naf_build(st, amp, p)) { s->failure = NAF_FAIL_PLAN; return -1; }

	int r = naf_exec(o, c, s, p->up, p->n_up, &s->up_done);
	if (r == NAF_X_UNCERTAIN) {
		s->uncertain = 1;
		s->error = -2;
		return -2;
	}
	int observe_failed = r == NAF_X_OBSERVE;
	unsigned saved_failure = s->failure;
	r = naf_exec(o, c, s, p->down, p->n_down, &s->down_done);
	if (r != NAF_X_OK) {
		s->uncertain = 1;
		s->error = -2;
		return -2;
	}
	unsigned mismatch = 0;
	if (naf_live(o, c, s, st->snap, live, &mismatch)) {
		s->failure = NAF_FAIL_READ;
		s->uncertain = 1;
		s->error = -2;
		return -2;
	}
	if (mismatch) {
		s->verify_mismatch = mismatch;
		s->failure = NAF_FAIL_VERIFY;
		s->uncertain = 1;
		s->error = -4;
		return -4;
	}
	s->active = 0;
	if (observe_failed) {
		s->failure = saved_failure;
		s->error = -3;
		return -3;
	}
	s->failure = NAF_FAIL_NONE;
	s->error = 0;
	return 0;
}

/* Continuous streaming interface. Caller is the sole worker and holds plan. */
int naf_stream_open(const struct naf_io *o, void *c, const struct nau_state *snap,
    const struct naf_amp_evidence amp[2], struct naf_plan *plan, struct naf_state *s)
{
    if (!o || !o->read || !o->write || !o->delay_us || !s || !plan || s->active || s->uncertain) return -1;
    *s = (struct naf_state){0};
    s->gate = naf_gate(snap, amp);
    if (s->gate) { s->failure = NAF_FAIL_GATE; return -1; }
    uint32_t live[NAU_SNAP_COUNT]; unsigned drift = 0;
    if (naf_live(o, c, s, snap->snap, live, &drift) || drift) { s->failure = NAF_FAIL_LIVE; return -1; }
    if (naf_build(snap, amp, plan) || !plan->n_up || plan->up[plan->n_up-1].kind != NAF_OBSERVE) return -1;
    s->entered = 1;
    int rc = naf_exec(o, c, s, plan->up, plan->n_up-1, &s->up_done);
    if (rc) { s->uncertain = 1; s->error = rc; return rc; }
    return 0;
}
int naf_stream_close(const struct naf_io *o, void *c, const struct nau_state *snap,
    const struct naf_plan *plan, struct naf_state *s)
{
    if (!o || !plan || !s || !snap || s->uncertain) return -1;
    if (!s->active) return 0;
    int rc = naf_exec(o, c, s, plan->down, plan->n_down, &s->down_done);
    if (rc) { s->uncertain = 1; return rc; }
    uint32_t live[NAU_SNAP_COUNT]; unsigned mismatch = 0;
    if (naf_live(o, c, s, snap->snap, live, &mismatch) || mismatch) {
        s->verify_mismatch = mismatch; s->uncertain = 1; return -2;
    }
    s->active = 0;
    return 0;
}
#endif /* NORD2 */
