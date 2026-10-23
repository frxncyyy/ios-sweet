#include "audio_stream.h"
/* 10^(-0.125/20), Q30. Exponentiation keeps attenuation error below one
 * sample-scale LSB; no floating point in the kernel or approximation in dB. */
unsigned nsa_gain(unsigned below) {
 if (below >= 768u) return 0;
 uint64_t result = UINT64_C(1) << 30, base = 1058400094u;
 while (below) {
  if (below & 1u) result = (result * base + (UINT64_C(1) << 29)) >> 30;
  base = (base * base + (UINT64_C(1) << 29)) >> 30;
  below >>= 1;
 }
 return (unsigned)((result + (1u << 14)) >> 15);
}
void nsa_initialize(struct nsa_stream *s, unsigned enabled) {
 if (!s) return;
 *s = (struct nsa_stream){0}; s->enabled = !!enabled; s->gain_q15 = 32768;
}
void nsa_flush(struct nsa_stream *s) {
 if (!s) return;
 s->read = s->write = s->count = 0; s->flushes++;
}
void nsa_flush_idle(struct nsa_stream *s, uint64_t now) {
 if (!s) return;
 uint64_t sound=s->last_sound_us;
 if (!sound || (now>=sound && now-sound>=750000)) nsa_flush(s);
}
void nsa_enable(struct nsa_stream *s, unsigned enabled) {
 if (!s) return;
 s->enabled = !!enabled;
 if (!s->enabled) nsa_flush(s);
}
void nsa_volume(struct nsa_stream *s, unsigned below, unsigned muted) {
 if (!s) return;
 s->below = below; s->muted = !!muted;
 s->gain_q15 = muted ? 0 : nsa_gain(below); s->volume_generation++;
 /* Samples queued at the old volume must not play after an immediate mute. */
 if (s->muted) nsa_flush(s);
}
unsigned nsa_enqueue(struct nsa_stream *s, const int16_t *p, unsigned n, uint64_t now) {
 if (!s || (!p && n) || n > NSA_QUEUE_FRAMES) return 0;
 unsigned take = n, peak = 0;
 if (!s->enabled || s->muted) take = 0;
 else if (take > NSA_QUEUE_FRAMES - s->count) take = NSA_QUEUE_FRAMES - s->count;
 s->writes++; s->written += n; s->accepted += take; s->dropped += n - take;
 for (unsigned i = 0; i < take; i++) {
  for (unsigned c = 0; c < 2; c++) {
   int32_t x = p[i * 2 + c]; unsigned level = x < 0 ? (unsigned)-x : (unsigned)x;
   if (level > peak) peak = level;
   s->pcm[s->write * 2 + c] = (int16_t)x;
  }
  s->write = (s->write + 1u) % NSA_QUEUE_FRAMES;
 }
 s->count += take;
 if (peak > 8u) { s->nonzero += take; s->last_sound_us = now; }
 if (peak > s->peak) s->peak = peak;
 return take;
}
unsigned nsa_dequeue(struct nsa_stream *s, int16_t *out, unsigned cap) {
 if (!s || !out) return 0;
 unsigned n = cap < s->count ? cap : s->count;
 for (unsigned i = 0; i < n; i++) {
  for (unsigned c = 0; c < 2; c++) {
   int32_t x = s->pcm[s->read * 2 + c];
   /* Division rounds negative samples towards zero, just as positive ones. */
   out[i * 2 + c] = (int16_t)((x * (int32_t)s->gain_q15) / 32768);
  }
  s->read = (s->read + 1u) % NSA_QUEUE_FRAMES;
 }
 s->count -= n; s->delivered += n; return n;
}
