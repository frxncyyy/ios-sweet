#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "audio_hw.h"

static struct naf_amp_evidence amps[2] = {{1,1,1,0,0},{1,1,1,0,0}};

static void cold(struct nau_state *s)
{
	memset(s, 0, sizeof(*s));
	s->snap_read = NAU_SNAP_COUNT;
	s->snap[NAU_S_SPM_POWERON] = (NAF_SPM_PROJECT_CODE << 16) | 1u;
	s->snap[NAU_S_AUDIO_PWR_CON] = 0x1112;
	s->snap[NAU_S_ADSP_PWR_CON] = 0x32;
	s->snap[NAU_S_IFR1_CG] = 1u << 25;
	s->snap[NAU_S_IFR3_CG] = 1u << 4;
	s->snap[NAU_S_PROT2_STA1] = 1u << 4;
	s->snap[NAU_S_CLK_CFG_7] = 1u << 31;
	s->snap[NAU_S_CLK_CFG_8] = 1u << 7;
	s->snap[NAU_S_CLK_CFG_12] = 1u << 7;
	s->snap[NAU_S_CLK_CFG_13] = 1u << 7;
	s->snap[NAU_S_APLL2_CON4] = 2;
	uint32_t code = 0;
	s->snap[NAU_S_APLL2_CON2] = naf_pll_pcw(NAF_APLL2_HZ, &code);
	s->snap[NAU_S_APLL2_CON1] = code << 24;
	s->snap[NAU_S_APLL2_TUNER] = s->snap[NAU_S_APLL2_CON2] + 1;
}

/* A register-only model of the selected set/clear aliases, PG status/ack and
 * DL1 pointer. It does not pretend to model clocks or analog audio. */
struct reg { uint32_t pa, v; };
struct cycle_model {
	struct reg r[256];
	unsigned n, reads, writes, fail_write, cur_reads, sram_writes, stuck_cur;
 uint32_t current, sram[NAH_RING_FRAMES]; uintptr_t owner;
};
static uint32_t *regp(struct cycle_model *m, uint32_t pa)
{
	for (unsigned i = 0; i < m->n; i++) if (m->r[i].pa == pa) return &m->r[i].v;
	assert(m->n < 256);
	m->r[m->n] = (struct reg){pa,0};
	return &m->r[m->n++].v;
}
static void model_init(struct cycle_model *m, const struct nau_state *s)
{
	memset(m,0,sizeof(*m));
	for (unsigned i = 0; i < NAU_SNAP_COUNT; i++)
		*regp(m,nau_snapshot_pa[i]) = s->snap[i];
}
static int model_read(void *p, uint32_t pa, uint32_t *v)
{
	struct cycle_model *m=p;
	m->reads++;
	if (pa==NAF_AFE_DL1_CUR && (*regp(m,NAF_AFE_DAC_CON0)&(1u<<4))) {
		*v=m->current;
	} else *v=*regp(m,pa);
	return 0;
}
static int model_write(void *p, uint32_t pa, uint32_t v)
{
	struct cycle_model *m=p;
	assert(naf_write_allowed(pa));
	m->writes++;
	if (m->fail_write && m->writes==m->fail_write) return -1;
	if (pa>=NAF_SRAM && pa<NAF_SRAM+NAF_SRAM_BYTES) {
		assert(!(pa&3u));
        m->sram[(pa-NAF_SRAM)/4]=v;
		m->sram_writes++;
		return 0;
	}
	if (pa==NAF_IFR_CG1_CLR) *regp(m,NAF_IFR_CG1_STA)&=~v;
	else if (pa==NAF_IFR_CG1_SET) *regp(m,NAF_IFR_CG1_STA)|=v;
	else if (pa==NAF_IFR_CG3_CLR) *regp(m,NAF_IFR_CG3_STA)&=~v;
	else if (pa==NAF_IFR_CG3_SET) *regp(m,NAF_IFR_CG3_STA)|=v;
	else if (pa==NAF_IFR_PROT2_CLR) *regp(m,NAF_IFR_PROT2_STA1)&=~v;
	else if (pa==NAF_IFR_PROT2_SET) *regp(m,NAF_IFR_PROT2_STA1)|=v;
	else if (pa==NAF_TOP_UPDATE1) { /* write-only trigger */ }
	else if (pa==NAF_TOP_SET(NAF_TOP_CFG7)||pa==NAF_TOP_SET(NAF_TOP_CFG8)||
	         pa==NAF_TOP_SET(NAF_TOP_CFG12)||pa==NAF_TOP_SET(NAF_TOP_CFG13))
		*regp(m,pa-4)|=v;
	else if (pa==NAF_TOP_CLR(NAF_TOP_CFG7)||pa==NAF_TOP_CLR(NAF_TOP_CFG8)||
	         pa==NAF_TOP_CLR(NAF_TOP_CFG12)||pa==NAF_TOP_CLR(NAF_TOP_CFG13))
		*regp(m,pa-8)&=~v;
	else if (pa==NAF_SPM_AUDIO_PWR) {
		*regp(m,pa)=(v&~(1u<<12))|((v&(1u<<8))?1u<<12:0);
		uint32_t *a=regp(m,NAF_SPM_PWR_STATUS), *b=regp(m,NAF_SPM_PWR_STATUS2);
		if ((v&0xcu)==0xcu) { *a|=1u<<21; *b|=1u<<21; }
		else { *a&=~(1u<<21); *b&=~(1u<<21); }
	} else *regp(m,pa)=v;
	return 0;
}
static int model_delay(void *p, unsigned us) { (void)p; assert(us<=1000); return 0; }


static int width_read(void *c,uint32_t pa,unsigned w,uint32_t *v)
{ assert(w==4); return model_read(c,pa,v); }
static int width_write(void *c,uint32_t pa,unsigned w,uint32_t v)
{ assert(w==4); return model_write(c,pa,v); }
static uintptr_t identity(void *c) { return ((struct cycle_model *)c)->owner; }
static int claim(void *c,unsigned acquire) { (void)c;(void)acquire;return 0; }
static struct nau_hw hw;
static struct cycle_model cm;
static void setup(void)
{
 memset(&hw,0,sizeof(hw)); cold(&hw.snapshot);
 /* Cold APLL2 can contain any rate; implementation must program and restore. */
 hw.snapshot.snap[NAU_S_APLL2_CON1]=0x05123456;
 hw.snapshot.snap[NAU_S_APLL2_CON2]=0x12345678;
 hw.snapshot.snap[NAU_S_APLL2_TUNER]=0x87654321;
 model_init(&cm,&hw.snapshot); cm.owner=77; cm.current=NAF_SRAM;
 hw.owner=77; hw.cookie=&cm;
 hw.io=(struct nah_io){width_read,width_write,model_delay,identity,claim};
 hw.status.initialized=1;
 hw.evidence[0]=amps[0]; hw.evidence[1]=amps[1];
}
#ifndef NAH_TEST_FULL
int main(void)
{
 setup();
 assert(nau_hw_start(&hw,48000,2,16,0,256,2000000,100)==NAH_E_BOARD_CONFIG);
 assert(cm.writes==0 && cm.reads==0);
 assert(nau_hw_start(&hw,44100,2,16,1,256,2000000,100)==NAH_E_ARG);
 assert(cm.writes==0);
 assert(nau_hw_start(&hw,48000,2,16,1,257,2000000,100)==NAH_E_ARG);
 assert(nau_hw_start(&hw,48000,2,16,1,256,2000000,100)==0);
 assert(nau_hw_tick(&hw,100)==0);
 uint32_t code=0,pcw=naf_pll_pcw(196608000,&code);
 assert(*regp(&cm,NAF_APM_APLL2_CON2)==pcw);
 assert((*regp(&cm,NAF_APM_APLL2_CON1)>>24&7)==code);
 assert(*regp(&cm,NAF_APM_APLL2_TUNER)==pcw+1);
 assert(*regp(&cm,NAF_AFE_I2S_CON3)&1);
 assert(cm.sram_writes==1024);
 /* Feed extreme stereo PCM, wrap four times and verify bounded scaling. */
 hw.status.silent_only=0; /* Test refill independently of missing board. */
 int16_t pcm[480]={0};
 for (unsigned i=0;i<240;i++) { pcm[2*i]=32767;pcm[2*i+1]=-32768; }
 for (unsigned i=0;i<3;i++) {
  assert(nau_hw_feed(&hw,pcm,240)==240);
  cm.current=NAF_SRAM+4*((i+1)*240%1024);
  assert(nau_hw_tick(&hw,5100+5000*i)==0);
 }
 for (unsigned i=0;i<720;i++) assert(cm.sram[i]==0xff0000ffu);
 hw.status.silent_only=1; /* Avoid amp monitoring in isolated refill model. */
 cm.current=NAF_SRAM+4*960; assert(nau_hw_tick(&hw,20100)==0);
 cm.current=NAF_SRAM+4*176; assert(nau_hw_tick(&hw,25100)==0);
 assert(hw.status.consumed_frames==1200 && hw.status.pcm_frames==720);
 /* Wrapped consumed words overwritten with zeros, stale PCM cannot replay. */
 for (unsigned i=0;i<176;i++) assert(cm.sram[i]==0);
 assert(hw.status.silence_frames==480 && hw.status.underruns==2);
 unsigned calls=cm.reads+cm.writes; cm.owner=99;
 assert(nau_hw_tick(&hw,30100)==NAH_E_WORKER);
 assert(cm.reads+cm.writes==calls); cm.owner=77;
 assert(nau_hw_stop(&hw)==0 && !hw.afe.active && !hw.status.uncertain);
 assert(*regp(&cm,NAF_APM_APLL2_CON2)==0x12345678);
 assert(*regp(&cm,NAF_APM_APLL2_TUNER)==0x87654321);
 assert((*regp(&cm,NAF_APM_APLL2_CON1)&0x7fffffff)==0x05123456);
 setup(); assert(nau_hw_start(&hw,48000,2,16,1,1,2000000,0)==0);
 assert(nau_hw_tick(&hw,0)==0);
 cm.current=NAF_SRAM-4; assert(nau_hw_tick(&hw,5000)==NAH_E_POINTER);
 assert(!hw.status.running && !hw.status.uncertain);
 setup(); assert(nau_hw_start(&hw,48000,2,16,1,1,2000000,0)==0);
 assert(nau_hw_tick(&hw,0)==0);
 assert(nau_hw_tick(&hw,20000)==NAH_E_DEADLINE); assert(!hw.status.running);
 setup(); assert(nau_hw_start(&hw,48000,2,16,1,1,2000000,0)==0);
 assert(nau_hw_tick(&hw,0)==0);
 for (unsigned i=1;i<=2;i++) assert(nau_hw_tick(&hw,i*5000)==0);
 assert(nau_hw_tick(&hw,15000)==NAH_E_CLOCK_STALLED); assert(!hw.status.running);
 setup(); cm.fail_write=1;
 assert(nau_hw_start(&hw,48000,2,16,1,1,2000000,0)==NAH_E_UNCERTAIN);
 calls=cm.reads+cm.writes;
 assert(nau_hw_start(&hw,48000,2,16,1,1,2000000,0)==NAH_E_ARG);
 assert(cm.reads+cm.writes==calls);
 puts("PASS: continuous DMA model, cold PLL programming/restore, stereo scaling, wrap, silence, worker confinement, deadline/pointer/stall and failure isolation");
}

#endif
