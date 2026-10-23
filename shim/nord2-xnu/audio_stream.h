#ifndef NORD2_AUDIO_STREAM_H
#define NORD2_AUDIO_STREAM_H
#include <stdint.h>
#define NSA_RATE 48000u
#define NSA_CHANNELS 2u
#define NSA_FRAME_BYTES 4u
#define NSA_QUEUE_FRAMES 8192u
/* The native wrapper serializes these operations with one short mutex. No
 * allocation, hardware access or blocking operation belongs in this queue. */
struct nsa_stream {
 int16_t pcm[NSA_QUEUE_FRAMES * NSA_CHANNELS];
 uint32_t read, write, count, enabled, below, muted, gain_q15;
 uint64_t written, accepted, dropped, delivered, nonzero, last_sound_us;
 uint32_t peak, opens, writes, volume_generation, flushes;
};
void nsa_initialize(struct nsa_stream *, unsigned enabled);
unsigned nsa_enqueue(struct nsa_stream *, const int16_t *, unsigned frames, uint64_t now_us);
unsigned nsa_dequeue(struct nsa_stream *, int16_t *, unsigned capacity);
void nsa_enable(struct nsa_stream *, unsigned enabled);
void nsa_volume(struct nsa_stream *, unsigned eighth_db_below, unsigned muted);
void nsa_flush(struct nsa_stream *);
unsigned nsa_gain(unsigned eighth_db_below);
#endif
