#if defined(KERNEL)
#include <pexpert/arm64/board_config.h>
#endif
#if defined(NORD2)
/* Selected MT6893 I2Cv2 PIO derivative for multi-channel I2C8 (GPL-2.0
 * source contract documented in AUDIO-REPORT.md section 5). */
#include "audio_i2c8.h"
#include "audio_board.h"

#define NA8_SH 0x11d04000u
#define NA8_CH 0x11d04100u
#define NA8_CCU 0x11d04200u
#define NA8_GATE 0x11d05000u
#define NA8_DMA 0x11000780u
#define NA8_GATE_BIT (1u << 4)
#define NA8_I3C_EN (1u << 15)

static const uint8_t na8_slave[2] = {0x34, 0x37};

static int na8_silicon_value(unsigned reg, uint16_t value)
{
 if (reg==0x0f) return value==0x5a6b || value==0;
 if (reg==0xa0) return 1; /* only volatile lock key, never MTP row */
 if (reg==0xa1) return value==0x5a || value==0;
 if (reg==0x02 && value==0x0628) return 1;
 if (reg==0x02 && value==0x22a8) return 1;
 if (reg==0x02 && value==0x22c8) return 1;
 if (reg==0x4c && value==0x00e9) return 1;
 if (reg==0x51 && value==0x0020) return 1;
 if (reg==0x52 && value==0x17d0) return 1;
 if (reg==0x52 && value==0x57dc) return 1;
 if (reg==0x53 && value==0x003e) return 1;
 if (reg==0x56 && value==0x0011) return 1;
 if (reg==0x56 && value==0x0400) return 1;
 if (reg==0x58 && value==0x0200) return 1;
 if (reg==0x58 && value==0x16a4) return 1;
 if (reg==0x59 && value==0x0001) return 1;
 if (reg==0x5f && value==0x0180) return 1;
 if (reg==0x61 && value==0x0110) return 1;
 if (reg==0x61 && value==0x0183) return 1;
 if (reg==0x63 && value==0x005a) return 1;
 if (reg==0x63 && value==0x055a) return 1;
 if (reg==0x65 && value==0x0542) return 1;
 if (reg==0x66 && value==0x0701) return 1;
 if (reg==0x6f && value==0x0082) return 1;
 if (reg==0x6f && value==0x00a3) return 1;
 if (reg==0x6f && value==0x00a5) return 1;
 if (reg==0x70 && value==0x07f8) return 1;
 if (reg==0x70 && value==0xa3eb) return 1;
 if (reg==0x70 && value==0xa3fb) return 1;
 if (reg==0x71 && value==0x007e) return 1;
 if (reg==0x71 && value==0x107e) return 1;
 if (reg==0x73 && value==0x0007) return 1;
 if (reg==0x73 && value==0x0047) return 1;
 if (reg==0x73 && value==0x0187) return 1;
 if (reg==0x74 && value==0x5068) return 1;
 if (reg==0x74 && value==0x5098) return 1;
 if (reg==0x75 && value==0x0d28) return 1;
 if (reg==0x75 && value==0x8d28) return 1;
 if (reg==0x80 && value==0x0000) return 1;
 if (reg==0x83 && value==0x009a) return 1;
 if (reg==0x83 && value==0x0594) return 1;
 if (reg==0x83 && value==0x0595) return 1;
 if (reg==0x83 && value==0x071c) return 1;
 if (reg==0x83 && value==0x0799) return 1;
 if (reg==0x84 && value==0x0001) return 1;
 if (reg==0x84 && value==0x0081) return 1;
 if (reg==0x84 && value==0x0211) return 1;
 if (reg==0x85 && value==0x0001) return 1;
 if (reg==0x85 && value==0x0380) return 1;
 if (reg==0x85 && value==0x0382) return 1;
 if (reg==0x88 && value==0x0000) return 1;
 if (reg==0x8c && value==0x0210) return 1;
 if (reg==0xc4 && value==0x2001) return 1;
 if (reg==0xd5 && value==0x0000) return 1;
 if (reg==0xd5 && value==0x004d) return 1;
 return 0;
}

static int
na8_allowed(unsigned slave, unsigned reg)
{
	return (slave == 0x34 || slave == 0x37) &&
        (reg == 0x03 || reg == 0x00 || reg == 0x01 || reg == 0x02 ||
         reg == 0x04 || nah_board_register_allowed(reg) || reg == 0xfb || na8_silicon_value(reg,0) || reg == 0x10 || reg == 0x11 || reg == 0x14 ||
         (reg >= 0x20 && reg <= 0x27) || reg == 0x5f);
}

static void
na8_fresh(struct na8_state *s)
{
	s->entered = 1;
	s->polls = 0;
	s->error = -1;
	s->stage = 1;
	s->failure = s->last_valid = s->last_write = s->last_width = 0;
	s->nack = s->i3c = s->restoring = 0;
	s->last_addr = s->last_value = 0;
	s->mux = s->parent_hz = s->pinmux = s->gate = s->pwr = s->pwr2 = 0;
	s->ifr1 = s->ifr2 = s->dma_en = s->sh_start = s->ccu_start = s->ccu_ctrl = 0;
	s->prior_irq = s->prior_ack = s->completion_irq = 0;
	s->dma_checked = 0;
}

int
na8_return_ok(const struct na8_state *s)
{
	return s && !s->inflight && !s->uncertain;
}

static int
na8_transfer16(const struct na8_io *o, void *c, struct na8_state *s,
    unsigned slave, unsigned reg, uint16_t *value, unsigned writing)
{
	if (!s || s->offline || s->inflight) { return -1; }
	if (!o || !o->read || !o->write || !o->delay_us || !value || (!na8_allowed(slave, reg) && !(writing && (slave==0x34 || slave==0x37) && na8_silicon_value(reg,*value)))) {
		s->offline = 1;
		s->failure = 19;
		return -1;
	}
	if (writing && !(reg == 0x00 || reg == 0x01 || reg == 0x02 || reg == 0x04 ||
        (reg >= 0x20 && reg <= 0x27) || reg == 0x5f || nah_board_register_allowed(reg) || na8_silicon_value(reg,*value))) {
        s->failure = 19; return -1;
    }
    na8_fresh(s);
	uint32_t v = 0;
	uint32_t old[15];
	unsigned touched = 0, wrote = 0;
	/* Channel-relative V2 offsets written by mt_i2c_do_transfer(): SLAVE_ADDR,
	 * INTR_MASK, CONTROL, TRANSFER_LEN, TRANSAC_LEN, DELAY_LEN, TIMING,
	 * EXT_CONF, LTIMING, HS, IO_CONFIG, MCU_INTR, TRANSFER_LEN_AUX, CLOCK_DIV,
	 * HW_TIMEOUT (i2c-mtk.h:252-282, i2c-mtk.c:963-1055, 978-1203). */
	static const uint32_t offs[15] = {4, 8, 0x10, 0x14, 0x18, 0x1c, 0x20, 0x28, 0x2c,
	    0x30, 0x34, 0x40, 0x44, 0x48, 0x4c};
	/* mask 0 (polled), CONTROL ACKERR_DET|CLK_EXT|DIR_CHANGE|RS = 0x3a, write 1,
	 * transac 2, DELAY_LEN 0x0a, ST start 0x8001, HS 0, open drain 3, MCU_INTR 1,
	 * aux read 2, clock-div 5 -> 0x404 (DT:9466-9488, i2c-mtk.c:983-1203).
	 * INTR_MASK=0 is deliberate: this diagnostic polls and must not assert the
	 * controller IRQ; the source's own IRQ handler masks all interrupts and
	 * then reads INTR_STAT, so status latching does not depend on the mask
	 * (i2c-mtk.c:1697-1701). HW_TIMEOUT is filled in below. */
	uint32_t next[15] = {(uint32_t)slave << 1, 0, 0x3a, 1, 2, 10, 0, 0x8001, 0,
	    0, 3, 1, 2, 0x404, 0};
	/* Standard source ST_OP write: CONTROL ACKERR_DET|CLK_EXT (0x28),
     * one transaction, three payload bytes. No DIR_CHANGE/repeated-start. */
    if (writing) { next[2] = 0x28; next[3] = 3; next[4] = 1; next[12] = 0; }
    s->calls++;
#define R(pa, w, out) do { s->last_addr = (pa); s->last_width = (w); s->last_write = 0; \
	s->last_valid = 0; s->last_value = 0; \
	if (o->read(c, (pa), (w), (out))) { s->failure = 10; goto fail; } \
	s->last_value = *(out); s->last_valid = 1; } while (0)
#define W(pa, w, val) do { s->last_addr = (pa); s->last_width = (w); s->last_write = 1; \
	s->last_value = (val); s->last_valid = 1; s->mutated = 1; wrote = 1; \
	if (o->write(c, (pa), (w), (val))) { s->failure = 11; goto uncertain; } } while (0)
#define REFUSE(code) do { s->failure = (code); goto fail; } while (0)

	/* 1-3: clock source and timing, as reviewed in mtk_i2c_read.c:24-49
	 * (i2c_sel CLK_CFG_10 [9:8], pdn15, clk-mt6885.c:1886-1890, 1102-1106). */
	uint32_t mux = 0, mux_after = 0, parent = 26000000;
	R(0x100000b0, 4, &mux);
	s->mux = mux;
	if (mux & 0x8000) { REFUSE(1); }
	unsigned select = (mux >> 8) & 3;
	if (select == 3) { REFUSE(1); }
	s->stage = 2;
	if (select) {
		uint32_t pll = select == 1 ? 0x1000c340 : 0x1000c308, con = 0, pcw = 0, again = 0, power = 0;
		R(pll, 4, &con);
		R(pll + 4, 4, &pcw);
		R(pll + 12, 4, &power);
		R(pll + 4, 4, &again);
		if (pcw != again || !(con & 1) || !(con & 0x800000) || (power & 3) != 1) { REFUSE(2); }
		unsigned div = 1u << ((pcw >> 24) & 7);
		uint64_t hz = ((uint64_t)26000000 * (pcw & 0x3fffff) + 16383) >> 14;
		hz = (hz + div - 1) / div;
		hz /= select == 1 ? 32 : 20;
		if (hz < 1000000 || hz > 250000000) { REFUSE(2); }
		parent = (uint32_t)hz;
	}
	s->parent_hz = parent;
	R(0x100000b0, 4, &mux_after);
	if (mux != mux_after) { REFUSE(1); }
	s->stage = 3;
	unsigned hclk = (parent / 5) / 1000, min_div = ((hclk >> 1) + 99) / 100, best = 512, sample = 0, step = 0;
	for (unsigned a = 1; a <= 8; a++) {
		unsigned b = (min_div + a - 1) / a;
		if (!b || b > 64) { continue; }
		if (a * b < best) {
			best = a * b;
			sample = a;
			step = b;
			if (best == min_div) { break; }
		}
	}
	if (!sample || !step || hclk / (2 * sample * step) > 100) { REFUSE(3); }
	sample--;
	if (sample) { step--; }
	next[6] = (sample << 8) | step | 1; /* I2C_TIMEOUT_EN, i2c-mtk.c:1051 */
	next[8] = (sample << 6) | step;
	/* Same HW timeout value as i2c-mtk.c:963-977: 2 ms at hclk, in ltiming
	 * step units. The source writes it on every V2 transfer. */
	unsigned tv2 = ((next[8] & 0x3f) + 1) * (((next[8] & 0x1c0) >> 6) + 1);
	next[14] = ((hclk * 2) + tv2 - 1) / tv2;

	/* 4: GPIO122/123 must already be SCL8/SDA8 (pinctrl-mtk-mt6885.h:1283-1295). */
	s->stage = 4;
	R(0x100053f0, 4, &v);
	s->pinmux = v;
	if ((v & 0xff00) != 0x1100) { REFUSE(4); }
 /* Observe only existing input-enable state; never enable pads to claim idle. */
 R(0x11d10070,4,&v);if((v&0x2008u)!=0x2008u){REFUSE(20);}
 R(0x10005230,4,&v);if((v&0x0c000000u)!=0x0c000000u){REFUSE(20);}

	/* 5: the CCU master needs CAM MTCMOS (DT ccu clocks CAM_PWR = SCP_SYS_CAM;
	 * CAM_PWR_STA_MASK bit23, clk-mt6885-pg.c:565). */
	s->stage = 5;
	R(0x1000616c, 4, &s->pwr);
	R(0x10006170, 4, &s->pwr2);
	if ((s->pwr | s->pwr2) & (1u << 23)) { REFUSE(16); }

	/* 6: never ungate an unknown controller to inspect it. */
	s->stage = 6;
	R(NA8_GATE + 0xe00, 4, &s->gate);
	if (s->gate & NA8_GATE_BIT) { REFUSE(5); }

	/* 7: APDMA (ap_dma_ps IFRAO1 bit18, apdma IFRAO2 bit31; clk-mt6885.c:2115-2125,
	 * 2304-2305, 2317-2318). Registers only read when both clocks are ungated. */
	s->stage = 7;
	R(0x10001094, 4, &s->ifr1);
	R(0x100010d8, 4, &s->ifr2);
	if (!(s->ifr1 & (1u << 18)) && !(s->ifr2 & (1u << 31))) {
		R(NA8_DMA + 0x08, 4, &s->dma_en); /* OFFSET_EN, i2c-mtk.h:148 */
		s->dma_checked = 1;
		if (s->dma_en) { REFUSE(7); }
	}

	/* 8: all three channels idle; START bit0 TRANSAC_START, bit1 RESUME_ARBIT
	 * (i2c-mtk.h:55-56, i2c-mtk.c:1253-1256). The source reads bit15 I3C_EN
	 * from V2 DMA_FSM_DEBUG at 0xec, not DEBUGSTAT at 0xe4
	 * (i2c-mtk.h:275,295; i2c-mtk.c:1040). Refuse that separate mode. */
	s->stage = 8;
	R(NA8_SH + 0x24, 2, &s->sh_start);
	if (s->sh_start & 3) { REFUSE(18); }
	R(NA8_CCU + 0x24, 2, &s->ccu_start);
	if (s->ccu_start & 3) { REFUSE(17); }
	R(NA8_CCU + 0x10, 2, &s->ccu_ctrl);
	if (s->ccu_ctrl & 4) { REFUSE(17); }
 R(NA8_SH+0x10,2,&v);if(v&4){REFUSE(7);}
 R(NA8_SH+0xf4,2,&v);if(v){REFUSE(9);}
 R(NA8_CCU+0xf4,2,&v);if(v){REFUSE(9);}
	R(NA8_CH + 0x24, 2, &v);
	if (v & 3) { REFUSE(6); }
	R(NA8_CH + 0x10, 2, &v);
	if (v & 4) { REFUSE(7); } /* I2C_CONTROL_DMA_EN */
	R(NA8_CH + 0x0c, 2, &v);
	s->prior_irq = v;
	if (v != 0 && v != 1) { REFUSE(8); }
	R(NA8_CH + 0xf4, 2, &v);
	if (v) { REFUSE(9); }
	R(NA8_CH + 0xec, 2, &s->i3c);
	if (s->i3c & NA8_I3C_EN) { REFUSE(21); }
	if (s->prior_irq == 1) {
		/* Same exact-completion adoption as the reviewed battery handoff;
		 * every idle condition is re-read before the single W1C. */
		R(NA8_SH + 0x24, 2, &v);
		if (v & 3) { REFUSE(18); }
		R(NA8_CCU + 0x24, 2, &v);
		if (v & 3) { REFUSE(17); }
		R(NA8_CH + 0x24, 2, &v);
		if (v & 3) { REFUSE(6); }
		R(NA8_CH + 0x10, 2, &v);
		if (v & 4) { REFUSE(7); }
		R(NA8_CH + 0xf4, 2, &v);
		if (v) { REFUSE(9); }
		R(NA8_CH + 0x0c, 2, &v);
		if (v != 1) { REFUSE(8); }
		W(NA8_CH + 0x0c, 2, 1);
		s->prior_ack = 1;
		R(NA8_CH + 0x0c, 2, &v);
		if (v) { s->failure = 8; goto uncertain; }
	}

	s->stage = 9;
	for (unsigned i = 0; i < 15; i++) { R(NA8_CH + offs[i], 2, &old[i]); }
	/* Source clears the multi-channel FIFO before programming a new transfer
	 * (i2c-mtk.c:1071-1077). */
	W(NA8_CH + 0x38, 2, 5); /* FIFO_ADDR_CLR_MCH|FIFO_ADDR_CLR */
	s->stage = 10;
	for (unsigned i = 0; i < 15; i++) {
		touched = i + 1;
		W(NA8_CH + offs[i], 2, next[i]);
	}
	s->stage = 11;
	W(NA8_CH + 0x00, 1, reg);
    if (writing) {
        W(NA8_CH + 0x00, 1, (unsigned)(*value >> 8));
        W(NA8_CH + 0x00, 1, (unsigned)(*value & 255));
    }
	s->inflight = 1;
	W(NA8_CH + 0x24, 2, 1);
	s->stage = 12;
	unsigned poll_limit=s->poll_limit && s->poll_limit<2000 ? s->poll_limit:2000;
	for (unsigned i = 0; i < poll_limit; i++) {
		R(NA8_CH + 0x0c, 2, &v);
		s->polls++;
		s->completion_irq = v;
		if (v & 0x1ff) { break; }
		if (o->delay_us(c, 10)) { s->failure = 12; goto uncertain; }
	}
	/* Exact completion only. ACKERR is reported with TRANSAC_COMP on a
	 * multi-channel bus (i2c-mtk.c:1326-1330). A completed transfer with only
	 * ACKERR is the source's bounded multi-channel NACK path, which returns
	 * -EREMOTEIO without init_hw/RESUME_ARBIT; it is handled below. Every
	 * other non-clean status is uncertain and is never repaired here. */
	if (v == 3) {
		s->stage = 13;
		R(NA8_CH + 0x24, 2, &v);
		if (v & 3) { s->failure = 6; goto uncertain; }
		s->inflight = 0;
		s->nack = 1; /* also selects the binding's W1C allowlist */
		s->nacks++;
		W(NA8_CH + 0x0c, 2, 3); /* clear exactly our completion + ACKERR */
		R(NA8_CH + 0x0c, 2, &v);
		if (v) { s->failure = 8; goto uncertain; }
		W(NA8_CH + 0x38, 2, 5);
		goto restore;
	}
	if (v != 1) { s->failure = v ? 14 : 13; goto uncertain; }
	s->stage = 13;
	R(NA8_CH + 0x24, 2, &v);
	if (v & 3) { s->failure = 6; goto uncertain; }
	if (!writing) {
	uint32_t hi = 0, lo = 0;
	R(NA8_CH + 0x00, 1, &hi);
	R(NA8_CH + 0x00, 1, &lo);
	if (hi > 255 || lo > 255) { s->failure = 15; goto uncertain; }
	*value = (uint16_t)((hi << 8) | lo);
    }
	s->inflight = 0;
	s->stage = 14;
	W(NA8_CH + 0x0c, 2, 1);
	W(NA8_CH + 0x38, 2, 5);
restore:
	s->stage = 15;
	s->restoring = 1;
	while (touched) {
		unsigned i = --touched;
		W(NA8_CH + offs[i], 2, old[i]);
	}
	s->restoring = 0;
	s->stage = 16;
	s->error = 0;
	return s->nack ? 1 : 0;
fail:
	/* A read failure or refusal. Before any write in this call the
	 * controller is as an earlier clean call (or boot) left it; after one,
	 * the outcome is uncertain. */
	s->offline = 1;
	if (wrote) { s->uncertain = 1; s->error = -2; return -2; }
	s->error = -1;
	return -1;
uncertain:
	/* A failed write may still have committed: keep setup, no cleanup. */
	s->offline = 1;
	s->uncertain = 1;
	s->error = -2;
	return -2;
#undef R
#undef W
#undef REFUSE
}

int na8_read16(const struct na8_io *o, void *c, struct na8_state *s,
    unsigned slave, unsigned reg, uint16_t *value)
{
 int rc=na8_transfer16(o,c,s,slave,reg,value,0);
 if (s) {
  s->prior_amp_valid=rc==0;
  if (!rc) { s->prior_amp_slave=slave; s->prior_amp_reg=reg; s->prior_amp_value=*value; }
 }
 return rc;
}
int na8_write16(const struct na8_io *o, void *c, struct na8_state *s,
    unsigned slave, unsigned reg, uint16_t value)
{
 if (!s || s->offline || s->inflight) return -1;
 /* Every runtime/profile RMW must preserve unrelated bits of the immediately
  * preceding same-slave/register read. Silicon errata and volatile key
  * writes are separate exact-value allowlisted source operations. */
 int allowed=na8_silicon_value(reg,value);
 if (!allowed && s->prior_amp_valid && s->prior_amp_slave==slave && s->prior_amp_reg==reg) {
  uint16_t old=s->prior_amp_value;
  allowed=nah_board_change_allowed(reg,old,value) ||
   (reg==0x00 && !((old^value)&~9u)) ||
   (reg==0x01 && !((old^value)&~4u)) ||
   (reg==0x02 && !((old^value)&~15u) && (value&15)==8) ||
   (reg==0x04 && !((old^value)&~256u) && !(value&256));
 }
 s->prior_amp_valid=0;
 if (!allowed) { s->failure=19; return -1; }
 return na8_transfer16(o,c,s,slave,reg,&value,1);
}

unsigned
na8_classify(uint16_t rev)
{
	/* Exact revisions with init tables: tfa_init.c:903-961 (9874), 807-847 (9873). */
	static const uint16_t exact[] = {0x0a74, 0x0b74, 0x0c74, 0x0a73, 0x0b73, 0x1a73};
	/* Low bytes dispatched by the selected probe, tfa98xx.c:5076-5143. */
	static const uint8_t family[] = {0x72, 0x73, 0x74, 0x88, 0x13, 0x94, 0x80, 0x81,
	    0x92, 0x12, 0x97, 0x96};
	for (unsigned i = 0; i < sizeof(exact) / sizeof(exact[0]); i++) {
		if (rev == exact[i]) { return NA8_SUPPORTED; }
	}
	unsigned low = rev & 0xff;
	if (low == 0x73 || low == 0x74) { return NA8_UNLISTED_REV; }
	for (unsigned i = 0; i < sizeof(family); i++) {
		if (low == family[i]) { return NA8_OTHER_FAMILY; }
	}
	return NA8_UNKNOWN;
}

int
na8_probe(const struct na8_io *o, void *c, struct na8_state *s, struct na8_amp amp[2])
{
	if (!s || !amp) { return -1; }
	for (unsigned i = 0; i < 2; i++) {
		amp[i] = (struct na8_amp){0};
		amp[i].addr = na8_slave[i];
	}
	for (unsigned i = 0; i < 2; i++) {
		struct na8_amp *a = &amp[i];
		uint16_t v = 0;
		a->rc = na8_read16(o, c, s, a->addr, 0x03, &v);
		if (a->rc == 1) {
			/* Clean NACK: no REV value, no further pointer on this amp. The
			 * controller is idle and its config restored, so the other
			 * amplifier may still be identified. */
			a->cls = NA8_NACK;
			continue;
		}
		if (a->rc) { a->cls = NA8_FAILED; return a->rc; }
		a->reads = 1;
		a->rev = v;
		a->cls = na8_classify(v);
		if (a->cls != NA8_SUPPORTED) { continue; } /* raw REV kept, nothing else read */
		/* PWDN 0x0000, AMPE 0x0030, MANSTATE 0x1433 in tfa9874_tfafieldnames.h,
		 * tfa9873_tfafieldnames.h and tfa9873_tfafieldnames_B0.h alike. */
		a->rc = na8_read16(o, c, s, a->addr, 0x00, &a->sys0);
		if (a->rc == 1) { a->cls = NA8_PARTIAL; continue; } /* clean, amp-local */
		if (a->rc) { a->cls = NA8_FAILED; return a->rc; }
		a->reads = 2;
		a->rc = na8_read16(o, c, s, a->addr, 0x14, &a->status14);
		if (a->rc == 1) { a->cls = NA8_PARTIAL; continue; }
		if (a->rc) { a->cls = NA8_FAILED; return a->rc; }
		a->reads = 3;
		a->pwdn = a->sys0 & 1;
		a->ampe = (a->sys0 >> 3) & 1;
		a->manstate = (a->status14 >> 3) & 0xf;
	}
	return 0;
}
#endif /* NORD2 */
