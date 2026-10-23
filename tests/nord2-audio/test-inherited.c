/* Real 8e6697a2 boot snapshot (original69 words; appended request SYNTHETIC0) through the actual
 * nau_hw_* lifecycle and actual audio_{hw,afe,id,i2c8,amp,board} code.
 * SIMULATED, never hardware proof: I2C8 shadow/CCU idle, APSTART2 as measured
 * on 7d2ca8f9 (bit1 retained through transfers), APDMA EN0. AP CONTROL28,
 * IRQ1, FIFO0 and DEBUG8800 measured on62a89dcc; TFA9874 0b74/0a74 at POR SYS_CONTROL0
 * 0x0001 with CLKS ready; a register-only SPM/infra/topckgen/AFE model
 * (test-stream.c). Cases: classify plan cycles mixed refuse-rate
 * refuse-status refuse-adsp refuse-coldmux drift-audioh drift-adsp
 * powerup-timeout protect-timeout ampe. Usage: test-inherited CASE [board]. */
#define main original_full_main
#include "test-full.c"
#undef main
#include "actual-snapshot.inc"

enum { F_NONE, F_POWERUP, F_PROTECT_STOP };
static unsigned fault, strict, afe_accesses, i2s_routes, dl1_routes, apll_writes;
static unsigned pend7, pend8, pend12, pend13;
static uint32_t base[NAU_SNAP_COUNT];

static uint32_t reg(uint32_t pa) { return *regp(&cm,pa); }
static void afe_window(uint32_t pa)
{
 if (pa<NAF_AFE || pa>=NAF_AFE+0x4000u) return;
 /* Any AFE/SRAM access: domain on and unisolated, protect released, PG
  * parents open, audio_sel/intbus on clk26m with committed updates
  * (pg.c:3314-3355, afe-clk.c:212-262, clk-mux.c:178-224). */
 assert((reg(NAF_SPM_PWR_STATUS)&(1u<<21)) && (reg(NAF_SPM_PWR_STATUS2)&(1u<<21)));
 assert((reg(NAF_SPM_AUDIO_PWR)&0x111fu)==0x000du);
 assert(!(reg(0x10001710)&(1u<<4)));
 assert(!(reg(NAF_IFR_CG1_STA)&(1u<<25)) && !(reg(NAF_IFR_CG3_STA)&(1u<<4)));
 assert(!(reg(NAF_TOP_CFG7)&((1u<<31)|(3u<<24))) && !(reg(NAF_TOP_CFG8)&((1u<<7)|3u)));
 assert(!pend7 && !pend8);
 afe_accesses++;
}
static void route_checks(uint32_t pa,uint32_t v)
{
 if (pa==NAF_AFE_I2S_CON3 && (v&1u) && !(reg(pa)&1u)) {
  /* I2S3_EN only after the complete 48k engine path: APLL2 196.608MHz live,
   * aud_2=apll2_ck, engen2=apll2_d4 (afe-clk.c:149-184,421-451), aud_24m
   * open, AFE_24M_ON, HD_EN, 48k I2S, 32-bit WLEN, pins function1. */
  assert((reg(NAF_TOP_CFG13)&0x81u)==1u && (reg(NAF_TOP_CFG12)&0x83u)==2u && !pend12 && !pend13);
  assert((reg(NAF_APM_APLL2_CON0)&1u) && (reg(NAF_APM_APLL2_CON4)&3u)==1u);
  assert(naf_pll_rate(reg(NAF_APM_APLL2_CON2),(reg(NAF_APM_APLL2_CON1)>>24)&7u)==NAF_APLL2_HZ);
  assert(!(reg(NAF_AFE_TOP_CON0)&(1u<<9)) && (reg(NAF_AFE_HD_ENGEN)&2u));
  assert((v&(1u<<12)) && (v&(1u<<3)) && (v&(1u<<1)) && ((v>>8)&15u)==NAF_RATE_48K);
  assert((reg(NAF_GPIO_MODE_32)&0xf0ffu)==0x1011u);
  i2s_routes++;
 }
 if (pa==NAF_AFE_DAC_CON0 && (v&16u) && !(reg(pa)&16u)) {
  assert((v&1u) && (reg(NAF_AFE_CONN0)&(1u<<5)) && (reg(NAF_AFE_CONN1)&(1u<<6)));
  assert(reg(NAF_AFE_DL1_BASE)==NAF_SRAM && reg(NAF_AFE_DL1_END)==NAF_SRAM+NAF_SRAM_BYTES-1u);
  assert(((reg(NAF_AFE_DL1_CON0)>>24)&15u)==NAF_RATE_48K && (reg(NAF_AFE_DL1_CON0)&(1u<<5)));
  assert(reg(NAF_AFE_I2S_CON3)&1u);
  dl1_routes++;
 }
}
static int audited_read(void *c,uint32_t pa,unsigned w,uint32_t *v)
{ afe_window(pa); return allread(c,pa,w,v); }
static int audited_write(void *c,uint32_t pa,unsigned w,uint32_t v)
{
 afe_window(pa);
 if (w==4) route_checks(pa,v);
 /* Never ADSP; never the live APLL2 power/rate/tuner or AFE tuner loop. */
 assert(pa!=0x10006358u);
 if (pa==NAF_APM_APLL2_CON0 || pa==NAF_APM_APLL2_CON1 || pa==NAF_APM_APLL2_CON2 ||
     pa==NAF_APM_APLL2_CON4 || pa==NAF_APM_APLL2_TUNER || pa==NAF_APM_AP_PLL_CON3 ||
     pa==NAF_AFE_APLL2_TCFG) apll_writes++;
 if (pa==NAF_AFE_TOP_CON0) assert(!((v^reg(pa))&(1u<<18)));
 if (pa==NAF_IFR_PROT2_SET || pa==NAF_IFR_PROT2_CLR) assert(v==(1u<<4)); /* never ADSP bit3 */
 if (strict) {
  /* Real tree: borrowed infra gates and APLL2 consumers already on their
   * playback parents get no write at all, not even transiently. */
  assert(pa!=NAF_IFR_CG1_SET && pa!=NAF_IFR_CG1_CLR && pa!=NAF_IFR_CG3_SET && pa!=NAF_IFR_CG3_CLR);
  assert(pa!=NAF_TOP_SET(NAF_TOP_CFG12) && pa!=NAF_TOP_CLR(NAF_TOP_CFG12));
  assert(pa!=NAF_TOP_SET(NAF_TOP_CFG13) && pa!=NAF_TOP_CLR(NAF_TOP_CFG13));
  if (pa==NAF_TOP_SET(NAF_TOP_CFG7) || pa==NAF_TOP_CLR(NAF_TOP_CFG7)) assert(!(v&~(3u<<24)));
  if (pa==NAF_TOP_SET(NAF_TOP_CFG8) || pa==NAF_TOP_CLR(NAF_TOP_CFG8)) assert(!(v&~3u));
 }
 /* clk-mux.c:178-224: a parent field change is committed by its update bit. */
 if ((pa==NAF_TOP_SET(NAF_TOP_CFG7) || pa==NAF_TOP_CLR(NAF_TOP_CFG7)) && (v&(3u<<24))) pend7=1;
 if ((pa==NAF_TOP_SET(NAF_TOP_CFG8) || pa==NAF_TOP_CLR(NAF_TOP_CFG8)) && (v&3u)) pend8=1;
 if ((pa==NAF_TOP_SET(NAF_TOP_CFG12) || pa==NAF_TOP_CLR(NAF_TOP_CFG12)) && (v&3u)) pend12=1;
 if ((pa==NAF_TOP_SET(NAF_TOP_CFG13) || pa==NAF_TOP_CLR(NAF_TOP_CFG13)) && (v&1u)) pend13=1;
 if (pa==NAF_TOP_UPDATE1) {
  assert(v && !(v&(v-1u))); /* one mux per update, as the source */
  if (v&1u) pend7=0;
  if (v&2u) pend8=0;
  if (v&(1u<<17)) pend12=0;
  if (v&(1u<<21)) pend13=0;
 }
 if (fault==F_PROTECT_STOP && pa==NAF_IFR_PROT2_SET) { cm.writes++; *regp(&cm,0x10001710)|=v; return 0; } /* request commits, ack never arrives */
 int r=allwrite(c,pa,w,v);
 if (fault==F_POWERUP && pa==NAF_SPM_AUDIO_PWR) {
  *regp(&cm,NAF_SPM_PWR_STATUS)&=~(1u<<21); *regp(&cm,NAF_SPM_PWR_STATUS2)&=~(1u<<21);
 }
 return r;
}
static const struct nah_io audited={audited_read,audited_write,alldelay,allworker,alllease};

static void prepare(const uint32_t *words)
{
 full_prepare(0x0b74,0x0a74,0x80); /* historical NOCLK sticky, source read clears */
 bm.ch[1][0x24/2]=2;
 bm.ch[1][0x10/2]=0x28;bm.saved[2]=0x28;bm.ch[1][0x0c/2]=1;bm.ch[1][0xec/2]=0x8800;
 memcpy(base,words,sizeof base);
 for (unsigned i=0;i<NAU_SNAP_COUNT;i++) *regp(&cm,nau_snapshot_pa[i])=words[i];
 /* Bus-model views of the same captured words. */
 bm.pinmux=words[NAU_S_MODE_120]; bm.pads=words[NAU_S_DIN_96]; bm.ies=words[NAU_S_BM_IES122];
 bm.gate=words[NAU_S_IMPS_CG]; bm.ifr1=words[NAU_S_IFR1_CG]; bm.ifr2=words[NAU_S_IFR2_CG];
 bm.hclk=24960; /* captured univpll_d5_d4 124.8MHz / DT clock-div5 */
 fault=F_NONE; strict=0; afe_accesses=i2s_routes=dl1_routes=apll_writes=0;
 pend7=pend8=pend12=pend13=0;
}
static void restored(void)
{
 /* Every captured word, through the same read paths init used. POWERON
  * high half is a write-only key (pg.c:3267). */
 for (unsigned i=0;i<NAU_SNAP_COUNT;i++) {
  uint32_t v=0,mask=i==NAU_S_SPM_POWERON ? 1u:0xffffffffu;
  if(i==NAU_S_PROT2_STA1) mask&=~(1u<<4); /* releaseACK is diagnostic, source ignores */
  assert(!allread(0,nau_snapshot_pa[i],4,&v));
  if ((v^base[i])&mask) {
   fprintf(stderr,"restore index%u pa%x got%x expected%x\n",i,nau_snapshot_pa[i],v,base[i]);
   abort();
  }
 }
 m_check_restored(&bm);
 assert(bm.ch[1][0x24/2]==2); /* No RESUME action to restore volatile readback. */
 assert(bm.ch[1][0xec/2]==0x8800 && bm.ch[1][0x10/2]==0x28);
 for (unsigned i=2;i<M_SLAVES;i++) assert(!bm.slave[i].accessed);
 assert(!pend7 && !pend8 && !pend12 && !pend13);
}
static void init_ok(void)
{
 assert(!nau_hw_init(&hw,&audited,0,nah_board_profile()));
 assert(hw.status.initialized && !hw.status.gate);
 assert(hw.amps[0].rev==0x0b74 && hw.amps[1].rev==0x0a74);
 assert(hw.status.amp_status[0]==1 && hw.status.amp_status[1]==1);
 /* Init touches only the two reset GPIOs and I2C8; reset left released. */
 assert(!afe_accesses && !cm.writes && bm.dma_en_reads);
 assert((reg(0x10005130)&(1u<<15))==(base[NAU_S_DOUT_96]&(1u<<15)));
 assert((reg(0x10005140)&(1u<<20))==(base[NAU_S_DOUT_128]&(1u<<20)));
}
static void cycle(uint64_t t,unsigned silent,unsigned idle_stop)
{
 static int16_t data[480];
 for (unsigned i=0;i<480;i++) data[i]=(i&1)?-16000:16000;
 unsigned routes=i2s_routes,dl1=dl1_routes,pcm=hw.status.pcm_frames;
 cm.current=NAF_SRAM;
 assert(!nau_hw_start(&hw,48000,2,16,silent,256,0,t));
 assert(i2s_routes==routes+1 && dl1_routes==dl1+1 && hw.status.running && hw.afe.active);
 for (unsigned i=0;i<2;i++) assert((bm.slave[i].reg[0]&9)==(silent ? 1:8));
 assert(!nau_hw_tick(&hw,t));
 for (unsigned j=1;j<=12;j++) {
  assert(nau_hw_feed(&hw,data,240)==240);
  cm.current=NAF_SRAM+4*(240*j%1024);
  assert(!nau_hw_tick(&hw,t+5000*j));
 }
 if (!silent) assert(hw.status.pcm_frames==pcm+2880);
 if (idle_stop) {
  int r=0;
  for (unsigned k=1;!r;k++) {
   assert(k<200);
   cm.current=NAF_SRAM+4*(240*(12+k)%1024);
   r=nau_hw_tick(&hw,t+60000+5000*k);
  }
  assert(r==NAH_E_IDLE);
 } else {
  assert(!nau_hw_stop(&hw));
 }
 assert(!hw.status.running && !hw.status.uncertain && !hw.afe.active && !hw.status.stop_error);
 for (unsigned i=0;i<2;i++) assert((bm.slave[i].reg[0]&9)==1);
 restored();
}
static void load_board(const char *path)
{
 static uint8_t blob[4163];
 static const uint8_t digest[32]={0x63,0x53,0x00,0xfb,0xc9,0x6f,0x25,0x8f,0xf9,0xa3,0x5f,0x82,0x43,0x5f,0x2e,0x9d,0x3f,0xf6,0xf1,0x37,0x0c,0x15,0x60,0x67,0xb4,0x01,0x46,0xe0,0x11,0x03,0x86,0x92};
 FILE *f=fopen(path,"rb");assert(f);
 assert(fread(blob,1,sizeof blob,f)==sizeof blob && fgetc(f)==EOF);fclose(f);
 assert(!nah_board_load(blob,sizeof blob,digest));
}

/* ---- pure classification ---- */
static unsigned gate_of(const uint32_t *w)
{
 static struct nau_state s;
 memset(&s,0,sizeof s); s.snap_read=NAU_SNAP_COUNT;
 memcpy(s.snap,w,sizeof s.snap);
 return naf_gate(&s,amps);
}
static void classify(void)
{
 uint32_t w[NAU_SNAP_COUNT];
 struct naf_own o;
 /* Real snapshot: canonical refused 0xf8; inherited tree now classified. */
 memcpy(w,actual_snapshot,sizeof w);
 assert(!gate_of(w) && !naf_ownership(w,&o));
 assert(naf_own_mask(&o)==0x800651ffu);
 assert(o.adsp_on && o.prot_clear && o.apll2_borrowed && !o.ifr1_gated && !o.ifr3_gated);
 assert(o.audio_parent==1 && o.intbus_parent==1 && o.engen2_parent==2 && o.aud2_parent==1);
 /* ADSP: only the two exact source end states with coherent status. */
 unsigned accepted=0;
 for (uint32_t x=0;x<0x4000u;x++) {
  if (x&~0x227fu) continue;
  for (unsigned st=0;st<4;st++) {
   memcpy(w,actual_snapshot,sizeof w);
   w[NAU_S_ADSP_PWR_CON]=x;
   w[NAU_S_PWR_STATUS]=(w[NAU_S_PWR_STATUS]&~(1u<<22))|((st&1u)<<22);
   w[NAU_S_PWR_STATUS_2ND]=(w[NAU_S_PWR_STATUS_2ND]&~(1u<<22))|((st>>1)<<22);
   unsigned ok=(st==0 && x==0x32u) || (st==3 && x==0x224du), g=gate_of(w);
   assert(!!(g&NAF_G_ADSP_ON)==!ok && !(g&~NAF_G_ADSP_ON));
   accepted+=ok;
  }
 }
 assert(accepted==2);
 /* AUDIO: only exact power-down end state, both status words clear. */
 for (uint32_t x=0;x<0x2000u;x++) {
  if (x&~0x111fu) continue;
  memcpy(w,actual_snapshot,sizeof w); w[NAU_S_AUDIO_PWR_CON]=x;
  assert(!!(gate_of(w)&NAF_G_AUDIO_ON)==(x!=0x1112u));
 }
 memcpy(w,actual_snapshot,sizeof w); w[NAU_S_PWR_STATUS_2ND]|=1u<<21;
 assert(gate_of(w)&NAF_G_AUDIO_ON);
 /* Protect and infra gates: every inherited combination is acceptable. */
 for (unsigned m=0;m<8;m++) {
  memcpy(w,actual_snapshot,sizeof w);
  if (m&1) {w[NAU_S_PROT2_STA1]|=1u<<4;w[NAU_S_PROT2_EN]|=1u<<4;}
  if (m&2) w[NAU_S_IFR1_CG]|=1u<<25;
  if (m&4) w[NAU_S_IFR3_CG]|=1u<<4;
  assert(!gate_of(w));
 }
 /* APLL2 power shape x tuner enable. */
 for (unsigned m=0;m<16;m++) {
  memcpy(w,actual_snapshot,sizeof w);
  w[NAU_S_APLL2_CON0]=(w[NAU_S_APLL2_CON0]&~1u)|(m&1u);
  w[NAU_S_APLL2_CON4]=(w[NAU_S_APLL2_CON4]&~3u)|((m>>1)&3u);
  if (m&8) w[NAU_S_AP_PLL_CON3]|=1u<<5;
  unsigned pll_off=!(m&1) && ((m>>1)&3)==2 && !(m&8), live=(m&1) && ((m>>1)&3)==1, g=gate_of(w);
  if (live) assert(!g);                         /* tuner == PCW+1 in capture */
  else if (pll_off) assert(g==NAF_G_MUX);       /* consumers live on an off PLL */
  else assert(g==NAF_G_APLL2_ON);
 }
 memcpy(w,actual_snapshot,sizeof w); /* cold PLL with gated 26MHz consumers */
 w[NAU_S_APLL2_CON0]&=~1u; w[NAU_S_APLL2_CON4]=(w[NAU_S_APLL2_CON4]&~3u)|2u;
 w[NAU_S_CLK_CFG_12]=(w[NAU_S_CLK_CFG_12]&~0x83u)|0x80u; w[NAU_S_CLK_CFG_13]=(w[NAU_S_CLK_CFG_13]&~0x81u)|0x80u;
 assert(!gate_of(w) && !naf_ownership(w,&o) && !o.apll2_borrowed);
 /* Live rate: exact 196.608MHz by the source recalc, any postdivider. */
 memcpy(w,actual_snapshot,sizeof w); w[NAU_S_APLL2_CON2]^=0x100000u;
 assert(gate_of(w)==NAF_G_APLL2_RATE);
 memcpy(w,actual_snapshot,sizeof w); w[NAU_S_APLL2_CON1]=(w[NAU_S_APLL2_CON1]&~(7u<<24))|(5u<<24);
 assert(gate_of(w)==NAF_G_APLL2_RATE);
 uint32_t code=0,pcw=naf_pll_pcw(NAF_APLL2_HZ,&code);
 memcpy(w,actual_snapshot,sizeof w); w[NAU_S_APLL2_CON1]=code<<24; w[NAU_S_APLL2_CON2]=pcw;
 assert(code==3 && !gate_of(w));
 memcpy(w,actual_snapshot,sizeof w); w[NAU_S_AP_PLL_CON3]|=1u<<5; w[NAU_S_APLL2_TUNER]=w[NAU_S_APLL2_CON2];
 assert(gate_of(w)==NAF_G_APLL2_TUNER);
 /* Mux adoption matrix with the live PLL. */
 for (unsigned m=0;m<4;m++) {
  memcpy(w,actual_snapshot,sizeof w);
  w[NAU_S_CLK_CFG_13]=(w[NAU_S_CLK_CFG_13]&~0x81u)|((m&2u)<<6)|(m&1u);
  unsigned ok=(m&1u) || (m&2u);
  assert(!!(gate_of(w)&NAF_G_MUX)==!ok);
 }
 for (unsigned m=0;m<8;m++) {
  memcpy(w,actual_snapshot,sizeof w);
  w[NAU_S_CLK_CFG_12]=(w[NAU_S_CLK_CFG_12]&~0x83u)|((m&4u)<<5)|(m&3u);
  unsigned ok=(m&3u)==2 || ((m&4u) && !(m&3u));
  assert(!!(gate_of(w)&NAF_G_MUX)==!ok);
 }
 for (unsigned m=0;m<8;m++) {
  memcpy(w,actual_snapshot,sizeof w);
  w[NAU_S_CLK_CFG_8]=(w[NAU_S_CLK_CFG_8]&~0x83u)|((m&4u)<<5)|(m&3u);
  assert(!!(gate_of(w)&NAF_G_MUX)==((m&3u)==3));
  memcpy(w,actual_snapshot,sizeof w);
  w[NAU_S_CLK_CFG_7]=(w[NAU_S_CLK_CFG_7]&~((1u<<31)|(3u<<24)))|((m&4u)<<29)|((m&3u)<<24);
  assert(!gate_of(w));
 }
 /* Original cold fixture still classifies as fully acquired. */
 struct nau_state c; cold(&c);
 assert(!naf_gate(&c,amps) && !naf_ownership(c.snap,&o));
 assert(naf_own_mask(&o)==0x80000000u);
 puts("PASS classify: real snapshot accepted (own 800651ff), exact ADSP/AUDIO end states only, live APLL2 rate/tuner/consumer matrix, cold fixture unchanged");
}
static unsigned find(const struct naf_op *ops,unsigned n,unsigned kind,uint32_t pa,uint32_t value,unsigned from)
{
 for (unsigned i=from;i<n;i++)
  if (ops[i].kind==kind && ops[i].pa==pa && (kind!=NAF_W || ops[i].value==value)) return i;
 return n;
}
static void plan(void)
{
 static struct naf_plan p;
 struct nau_state s; memset(&s,0,sizeof s); s.snap_read=NAU_SNAP_COUNT;
 memcpy(s.snap,actual_snapshot,sizeof s.snap);
 assert(!naf_build(&s,amps,&p));
 assert(p.n_up==48 && p.n_down==35);
 const struct naf_op *u=p.up,*d=p.down;
 for (unsigned l=0;l<2;l++) {
  const struct naf_op *ops=l ? d:u; unsigned n=l ? p.n_down:p.n_up;
  for (unsigned i=0;i<n;i++) {
   uint32_t pa=ops[i].pa;
   if (ops[i].kind!=NAF_RMW && ops[i].kind!=NAF_W) continue;
   assert(pa!=NAF_APM_APLL2_CON0 && pa!=NAF_APM_APLL2_CON1 && pa!=NAF_APM_APLL2_CON2);
   assert(pa!=NAF_APM_APLL2_CON4 && pa!=NAF_APM_APLL2_TUNER && pa!=NAF_APM_AP_PLL_CON3);
   assert(pa!=NAF_AFE_APLL2_TCFG && pa!=NAF_IFR_CG1_SET && pa!=NAF_IFR_CG1_CLR);
   assert(pa!=NAF_IFR_CG3_SET && pa!=NAF_IFR_CG3_CLR);
   assert((pa&~0xcu)!=NAF_TOP_CFG12 || pa==NAF_TOP_CFG12);
   assert((pa&~0xcu)!=NAF_TOP_CFG13 || pa==NAF_TOP_CFG13);
  }
 }
 /* Up: audio_sel then intbus to clk26m, each committed, before first AFE op. */
 unsigned a=find(u,p.n_up,NAF_W,NAF_TOP_CLR(NAF_TOP_CFG7),3u<<24,0);
 unsigned b=find(u,p.n_up,NAF_W,NAF_TOP_UPDATE1,1u,a);
 unsigned c=find(u,p.n_up,NAF_W,NAF_TOP_CLR(NAF_TOP_CFG8),3u,b);
 unsigned e=find(u,p.n_up,NAF_W,NAF_TOP_UPDATE1,2u,c);
 unsigned f=find(u,p.n_up,NAF_RMW,NAF_AFE_TOP_CON0,0,0);
 unsigned prot=find(u,p.n_up,NAF_W,NAF_IFR_PROT2_CLR,1u<<4,0);
 assert(prot<a && a<b && b<c && c<e && e<f && f<p.n_up);
 /* Down: audio_sel back to parent1 after AFE off; protect set for power-off,
  * then released to the inherited clear state; intbus back to parent1. */
 unsigned g=find(d,p.n_down,NAF_W,NAF_TOP_SET(NAF_TOP_CFG7),1u<<24,0);
 unsigned h=find(d,p.n_down,NAF_W,NAF_IFR_PROT2_SET,1u<<4,0);
 unsigned k=find(d,p.n_down,NAF_POLL,NAF_SPM_PWR_STATUS2,0,0);
 unsigned m=find(d,p.n_down,NAF_W,NAF_IFR_PROT2_CLR,1u<<4,k);
 unsigned q=find(d,p.n_down,NAF_W,NAF_TOP_SET(NAF_TOP_CFG8),1u,m);
 assert(g<h && h<k && k<m && m<q && q<p.n_down);
 assert(d[g+1].pa==NAF_TOP_UPDATE1 && d[g+1].value==1u && d[q+1].pa==NAF_TOP_UPDATE1 && d[q+1].value==2u);
 assert(d[p.n_down-1].pa==NAF_SPM_POWERON && d[p.n_down-1].value==((NAF_SPM_PROJECT_CODE<<16)|1u));
 /* Cold fixture plan size is the cold plan remains source-equivalent with SET-request verification (73/50). */
 cold(&s);
 assert(!naf_build(&s,amps,&p) && p.n_up==73 && p.n_down==50);
 assert(find(p.up,p.n_up,NAF_W,NAF_APM_APLL2_CON2,naf_pll_pcw(NAF_APLL2_HZ,0),0)<p.n_up);
 puts("PASS plan: real snapshot 48/35 ops, no APLL2/ADSP/infra/consumer writes, 26MHz bus route ordered, exact restore order; cold plan 73/50");
}

#ifndef INHERITED_MODEL_ONLY
int main(int argc,char **argv)
{
 assert(argc==2 || argc==3);
 const char *k=argv[1];
 if (!strcmp(k,"classify")) { classify(); return 0; }
 if (!strcmp(k,"plan")) { plan(); return 0; }
 assert(argc==3); load_board(argv[2]);
 if (!strcmp(k,"cycles")) {
  prepare(actual_snapshot); strict=1;
  init_ok();
  assert(hw.status.ownership==0x800651ffu);
  for (unsigned n=0;n<3;n++) cycle(2000000ull*(n+1),0,0);
  cycle(8000000,1,0);            /* silent AFE-only diagnostic */
  cycle(10000000,0,1);           /* HAL idle-driven stop */
  cycle(14000000,0,0);           /* restart after idle stop */
  assert(!apll_writes && afe_accesses && i2s_routes==6 && dl1_routes==6);
  for (unsigned i=0;i<bm.ntrans;i++) {
   assert(bm.trans[i].addr==0x34 || bm.trans[i].addr==0x37);
   assert(bm.trans[i].ptr!=0xa3 && bm.trans[i].ptr!=0xa7 && bm.trans[i].ptr!=0xa8);
  }
  printf("PASS cycles: real snapshot init + 5 playback/silent/idle cycles, every word restored, no ADSP/APLL2/infra/consumer writes (%u simulated I2C transfers)\n",bm.ntrans);
  return 0;
 }
 if (!strcmp(k,"mixed")) {
  /* Live APLL2 but every other element in its acquire state: ADSP off,
   * protect set, infra gated, muxes gated on non-26MHz/26MHz parents. */
  uint32_t w[NAU_SNAP_COUNT]; memcpy(w,actual_snapshot,sizeof w);
  w[NAU_S_ADSP_PWR_CON]=0x32u; w[NAU_S_PWR_STATUS]&=~(1u<<22); w[NAU_S_PWR_STATUS_2ND]&=~(1u<<22);
  w[NAU_S_PROT2_STA1]|=1u<<4; w[NAU_S_PROT2_EN]|=1u<<4; w[NAU_S_IFR1_CG]|=1u<<25; w[NAU_S_IFR3_CG]|=1u<<4;
  w[NAU_S_CLK_CFG_7]=(w[NAU_S_CLK_CFG_7]&~((1u<<31)|(3u<<24)))|(1u<<31)|(2u<<24);
  w[NAU_S_CLK_CFG_8]=(w[NAU_S_CLK_CFG_8]&~0x83u)|0x82u;
  w[NAU_S_CLK_CFG_12]=(w[NAU_S_CLK_CFG_12]&~0x83u)|0x80u;
  w[NAU_S_CLK_CFG_13]=(w[NAU_S_CLK_CFG_13]&~0x81u)|0x80u;
  prepare(w);
  init_ok();
  assert(hw.status.ownership==0x8000a010u);
  cycle(2000000,0,0); cycle(4000000,0,0);
  assert(!apll_writes);
  puts("PASS mixed: live APLL2 with acquired ADSP-off/protect/infra/mux states, exact acquire+restore twice");
  return 0;
 }
 if (!strncmp(k,"refuse-",7)) {
  prepare(actual_snapshot); strict=1;
  unsigned want=0;
  if (!strcmp(k,"refuse-rate")) { *regp(&cm,NAF_APM_APLL2_CON2)^=0x100000u; want=NAF_G_APLL2_RATE; }
  else if (!strcmp(k,"refuse-status")) { *regp(&cm,NAF_SPM_PWR_STATUS2)^=1u<<22; want=NAF_G_ADSP_ON; }
  else if (!strcmp(k,"refuse-adsp")) { *regp(&cm,0x10006358u)^=1u; want=NAF_G_ADSP_ON; }
  else if (!strcmp(k,"refuse-coldmux")) {
   *regp(&cm,NAF_APM_APLL2_CON0)&=~1u; *regp(&cm,NAF_APM_APLL2_CON4)=(reg(NAF_APM_APLL2_CON4)&~3u)|2u;
   want=NAF_G_MUX;
  } else assert(0);
  assert(nau_hw_init(&hw,&audited,0,nah_board_profile())==NAH_E_COLD);
  assert(hw.status.gate==want && !hw.status.initialized);
  assert(!cm.writes && !bm.starts && !afe_accesses);
  printf("PASS %s: refused gate=%x before any write or I2C transfer\n",k,want);
  return 0;
 }
 if (!strncmp(k,"drift-",6)) {
  prepare(actual_snapshot); strict=1; init_ok();
  uint32_t pa=0,old=0,bit=0;
  if (!strcmp(k,"drift-audioh")) { pa=0x10000100u; old=reg(pa); *regp(&cm,pa)^=1u; bit=1u<<12; }
  else if (!strcmp(k,"drift-adsp")) {
   pa=0x10006358u; old=reg(pa); *regp(&cm,pa)=0x32u;  /* coherent ADSP power-down by another owner */
   *regp(&cm,NAF_SPM_PWR_STATUS)&=~(1u<<22); *regp(&cm,NAF_SPM_PWR_STATUS2)&=~(1u<<22);
   bit=(1u<<1)|(1u<<2)|(1u<<3);
  } else assert(0);
  unsigned writes=cm.writes,starts=bm.starts;
  assert(nau_hw_start(&hw,48000,2,16,0,256,0,1000000)==NAH_E_COLD);
  assert(cm.writes==writes && bm.starts==starts && !afe_accesses && !hw.status.uncertain);
  assert(hw.afe.failure==NAF_FAIL_LIVE && hw.afe.verify_mismatch==bit);
  /* Refusal was clean: once the field is back the same object starts. */
  *regp(&cm,pa)=old;
  if (!strcmp(k,"drift-adsp")) { *regp(&cm,NAF_SPM_PWR_STATUS)|=1u<<22; *regp(&cm,NAF_SPM_PWR_STATUS2)|=1u<<22; }
  cycle(2000000,0,0);
  printf("PASS %s: concurrent change refused before writes/transfers, clean retry\n",k);
  return 0;
 }
 if (!strcmp(k,"powerup-timeout")) {
  prepare(actual_snapshot); strict=1; init_ok(); fault=F_POWERUP;
  assert(nau_hw_start(&hw,48000,2,16,0,256,0,1000000)==NAH_E_UNCERTAIN);
  assert(hw.status.uncertain && !hw.status.running && hw.afe.uncertain);
  assert(hw.afe.failure==NAF_FAIL_POLL && hw.afe.phase==NAF_PH_POWER && hw.afe.last_pa==NAF_SPM_PWR_STATUS);
  assert(!afe_accesses && (bm.slave[0].reg[0]&9)==1 && (bm.slave[1].reg[0]&9)==1);
  unsigned rd=cm.reads,wr=cm.writes,st=bm.starts;
  assert(nau_hw_stop(&hw)==0 && nau_hw_start(&hw,48000,2,16,1,1,0,2000000)==NAH_E_ARG);
  assert(cm.reads==rd && cm.writes==wr && bm.starts==st);
  puts("PASS powerup-timeout: MTCMOS ack loss latches uncertain before AFE/amp access, no cleanup or retry MMIO");
  return 0;
 }
 if (!strcmp(k,"protect-timeout")) {
  prepare(actual_snapshot); strict=1; init_ok();
  cm.current=NAF_SRAM;
  assert(!nau_hw_start(&hw,48000,2,16,0,256,0,1000000) && !nau_hw_tick(&hw,1000000));
  fault=F_PROTECT_STOP;
  assert(nau_hw_stop(&hw)==NAH_E_UNCERTAIN);
  assert(hw.status.uncertain && hw.status.stop_error==NAH_E_UNCERTAIN && !hw.status.running);
  assert(hw.afe.failure==NAF_FAIL_POLL && hw.afe.phase==NAF_PH_POWER_OFF);
  /* Outputs were disabled first; stream, I2S and AFE are off; the domain is
   * left powered (no SRAM_PDN/ISO without protection), never forced. */
  assert((bm.slave[0].reg[0]&9)==1 && (bm.slave[1].reg[0]&9)==1);
  assert(!(reg(NAF_AFE_DAC_CON0)&0x11u) && !(reg(NAF_AFE_I2S_CON3)&0x1001u));
  assert((reg(NAF_SPM_AUDIO_PWR)&0x111fu)==0x000du && ((reg(NAF_TOP_CFG7)>>24)&3u)==1u);
  assert((reg(0x10005130)&(1u<<15))==(base[NAU_S_DOUT_96]&(1u<<15)));
  unsigned rd=cm.reads,wr=cm.writes;
  assert(nau_hw_start(&hw,48000,2,16,1,1,0,2000000)==NAH_E_ARG && cm.reads==rd && cm.writes==wr);
  puts("PASS protect-timeout: power-off protect ack loss after clean amp/stream stop latches uncertain, no forced power-down");
  return 0;
 }
 if (!strcmp(k,"ampe")) {
  prepare(actual_snapshot); strict=1;
  bm.slave[1].reg[0]=0x0009; /* AMPE latched after the reset pulse */
  assert(nau_hw_init(&hw,&audited,0,nah_board_profile())==NAH_E_AMPS);
  assert(hw.status.gate==NAF_G_AMPS && !hw.status.initialized && hw.status.amp_status[1]==9);
  assert(!cm.writes && !afe_accesses && !hw.reset_changed);
  m_check_trans(&bm);
  assert((reg(0x10005130)&(1u<<15))==(base[NAU_S_DOUT_96]&(1u<<15)));
  assert((reg(0x10005030)&(1u<<15))==(base[NAU_S_DIR_96]&(1u<<15)));
  assert((reg(0x10005140)&(1u<<20))==(base[NAU_S_DOUT_128]&(1u<<20)));
  assert((reg(0x10005040)&(1u<<20))==(base[NAU_S_DIR_128]&(1u<<20)));
  puts("PASS ampe: post-reset amplifier evidence refusal is NAH_E_AMPS, saved sys0 reported, resets restored, no AFE write");
  return 0;
 }
 fprintf(stderr,"unknown case %s\n",k);
 return 2;
}

#endif /* INHERITED_MODEL_ONLY */
