/* Print the naf_build programmes for fixed snapshots. Uses only the API that
 * exists in BOTH canonical 66fc36e and candidate audio_afe.{c,h}; build it
 * against each and diff: the two cold fixtures must be identical (cold path
 * unchanged), the real snapshot is refused by canonical (gate f8). */
#include <stdio.h>
#include <string.h>
#include "audio_afe.h"
#include "actual-snapshot.inc"

/* Same words as tests/nord2-audio/test-stream.c cold() / setup(). */
static void cold(uint32_t *s, unsigned setup)
{
	memset(s, 0, NAU_SNAP_COUNT * sizeof(*s));
	s[NAU_S_SPM_POWERON] = (NAF_SPM_PROJECT_CODE << 16) | 1u;
	s[NAU_S_AUDIO_PWR_CON] = 0x1112;
	s[NAU_S_ADSP_PWR_CON] = 0x32;
	s[NAU_S_IFR1_CG] = 1u << 25;
	s[NAU_S_IFR3_CG] = 1u << 4;
	s[NAU_S_PROT2_STA1] = 1u << 4;
	s[NAU_S_CLK_CFG_7] = 1u << 31;
	s[NAU_S_CLK_CFG_8] = 1u << 7;
	s[NAU_S_CLK_CFG_12] = 1u << 7;
	s[NAU_S_CLK_CFG_13] = 1u << 7;
	s[NAU_S_APLL2_CON4] = 2;
	uint32_t code = 0;
	s[NAU_S_APLL2_CON2] = naf_pll_pcw(NAF_APLL2_HZ, &code);
	s[NAU_S_APLL2_CON1] = code << 24;
	s[NAU_S_APLL2_TUNER] = s[NAU_S_APLL2_CON2] + 1;
	if (setup) {
		s[NAU_S_APLL2_CON1] = 0x05123456;
		s[NAU_S_APLL2_CON2] = 0x12345678;
		s[NAU_S_APLL2_TUNER] = 0x87654321;
		s[NAU_S_MODE_32] = 0x1011;
		s[NAU_S_DIR_32] = 0xb;
	}
}

static void dump(const char *name, const uint32_t *w)
{
	static struct nau_state st;
	static struct naf_plan p;
	static const struct naf_amp_evidence amps[2] = {{1,1,1,0,0},{1,1,1,0,0}};
	memset(&st, 0, sizeof st);
	st.snap_read = NAU_SNAP_COUNT;
	memcpy(st.snap, w, sizeof st.snap);
	unsigned g = naf_gate(&st, amps);
	if (naf_build(&st, amps, &p)) { printf("%s refused gate=%x\n", name, g); return; }
	printf("%s gate=%x up=%u down=%u\n", name, g, p.n_up, p.n_down);
	for (unsigned l = 0; l < 2; l++) {
		const struct naf_op *ops = l ? p.down : p.up;
		unsigned n = l ? p.n_down : p.n_up;
		for (unsigned i = 0; i < n; i++)
			printf("%s %s %3u kind%u ph%02u pa%08x mask%08x value%08x arg%u step%u\n",
			    name, l ? "down" : "up", i, (unsigned)ops[i].kind, (unsigned)ops[i].phase,
			    (unsigned)ops[i].pa, (unsigned)ops[i].mask, (unsigned)ops[i].value,
			    (unsigned)ops[i].arg, (unsigned)ops[i].step_us);
	}
}

int main(void)
{
	uint32_t w[NAU_SNAP_COUNT];
	cold(w, 0); dump("cold", w);
	cold(w, 1); dump("cold-setup", w);
	dump("actual", actual_snapshot);
	return 0;
}
