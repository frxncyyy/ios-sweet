#ifndef NORD2_AUDIO_STREAM_H
#define NORD2_AUDIO_STREAM_H
#include <stdint.h>
#define NSA_RATE 48000u
#define NSA_CHANNELS 2u
#define NSA_FRAME_BYTES 4u
/* Pinned HAL advertises up to16384 frames per period. Retain two
 * periods across bounded cold startup; writes remain nonblocking. */
#define NSA_QUEUE_FRAMES 32768u
/* The native wrapper serializes these operations with one short mutex. No
 * allocation, hardware access or blocking operation belongs in this queue. */
struct nsa_stream {
 int16_t pcm[NSA_QUEUE_FRAMES * NSA_CHANNELS];
 uint32_t read, write, count, enabled, below, muted, gain_q15, sound_frames;
 uint64_t written, accepted, dropped, delivered, nonzero, last_sound_us;
 uint32_t peak, opens, writes, volume_generation, flushes;
};
void nsa_initialize(struct nsa_stream *, unsigned enabled);
unsigned nsa_enqueue(struct nsa_stream *, const int16_t *, unsigned frames, uint64_t now_us);
unsigned nsa_dequeue(struct nsa_stream *, int16_t *, unsigned capacity);
void nsa_enable(struct nsa_stream *, unsigned enabled);
void nsa_volume(struct nsa_stream *, unsigned eighth_db_below, unsigned muted);
void nsa_flush(struct nsa_stream *);
/* Caller holds the producer lock and samples now while holding it. */
void nsa_flush_idle(struct nsa_stream *, uint64_t now_us);
/* Producer lock held. pending_output describes downstream PCM; drain_us
 * additionally covers the SRAM ring after the last queued sound leaves it. */
int nsa_idle_ready(const struct nsa_stream *, uint64_t now_us,
 unsigned pending_output, uint64_t *last_pending_us, uint64_t drain_us);
unsigned nsa_gain(unsigned eighth_db_below);
#endif
