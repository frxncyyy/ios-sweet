/* SPDX-License-Identifier: MIT */
#ifndef NORD2_AUDIO_INSTALL_H
#define NORD2_AUDIO_INSTALL_H
/* Call on EVERY boot after root mount and before load_init_program, even with
 * no audio bootarg. Zero: safe to proceed. Nonzero: reject launchd and enter
 * parent-selected recovery policy; console log records phase/error/recovery. */
int nord2_audio_install(void);
#endif
