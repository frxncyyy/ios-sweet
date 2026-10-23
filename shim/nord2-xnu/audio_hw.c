#if defined(KERNEL)
#include <pexpert/arm64/board_config.h>
#endif
#if defined(NORD2)
#include "audio_hw.h"

static int worker(struct nau_hw *h)
{ return h && h->io.worker && h->owner && h->io.worker(h->cookie) == h->owner; }
static int rd32(void *c, uint32_t pa, uint32_t *v)
{ struct nau_hw *h=c; return h->io.read(h->cookie,pa,4,v); }
static int wr32(void *c, uint32_t pa, uint32_t v)
{ struct nau_hw *h=c; return h->io.write(h->cookie,pa,4,v); }
static int delay(void *c, unsigned us)
{ struct nau_hw *h=c; return h->io.delay_us(h->cookie,us); }
static int busrd(void *c, uint32_t pa, unsigned w, uint32_t *v)
{ struct nau_hw *h=c; return h->io.read(h->cookie,pa,w,v); }
static int buswr(void *c, uint32_t pa, unsigned w, uint32_t v)
{ struct nau_hw *h=c; return h->io.write(h->cookie,pa,w,v); }
static const struct naf_io afe_io={rd32,wr32,delay};
static const struct na8_io bus_io={busrd,buswr,delay};
static int lease(struct nau_hw *h, unsigned acquire)
{
 if (acquire && h->leased) return 0;
 if (!acquire && !h->leased) return 0;
 if (h->io.lease(h->cookie,acquire)) return NAH_E_IO;
 h->leased=acquire; return 0;
}
static int amp_read(struct nau_hw *h,unsigned amp,unsigned reg,uint16_t *v)
{ return na8_read16(&bus_io,h,&h->bus,h->amps[amp].addr,reg,v) ? NAH_E_IO:0; }
static int amp_rmw(struct nau_hw *h,unsigned amp,unsigned reg,uint16_t mask,uint16_t value)
{
 uint16_t old=0,check=0;
 if (amp_read(h,amp,reg,&old)) return NAH_E_IO;
 uint16_t next=(uint16_t)((old & ~mask)|(value & mask));
 if (na8_write16(&bus_io,h,&h->bus,h->amps[amp].addr,reg,next)) return NAH_E_IO;
 if (amp_read(h,amp,reg,&check) || ((check^next)&mask)) return NAH_E_IO;
 return 0;
}
static int amp_cold(struct nau_hw *h,unsigned amp)
{
 /* tfa98xx_powerdown + probus AMPE, no fake no-DSP mute callback. */
 if (amp_rmw(h,amp,0x00,8,0) || amp_rmw(h,amp,0x00,1,1)) return NAH_E_IO;
 for (unsigned k=0;k<50;k++) {
  uint16_t v=0;
  if (amp_read(h,amp,0x14,&v)) return NAH_E_IO;
  h->status.amp_status[amp]=v;
  if (!(v & 0x78)) return 0;
  if (delay(h,1000)) return NAH_E_IO;
 }
 return NAH_E_AMPS;
}
static int amp_enable(struct nau_hw *h,unsigned amp)
{
 /* tfa_dev_set_state(OPERATING): PWDN0 -> MANSCONF1 -> AMPE1.
  * Stable clocks verified before output enable, exact family CLKS field. */
 if (amp_rmw(h,amp,0x00,1,0) || amp_rmw(h,amp,0x01,4,4)) return NAH_E_IO;
 unsigned is74=(h->amps[amp].rev & 255)==0x74;
 unsigned clockreg=is74 ? 0x11:0x10, clockmask=is74 ? 0x80:0x40;
 unsigned ready=0;
 for (unsigned k=0;k<50;k++) {
  uint16_t v=0;
  if (amp_read(h,amp,clockreg,&v)) return NAH_E_IO;
  if (v & clockmask) { ready=1; break; }
  if (delay(h,1000)) return NAH_E_IO;
 }
 if (!ready) return NAH_E_AMPS;
 /* Selected tfa98xx.c956 explicitly removes cold-start NOCLK sticky by
  * reading once. Otherwise our first monitor would misclassify historical
  * startup loss as a runtime failure. Never discard a current TDM fault;
  * reread NOCLK and CLKS before enabling the output. */
 uint16_t status=0,clocks=0;
 unsigned tdmmask=is74 ? 0x40:0x400, noclkmask=is74 ? 0x80:0x100;
 if (amp_read(h,amp,0x10,&status)) return NAH_E_IO;
 if (status&tdmmask) return NAH_E_AMP_FAULT;
 if (amp_read(h,amp,0x10,&status) || amp_read(h,amp,clockreg,&clocks)) return NAH_E_IO;
 h->status.amp_status[amp]=status;
 if ((status&(tdmmask|noclkmask)) || !(clocks&clockmask)) return NAH_E_AMP_FAULT;
 return amp_rmw(h,amp,0x00,8,8);
}
static int board_valid(const struct nau_hw *h)
{
 const struct nah_board *b=h->board;
 if (!b || !b->apply) return 0;
 unsigned nonzero=0;
 for (unsigned i=0;i<32;i++) nonzero|=b->source_sha256[i];
 if (!nonzero) return 0;
 if (b->supports) return b->supports(h->amps[0].rev,h->amps[0].addr) && b->supports(h->amps[1].rev,h->amps[1].addr);
 return b->revision[0]==h->amps[0].rev && b->revision[1]==h->amps[1].rev;
}
static int bus_clock_prepare(struct nau_hw *h)
{
 if (h->bus_clock_prepared) return 0;
 uint32_t pwr=0,pwr2=0,mux=0,gate=0;
 if (rd32(h,NAF_SPM_PWR_STATUS,&pwr) || rd32(h,NAF_SPM_PWR_STATUS2,&pwr2) ||
     rd32(h,0x100000b0,&mux) || rd32(h,0x11d05e00,&gate)) return NAH_E_IO;
 if ((pwr|pwr2)&(1u<<23)) return NAH_E_AMPS; /* CAM/CCU master must be off */
 /* Source clk-mt6885.c1886: i2c_sel PDN15; enable preserves parent.
  * Source clk-mt6885-i2c.c110: IMPS I2C8 gatebit4, CLR e04/SET e08.
  * Enabling a gated clock is needed before controller-idle can be read;
  * no peripheral transfer or controller reset is initiated by these writes. */
 if (mux&(1u<<15)) {
  h->bus_clock_changed|=1;
  if (wr32(h,0x100000b8,1u<<15)) return NAH_E_IO;
 }
 if (gate&(1u<<4)) {
  h->bus_clock_changed|=2;
  if (wr32(h,0x11d05e04,1u<<4)) return NAH_E_IO;
 }
 if (rd32(h,0x100000b0,&mux) || rd32(h,0x11d05e00,&gate) ||
     (mux&(1u<<15)) || (gate&(1u<<4))) return NAH_E_IO;
 h->bus_clock_prepared=1;
 return 0;
}
static int bus_clock_restore(struct nau_hw *h)
{
 if (!h->bus_clock_changed) { h->bus_clock_prepared=0; return 0; }
 /* Never gate a possibly active shared controller after a transfer fault. */
 if (h->bus.uncertain || h->bus.inflight) return NAH_E_UNCERTAIN;
 uint32_t pwr=0,pwr2=0;
 if (rd32(h,NAF_SPM_PWR_STATUS,&pwr) || rd32(h,NAF_SPM_PWR_STATUS2,&pwr2) ||
     ((pwr|pwr2)&(1u<<23))) return NAH_E_IO;
 if ((h->bus_clock_changed&2) && wr32(h,0x11d05e08,1u<<4)) return NAH_E_IO;
 /* i2c_sel is shared with OTHER controllers. Enabling it is monotonic;
  * disabling after playback could cut another worker's transaction. The
  * source CCF has global refcounts, this port does not. Retain that one
  * shared clock if we enabled it; restore only exclusively leased I2C8. */
 if (h->bus_clock_changed&1) h->status.retained_shared_clock=1;
 h->bus_clock_changed=h->bus_clock_prepared=0;
 return 0;
}
static int reset_restore(struct nau_hw *h)
{
 if (!h->reset_changed) return 0;
 static const uint32_t dir[2]={0x10005030,0x10005040};
 static const uint32_t dout[2]={0x10005130,0x10005140};
 static const unsigned bit[2]={15,20};
 static const unsigned di[2]={NAU_S_DIR_96,NAU_S_DIR_128};
 static const unsigned oi[2]={NAU_S_DOUT_96,NAU_S_DOUT_128};
 int error=0;
 for (unsigned i=0;i<2;i++) {
  uint32_t m=1u<<bit[i];
  /* Drop output before restoring latch if inherited direction was input. */
  if (!(h->snapshot.snap[di[i]]&m) && wr32(h,dir[i]+8,m)) error=NAH_E_IO;
  if (wr32(h,dout[i]+((h->snapshot.snap[oi[i]]&m)?4:8),m)) error=NAH_E_IO;
  if ((h->snapshot.snap[di[i]]&m) && wr32(h,dir[i]+4,m)) error=NAH_E_IO;
 }
 if (!error) h->reset_changed=0;
 h->status.reset_error=error;
 return error;
}
static int reset_hold_fault(struct nau_hw *h)
{
 /* Only the two already-owned GPIO reset lines. This cannot address any
  * I2C slave or reset/clock the shared controller. Set latch before output
  * enable so both amps are held in external reset, even if AMPE latched. */
 if (!h->reset_changed) return NAH_E_IO;
 int error=0;
 if (wr32(h,0x10005134,1u<<15)) error=NAH_E_IO;
 if (wr32(h,0x10005144,1u<<20)) error=NAH_E_IO;
 if (wr32(h,0x10005034,1u<<15)) error=NAH_E_IO;
 if (wr32(h,0x10005044,1u<<20)) error=NAH_E_IO;
 uint32_t latch0=0,latch1=0,dir0=0,dir1=0;
 if (rd32(h,0x10005130,&latch0) || rd32(h,0x10005140,&latch1) ||
     rd32(h,0x10005030,&dir0) || rd32(h,0x10005040,&dir1) ||
     !(latch0&(1u<<15)) || !(latch1&(1u<<20)) ||
     !(dir0&(1u<<15)) || !(dir1&(1u<<20))) error=NAH_E_IO;
 h->status.amp_reset_held=!error;
 h->status.reset_error=error;
 return error;
}
static int amps_prepare(struct nau_hw *h)
{
 if (h->reset_changed) return 0;
 /* Source tfa98xx.c:4740-4749 resetpulse, probe GPIOF_OUT_INIT_LOW:5037.
  * Pinmux must alreadyGPIO; no takeover of anotherperipheral. */
 struct nau_pin pins[NAU_PIN_COUNT]; nau_pin_decode(h->snapshot.snap,pins);
 if (pins[4].mode || pins[7].mode) return NAH_E_AMPS;
 const struct naf_amp_evidence dummy[2]={{1,1,1,0,0},{1,1,1,0,0}};
 h->status.gate=naf_gate(&h->snapshot,dummy);
 if (h->status.gate) return NAH_E_COLD;
 if (lease(h,1)) return NAH_E_IO;
 if (bus_clock_prepare(h)) { (void)bus_clock_restore(h); (void)lease(h,0); return NAH_E_IO; }
 /* A real read or cleanNACK proves the entire source bus preflight and
  * AP config restoration before any GPIO mutation. */
 uint16_t rev=0;
 int rc=na8_read16(&bus_io,h,&h->bus,0x34,0x03,&rev);
 if (rc<0) { (void)bus_clock_restore(h); (void)lease(h,0); return NAH_E_IO; }
 h->reset_changed=1;
 if (wr32(h,0x10005134,1u<<15) || wr32(h,0x10005144,1u<<20) ||
     wr32(h,0x10005034,1u<<15) || wr32(h,0x10005044,1u<<20)) goto fail;
 for (unsigned i=0;i<5;i++) if (delay(h,1000)) goto fail;
 if (wr32(h,0x10005138,1u<<15) || wr32(h,0x10005148,1u<<20)) goto fail;
 for (unsigned i=0;i<5;i++) if (delay(h,1000)) goto fail;
 rc=na8_probe(&bus_io,h,&h->bus,h->amps);
 if (rc) goto fail;
 for (unsigned i=0;i<2;i++) {
  h->status.amp_revision[i]=h->amps[i].rev;
  /* Post-reset SYS_CONTROL0 for the saved log: PWDN bit0, AMPE bit3. */
  h->status.amp_status[i]=h->amps[i].sys0;
  if (h->amps[i].cls!=NA8_SUPPORTED) { rc=NAH_E_AMPS; goto fail; }
  h->evidence[i]=(struct naf_amp_evidence){1,1,h->amps[i].pwdn,h->amps[i].ampe,h->amps[i].manstate};
 }
 if (lease(h,0)) goto fail;
 return 0;
fail:
 (void)reset_restore(h); (void)bus_clock_restore(h); (void)lease(h,0);
 return rc<0 ? rc:NAH_E_IO;
}
int nau_hw_init(struct nau_hw *h,const struct nah_io *io,void *cookie,const struct nah_board *board)
{
 if (!h || !io || !io->read || !io->write || !io->worker || !io->delay_us || !io->lease ||
     h->snapshot.entered || h->status.initialized || h->afe.active || h->afe.uncertain) return NAH_E_ARG;
 uintptr_t owner=io->worker(cookie);
 if (!owner) return NAH_E_WORKER;
 /* Caller guarantees a fresh zeroed object. No stack-size-sensitive copies. */
 h->io=*io; h->cookie=cookie; h->owner=owner; h->board=board; h->snapshot.entered=1;
 for (unsigned i=0;i<NAU_SNAP_COUNT;i++) {
  if (io->read(cookie,nau_snapshot_pa[i],4,&h->snapshot.snap[i])) {
   h->snapshot.failed=1; return h->status.error=NAH_E_IO;
  }
  h->snapshot.snap_read++;
 }
 /* Pure decode of the inherited tree for the log; plans derive it again. */
 struct naf_own own;
 (void)naf_ownership(h->snapshot.snap,&own);
 h->status.ownership=naf_own_mask(&own);
 int rc=amps_prepare(h);
 if (rc) return h->status.error=rc;
 h->status.gate=naf_gate(&h->snapshot,h->evidence);
 if (h->status.gate) {
  (void)reset_restore(h);
  return h->status.error= h->status.gate==NAF_G_AMPS ? NAH_E_AMPS:NAH_E_COLD;
 }
 h->status.initialized=1;
 return h->status.error=0;
}
int nau_hw_start(struct nau_hw *h,unsigned rate,unsigned channels,unsigned bits,
 unsigned silent,unsigned gain,unsigned duration,uint64_t now)
{
 if (!worker(h)) return NAH_E_WORKER;
 if (!h->status.initialized || h->status.running || h->status.uncertain ||
     rate!=NAH_RATE || channels!=2 || bits!=16 || silent>1 || gain>NAH_MAX_GAIN_Q15 ||
     duration>NAH_MAX_TEST_US || now>UINT64_MAX-duration) return NAH_E_ARG;
 /* This refusal must happen before any mutation or controller programming. */
 if (!silent && !board_valid(h)) return h->status.error=NAH_E_BOARD_CONFIG;
 if (!silent && !h->reset_changed) {
  int prep=amps_prepare(h);
  if (prep) return h->status.error=prep;
 }
 h->status.silent_only=silent; h->gain_q15=gain;
 h->qr=h->qw=h->qcount=h->last_frame=h->idle_ticks=0;
 h->last_us=h->last_feed_us=now; h->deadline_us=duration ? now+duration:0;
 h->first_tick=1; h->new_input=0; h->pointer_waiting=0; h->last_progress_us=now;
 int rc=naf_stream_open(&afe_io,h,&h->snapshot,h->evidence,&h->plan,&h->afe);
 if (rc) {
  h->status.uncertain=h->afe.uncertain;
  return h->status.error= h->afe.uncertain ? NAH_E_UNCERTAIN:NAH_E_COLD;
 }
 h->status.running=1;
 /* AFE is clocking a zero-filled SRAM ring before any amplifier comes on. */
 if (!silent) {
  if ((rc=lease(h,1))) goto fail;
  for (unsigned i=0;i<2;i++) {
   if ((rc=amp_cold(h,i))) goto fail;
   if (nah_amp_cold_init(&bus_io,h,&h->bus,h->amps[i].addr,h->amps[i].rev)) { rc=NAH_E_AMPS; goto fail; }
   if (h->board->apply(h,&bus_io,&h->bus,i)) { rc=NAH_E_BOARD_CONFIG; goto fail; }
   if (nah_amp_lock(&bus_io,h,&h->bus,h->amps[i].addr)) { rc=NAH_E_IO; goto fail; }
   /* Source rateenum8=48000Hz in TFA AUDFS; siliconinit+container
    * do not establish sample rate themselves. */
   if ((rc=amp_rmw(h,i,0x02,0x000f,8))) goto fail;
   /* Board callback must preserve powered-down, output-disabled state. */
   uint16_t v=0;
   if (amp_read(h,i,0x00,&v) || (v&9)!=1) { rc=NAH_E_BOARD_CONFIG; goto fail; }
  }
  for (unsigned i=0;i<2;i++) if ((rc=amp_enable(h,i))) goto fail;
  if ((rc=lease(h,0))) goto fail;
 }
 h->status.error=0;
 return 0;
fail:
 h->status.error=rc;
 (void)nau_hw_stop(h);
 return rc;
}
int nau_hw_feed(struct nau_hw *h,const int16_t *pcm,unsigned frames)
{
 if (!worker(h)) return NAH_E_WORKER;
 if (!h->status.running || (!pcm && frames)) return NAH_E_ARG;
 unsigned room=NAH_QUEUE_FRAMES-h->qcount;
 if (frames>room) frames=room;
 for (unsigned i=0;i<frames;i++) {
  h->queue[h->qw*2]=pcm[i*2]; h->queue[h->qw*2+1]=pcm[i*2+1];
  h->qw=(h->qw+1)%NAH_QUEUE_FRAMES;
 }
 h->qcount+=frames; h->status.fed_frames+=frames;
 if (frames) h->new_input=1;
 h->status.queued_frames=h->qcount;
 return (int)frames;
}
static int terminate(struct nau_hw *h,int reason)
{ h->status.error=reason; (void)nau_hw_stop(h); return reason; }
int nau_hw_tick(struct nau_hw *h,uint64_t now)
{
 if (!worker(h)) return NAH_E_WORKER;
 if (!h->status.running) return NAH_E_ARG;
 if (now<h->last_us) return terminate(h,NAH_E_DEADLINE);
 uint64_t gap=now-h->last_us;
 if (!h->first_tick && gap>=20000) return terminate(h,NAH_E_DEADLINE);
 if (!h->first_tick && gap>h->status.max_tick_gap_us) h->status.max_tick_gap_us=(uint32_t)gap;
 if (h->deadline_us && now>=h->deadline_us) return nau_hw_stop(h);
 uint32_t pa=0;
 if (rd32(h,NAF_AFE_DL1_CUR,&pa)) return terminate(h,NAH_E_IO);
 h->status.current_pa=pa;
 if (h->first_tick && !h->pointer_waiting) {
  h->pointer_waiting=1; h->last_progress_us=now;
 }
 /* Selected mtk-afe-platform-driver.c105-110 handles initial CUR0 as
  * a zero PCM position. Keep the prefilled ring silent while waiting at
  * most12ms for a real pointer; never normalize a bad pointer after start. */
 if (!pa && h->first_tick) {
  if (now-h->last_progress_us>=12000) return terminate(h,NAH_E_CLOCK_STALLED);
  h->last_us=now; return 0;
 }
 if (pa<NAF_SRAM || pa>=NAF_SRAM+NAF_SRAM_BYTES || (pa&3))
  return terminate(h,NAH_E_POINTER);
 uint32_t current=(pa-NAF_SRAM)/4;
 if (h->first_tick) {
  h->first_tick=0; h->pointer_waiting=0; h->last_frame=current; h->last_us=now; h->last_feed_us=now; h->last_progress_us=now;
  return 0;
 }
 if (h->new_input) { h->last_feed_us=now; h->new_input=0; }
 if (!h->deadline_us && !h->qcount && now-h->last_feed_us>=750000)
  return terminate(h,NAH_E_IDLE);
 uint32_t consumed=(current+NAH_RING_FRAMES-h->last_frame)%NAH_RING_FRAMES;
 if (consumed>NAH_RATE/100) return terminate(h,NAH_E_DEADLINE);
 /* If a bounded worker misses a complete revolution, ownership is lost.
  * Timestamp gap above avoids that ambiguity; no chasing current pointer. */
 /* Measure elapsed time since actual progress, not individual tick gap:
  * the real worker runs every2ms, so a >=4ms-per-tick counter never fires. */
 if (consumed) h->last_progress_us=now;
 else if (now-h->last_progress_us>=12000) return terminate(h,NAH_E_CLOCK_STALLED);
 unsigned starved=0;
 for (unsigned i=0;i<consumed;i++) {
  int32_t left=0,right=0;
  if (h->qcount && !h->status.silent_only) {
   left=(int32_t)h->queue[h->qr*2]*(int32_t)h->gain_q15/32768;
   right=(int32_t)h->queue[h->qr*2+1]*(int32_t)h->gain_q15/32768;
   h->qr=(h->qr+1)%NAH_QUEUE_FRAMES; h->qcount--; h->status.pcm_frames++;
  } else {
   /* Silent diagnostic drains producer too, avoiding queue blockage. */
   if (h->qcount) { h->qr=(h->qr+1)%NAH_QUEUE_FRAMES; h->qcount--; }
   h->status.silence_frames++; starved=1;
  }
  uint32_t word=(uint16_t)left | ((uint32_t)(uint16_t)right<<16);
  if (wr32(h,NAF_SRAM+4*((h->last_frame+i)%NAH_RING_FRAMES),word))
   return terminate(h,NAH_E_IO);
 }
 if (starved) h->status.underruns++;
 h->last_frame=current; h->last_us=now;
 h->status.consumed_frames+=consumed; h->status.queued_frames=h->qcount; h->status.ticks++;
 /* Every20ms inspect source-defined TDM fault/no-clock status. */
 if (!h->status.silent_only && !(h->status.ticks%4)) {
  if (lease(h,1)) return terminate(h,NAH_E_IO);
  int fault=0; h->bus.poll_limit=200; /* <=2ms, fit within DMA headroom */
  for (unsigned i=0;i<2;i++) {
   uint16_t v=0;
   if (amp_read(h,i,0x10,&v)) { fault=NAH_E_IO; break; }
   h->status.amp_status[i]=v;
   unsigned mask=(h->amps[i].rev&255)==0x74 ? 0xc0:0x500;
   if (v&mask) { fault=NAH_E_AMP_FAULT; break; }
  }
  h->bus.poll_limit=0;
  if (lease(h,0) && !fault) fault=NAH_E_IO;
  if (fault) return terminate(h,fault);
 }
 return 0;
}
int nau_hw_stop(struct nau_hw *h)
{
 if (!worker(h)) return NAH_E_WORKER;
 if (!h->status.running) return h->status.stop_error;
 int err=0;
 if (!h->status.silent_only) {
  if (lease(h,1)) err=NAH_E_IO;
  else {
   /* A refused preflight can latch offline before any controller write,
    * but output disable is still unverified for amps already running. */
   if (h->bus.offline) err=NAH_E_IO;
   /* A bus failure is sticky: no next-slave access after uncertainty. */
   for (unsigned i=0;i<2 && !h->bus.offline;i++)
    if (amp_cold(h,i) || nah_amp_lock(&bus_io,h,&h->bus,h->amps[i].addr)) { err=NAH_E_IO; if (h->bus.offline) break; }
  }
  if (lease(h,0)) err=NAH_E_IO;
 }
 /* Unknown amplifier write/transfer result must first force both local
  * amp reset GPIOs high, before any digital clock/DMA stop. Do not restore
  * those pins to inherited inputs in this permanently latched error state. */
 unsigned hold_reset=!h->status.silent_only && (h->bus.uncertain || err);
 if (hold_reset && reset_hold_fault(h)) err=NAH_E_IO;
 /* If amplifier access failed, stopping the owned digital source still
  * removes excitation. Never reset shared bus or panic iOS/WiFi. */
 if (naf_stream_close(&afe_io,h,&h->snapshot,&h->plan,&h->afe)) err=NAH_E_UNCERTAIN;
 if (!hold_reset && reset_restore(h)) err=NAH_E_IO;
 if (lease(h,1)) err=NAH_E_IO;
 else { if (bus_clock_restore(h)) err=NAH_E_IO; if (lease(h,0)) err=NAH_E_IO; }
 h->status.running=0; h->qcount=0; h->status.queued_frames=0;
 h->status.uncertain=h->afe.uncertain || h->bus.uncertain || !!err;
 h->status.stop_error=err;
 if (err && !h->status.error) h->status.error=err;
 return err;
}
void nau_hw_status(const struct nau_hw *h,struct nah_status *out)
{
 if (h && out) {
  *out=h->status;
  out->uncertain=h->status.uncertain || h->afe.uncertain || h->bus.uncertain;
  out->afe_phase=h->afe.phase; out->afe_op=h->afe.op;
  out->i2c_stage=h->bus.stage; out->i2c_failure=h->bus.failure;
  out->last_pa=h->afe.last_pa;
  out->i2c_last_pa=h->bus.last_addr; out->i2c_last_value=h->bus.last_value;
 }
}
#endif
