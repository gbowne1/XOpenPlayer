
#define _XOPEN_SOURCE 600

#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdint.h>

/* Logging utility */
void log_event(const char *message);

/* Endian conversion (if needed for WAV handling) */
uint32_t swap_endian32(uint32_t value);
uint16_t swap_endian16(uint16_t value);

/* Clamping values (e.g., volume, progress bar) */
float clamp(float value, float min, float max);

#endif
/* UTIL_H */
