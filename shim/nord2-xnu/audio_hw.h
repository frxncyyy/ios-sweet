#ifndef NORD2_AUDIO_HW_H
#define NORD2_AUDIO_HW_H
#include <stdint.h>
#include "audio_afe.h"
#include "audio_i2c8.h"
#include "audio_amp.h"

#define NAH_RATE 48000u
#define NAH_RING_FRAMES (NAF_SRAM_BYTES / 4u)
#define NAH_QUEUE_FRAMES 8192u
#define NAH_MAX_GAIN_Q15 256u /* full-scale input <= -42dBFS at DMA */
#define NAH_MAX_TEST_US 2000000u
/* Failure values are stable public worker status, never a kernel panic. */
enum { NAH_OK=0, NAH_E_ARG=-1, NAH_E_WORKER=-2, NAH_E_IO=-3,
 NAH_E_COLD=-4, NAH_E_AMPS=-5, NAH_E_BOARD_CONFIG=-6,
 NAH_E_POINTER=-7, NAH_E_DEADLINE=-8, NAH_E_UNCERTAIN=-9,
 NAH_E_CLOCK_STALLED=-10, NAH_E_AMP_FAULT=-11, NAH_E_IDLE=-12 };
/* These callbacks are invoked exclusively by the owning audio worker.
 * read/write operate mapped physical registers, with a DSB before returning.
 * Access 0x11212000 through existing AFE map +0x2000; NEVER create a second
 * Device mapping of the same 16KiB physical page. No allocations in tick.
 * lease acquires/releases a shared I2C8 lease, independent of PMIF/touch.
 * It must serialize every other AP I2C8 user; CCU is checked by transport.
 * worker returns stable current-thread identity, 0 for invalid context. */
struct nah_io {
 int (*read)(void *, uint32_t, unsigned, uint32_t *);
 int (*write)(void *, uint32_t, unsigned, uint32_t);
 int (*delay_us)(void *, unsigned);
 uintptr_t (*worker)(void *);
 int (*lease)(void *, unsigned acquire);
};
/* Board profile cannot be supplied by USB/userspace. Immutable boot ADT data,
 * verified against the pinned Nord2 container and observed amp REV is required.
 * apply runs source-backed volatile init + exact board profile while PWDN=1,
 * AMPE=0; it must not touch OTP/MTP or rail bucks. The amp
 * control path still performs independent mute/clock/readback checks.
 * This callback boundary avoids manufacturing unknown analog settings. */
struct nah_board {
 uint16_t revision[2];
 uint8_t source_sha256[32];
 int (*apply)(void *, const struct na8_io *, struct na8_state *, unsigned amp);
 int (*supports)(uint16_t revision, unsigned slave);
};
struct nah_status {
 unsigned initialized, running, silent_only, uncertain, gate, retained_shared_clock;
 unsigned amp_reset_held; /* uncertain amp transport: local GPIO reset latched high */
 unsigned afe_phase, afe_op, i2c_stage, i2c_failure;
 uint32_t last_pa;
 int error, stop_error, reset_error;
 uint32_t current_pa, consumed_frames, queued_frames, fed_frames;
 uint32_t pcm_frames, silence_frames, underruns, ticks, max_tick_gap_us;
 uint16_t amp_revision[2], amp_status[2];
};
struct nau_hw {
 struct nah_io io; void *cookie; uintptr_t owner;
 const struct nah_board *board;
 struct nau_state snapshot;
 struct naf_amp_evidence evidence[2];
 struct na8_state bus;
 struct na8_amp amps[2];
 struct naf_plan plan;
 struct naf_state afe;
 struct nah_status status;
 int16_t queue[NAH_QUEUE_FRAMES * 2u];
 uint32_t qr, qw, qcount, last_frame, idle_ticks;
 uint64_t last_us, deadline_us, last_feed_us, last_progress_us;
 unsigned gain_q15, leased, first_tick, new_input, reset_changed, bus_clock_prepared, bus_clock_changed, pointer_waiting;
};
/* h must be static/heap, zeroed once per boot; never place on kernel stack.
 * init snapshots original reset GPIO111/148 fields, checks coldAFE and idlebus,
 * then source reset pulse high5ms/low5ms when pinsareGPIO. Stop restores saved
 * direction/latch fields; no controller reset or bitbang ever occurs. */
int nau_hw_init(struct nau_hw *, const struct nah_io *, void *, const struct nah_board *);
/* silent_only=1 proves AFE without amp enable; normal start returns
 * NAH_E_BOARD_CONFIG with zero hardware writes if board is missing.
 * Both modes run actual continuous DMA and tick. duration_us=0 is continuous
 * HAL playback with 750ms input-idle watchdog; nonzero tests are capped2s.
 * rate must be48000/channels2/bits16. gain0..256 scales queue PCM; no amp gain
 * guessed or modified. now_us is monotonic microseconds from worker clock. */
int nau_hw_start(struct nau_hw *, unsigned rate, unsigned channels, unsigned bits,
 unsigned silent_only, unsigned gain_q15, unsigned duration_us, uint64_t now_us);
/* feed returns accepted frames (0 if full), or negative error. Only worker;
 * producer/devfs queues its data externally before this call. */
int nau_hw_feed(struct nau_hw *, const int16_t *, unsigned frames);
/* Call at <=5ms cadence. >=20ms gap stops before overwriting ambiguous ring.
 * First tick establishes the post-startup pointer/time baseline while DMA
 * still contains only zeros (amp startup can take bounded50ms polls).
 * tick writes only consumed frames behind DMA, prefilled zero ring is21.33ms.
 * Starvation writes zeros (never leaves stale previously consumed content).
 * Nonzero terminal result requires reporting status; no automatic restart. */
int nau_hw_tick(struct nau_hw *, uint64_t now_us);
int nau_hw_stop(struct nau_hw *);
void nau_hw_status(const struct nau_hw *, struct nah_status *);
#endif
