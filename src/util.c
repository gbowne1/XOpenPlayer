
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
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "util.h"

typedef enum {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_DEBUG
} LogLevel;

static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
static FILE *log_file = NULL;

int init_logging(const char *filename) {
    log_file = fopen(filename, "a");
    if (!log_file) {
        fprintf(stderr, "ERROR: Unable to open log file: %s\n", filename);
        return -1;
    }
    return 0;
}

void log_event(const char *message) {
    time_t now;
    time(&now);
    char *time_str = ctime(&now);
    time_str[strlen(time_str) - 1] = '\0';

    pthread_mutex_lock(&log_mutex);
    
    struct tm *local = localtime(&now);
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
    pthread_mutex_unlock(&log_mutex);
}

void close_logging() {
    if (log_file) {
        fclose(log_file);
        log_file = NULL;
    }
}

void cleanup_resources(Display *display, Window window) {
    if (window) {
        XDestroyWindow(display, window);
    }
    if (display) {
        XCloseDisplay(display);
    }
    log_event("Resources cleaned up.");
}

uint32_t swap_endian32(uint32_t value) {
    uint32_t ret = ((value >> 24) & 0xFF) |
                   ((value >> 8)  & 0xFF00) |
                   ((value << 8)  & 0xFF0000) |
                   ((value << 24) & 0xFF000000);
    return ret;
}

uint16_t swap_endian16(uint16_t value) {
    uint16_t ret = (value >> 8) | (value << 8);
    return ret;
}

float clamp(float value, float min, float max) {
    float r0 = (value > max) ? max : value;
    float r1 = (value < min) ? min : r0;
    return r1;
} 
