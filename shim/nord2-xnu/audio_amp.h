#ifndef NORD2_AUDIO_AMP_H
#define NORD2_AUDIO_AMP_H
#include "audio_i2c8.h"
/* Selected tfa_init.c silicon errata only. This does NOT replace the board
 * container/profile. Must run PWDN1/AMPE0, shared I2C8 lease held, sole worker.
 * Protected volatile settings require source key unlock0f/fb/a0/a1; a1 is
 * a volatile key latch, not OTP programming. Never writes a3/a7/a8 or any
 * MTP row/copy/start register. Caller must close key2 after board application using nah_amp_lock(). */
int nah_amp_cold_init(const struct na8_io *, void *, struct na8_state *,
 unsigned slave, uint16_t revision);
int nah_amp_lock(const struct na8_io *, void *, struct na8_state *, unsigned slave);
#endif
