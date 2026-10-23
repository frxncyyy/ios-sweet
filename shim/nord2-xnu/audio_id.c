#if defined(KERNEL)
#include <pexpert/arm64/board_config.h>
#endif
#if defined(NORD2)
#include "audio_id.h"

/*
 * Register sources (Android tree 04d00c2565764c3922c9b4826b5acff1106e533f,
 * CONFIG_COMMON_CLK_MT6893 -> clk-mt6885*.c, CONFIG_SND_SOC_MT6885):
 * drivers/clk/mediatek/clk-mt6885-pg.c, clk-mt6885.c, clk-mt6885-audsys.c,
 * clk-mt6885-i2c.c, drivers/pinctrl/mediatek/pinctrl-mt6885.c,
 * drivers/i2c/busses/i2c-mtk.h, sound/soc/mediatek/mt6885/mt6885-reg.h.
 */
const uint32_t nau_snapshot_pa[NAU_SNAP_COUNT] = {
	[NAU_S_PWR_STATUS] = 0x1000616c,  /* AUDIO bit21, CAM bit23 */
	[NAU_S_PWR_STATUS_2ND] = 0x10006170,
	[NAU_S_AUDIO_PWR_CON] = 0x10006354,
	[NAU_S_ADSP_PWR_CON] = 0x10006358,
	[NAU_S_IFR1_CG] = 0x10001094,     /* ifrao_audio bit25, ifrao_ap_dma_ps bit18 (1 = gated) */
	[NAU_S_IFR2_CG] = 0x100010d8,     /* ifrao_apdma bit31 */
	[NAU_S_IFR3_CG] = 0x100010ac,     /* ifrao_audio26m bit4 */
	[NAU_S_PROT2_STA1] = 0x10001724,  /* AUDIO_PROT_STEP1_0 ack bit4 */
	[NAU_S_CLK_CFG_7] = 0x10000080,   /* audio_sel [25:24], pdn31 */
	[NAU_S_CLK_CFG_8] = 0x10000090,   /* aud_intbus_sel [1:0], pdn7 */
	[NAU_S_CLK_CFG_10] = 0x100000b0,  /* i2c_sel [9:8], pdn15 */
	[NAU_S_CLK_CFG_11] = 0x100000c0,  /* aud_engen1_sel [25:24], pdn31 */
	[NAU_S_CLK_CFG_12] = 0x100000d0,  /* aud_engen2_sel [1:0] pdn7; aud_1_sel 24, pdn31 */
	[NAU_S_CLK_CFG_13] = 0x100000e0,  /* aud_2_sel 0, pdn7 */
	[NAU_S_CLK_CFG_15] = 0x10000100,  /* audio_h_sel [1:0], pdn7 */
	[NAU_S_AUDDIV_0] = 0x10000320,
	[NAU_S_AUDDIV_1] = 0x10000324,
	[NAU_S_AUDDIV_2] = 0x10000328,
	[NAU_S_AUDDIV_3] = 0x10000334,
	[NAU_S_AUDDIV_4] = 0x10000338,
	[NAU_S_AP_PLL_CON3] = 0x1000c00c,
	[NAU_S_APLL1_TUNER] = 0x1000c040,
	[NAU_S_APLL2_TUNER] = 0x1000c044,
	[NAU_S_APLL1_CON0] = 0x1000c318,
	[NAU_S_APLL1_CON1] = 0x1000c31c,
	[NAU_S_APLL1_CON2] = 0x1000c320,
	[NAU_S_APLL1_CON3] = 0x1000c324,
	[NAU_S_APLL1_CON4] = 0x1000c328,
	[NAU_S_APLL2_CON0] = 0x1000c32c,
	[NAU_S_APLL2_CON1] = 0x1000c330,
	[NAU_S_APLL2_CON2] = 0x1000c334,
	[NAU_S_APLL2_CON3] = 0x1000c338,
	[NAU_S_APLL2_CON4] = 0x1000c33c,
	[NAU_S_MAINPLL_CON0] = 0x1000c340,
	[NAU_S_MAINPLL_CON1] = 0x1000c344,
	[NAU_S_MAINPLL_CON3] = 0x1000c34c,
	[NAU_S_UNIVPLL_CON0] = 0x1000c308,
	[NAU_S_UNIVPLL_CON1] = 0x1000c30c,
	[NAU_S_UNIVPLL_CON3] = 0x1000c314,
	[NAU_S_IMPS_CG] = 0x11d05e00,     /* I2C7 bit3, I2C8 bit4 (1 = gated) */
	[NAU_S_MODE_32] = 0x10005340,
	[NAU_S_MODE_104] = 0x100053d0,
	[NAU_S_MODE_120] = 0x100053f0,
	[NAU_S_MODE_144] = 0x10005420,
	[NAU_S_DIR_32] = 0x10005010,
	[NAU_S_DIR_96] = 0x10005030,
	[NAU_S_DIR_128] = 0x10005040,
	[NAU_S_DOUT_32] = 0x10005110,
	[NAU_S_DOUT_96] = 0x10005130,
	[NAU_S_DOUT_128] = 0x10005140,
	[NAU_S_DIN_32] = 0x10005210,
	[NAU_S_DIN_96] = 0x10005230,
	[NAU_S_DIN_128] = 0x10005240,
	[NAU_S_BM_EH122] = 0x11d10040,
	[NAU_S_BM_EH123] = 0x11d10050,
	[NAU_S_BM_IES111] = 0x11d10060,
	[NAU_S_BM_IES122] = 0x11d10070,
	[NAU_S_BM_PD111] = 0x11d10080,
	[NAU_S_BM_PD122] = 0x11d10090,
	[NAU_S_BM_PU111] = 0x11d100a0,
	[NAU_S_BM_PU122] = 0x11d100b0,
	[NAU_S_BM_RSEL] = 0x11d100e0,
	[NAU_S_RM_IES148] = 0x11c20020,
	[NAU_S_RM_PD148] = 0x11c20030,
	[NAU_S_RM_PU148] = 0x11c20050,
	[NAU_S_LT_IES32] = 0x11f20040,
	[NAU_S_LT_PD32] = 0x11f20060,
	[NAU_S_LT_PU32] = 0x11f20080,
	[NAU_S_SPM_POWERON] = 0x10006000, /* POWERON_CONFIG_EN; read only here */
};

const uint32_t nau_afe_pa[NAU_AFE_COUNT] = {
	[NAU_A_TOP_CON0] = 0x11210000,
	[NAU_A_TOP_CON1] = 0x11210004,
	[NAU_A_TOP_CON2] = 0x11210008,
	[NAU_A_TOP_CON3] = 0x1121000c,
	[NAU_A_DAC_CON0] = 0x11210010,
	[NAU_A_I2S_CON3] = 0x11210040,    /* DAI I2S3 uses the I2S4_* fields */
	[NAU_A_DL1_CON0] = 0x1121004c,
	[NAU_A_CONN28] = 0x112104bc,
	[NAU_A_CONN29] = 0x112104c0,
};

const uint32_t nau_dma_pa[NAU_DMA_COUNT] = {
	[NAU_D_INT_FLAG] = 0x11000780,
	[NAU_D_INT_EN] = 0x11000784,
	[NAU_D_EN] = 0x11000788,
	[NAU_D_CON] = 0x11000798,
};

static const uint16_t nau_i2c8_off[NAU_C_PER_CH] = {0x08, 0x0c, 0x10, 0x24, 0xf4};

uint32_t
nau_i2c8_pa(unsigned index)
{
	if (index >= NAU_I2C8_COUNT) { return 0; }
	return 0x11d04000u + 0x100u * (index / NAU_C_PER_CH) + nau_i2c8_off[index % NAU_C_PER_CH];
}

const unsigned nau_pin_list[NAU_PIN_COUNT] = {32, 33, 34, 35, 111, 122, 123, 148};

static const struct {
	uint8_t mode, gpio, ies, pd, pu, bit;
} nau_pin_loc[NAU_PIN_COUNT] = {
	{NAU_S_MODE_32, 0, NAU_S_LT_IES32, NAU_S_LT_PD32, NAU_S_LT_PU32, 3},
	{NAU_S_MODE_32, 0, NAU_S_LT_IES32, NAU_S_LT_PD32, NAU_S_LT_PU32, 5},
	{NAU_S_MODE_32, 0, NAU_S_LT_IES32, NAU_S_LT_PD32, NAU_S_LT_PU32, 2},
	{NAU_S_MODE_32, 0, NAU_S_LT_IES32, NAU_S_LT_PD32, NAU_S_LT_PU32, 4},
	{NAU_S_MODE_104, 1, NAU_S_BM_IES111, NAU_S_BM_PD111, NAU_S_BM_PU111, 24},
	{NAU_S_MODE_120, 1, NAU_S_BM_IES122, NAU_S_BM_PD122, NAU_S_BM_PU122, 3},
	{NAU_S_MODE_120, 1, NAU_S_BM_IES122, NAU_S_BM_PD122, NAU_S_BM_PU122, 13},
	{NAU_S_MODE_144, 2, NAU_S_RM_IES148, NAU_S_RM_PD148, NAU_S_RM_PU148, 5},
};
static const uint8_t nau_gpio_word[3][3] = {
	{NAU_S_DIR_32, NAU_S_DOUT_32, NAU_S_DIN_32},
	{NAU_S_DIR_96, NAU_S_DOUT_96, NAU_S_DIN_96},
	{NAU_S_DIR_128, NAU_S_DOUT_128, NAU_S_DIN_128},
};

void
nau_pin_decode(const uint32_t s[NAU_SNAP_COUNT], struct nau_pin out[NAU_PIN_COUNT])
{
	for (unsigned i = 0; i < NAU_PIN_COUNT; i++) {
		unsigned pin = nau_pin_list[i], g = nau_pin_loc[i].gpio, b = nau_pin_loc[i].bit;
		out[i].pin = pin;
		out[i].mode = (s[nau_pin_loc[i].mode] >> (pin % 8 * 4)) & 0xf;
		out[i].dir = (s[nau_gpio_word[g][0]] >> (pin % 32)) & 1;
		out[i].dout = (s[nau_gpio_word[g][1]] >> (pin % 32)) & 1;
		out[i].din = (s[nau_gpio_word[g][2]] >> (pin % 32)) & 1;
		out[i].ies = (s[nau_pin_loc[i].ies] >> b) & 1;
		out[i].pd = (s[nau_pin_loc[i].pd] >> b) & 1;
		out[i].pu = (s[nau_pin_loc[i].pu] >> b) & 1;
	}
}

int
nau_amp_reset(const struct nau_pin p[NAU_PIN_COUNT], unsigned amp)
{
	if (amp >= 2) { return -1; }
	const struct nau_pin *r = &p[amp ? 7 : 4];
	if (r->mode != 0) { return -1; }
	return r->dir ? (int)r->dout : (r->ies ? (int)r->din : -1);
}

unsigned
nau_afe_block(const uint32_t s[NAU_SNAP_COUNT], unsigned snap_read)
{
	if (snap_read != NAU_SNAP_COUNT) { return NAU_AFE_BLOCK_SNAPSHOT; }
	unsigned block = 0;
	/* AUDIO_PWR_STA_MASK and the power-on sequence: clk-mt6885-pg.c:3314-3355. */
	if (!(s[NAU_S_PWR_STATUS] & (1u << 21)) || !(s[NAU_S_PWR_STATUS_2ND] & (1u << 21))) {
		block |= NAU_AFE_BLOCK_STATUS;
	}
	if ((s[NAU_S_AUDIO_PWR_CON] & 0x111fu) != 0x000du) { block |= NAU_AFE_BLOCK_PWR_CON; }
	if ((s[NAU_S_IFR1_CG] & (1u << 25)) || (s[NAU_S_IFR3_CG] & (1u << 4))) {
		block |= NAU_AFE_BLOCK_INFRA;
	}
	if (s[NAU_S_PROT2_STA1] & (1u << 4)) { block |= NAU_AFE_BLOCK_PROTECT; }
	if ((s[NAU_S_CLK_CFG_7] & (1u << 31)) || (s[NAU_S_CLK_CFG_8] & (1u << 7))) {
		block |= NAU_AFE_BLOCK_MUX;
	}
	return block;
}

unsigned
nau_i2c8_block(const uint32_t s[NAU_SNAP_COUNT], unsigned snap_read)
{
	if (snap_read != NAU_SNAP_COUNT) { return NAU_I2C8_BLOCK_SNAPSHOT; }
	unsigned block = 0;
	if (s[NAU_S_CLK_CFG_10] & (1u << 15)) { block |= NAU_I2C8_BLOCK_MUX; }
	if (s[NAU_S_IMPS_CG] & (1u << 4)) { block |= NAU_I2C8_BLOCK_GATED; }
	/* Same parent predicate as reviewed mtk_i2c_read.c:29-32: en bit0,
	 * rstb bit23, CON3 power on/iso off. */
	unsigned select = (s[NAU_S_CLK_CFG_10] >> 8) & 3;
	if (select == 3) { block |= NAU_I2C8_BLOCK_PLL; }
	if (select == 1 || select == 2) {
		uint32_t con = s[select == 1 ? NAU_S_MAINPLL_CON0 : NAU_S_UNIVPLL_CON0];
		uint32_t pwr = s[select == 1 ? NAU_S_MAINPLL_CON3 : NAU_S_UNIVPLL_CON3];
		if (!(con & 1) || !(con & 0x800000) || (pwr & 3) != 1) { block |= NAU_I2C8_BLOCK_PLL; }
	}
	return block;
}

unsigned
nau_dma_block(const uint32_t s[NAU_SNAP_COUNT], unsigned snap_read)
{
	if (snap_read != NAU_SNAP_COUNT) { return NAU_DMA_BLOCK_SNAPSHOT; }
	unsigned block = 0;
	if (s[NAU_S_IFR1_CG] & (1u << 18)) { block |= NAU_DMA_BLOCK_PS; }
	if (s[NAU_S_IFR2_CG] & (1u << 31)) { block |= NAU_DMA_BLOCK_APDMA; }
	return block;
}

unsigned
nau_i2c8_ready(const struct nau_state *st)
{
	if (!st || st->failed || st->snap_read != NAU_SNAP_COUNT) { return NAU_READY_SNAPSHOT; }
	const uint32_t *s = st->snap;
	unsigned r = 0, b = nau_i2c8_block(s, st->snap_read);
	if (b & (NAU_I2C8_BLOCK_MUX | NAU_I2C8_BLOCK_PLL)) { r |= NAU_READY_MUX; }
	if (b & NAU_I2C8_BLOCK_GATED) { r |= NAU_READY_GATED; }
	if ((s[NAU_S_MODE_120] & 0xff00) != 0x1100) { r |= NAU_READY_PINMUX; }
	if ((s[NAU_S_PWR_STATUS] | s[NAU_S_PWR_STATUS_2ND]) & (1u << 23)) { r |= NAU_READY_CAM; }
	if (st->i2c8_read != NAU_I2C8_COUNT) { return r | NAU_READY_UNREAD; }
	const uint32_t *c = st->i2c8;
	for (unsigned ch = 0; ch < NAU_CH_COUNT; ch++) {
		if (c[NAU_I2C8_INDEX(ch, NAU_C_START)] & 3) { r |= NAU_READY_START; }
	}
	if ((c[NAU_I2C8_INDEX(NAU_CH_SHADOW, NAU_C_CTRL)] | c[NAU_I2C8_INDEX(NAU_CH_AP, NAU_C_CTRL)] | c[NAU_I2C8_INDEX(NAU_CH_CCU, NAU_C_CTRL)]) & 4) {
		r |= NAU_READY_DMA;
	}
	if (st->dma_read == NAU_DMA_COUNT && st->dma[NAU_D_EN]) { r |= NAU_READY_DMA; }
	uint32_t irq = c[NAU_I2C8_INDEX(NAU_CH_AP, NAU_C_STAT)];
	if (irq != 0 && irq != 1) { r |= NAU_READY_IRQ; }
	for(unsigned ch=0;ch<NAU_CH_COUNT;ch++)if(c[NAU_I2C8_INDEX(ch,NAU_C_FIFO)])r|=NAU_READY_FIFO;
 if((s[NAU_S_BM_IES122]&0x2008u)!=0x2008u || (s[NAU_S_DIN_96]&0x0c000000u)!=0x0c000000u)r|=NAU_READY_LINES;
	return r;
}

int
nau_read_allowed(uint32_t pa, unsigned width)
{
	if (width == 4) {
		for (unsigned i = 0; i < NAU_SNAP_COUNT; i++) {
			if (nau_snapshot_pa[i] == pa) { return 1; }
		}
		for (unsigned i = 0; i < NAU_AFE_COUNT; i++) {
			if (nau_afe_pa[i] == pa) { return 1; }
		}
		for (unsigned i = 0; i < NAU_DMA_COUNT; i++) {
			if (nau_dma_pa[i] == pa) { return 1; }
		}
		return 0;
	}
	if (width == 2) {
		for (unsigned i = 0; i < NAU_I2C8_COUNT; i++) {
			if (nau_i2c8_pa(i) == pa) { return 1; }
		}
	}
	return 0;
}

static int
nau_read(const struct nau_ops *o, void *c, struct nau_state *s, uint32_t pa, unsigned w, uint32_t *v)
{
	s->callbacks++;
	if (o->read(c, pa, w, v)) {
		s->failed = 1;
		s->error = -4;
		s->stage = NAU_DONE;
		return -1;
	}
	return 0;
}

int
nau_run(const struct nau_ops *o, void *c, unsigned optin, struct nau_state *s)
{
	if (optin != 1 || !o || !s) { return -1; }
	if (s->entered) { return -2; }
	*s = (struct nau_state){0};
	s->entered = 1;
	if (!o->read) {
		s->failed = 1;
		s->error = -3;
		return -3;
	}
	s->stage = NAU_SNAPSHOT;
	for (unsigned i = 0; i < NAU_SNAP_COUNT; i++) {
		if (nau_read(o, c, s, nau_snapshot_pa[i], 4, &s->snap[i])) { return -4; }
		s->snap_read++;
	}
	s->stage = NAU_AFE;
	s->afe_block = nau_afe_block(s->snap, s->snap_read);
	for (unsigned i = 0; !s->afe_block && i < NAU_AFE_COUNT; i++) {
		if (nau_read(o, c, s, nau_afe_pa[i], 4, &s->afe[i])) { return -4; }
		s->afe_read++;
	}
	s->stage = NAU_I2C8;
	s->i2c8_block = nau_i2c8_block(s->snap, s->snap_read);
	for (unsigned i = 0; !s->i2c8_block && i < NAU_I2C8_COUNT; i++) {
		if (nau_read(o, c, s, nau_i2c8_pa(i), 2, &s->i2c8[i])) { return -4; }
		s->i2c8_read++;
	}
	s->stage = NAU_DMA;
	s->dma_block = nau_dma_block(s->snap, s->snap_read);
	for (unsigned i = 0; !s->dma_block && i < NAU_DMA_COUNT; i++) {
		if (nau_read(o, c, s, nau_dma_pa[i], 4, &s->dma[i])) { return -4; }
		s->dma_read++;
	}
	s->stage = NAU_DONE;
	return 0;
}
#endif /* NORD2 */
