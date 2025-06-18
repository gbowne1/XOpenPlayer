
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

#include <stdlib.h>
#include <string.h>
#include "wav.h"

void read_wav_header(FILE *file, WAVHeader *header) {
    fread(header, sizeof(WAVHeader), 1, file);

    if (strncmp(header->riff, "RIFF", 4) != 0 || strncmp(header->wave, "WAVE", 4) != 0) {
        fprintf(stderr, "Error: Not a valid WAV file!\n");
        exit(1);
    }

    printf("WAV Loaded: %d Channels, %d Hz, %d-bit\n",
           header->num_channels, header->sample_rate, header->bits_per_sample);
}

void *read_pcm_data(FILE *file, uint32_t data_size) {
    void *buffer = malloc(data_size);
    fread(buffer, data_size, 1, file);
    return buffer;
}

void play_wav(const char *filename) {
    WAVHeader header;
    void *pcm_data;
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error: Could not open WAV file!\n");
        return;
    }

    read_wav_header(file, &header);
    pcm_data = read_pcm_data(file, header.data_size);

    /* Playback logic using an audio library like PortAudio, SDL2, etc. */

    free(pcm_data);
    fclose(file);
}

