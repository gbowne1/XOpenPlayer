/*********************************************************************
 * The Open Group Base Specifications Issue 6
 * IEEE Std 1003.1, 2004 Edition
 *
 *    An XSI-conforming application should ensure that the feature
 *    test macro _XOPEN_SOURCE is defined with the value 600 before
 *    inclusion of any header. This is needed to enable the
 *    functionality described in The _POSIX_C_SOURCE Feature Test
 *    Macro and in addition to enable the XSI extension.
 *
 *********************************************************************/
#if ! defined (_XOPEN_SOURCE)
#define _XOPEN_SOURCE 600
#endif

#ifndef AUDIO_H
#define AUDIO_H

#include <stdint.h>

/* Initialize audio backend (if needed) */
int audio_init(void);

/* Play PCM data */
int audio_play(const void *data, uint32_t size, uint16_t sample_rate, uint16_t bits_per_sample, uint16_t channels);

/* Stop audio playback */
void audio_stop(void);

/* Pause audio */
void audio_pause(void);

/* Resume playback */
void audio_resume(void);

/* Set playback volume (0.0 - 1.0) */
void audio_set_volume(float volume);

/* Cleanup any audio backend or resources */
void audio_cleanup(void);

#endif /* AUDIO_H */
