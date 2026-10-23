#ifndef NORD2_AUDIO_ID_H
#define NORD2_AUDIO_ID_H
#include <stdint.h>

/*
 * Nord2 speaker path, milestone 1a: NON-MUTATING snapshot (revision 3).
 *
 * Every access is an MMIO read through ops->read of an address in one of the
 * tables below; this module has no write path at all. The GPIO bit-bang I2C8
 * takeover of revision 1 was removed (parent review A1/A2): pin state does not
 * prove I2C8 controller/DMA idle, and I2C8 carries the DDR/UFS rail bucks.
 *
 * Conditional apertures, read only when the snapshot proves their clocks on:
 * - AFE (0x11210000): AUDIO MTCMOS on, unisolated, clocked, unprotected;
 * - I2C8 controller shadow/AP/CCU channels (16-bit): i2c_sel not powered down
 *   and the controller gate CLK_IMPS_AP_I2C8_RO ungated. DATA_PORT is never
 *   read (it pops the FIFO);
 * - APDMA I2C8 channel (0x11000780): ap_dma_ps and apdma both ungated.
 * The first read callback failure is sticky: no further callback is made.
 *
 * Revision 3 keeps that snapshot contract and changes only milestone 1b
 * (audio_i2c8.c): a clean completed NACK is a bounded result rather than an
 * uncertain fault, the source V2 HW timeout is programmed/restored, and an
 * AP-channel I3C_EN is retained as read-only evidence; the fixed100kHz
 * transport follows the source legacy-I2C branch without changing it.
 */

enum nau_snap {
	/* SPM 0x10006000 (clk-mt6885-pg.c:166-192) */
	NAU_S_PWR_STATUS, NAU_S_PWR_STATUS_2ND, NAU_S_AUDIO_PWR_CON, NAU_S_ADSP_PWR_CON,
	/* infracfg_ao 0x10001000 (clk-mt6885.c:2109-2136, clk-mt6885-pg.c:235) */
	NAU_S_IFR1_CG, NAU_S_IFR2_CG, NAU_S_IFR3_CG, NAU_S_PROT2_STA1,
	/* topckgen 0x10000000 (clk-mt6885.c:57-89, 1834-1989, 2017-2089) */
	NAU_S_CLK_CFG_7, NAU_S_CLK_CFG_8, NAU_S_CLK_CFG_10, NAU_S_CLK_CFG_11, NAU_S_CLK_CFG_12,
	NAU_S_CLK_CFG_13, NAU_S_CLK_CFG_15,
	NAU_S_AUDDIV_0, NAU_S_AUDDIV_1, NAU_S_AUDDIV_2, NAU_S_AUDDIV_3, NAU_S_AUDDIV_4,
	/* apmixedsys 0x1000c000 (clk-mt6885.c:167-231, 2523-2534) */
	NAU_S_AP_PLL_CON3, NAU_S_APLL1_TUNER, NAU_S_APLL2_TUNER,
	NAU_S_APLL1_CON0, NAU_S_APLL1_CON1, NAU_S_APLL1_CON2, NAU_S_APLL1_CON3, NAU_S_APLL1_CON4,
	NAU_S_APLL2_CON0, NAU_S_APLL2_CON1, NAU_S_APLL2_CON2, NAU_S_APLL2_CON3, NAU_S_APLL2_CON4,
	/* i2c_sel parents mainpll_d4_d8 / univpll_d5_d4 (clk-mt6885.c:194-201, 1102-1106) */
	NAU_S_MAINPLL_CON0, NAU_S_MAINPLL_CON1, NAU_S_MAINPLL_CON3,
	NAU_S_UNIVPLL_CON0, NAU_S_UNIVPLL_CON1, NAU_S_UNIVPLL_CON3,
	/* imp_iic_wrap_s 0x11d05000 gate status (clk-mt6885-i2c.c:110-135) */
	NAU_S_IMPS_CG,
	/* GPIO 0x10005000 (pinctrl-mt6885.c:26-85) */
	NAU_S_MODE_32, NAU_S_MODE_104, NAU_S_MODE_120, NAU_S_MODE_144,
	NAU_S_DIR_32, NAU_S_DIR_96, NAU_S_DIR_128,
	NAU_S_DOUT_32, NAU_S_DOUT_96, NAU_S_DOUT_128,
	NAU_S_DIN_32, NAU_S_DIN_96, NAU_S_DIN_128,
	/* IOCFG_BM 0x11d10000: GPIO111/122/123 (pinctrl-mt6885.c:223-235,333-345,429-441,527-528,615-705) */
	NAU_S_BM_EH122, NAU_S_BM_EH123, NAU_S_BM_IES111, NAU_S_BM_IES122,
	NAU_S_BM_PD111, NAU_S_BM_PD122, NAU_S_BM_PU111, NAU_S_BM_PU122, NAU_S_BM_RSEL,
	/* IOCFG_RM 0x11c20000: GPIO148 (pinctrl-mt6885.c:247,357,453) */
	NAU_S_RM_IES148, NAU_S_RM_PD148, NAU_S_RM_PU148,
	/* IOCFG_LT 0x11f20000: GPIO32..35 (pinctrl-mt6885.c:179-182,299-302,395-398) */
	NAU_S_LT_IES32, NAU_S_LT_PD32, NAU_S_LT_PU32,
	/* Revision5: SPM POWERON_CONFIG_EN, read only, for the audio_afe.c gate
	 * (clk-mt6885-pg.c:166, 263, 3267). */
	NAU_S_SPM_POWERON,
	/* Append-only: protection request, distinct from asynchronous STA1 ack. */
	NAU_S_PROT2_EN,
	NAU_SNAP_COUNT
};

enum nau_afe {
	/* AFE 0x11210000 (mt6885-reg.h:2355-2377, 2564-2565) */
	NAU_A_TOP_CON0, NAU_A_TOP_CON1, NAU_A_TOP_CON2, NAU_A_TOP_CON3,
	NAU_A_DAC_CON0, NAU_A_I2S_CON3, NAU_A_DL1_CON0, NAU_A_CONN28, NAU_A_CONN29,
	NAU_AFE_COUNT
};

/* I2C8 16-bit registers; channel c at 0x11d04000 + 0x100*c (0 shadow, 1 AP,
 * 2 CCU). V2 offsets i2c-mtk.h:252-277: INTR_MASK 08, INTR_STAT 0c,
 * CONTROL 10, START 24, FIFO_STAT f4. */
enum { NAU_C_MASK, NAU_C_STAT, NAU_C_CTRL, NAU_C_START, NAU_C_FIFO, NAU_C_PER_CH };
enum { NAU_CH_SHADOW, NAU_CH_AP, NAU_CH_CCU, NAU_CH_COUNT };
#define NAU_I2C8_COUNT (NAU_C_PER_CH * NAU_CH_COUNT)
#define NAU_I2C8_INDEX(ch, reg) ((ch) * NAU_C_PER_CH + (reg))

/* APDMA I2C8 channel 32-bit registers (i2c-mtk.h:145-152). */
enum { NAU_D_INT_FLAG, NAU_D_INT_EN, NAU_D_EN, NAU_D_CON, NAU_DMA_COUNT };

/* Reasons an aperture was not read; 0 means it was read. */
enum {
	NAU_AFE_BLOCK_SNAPSHOT = 1u << 0,
	NAU_AFE_BLOCK_STATUS = 1u << 1,   /* PWR_STATUS/2ND bit21 not both set */
	NAU_AFE_BLOCK_PWR_CON = 1u << 2,  /* ON/ON_2ND/RST_B, ISO/CLK_DIS/SRAM_PDN */
	NAU_AFE_BLOCK_INFRA = 1u << 3,    /* ifrao_audio or ifrao_audio26m gated */
	NAU_AFE_BLOCK_PROTECT = 1u << 4,  /* AUDIO bus protection step1 set */
	NAU_AFE_BLOCK_MUX = 1u << 5,      /* audio_sel or aud_intbus_sel pdn */
};
enum {
	NAU_I2C8_BLOCK_SNAPSHOT = 1u << 0,
	NAU_I2C8_BLOCK_MUX = 1u << 1,     /* i2c_sel pdn (CLK_CFG_10 bit15) */
	NAU_I2C8_BLOCK_GATED = 1u << 2,   /* CLK_IMPS_AP_I2C8_RO gated (sta bit4) */
	NAU_I2C8_BLOCK_PLL = 1u << 3,     /* select 3, or selected PLL not on/powered */
};
enum {
	NAU_DMA_BLOCK_SNAPSHOT = 1u << 0,
	NAU_DMA_BLOCK_PS = 1u << 1,       /* ifrao_ap_dma_ps gated (IFRAO1 bit18) */
	NAU_DMA_BLOCK_APDMA = 1u << 2,    /* ifrao_apdma gated (IFRAO2 bit31) */
};
/* Why milestone 1b (audio_i2c8.c) would refuse, from snapshot data alone.
 * 1b repeats every check itself; this is only the report/level-2 gate. */
enum {
	NAU_READY_SNAPSHOT = 1u << 0, NAU_READY_MUX = 1u << 1, NAU_READY_PINMUX = 1u << 2,
	NAU_READY_CAM = 1u << 3, NAU_READY_GATED = 1u << 4, NAU_READY_START = 1u << 5,
	NAU_READY_DMA = 1u << 6, NAU_READY_IRQ = 1u << 7, NAU_READY_FIFO = 1u << 8,
	NAU_READY_LINES = 1u << 10, /* existing input enable + pads high required */
	NAU_READY_UNREAD = 1u << 9,       /* controller registers not observed */
};

struct nau_ops {
	/* Read one allowlisted physical register of width 2 or 4. 0 on success. */
	int (*read)(void *, uint32_t, unsigned, uint32_t *);
};

enum nau_stage { NAU_IDLE = 0, NAU_SNAPSHOT = 1, NAU_AFE = 2, NAU_I2C8 = 3, NAU_DMA = 4, NAU_DONE = 5 };

struct nau_state {
	unsigned entered, stage, failed, callbacks;
	int error;
	unsigned snap_read;
	uint32_t snap[NAU_SNAP_COUNT];
	unsigned afe_block, afe_read;
	uint32_t afe[NAU_AFE_COUNT];
	unsigned i2c8_block, i2c8_read;
	uint32_t i2c8[NAU_I2C8_COUNT];
	unsigned dma_block, dma_read;
	uint32_t dma[NAU_DMA_COUNT];
};

#define NAU_PIN_COUNT 8
struct nau_pin {
	unsigned pin, mode, dir, dout, din, ies, pu, pd;
};
extern const unsigned nau_pin_list[NAU_PIN_COUNT]; /* 32,33,34,35,111,122,123,148 */
void nau_pin_decode(const uint32_t snap[NAU_SNAP_COUNT], struct nau_pin out[NAU_PIN_COUNT]);
/* Active-high TFA reset (tfa98xx.c:4740-4749) on GPIO111 (amp 0) / GPIO148
 * (amp 1): 1 asserted, 0 released, -1 unknown (not GPIO mode, input disabled). */
int nau_amp_reset(const struct nau_pin pins[NAU_PIN_COUNT], unsigned amp);

extern const uint32_t nau_snapshot_pa[NAU_SNAP_COUNT];
extern const uint32_t nau_afe_pa[NAU_AFE_COUNT];
extern const uint32_t nau_dma_pa[NAU_DMA_COUNT];
uint32_t nau_i2c8_pa(unsigned index);

unsigned nau_afe_block(const uint32_t snap[NAU_SNAP_COUNT], unsigned snap_read);
unsigned nau_i2c8_block(const uint32_t snap[NAU_SNAP_COUNT], unsigned snap_read);
unsigned nau_dma_block(const uint32_t snap[NAU_SNAP_COUNT], unsigned snap_read);
unsigned nau_i2c8_ready(const struct nau_state *);
/* 1 if (pa, width) may be passed to ops->read. */
int nau_read_allowed(uint32_t pa, unsigned width);

/*
 * optin must equal 1; any other value, a NULL argument or a second call
 * performs no callback. Returns 0 when every enabled aperture was read;
 * -1 disabled, -2 reentry, -3 bad ops, -4 read callback failed (sticky).
 */
int nau_run(const struct nau_ops *, void *, unsigned optin, struct nau_state *);
#endif
