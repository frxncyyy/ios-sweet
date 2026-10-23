/* Register-level model of the Nord2 I2C8 multi-channel I2Cv2 controller and
 * its slaves, for host tests of audio_i2c8.c and audio_id_bind.inc.
 * Channels: 0 shadow 0x11d04000, 1 AP 0x11d04100, 2 CCU 0x11d04200.
 * Any access outside the modelled set aborts the test (no silent default).
 * Slaves: TFA98xx 0x34/0x37 and the other I2C8 devices 0x2f/0x35/0x42/0x51/
 * 0x56 (wl2868c, fan53870, fsa4480, DDR buck, UFS buck). A transaction can
 * only be started by an AP-channel START write; each one is logged. A slave
 * marked nack, or an absent slave, completes with TRANSAC_COMP|ACKERR and
 * supplies no data byte. */
#ifndef NORD2_AUDIO_I2C8_MODEL_H
#define NORD2_AUDIO_I2C8_MODEL_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define M_CHECK(c) do { if (!(c)) { fprintf(stderr, "%s:%d MODEL %s\n", __FILE__, __LINE__, #c); abort(); } } while (0)
#define M_SLAVES 7

enum { M_NORMAL, M_HANG, M_BUSERR, M_SLOW };
/* Selected V2 map: DEBUGSTAT is 0xe4; I3C_EN is read by the driver from
 * DMA_FSM_DEBUG at 0xec (i2c-mtk.h:275,295; i2c-mtk.c:1040). */
enum { M_V2_DEBUGSTAT = 0xe4, M_V2_DMA_FSM_DEBUG = 0xec };

struct m_slave {
	uint8_t addr;
	int present;
	int nack;
	uint16_t reg[256];
	unsigned accessed;
};

struct i2c8_model {
	uint16_t ch[3][0x80];            /* 16-bit registers, index = offset / 2 */
	uint32_t pads,ies;
	uint32_t mux, mainpll[4], univpll[4], pinmux, pwr, pwr2, gate, ifr1, ifr2;
	uint32_t dma[8];                 /* APDMA 0x11000780.. by offset / 4 */
	unsigned hclk;                   /* parent/div/1000, default 26 MHz/5 */
	struct m_slave slave[M_SLAVES];
	uint8_t tx[8], rx[8];
	unsigned ntx, nrx, rxpos;
	unsigned behaviour, slow_polls, polls, noclk_persistent;
	unsigned irq_sticky;             /* INTR_STAT W1C has no effect (fault) */
	unsigned start_stuck;            /* completion leaves transaction bit0 set */
	unsigned starts, ntrans, dma_en_reads, gate_reads;
	unsigned setup_hw_timeout;       /* last programmed 0x4c, before restore */
	uint16_t saved[15];              /* inherited AP config, for restore check */
	struct { uint8_t addr, ptr; } trans[4096];
};

static inline void
m_init(struct i2c8_model *m)
{
	memset(m, 0, sizeof(*m));
	m->pads=0x0c000000u;m->ies=0x2008u;
	m->pinmux = 0x21111111;          /* GPIO122/123 fn1 (bits 8..15 = 0x11) */
	m->ifr1 = 1u << 18;              /* ap_dma_ps gated: APDMA not readable */
	m->hclk = 5200;                  /* 26 MHz parent, DT clock-div 5 */
	m->mainpll[0] = 0x800001;        /* en + rstb */
	m->mainpll[1] = 0x82000000u | 0x150000; /* postdiv field and pcw */
	m->mainpll[3] = 1;
	m->univpll[0] = 0x800001;
	m->univpll[1] = 0x82000000u | 0x180000;
	m->univpll[3] = 1;
	static const uint8_t addr[M_SLAVES] = {0x34, 0x37, 0x2f, 0x35, 0x42, 0x51, 0x56};
	for (unsigned i = 0; i < M_SLAVES; i++) {
		m->slave[i].addr = addr[i];
		m->slave[i].present = 1;
		for (unsigned r = 0; r < 256; r++) { m->slave[i].reg[r] = 0xdead; }
	}
	m->slave[0].reg[0x03] = 0x0b74;
	m->slave[0].reg[0x00] = 0x0001;
	m->slave[0].reg[0x14] = 0x0000;
	m->slave[1].reg[0x03] = 0x0a74;
	m->slave[1].reg[0x00] = 0x0019;
	m->slave[1].reg[0x14] = 0x0048;
	/* Inherited AP-channel configuration that must be restored exactly
	 * (the 14 source transfer words plus V2 HW_TIMEOUT 0x4c). */
	static const uint8_t cfg[15] = {4, 8, 0x10, 0x14, 0x18, 0x1c, 0x20, 0x28, 0x2c,
	    0x30, 0x34, 0x40, 0x44, 0x48, 0x4c};
	for (unsigned i = 0; i < 15; i++) { m->ch[1][cfg[i] / 2] = (uint16_t)(0x1100 + i); }
	m->ch[1][0x10 / 2] = 0x0038;     /* CONTROL without DMA_EN */
	for (unsigned i = 0; i < 15; i++) { m->saved[i] = m->ch[1][cfg[i] / 2]; }
}

/* The transport must restore every saved AP-channel word exactly. */
static inline void
m_check_restored(const struct i2c8_model *m)
{
	static const uint8_t cfg[15] = {4, 8, 0x10, 0x14, 0x18, 0x1c, 0x20, 0x28, 0x2c,
	    0x30, 0x34, 0x40, 0x44, 0x48, 0x4c};
	for (unsigned i = 0; i < 15; i++) {
		M_CHECK(m->ch[1][cfg[i] / 2] == m->saved[i]);
	}
}

static inline int
m_is_config(unsigned off)
{
	switch (off) {
	case 4: case 8: case 0x10: case 0x14: case 0x18: case 0x1c: case 0x20: case 0x28:
	case 0x2c: case 0x30: case 0x34: case 0x40: case 0x44: case 0x48: case 0x4c:
		return 1;
	default:
		return 0;
	}
}

static inline void
m_start(struct i2c8_model *m)
{
	uint16_t *c = m->ch[1];
	m->starts++;
	/* START is a command, not an ordinary config register. Model the real
	 * AP readback2: launching with literal1 sets busybit0 while bit1 can
	 * persist; completion clears only bit0. No write of RESUME is allowed. */
	c[0x24 / 2] = (uint16_t)((c[0x24 / 2] & 2u) | 1u);
	/* The transport's exact WRRD programme (i2c-mtk.c:963-1213 derivative),
	 * including the source V2 HW timeout computed from LTIMING. */
	unsigned tv2 = ((c[0x2c / 2] & 0x3f) + 1) *
	    (((c[0x2c / 2] & 0x1c0) >> 6) + 1);
	unsigned writing=c[0x10/2]==0x28;
    M_CHECK(c[0x08 / 2] == 0 && (writing || c[0x10/2]==0x3a) &&
        c[0x14/2]==(writing?3:1) && c[0x18/2]==(writing?1:2) &&
        c[0x44/2]==(writing?0:2) && c[0x34 / 2] == 3 &&
	    c[0x48 / 2] == 0x404 && c[0x28 / 2] == 0x8001 && c[0x30 / 2] == 0 &&
	    c[0x40 / 2] == 1 && c[0x1c / 2] == 10 && (c[0x20 / 2] & 1) &&
	    c[0x4c / 2] == (uint16_t)((m->hclk * 2 + tv2 - 1) / tv2));
	m->setup_hw_timeout = c[0x4c / 2];
	M_CHECK(m->ntx == (writing?3:1) && m->nrx == 0);
	if (m->behaviour == M_HANG) { return; }
	uint8_t addr = (uint8_t)(c[0x04 / 2] >> 1), ptr = m->tx[0];
	M_CHECK(m->ntrans < 4096);
	m->trans[m->ntrans].addr = addr;
	m->trans[m->ntrans].ptr = ptr;
	m->ntrans++;
	m->ntx = 0;
	if (m->behaviour == M_BUSERR) { c[0x0c / 2] = 0x101; return; }
	if (!m->start_stuck) c[0x24 / 2] &= (uint16_t)~1u;
	struct m_slave *s = NULL;
	for (unsigned i = 0; i < M_SLAVES; i++) {
		if (m->slave[i].addr == addr && m->slave[i].present) { s = &m->slave[i]; }
	}
	if (!s || s->nack) { /* clean NACK, no data */
		c[0x0c / 2] = 0x3;
		return;
	}
	s->accessed++;
	if (writing) {
        s->reg[ptr]=(uint16_t)((m->tx[1]<<8)|m->tx[2]);
        c[0x0c/2]=1; return;
    }
    if (ptr==0x10 && m->noclk_persistent) s->reg[ptr]|=(s->reg[3]&255)==0x74 ? 0x80:0x100;
    m->rx[0] = (uint8_t)(s->reg[ptr] >> 8);
	m->rx[1] = (uint8_t)s->reg[ptr];
 /* Selected tfa98xx.c956 and tfa_dsp.c4654: NOCLK is clear-on-read. */
 if (ptr==0x10) s->reg[ptr]&=(uint16_t)~((s->reg[3]&255)==0x74 ? 0x80:0x100);
	m->nrx = 2;
	m->rxpos = 0;
	if (m->behaviour == M_SLOW) { m->polls = 0; c[0x24 / 2] |= 1u; return; }
	c[0x0c / 2] = 1;
}

static inline uint32_t
m_read(struct i2c8_model *m, uint32_t pa, unsigned w)
{
	switch (pa) {
	case 0x100000b0: M_CHECK(w == 4); return m->mux;
	case 0x1000c340: case 0x1000c344: case 0x1000c348: case 0x1000c34c:
		M_CHECK(w == 4); return m->mainpll[(pa - 0x1000c340) / 4];
	case 0x1000c308: case 0x1000c30c: case 0x1000c310: case 0x1000c314:
		M_CHECK(w == 4); return m->univpll[(pa - 0x1000c308) / 4];
	case 0x10005230: M_CHECK(w==4);return m->pads;
 case 0x11d10070:M_CHECK(w==4);return m->ies;
  case 0x100053f0: M_CHECK(w == 4); return m->pinmux;
	case 0x1000616c: M_CHECK(w == 4); return m->pwr;
	case 0x10006170: M_CHECK(w == 4); return m->pwr2;
	case 0x11d05e00: M_CHECK(w == 4); m->gate_reads++; return m->gate;
	case 0x10001094: M_CHECK(w == 4); return m->ifr1;
	case 0x100010d8: M_CHECK(w == 4); return m->ifr2;
	default: break;
	}
	if (pa >= 0x11000780 && pa < 0x110007a0) {
		/* APDMA registers are only clocked with both DMA gates open. */
		M_CHECK(w == 4 && !(m->ifr1 & (1u << 18)) && !(m->ifr2 & (1u << 31)));
		if (pa == 0x11000788) { m->dma_en_reads++; }
		return m->dma[(pa - 0x11000780) / 4];
	}
	M_CHECK(pa >= 0x11d04000 && pa < 0x11d04300);
	M_CHECK(!(m->gate & (1u << 4)));  /* registers only while clocked */
	unsigned ch = (pa - 0x11d04000) >> 8, off = pa & 0xff;
	if (w == 1) {
		M_CHECK(ch == 1 && off == 0 && m->rxpos < m->nrx);
		return m->rx[m->rxpos++];
	}
	M_CHECK(w == 2 && !(off & 1) && off != 0);
	/* This PIO derivative has no DEBUGSTAT read; silently treating it as
	 * DMA_FSM_DEBUG would invalidate the I3C refusal test. */
	M_CHECK(ch != 1 || off != M_V2_DEBUGSTAT);
	if (ch == 1 && off == 0x0c && m->behaviour == M_SLOW && m->nrx && !m->ch[1][6]) {
		if (++m->polls >= m->slow_polls) {
			m->ch[1][6] = 1;
			if (!m->start_stuck) m->ch[1][0x24 / 2] &= (uint16_t)~1u;
		}
	}
	return m->ch[ch][off / 2];
}

static inline void
m_write(struct i2c8_model *m, uint32_t pa, unsigned w, uint32_t v)
{
	/* Only AP-channel registers are writable through the transport. */
	M_CHECK((pa & ~0xffu) == 0x11d04100);
	unsigned off = pa & 0xff;
	uint16_t *c = m->ch[1];
	if (w == 1) {
		M_CHECK(off == 0 && m->ntx < 8 && v <= 0xff);
		m->tx[m->ntx++] = (uint8_t)v;
		return;
	}
	M_CHECK(w == 2 && v <= 0xffff);
	if (off == 0x0c) { if (!m->irq_sticky) { c[6] &= (uint16_t)~v; } return; }
	if (off == 0x38) { if (v & 1) { m->ntx = m->nrx = m->rxpos = 0; } return; }
	if (off == 0x24) { M_CHECK(v == 1); m_start(m); return; }
	M_CHECK(m_is_config(off));
	c[off / 2] = (uint16_t)v;
}

/* Every logged transaction targets a TFA with an allowlisted pointer. */
static inline void
m_check_trans(const struct i2c8_model *m)
{
	for (unsigned i = 0; i < m->ntrans; i++) {
		M_CHECK(m->trans[i].addr == 0x34 || m->trans[i].addr == 0x37);
		M_CHECK(m->trans[i].ptr == 0x03 || m->trans[i].ptr == 0x00 || m->trans[i].ptr == 0x14);
	}
	for (unsigned i = 2; i < M_SLAVES; i++) { M_CHECK(!m->slave[i].accessed); }
}
#endif
