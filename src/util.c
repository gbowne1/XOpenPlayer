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

#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include "utils.h"

void log_event(const char *message) {
    time_t now;
    struct tm *local;
    FILE *log = fopen("application_log.txt", "a");
    if (log) {
        now = time(NULL);
        local = localtime(&now);
        fprintf(log, "[%02d:%02d:%02d] Event: %s\n",
                local->tm_hour, local->tm_min, local->tm_sec, message);
        fclose(log);
    } else {
        fprintf(stderr, "WARN: Unable to open log file.\n");
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