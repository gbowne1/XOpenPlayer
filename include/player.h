
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

#ifndef PLAYER_H
#define PLAYER_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>

/* UI Constants
 * =====================
 * UI Layout Constants
 * =====================
 */
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
#define MAX_TITLE_LEN 256
#define MAX_ARTIST_LEN 256

/* =====================
 * Keyboard Input Constants
 * =====================
 */
#define KEY_PLAY            65   /* Spacebar */
#define KEY_STOP            39   /* 'S' key */
#define KEY_NEXT            57   /* 'N' key */
#define KEY_PREV            55   /* 'P' key */
#define KEY_VOL_UP          111  /* Up arrow */
#define KEY_VOL_DOWN        116  /* Down arrow */

/* =====================
 * Window Dimensions
 * =====================
 */
#define WINDOW_WIDTH        800
#define WINDOW_HEIGHT       600

typedef struct {
    GC gc;
    Font font;
    XFontStruct *font_info;
    Atom wm_delete_window;
} ResourceManager;

typedef struct {
    char title[MAX_TITLE_LEN];
    char artist[MAX_ARTIST_LEN];

    /* For display/progress bar */
    int duration_seconds;
} TrackInfo;

/* Player state structure */
typedef struct PlayerState {
    int playing;
    float volume;
    float progress;
    int current_track;
    int track_count;
    TrackInfo tracks[MAX_TRACKS];
} PlayerState;

/* Core player functions */
void init_player(Display *display, Window window);
void draw_menu(Display *display, Window window, int width);
void display_welcome_message(Display *display, Window window, int width);

extern PlayerState player_state;

/* Event handlers */
void handle_keypress(XKeyEvent *event, Display *display, Window window, int window_width);
void handle_mouse_click(XButtonEvent *event, Display *display, Window window, int window_width);
void cleanup_resources(Display *display, Window window);
void cleanup_player(Display *display);
void handle_error(int error_code, Display *display);
void handle_resize(Display *display, Window window, int new_width, int new_height);

/* Playback control functions */
void play(void);
void player_pause(void);
void stop(void);
void next_track(Display *display, Window window, int window_width);
void previous_track(Display *display, Window window, int window_width);

/* UI drawing functions */
void draw_button(Display *display, Window window, int x, int y, const char *label);
void draw_progress_bar(Display *display, Window window, int width);
void draw_volume_control(Display *display, Window window);
void draw_player_controls(Display *display, Window window, int width);

void handle_mouse_release(XButtonEvent *event);
void handle_mouse_motion(XMotionEvent *event, int window_width);

void display_track_info(Display *display, Window window, int width);
void load_tracks(const char *filename);
#endif
/* PLAYER_H */
