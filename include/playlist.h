
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

#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <stdio.h>

#define MAX_TITLE_LEN 256
#define MAX_ARTIST_LEN 256
#define MAX_TRACKS 100

struct Track {
    char title[MAX_TITLE_LEN];
    int duration_seconds; // Duration in seconds
};

struct PlayerState {
    struct Track tracks[MAX_TRACKS];
    int track_count;
};

extern struct PlayerState player_state;

// Public playlist functions
int load_playlist(const char* filename);
int load_m3u(const char* filename);
int load_pls(const char* filename);
int load_wpl(const char* filename);

#endif // PLAYLIST_H
