
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

#include "audio.h"
#include <stdio.h>

int audio_init(void) {
    printf("Audio initialized (stub)\n");
    return 0;
}

int audio_play(const void *data, uint32_t size, uint16_t sample_rate, uint16_t bits_per_sample, uint16_t channels) {
    printf("Playing audio (stub): %u bytes\n", size);
    return 0;
}

void audio_stop(void) {
    printf("Audio stopped (stub)\n");
}

void audio_pause(void) {
    printf("Audio paused (stub)\n");
}

void audio_resume(void) {
    printf("Audio resumed (stub)\n");
}

void audio_set_volume(float volume) {
    printf("Volume set to %.2f (stub)\n", volume);
}

void audio_cleanup(void) {
    printf("Audio cleaned up (stub)\n");
}
