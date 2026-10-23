#ifndef NORD2_AUDIO_BOARD_H
#define NORD2_AUDIO_BOARD_H
#include <stddef.h>
#include <stdint.h>
struct nah_board;
/* Caller computes digest from exactly blob[0..length). Lifetime only this call.
 * Once loaded immutable for boot; failed load leaves getterNULL. Worker only
 * starts after this initialization on the same thread. */
int nah_board_load(const uint8_t *blob,size_t length,const uint8_t digest[32]);
const struct nah_board *nah_board_profile(void);
int nah_board_register_allowed(unsigned reg);
int nah_board_change_allowed(unsigned reg,uint16_t old,uint16_t next);
#endif
