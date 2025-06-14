#ifndef PLAYER_H
#define PLAYER_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>

// UI Constants
// =====================
// UI Layout Constants
// =====================
#define BUTTON_WIDTH        100
#define BUTTON_HEIGHT       30
#define BUTTON_SPACING      20
#define BUTTON_Y            150

#define MENU_HEIGHT         40
#define MENU_ITEM_WIDTH     100

#define PROGRESS_BAR_HEIGHT 10
#define PROGRESS_BAR_Y      250

#define VOLUME_BAR_WIDTH    100
#define VOLUME_BAR_HEIGHT   5
#define VOLUME_BAR_Y        280

#define MAX_TRACKS 100
#define MAX_TITLE_LEN 128
#define MAX_ARTIST_LEN 64

// =====================
// Keyboard Input Constants
// =====================
#define KEY_PLAY            65   // Spacebar
#define KEY_STOP            39   // 'S' key
#define KEY_NEXT            57   // 'N' key
#define KEY_PREV            55   // 'P' key
#define KEY_VOL_UP          111  // Up arrow
#define KEY_VOL_DOWN        116  // Down arrow

// =====================
// Window Dimensions
// =====================
#define WINDOW_WIDTH        800
#define WINDOW_HEIGHT       600

typedef struct {
    char title[MAX_TITLE_LEN];
    char artist[MAX_ARTIST_LEN];
    int duration_seconds; // For display/progress bar
} TrackInfo;

// Player state structure
typedef struct PlayerState {
    int playing;
    float volume;
    float progress;
    int current_track;
    int track_count;
    TrackInfo tracks[MAX_TRACKS];
} PlayerState;


void log_event(const char *message);

// Core player functions
void init_player(Display *display, Window window);
void draw_menu(Display *display, Window window, int width);
void display_welcome_message(Display *display, Window window, int width);
// Event handlers
void handle_keypress(XKeyEvent *event, Display *display, Window window, int window_width);
void handle_mouse_click(XButtonEvent *event);
void cleanup_player(Display *display);

void handle_resize(int new_width, int new_height);

// Playback control functions
void play(void);
void pause(void);
void stop(void);
void next_track(Display *display, Window window, int window_width);
void previous_track(Display *display, Window window, int window_width);

// UI drawing functions
void draw_button(Display *display, Window window, int x, int y, const char *label);
void draw_progress_bar(Display *display, Window window, int width);
void draw_volume_control(Display *display, Window window);
void draw_player_controls(Display *display, Window window, int width);

void handle_mouse_release(XButtonEvent *event);
void handle_mouse_motion(XMotionEvent *event, int window_width);

#endif // PLAYER_H