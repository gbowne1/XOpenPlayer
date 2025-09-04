
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

#include <X11/Xlib.h>
#include <X11/Xutil.h>
/* #include <X11/extensions/XShm.h>
 * #include <X11/XKBlib.h>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "player.h"
#include "util.h"

/* Initial state */
/* the wild Greek has proclaimed that we can get away from this
static PlayerState player_state = {0, 0.5f, 0.0f};
*/
PlayerState player_state = {.progress=0.5f};

static GC global_gc;
static Font global_font;

/* Font metadata */
static XFontStruct *font_info;

/* should this not be an extern instead of static */
static int dragging_volume = 0;
static int dragging_progress = 0;

void init_player(Display *display, Window window) {
    /* Initialize player state */
    player_state.playing = 0;
    player_state.volume = 0.5f;
    player_state.progress = 0.0f;

    /* Create global graphics context */
    global_gc = XCreateGC(display, window, 0, NULL);
    if (!global_gc) {
        log_event("FAIL: create graphics context");
        fprintf(stderr, "FAIL: create graphics context\n");
        exit(EXIT_FAILURE);
    }

    /* Load a global font */
    global_font = XLoadFont(display, "fixed");
    font_info = XQueryFont(display, global_font);
    if (!font_info) {
        log_event("FAIL: load font");
        fprintf(stderr, "FAIL: load font\n");
        XFreeGC(display, global_gc);
        exit(EXIT_FAILURE);
    }

    log_event("Player initialized");
}


void cleanup_player(Display *display) {
    if (font_info) {
        XFreeFont(display, font_info);
        XUnloadFont(display, global_font);
        log_event("Font unloaded");
    }
    XFreeGC(display, global_gc);
    log_event("Graphics context freed");
}

void draw_button(Display *display, Window window, int x, int y, const char *label) {
    int text_width;

    /* Draw button background and used fixed size 100 x 30 */
    XSetForeground(display, global_gc, BlackPixel(display, DefaultScreen(display)));
    XFillRectangle(display, window, global_gc,
                   x, y, 100, 30);

    /* Draw button border */
    XSetForeground(display, global_gc, WhitePixel(display, DefaultScreen(display)));
    XDrawRectangle(display, window, global_gc,
                   x, y, 100, 30);

    /* Draw button label */
    text_width = XTextWidth(font_info, label, (int)strlen(label));
    XDrawString(display, window, global_gc,
                   x + (100 - text_width) / 2,
                   y + 20, label, (int)strlen(label));
}

void draw_progress_bar(Display *display, Window window, int width) {
    /* the window width may be dynamic */
    unsigned int progress_width = width - 100;
    int progress_x = 50;

    /* Draw progress bar background */
    XSetForeground(display, global_gc,
                   BlackPixel(display, DefaultScreen(display)));

    XFillRectangle(display, window, global_gc,
                   progress_x, 250, progress_width, 10);

    /* Draw progress */
    XSetForeground(display, global_gc,
                   WhitePixel(display, DefaultScreen(display)));

    XFillRectangle(display, window, global_gc,
                   progress_x, 250,
                   (unsigned int)(progress_width * player_state.progress), 10);
}

void draw_volume_control(Display *display, Window window) {
    int x = 50;

    /* Draw volume bar background */
    XSetForeground(display, global_gc,
                   BlackPixel(display, DefaultScreen(display)));

    XFillRectangle(display, window, global_gc,
                   x, 280, 100, 5);

    /* Draw current volume level */
    XSetForeground(display, global_gc,
                   WhitePixel(display, DefaultScreen(display)));

    XFillRectangle(display, window, global_gc,
                   x, 280, (int)(100 * player_state.volume), 5);

    /* Draw volume label at 6 chars wide */
    XDrawString(display, window, global_gc,
                x, 270, "Volume", 6);
}

void draw_player_controls(Display *display, Window window, int width) {
    int total_button_width = (BUTTON_WIDTH * 3) + (BUTTON_SPACING * 2);
    int start_x = (width - total_button_width) / 2;

    draw_button(display, window,
                start_x, BUTTON_Y, "Play");

    draw_button(display, window,
                start_x + BUTTON_WIDTH + BUTTON_SPACING, BUTTON_Y, "Pause");

    draw_button(display, window,
                start_x + 2 * (BUTTON_WIDTH + BUTTON_SPACING), BUTTON_Y, "Stop");

    draw_progress_bar(display, window, width);
    draw_volume_control(display, window);
}

/* void draw_menu(Display *display, Window window, int width) {
    Font font;
    int j, text_width, x_position;
    const char *menu_items[] = {"File", "Edit", "Help"};

    //  Create and set up graphics context //
    GC gc = XCreateGC(display, window, 0, NULL);
    if (!gc) {
        log_event("FAIL: create graphics context for menu");
        return;
    }

    /* Set up font 
    font = XLoadFont(display, "fixed");
    if (!font) {
        XFreeGC(display, gc);
        log_event("FAIL: load font");
      return;
    }

    //  Set background and text colors
    XSetBackground(display, gc,
                   WhitePixel(display, DefaultScreen(display)));

    XSetForeground(display, gc,
                   BlackPixel(display, DefaultScreen(display)));

    // Draw menu background 
    XFillRectangle(display, window, gc,
                   0, 0, width, MENU_HEIGHT);

	// Got to set a white foreground text for 
	// the subsequent XDrawString call below 
	XSetForeground(display, gc,
                   WhitePixel(display, DefaultScreen(display)));

    // Draw the 3 menu items
    for (j = 0; j < 3; j++) {

        text_width = XTextWidth(font_info, menu_items[j], strlen(menu_items[j]));
        x_position = j * MENU_ITEM_WIDTH + (MENU_ITEM_WIDTH - text_width) / 2;

        XDrawString(display, window, gc,
                    x_position, 25,
                    menu_items[j], strlen(menu_items[j]));

    }

    // Cleanup
    XUnloadFont(display, font);
    // obvious bug */
    /* XFreeFont(display, &font); *
    XFreeGC(display, gc);

} */

/* New draw_menu test this out commented out the old one for posterity */
void draw_menu(Display *display, Window window, int width) {
    int j, text_width, x_position;
    const char *menu_items[] = {"File", "Edit", "Help"};
    XSetForeground(display, global_gc, BlackPixel(display, DefaultScreen(display)));
    XFillRectangle(display, window, global_gc, 0, 0, width, MENU_HEIGHT);
    XSetForeground(display, global_gc, WhitePixel(display, DefaultScreen(display)));
    for (j = 0; j < 3; j++) {
        text_width = XTextWidth(font_info, menu_items[j], strlen(menu_items[j]));
        x_position = j * MENU_ITEM_WIDTH + (MENU_ITEM_WIDTH - text_width) / 2;
        XDrawString(display, window, global_gc, x_position, 25, menu_items[j], strlen(menu_items[j]));
    }
}

void handle_keypress(XKeyEvent *event, Display *display,
                     Window window, int window_width)
{
    KeySym keysym = XLookupKeysym(event, event->state);
    if (keysym == XK_space) {
        play();
    } else if (keysym == XK_s) {
        stop();
    } else if (keysym == XK_n) {
        /* Pass window_width */
        next_track(display, window, window_width);
    } else if (keysym == XK_p) {
        /* we also pass window_width */
        previous_track(display, window, window_width);
    } else if (keysym == XK_Up) {
        player_state.volume = fminf(player_state.volume + 0.1f, 1.0f);
    } else if (keysym == XK_Down) {
        player_state.volume = fmaxf(player_state.volume - 0.1f, 0.0f);
    }
}

void handle_mouse_click(XButtonEvent *event, Display *display, Window window, int window_width) {
    int progress_bar_width = window_width > 100 ? window_width - 100 : 100; // Prevent division by zero
    float progress_position, volume_position;

    if (event->y >= BUTTON_Y && event->y <= BUTTON_Y + BUTTON_HEIGHT) {
        int total_button_width = (BUTTON_WIDTH * 3) + (BUTTON_SPACING * 2);
        int start_x = (window_width - total_button_width) / 2;

        if (event->x >= start_x && event->x < start_x + BUTTON_WIDTH) {
            play();
            log_event("Play button clicked");
        }
        if (event->x >= start_x + BUTTON_WIDTH + BUTTON_SPACING && 
            event->x < start_x + 2 * BUTTON_WIDTH + BUTTON_SPACING) {
            player_pause();
            log_event("Pause button clicked");
        }
        if (event->x >= start_x + 2 * (BUTTON_WIDTH + BUTTON_SPACING) && 
            event->x < start_x + 3 * BUTTON_WIDTH + 2 * BUTTON_SPACING) {
            stop();
            log_event("Stop button clicked");
        }
    }
    if (event->y >= PROGRESS_BAR_Y && event->y <= PROGRESS_BAR_Y + PROGRESS_BAR_HEIGHT) {
        progress_position = (float)(event->x - 50) / progress_bar_width;
        player_state.progress = fmaxf(0.0f, fminf(1.0f, progress_position));
        dragging_progress = 1;
        log_event("Progress bar clicked");
        XClearWindow(display, window);
        draw_menu(display, window, window_width);
        draw_player_controls(display, window, window_width);
        display_track_info(display, window, window_width);
    } else if (event->y >= VOLUME_BAR_Y && event->y <= VOLUME_BAR_Y + VOLUME_BAR_HEIGHT) {
        volume_position = (float)(event->x - 50) / VOLUME_BAR_WIDTH;
        player_state.volume = fmaxf(0.0f, fminf(1.0f, volume_position));
        audio_set_volume(player_state.volume); // Update audio backend
        dragging_volume = 1;
        log_event("Volume bar clicked");
        XClearWindow(display, window);
        draw_menu(display, window, window_width);
        draw_player_controls(display, window, window_width);
        display_track_info(display, window, window_width);
    }
}

void play(void) {
    player_state.playing = 1;
    log_event("Playback started.");
    printf("Playing...\n");
}

void stop(void) {
    player_state.playing = 0;
    player_state.progress = 0.0f;
    log_event("Playback stopped.");
    printf("Stopped.\n");
}

void next_track(Display *display, Window window, int window_width)
{
    if (player_state.track_count > 0) {

        player_state.current_track =
            (player_state.current_track + 1) % player_state.track_count;

        /* reset the progress */
        player_state.progress = 0.0f;

        log_event("Next track selected.");
        /* Trigger a redraw to update the track info */
        XClearWindow(display, window);
        draw_menu(display, window, window_width);
        display_welcome_message(display, window, window_width);
        draw_player_controls(display, window, window_width);
        display_track_info(display, window, window_width);
    }
}

void player_pause(void) {
    player_state.playing = 0;
    log_event("Playback paused.");
    printf("Paused.\n");
}

void previous_track(Display *display, Window window, int window_width) {
    if (player_state.track_count > 0) {

        player_state.current_track =
            ( player_state.current_track - 1
            + player_state.track_count       ) % player_state.track_count;

        player_state.progress = 0.0f;
        log_event("Previous track selected.");
        /* Trigger a redraw to update the track info */
        XClearWindow(display, window);
        draw_menu(display, window, window_width);
        display_welcome_message(display, window, window_width);
        draw_player_controls(display, window, window_width);
        display_track_info(display, window, window_width);
    }
}

void display_welcome_message(Display *display, Window window, int width) {
    int text_width;
    const char *message = "Welcome to XOpenPlayer!";

    XSetForeground(display, global_gc,
                   BlackPixel(display, DefaultScreen(display)));

    text_width = XTextWidth(font_info, message, strlen(message));

    XDrawString(display, window, global_gc,
                (width - text_width) / 2, 100,
                message, strlen(message));

}

void load_tracks(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    player_state.track_count = 0;
    player_state.current_track = 0;
    while (fscanf(f, "%127[^\t]\t%63[^\t]\t%d\n",
                  player_state.tracks[player_state.track_count].title,
                  player_state.tracks[player_state.track_count].artist,
                  &player_state.tracks[player_state.track_count].duration_seconds) == 3) {
        player_state.track_count++;
        if (player_state.track_count >= MAX_TRACKS) break;
    }
    fclose(f);
}

void display_track_info(Display *display, Window window, int width) {
    TrackInfo *track;
    int text_width;
    char info[256];
    if (player_state.track_count == 0) return;

    track = &player_state.tracks[player_state.current_track];

    snprintf(info, sizeof(info), "Now Playing: %s - %s [%d:%02d]",
             track->title, track->artist,
             track->duration_seconds/60,
             track->duration_seconds%60);

    text_width = XTextWidth(font_info, info, strlen(info));
    XDrawString(display, window, global_gc,
               (width - text_width) / 2, 130,
               info, strlen(info));

}

/* this is mystical */
void handle_mouse_release(XButtonEvent *event)
{
    dragging_volume = 0;
    dragging_progress = 0;
}

void handle_mouse_motion(XMotionEvent *event, int window_width) {
    int x, progress_bar_width;
    float new_volume, new_progress;

    x = event->x;
    if (dragging_volume) {
        new_volume = (float)(x - 50) / 100.0f;
        player_state.volume = fmaxf(0.0f, fminf(1.0f, new_volume));
    }
    if (dragging_progress) {
        progress_bar_width = window_width - 100;
        new_progress = (float)(x - 50) / (float)progress_bar_width;
        player_state.progress = fmaxf(0.0f, fminf(1.0f, new_progress));
    }
}

void handle_resize(int new_width, int new_height) {
    /* Currently, you don't need to store width/height,
     * but you can use this to update layout or trigger redraws.
     *
     * Optionally log the new size
     */
    char log_msg[128];
    snprintf(log_msg, sizeof(log_msg),
             "Window resized to %dx%d.",
             new_width, new_height);

    log_event(log_msg);

    /* You could later add layout recalculations here if needed */
}
