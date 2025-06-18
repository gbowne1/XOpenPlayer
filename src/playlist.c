#include "playlist.h"
#include "player.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static const char* determine_format(const char* filename) {
    const char* ext = strrchr(filename, '.');
    if (!ext) return NULL;
    
    if (strcasecmp(ext, ".m3u") == 0) return "m3u";
    if (strcasecmp(ext, ".pls") == 0) return "pls";
    if (strcasecmp(ext, ".wpl") == 0) return "wpl";
    if (strcasecmp(ext, ".asx") == 0) return "asx";
    if (strcasecmp(ext, ".cue") == 0) return "cue";
    
    return NULL;
}

// Private format handlers
static int load_m3u_internal(const char* filename);
static int load_pls(const char* filename);
static int load_wpl(const char* filename);

// M3U format support
static int load_m3u(const char* filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        handle_playlist_error("Failed to open M3U file");
        return 0;
    }
    
    char line[1024];
    int track_count = 0;
    
    while (fgets(line, sizeof(line), f)) {
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n') continue;
        
        // Parse track information
        char title[MAX_TITLE_LEN] = {0};
        int duration = 0;
        
        // Extract metadata if available
        if (strstr(line, "#EXTINF:") == line) {
            // Parse extended info format
            sscanf(line, "#EXTINF:%d,%[^\n]", &duration, title);
        } else {
            // Basic filename format
            strncpy(title, line, sizeof(title) - 1);
            title[sizeof(title) - 1] = '\0'; // Ensure null termination
        }
        
        // Add to track list
        if (track_count < MAX_TRACKS) {
            strncpy(player_state.tracks[track_count].title, title, sizeof(title) - 1);
            player_state.tracks[track_count].title[sizeof(title) - 1] = '\0'; // Ensure null termination
            player_state.tracks[track_count].duration_seconds = duration;
            track_count++;
        }
    }
    
    fclose(f);
    return track_count;
}

static int load_pls(const char* filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        handle_playlist_error("Failed to open PLS file");
        return 0;
    }
    
    char line[1024];
    int track_count = 0;
    
    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, "File") == line) {
            char title[MAX_TITLE_LEN] = {0};
            if (sscanf(line, "File%d=%[^\n]", &track_count, title) != 2) {
                handle_playlist_error("Failed to parse track information in PLS file");
                fclose(f);
                return 0;
            }
            
            if (track_count < MAX_TRACKS) {
                strncpy(player_state.tracks[track_count].title, title, sizeof(title) - 1);
                player_state.tracks[track_count].title[sizeof(title) - 1] = '\0'; // Ensure null termination
                track_count++;
            }
        }
    }
    
    fclose(f);
    return track_count;
}

static int load_wpl(const char* filename) {
    // Implementation would use an XML parser
    // For simplicity, we'll use basic string parsing
    FILE *f = fopen(filename, "r");
    if (!f) {
        handle_playlist_error("Failed to open WPL file");
        return 0;
    }
    
    char line[1024];
    int track_count = 0;
    
    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, "<media:") == line) {
            // Extract media source
            char title[MAX_TITLE_LEN] = {0};
            sscanf(line, "<media:content url=\"%[^\"]", title);
            
            // Add to track list
            if (track_count < MAX_TRACKS) {
                strncpy(player_state.tracks[track_count].title, title, sizeof(title) - 1);
                player_state.tracks[track_count].title[sizeof(title) - 1] = '\0'; // Ensure null termination
                track_count++;
                        }
        }
    }
    
    fclose(f);
    return track_count;
}

// Public functions
int load_playlist(const char* filename) {
    const char* format = determine_format(filename);
    if (!format) return 0;
    
    player_state.track_count = 0;
    
    if (strcmp(format, "m3u") == 0) {
        return load_m3u(filename);
    } else if (strcmp(format, "pls") == 0) {
        return load_pls(filename);
    } else if (strcmp(format, "wpl") == 0) {
        return load_wpl(filename);
    }
    // Add other format handlers as needed
    
    return 0;
}

static void handle_playlist_error(const char* message) {
    log_event(message);
    fprintf(stderr, "Error: %s\n", message);
    player_state.track_count = 0;
}
