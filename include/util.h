
#define _XOPEN_SOURCE 600

#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdint.h>

#include <X11/Xlib.h>

/* Logging utility */
void log_event(const char *message);

void cleanup_resources(Display *display, Window window);

/* Endian conversion and testing (if needed for WAV handling) */
uint32_t swap_endian32(uint32_t value);
uint16_t swap_endian16(uint16_t value);
int is_big_endian(void);

/* Clamping values (e.g., volume, progress bar) */
float clamp(float value, float min, float max);

/* Logging functions*/
int init_logging(const char *filename);
void close_logging(void);

#endif
/* UTIL_H */
