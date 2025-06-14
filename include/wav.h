#ifndef WAV_H
#define WAV_H

#include <stdio.h>
#include <stdint.h>

// WAV file header structure
typedef struct {
    char riff[4];
    uint32_t file_size;
    char wave[4];
    char fmt[4];
    uint32_t fmt_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    char data[4];
    uint32_t data_size;
} WAVHeader;

// Function declarations
void read_wav_header(FILE *file, WAVHeader *header);
void *read_pcm_data(FILE *file, uint32_t data_size);
void play_wav(const char *filename);

#endif // WAV_H
