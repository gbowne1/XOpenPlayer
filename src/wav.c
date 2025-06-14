#include "wav.h"
#include <stdlib.h>
#include <string.h>

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
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error: Could not open WAV file!\n");
        return;
    }

    WAVHeader header;
    read_wav_header(file, &header);
    void *pcm_data = read_pcm_data(file, header.data_size);
    
    // Playback logic using an audio library like PortAudio, SDL2, etc.

    free(pcm_data);
    fclose(file);
}
