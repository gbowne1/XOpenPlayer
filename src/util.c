#include "utils.h"
#include <time.h>
#include <stdint.h>
#include <stdio.h>

void log_event(const char *message) {
    FILE *log = fopen("application_log.txt", "a");
    if (log) {
        time_t now = time(NULL);
        struct tm *local = localtime(&now);
        fprintf(log, "[%02d:%02d:%02d] %s\n",
                local->tm_hour, local->tm_min, local->tm_sec, message);
        fclose(log);
    }
}

uint32_t swap_endian32(uint32_t value) {
    return ((value >> 24) & 0xFF) |
           ((value >> 8)  & 0xFF00) |
           ((value << 8)  & 0xFF0000) |
           ((value << 24) & 0xFF000000);
}

uint16_t swap_endian16(uint16_t value) {
    return (value >> 8) | (value << 8);
}

float clamp(float value, float min, float max) {
    return (value < min) ? min : (value > max) ? max : value;
}
