
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
#include "util.h"
#include "player.h"
#include "audio.h"

void read_wav_header(FILE *file, WAVHeader *header) {
    fread(header, sizeof(WAVHeader), 1, file);

    if (strncmp(header->riff, "RIFF", 4) != 0 || strncmp(header->wave, "WAVE", 4) != 0) {
        fprintf(stderr, "Error: Not a valid WAV file!\n");
        exit(1);
    }

    if (is_big_endian()) {
        header->file_size = swap_endian32(header->file_size);
        header->fmt_size = swap_endian32(header->fmt_size);
        header->audio_format = swap_endian16(header->audio_format);
        header->num_channels = swap_endian16(header->num_channels);
        header->sample_rate = swap_endian32(header->sample_rate);
        header->byte_rate = swap_endian32(header->byte_rate);
        header->block_align = swap_endian16(header->block_align);
        header->bits_per_sample = swap_endian16(header->bits_per_sample);
        header->data_size = swap_endian32(header->data_size);
    }

    printf("WAV Loaded: %d Channels, %d Hz, %d-bit\n",
           header->num_channels, header->sample_rate, header->bits_per_sample);
    log_event("WAV header loaded");
}

void *read_pcm_data(FILE *file, uint32_t data_size) {
    void *buffer = malloc(data_size);
    if (!buffer) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        log_event("Failed to allocate memory for PCM data");
        return NULL;
    }
    if (fread(buffer, data_size, 1, file) != 1) {
        fprintf(stderr, "Error: Failed to read PCM data\n");
        log_event("Failed to read PCM data");
        free(buffer);
        return NULL;
    }
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

    // Simulate seeking based on player_state.progress
    if (player_state.progress > 0.0f && player_state.progress <= 1.0f) {
        uint32_t offset = (uint32_t)(header.data_size * player_state.progress);
        // Ensure offset is aligned to block_align
        offset -= offset % header.block_align;
        if (fseek(file, offset, SEEK_CUR) != 0) {
            fprintf(stderr, "Error: Failed to seek in WAV file\n");
            log_event("Failed to seek in WAV file");
            fclose(file);
            return;
        }
        header.data_size -= offset; // Adjust data_size for remaining data
    }

    pcm_data = read_pcm_data(file, header.data_size);
    if (!pcm_data) {
        fclose(file);
        return;
    }

    // Simulate playback (no audio backend)
    printf("Simulating playback of %s: %u bytes, %d Hz, %d-bit, %d channels\n",
           filename, header.data_size, header.sample_rate, header.bits_per_sample, header.num_channels);
    log_event("Simulating WAV playback");

    // Pass data to audio_play (stubbed)
    audio_play(pcm_data, header.data_size, header.sample_rate, header.bits_per_sample, header.num_channels);

    free(pcm_data);
    fclose(file);
}
