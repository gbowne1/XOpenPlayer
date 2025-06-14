#include <X11/Xlib.h>
#include <X11/Xutil.h>
// #include <X11/extensions/XShm.h>
// #include <X11/XKBlib.h>
#include "../include/player.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

// Static variables
static PlayerState player_state = {0, 0.5f, 0.0f}; // Initial state
static GC global_gc;                               // Global graphics context
static Font global_font;                           // Global font
static XFontStruct *font_info;                     // Font metadata
static int dragging_volume = 0;
static int dragging_progress = 0;

void log_event(const char *message) {
    FILE *log = fopen("application_log.txt", "a");
    if (log) {
        time_t now = time(NULL);
        struct tm *local = localtime(&now);
        fprintf(log, "[%02d:%02d:%02d] Event: %s\n",
                local->tm_hour, local->tm_min, local->tm_sec, message);
        fclose(log);
    } else {
        fprintf(stderr, "Warning: Unable to open log file.\n");
    }
}

void init_player(Display *display, Window window) {
    // Initialize player state
    player_state.playing = 0;
    player_state.volume = 0.5f;
    player_state.progress = 0.0f;

    // Create global graphics context
    global_gc = XCreateGC(display, window, 0, NULL);
    if (!global_gc) {
        log_event("Error: Failed to create graphics context.");
        fprintf(stderr, "Error: Failed to create graphics context.\n");
        exit(EXIT_FAILURE);
    }

    // Load a global font
    global_font = XLoadFont(display, "fixed");
    font_info = XQueryFont(display, global_font);
    if (!font_info) {
        log_event("Error: Failed to load font.");
        fprintf(stderr, "Error: Failed to load font.\n");
        XFreeGC(display, global_gc);  // Clean up if font loading fails
        exit(EXIT_FAILURE);
    }

    log_event("Player initialized successfully.");
}

void cleanup_player(Display *display) {
    if (font_info) {
        XFreeFont(display, font_info);
        XUnloadFont(display, global_font);
        log_event("Font unloaded.");
    }
    XFreeGC(display, global_gc);
    log_event("Graphics context freed.");
}

void draw_button(Display *display, Window window, int x, int y, const char *label) {
    // Draw button background
    XSetForeground(display, global_gc, BlackPixel(display, DefaultScreen(display)));
    XFillRectangle(display, window, global_gc, x, y, 100, 30); // Fixed size

    // Draw button border
    XSetForeground(display, global_gc, WhitePixel(display, DefaultScreen(display)));
    XDrawRectangle(display, window, global_gc, x, y, 100, 30);

    // Draw button label
    int text_width = XTextWidth(font_info, label, strlen(label));
    XDrawString(display, window, global_gc, x + (100 - text_width) / 2, y + 20, label, strlen(label));
}

void draw_progress_bar(Display *display, Window window, int width) {
    int progress_width = width - 100; // Dynamic based on window width
    int progress_x = 50;

    // Draw progress bar background
    XSetForeground(display, global_gc, BlackPixel(display, DefaultScreen(display)));
    XFillRectangle(display, window, global_gc, progress_x, 250, progress_width, 10);

    // Draw progress
    XSetForeground(display, global_gc, WhitePixel(display, DefaultScreen(display)));
    XFillRectangle(display, window, global_gc, progress_x, 250, 
                   (int)(progress_width * player_state.progress), 10);
}

void draw_volume_control(Display *display, Window window) {
    int x = 50;

    // Draw volume bar background
    XSetForeground(display, global_gc, BlackPixel(display, DefaultScreen(display)));
    XFillRectangle(display, window, global_gc, x, 280, 100, 5);

    // Draw current volume level
    XSetForeground(display, global_gc, WhitePixel(display, DefaultScreen(display)));
    XFillRectangle(display, window, global_gc, x, 280, (int)(100 * player_state.volume), 5);

    // Draw volume label
    XDrawString(display, window, global_gc, x, 270, "Volume", 6);
}

void draw_player_controls(Display *display, Window window, int width) {
    int total_button_width = (BUTTON_WIDTH * 3) + (BUTTON_SPACING * 2);
    int start_x = (width - total_button_width) / 2;

    draw_button(display, window, start_x, BUTTON_Y, "Play");
    draw_button(display, window, start_x + BUTTON_WIDTH + BUTTON_SPACING, BUTTON_Y, "Pause");
    draw_button(display, window, start_x + 2 * (BUTTON_WIDTH + BUTTON_SPACING), BUTTON_Y, "Stop");

    draw_progress_bar(display, window, width);
    draw_volume_control(display, window);
}

void draw_menu(Display *display, Window window, int width) {
    // Create and set up graphics context
    GC gc = XCreateGC(display, window, 0, NULL);
    if (!gc) {
        log_event("Error: Failed to create graphics context for menu");
        return;
    }

    // Set up font
    Font font = XLoadFont(display, "fixed");
    if (!font) {
        XFreeGC(display, gc);
        log_event("Error: Failed to load font");
        return;
    }

    // Set background and text colors
    XSetBackground(display, gc, WhitePixel(display, DefaultScreen(display)));
    XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));

    // Draw menu background
    XFillRectangle(display, window, gc, 0, 0, width, MENU_HEIGHT);

    // Draw menu items
    const char *menu_items[] = {"File", "Edit", "Help"};
    for (int i = 0; i < 3; i++) {
        int text_width = XTextWidth(font_info, menu_items[i], strlen(menu_items[i]));
        int x_position = i * MENU_ITEM_WIDTH + (MENU_ITEM_WIDTH - text_width) / 2;
        XDrawString(display, window, gc, x_position, 25, menu_items[i], strlen(menu_items[i]));
    }

    // Cleanup
    XFreeFont(display, font);
    XFreeGC(display, gc);
}

void handle_keypress(XKeyEvent *event, Display *display, Window window, int window_width) {
    KeySym keysym = XLookupKeysym(event, event->state);
    if (keysym == XK_space) {
        play();
    } else if (keysym == XK_s) {
        stop();
    } else if (keysym == XK_n) {
        next_track(display, window, window_width); // Pass window_width
    } else if (keysym == XK_p) {
        previous_track(display, window, window_width); // Pass window_width
    } else if (keysym == XK_Up) {
        player_state.volume = fminf(player_state.volume + 0.1f, 1.0f);
    } else if (keysym == XK_Down) {
        player_state.volume = fmaxf(player_state.volume - 0.1f, 0.0f);
    }
}

void handle_mouse_click(XButtonEvent *event, int ) {
    // Calculate button positions dynamically
    int total_button_width = (BUTTON_WIDTH * 3) + (BUTTON_SPACING * 2);
    int start_x = (800 - total_button_width) / 2;

    // Check if the click is within the button area
    if (event->y >= BUTTON_Y && event->y <= BUTTON_Y + BUTTON_HEIGHT) {
        if (event->x >= start_x && event->x < start_x + BUTTON_WIDTH) {
            play(); // Clicked the "Play" button
        } else if (event->x >= start_x + BUTTON_WIDTH + BUTTON_SPACING && event->x < start_x + 2 * BUTTON_WIDTH + BUTTON_SPACING) {
            pause(); // Clicked the "Pause" button
        } else if (event->x >= start_x + 2 * (BUTTON_WIDTH + BUTTON_SPACING) && event->x < start_x + 3 * BUTTON_WIDTH + 2 * BUTTON_SPACING) {
            stop(); // Clicked the "Stop" button
        }
    }

    // Handle volume and progress bar clicks
    if (event->y >= PROGRESS_BAR_Y && event->y <= PROGRESS_BAR_Y + PROGRESS_BAR_HEIGHT) {
        // Update progress
        float progress_position = (float)(event->x - 50) / 700.0f;
        player_state.progress = fmaxf(0.0f, fminf(1.0f, progress_position));
    } else if (event->y >= VOLUME_BAR_Y && event->y <= VOLUME_BAR_Y + VOLUME_BAR_HEIGHT) {
        // Update volume
        float volume_position = (float)(event->x - 50) / 100.0f;
        player_state.volume = fmaxf(0.0f, fminf(1.0f, volume_position));
    }
}

void play() {
    player_state.playing = 1;
    log_event("Playback started.");
    printf("Playing...\n");
}

void stop() {
    player_state.playing = 0;
    player_state.progress = 0.0f;
    log_event("Playback stopped.");
    printf("Stopped.\n");
}

void next_track(Display *display, Window window, int window_width)
{
    if (player_state.track_count > 0) {
        player_state.current_track = (player_state.current_track + 1) % player_state.track_count;
        player_state.progress = 0.0f; // Reset progress
        log_event("Next track selected.");
        // Trigger a redraw to update the track info
        XClearWindow(display, window);
        draw_menu(display, window, window_width);
        display_welcome_message(display, window, window_width);
        draw_player_controls(display, window, window_width);
        display_track_info(display, window, window_width);
    }
}

void pause() {
    player_state.playing = 0;
    log_event("Playback paused.");
    printf("Paused.\n");
}

void previous_track(Display *display, Window window, int window_width) {
    if (player_state.track_count > 0) {
        player_state.current_track = (player_state.current_track - 1 + player_state.track_count) % player_state.track_count;
        player_state.progress = 0.0f;
        log_event("Previous track selected.");
        // Trigger a redraw to update the track info
        XClearWindow(display, window);
        draw_menu(display, window, window_width);
        display_welcome_message(display, window, window_width);
        draw_player_controls(display, window, window_width);
        display_track_info(display, window, window_width);
    }
}

void display_welcome_message(Display *display, Window window, int width) {
    XSetForeground(display, global_gc, WhitePixel(display, DefaultScreen(display)));
    const char *message = "Welcome to XOpenPlayer!";
    int text_width = XTextWidth(font_info, message, strlen(message));
    XDrawString(display, window, global_gc, (width - text_width) / 2, 100, message, strlen(message));
}

void load_tracks(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    player_state.track_count = 0;
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
    if (player_state.track_count == 0) return;
    TrackInfo *track = &player_state.tracks[player_state.current_track];
    char info[256];
    snprintf(info, sizeof(info), "Now Playing: %s - %s [%d:%02d]",
             track->title, track->artist, track->duration_seconds/60, track->duration_seconds%60);
    int text_width = XTextWidth(font_info, info, strlen(info));
    XDrawString(display, window, global_gc, (width - text_width) / 2, 130, info, strlen(info));
}

void handle_mouse_release(XButtonEvent *event) {
    dragging_volume = 0;
    dragging_progress = 0;
}

void handle_mouse_motion(XMotionEvent *event, int window_width) {
    int x = event->x;
    if (dragging_volume) {
        float new_volume = (float)(x - 50) / 100.0f;
        player_state.volume = fmaxf(0.0f, fminf(1.0f, new_volume));
    }
    if (dragging_progress) {
        int progress_bar_width = window_width - 100;
        float new_progress = (float)(x - 50) / (float)progress_bar_width;
        player_state.progress = fmaxf(0.0f, fminf(1.0f, new_progress));
    }
}

void handle_resize(int new_width, int new_height) {
    // Currently, you don't need to store width/height,
    // but you can use this to update layout or trigger redraws.

    // Optionally log the new size
    char log_msg[128];
    snprintf(log_msg, sizeof(log_msg), "Window resized to %dx%d.", new_width, new_height);
    log_event(log_msg);

    // You could later add layout recalculations here if needed
}