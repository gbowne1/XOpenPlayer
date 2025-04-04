#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "../include/player.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Static variables
static PlayerState player_state = {0, 0.5f, 0.0f}; // Initial state
static GC global_gc;                               // Global graphics context
static Font global_font;                           // Global font
static XFontStruct *font_info;                     // Font metadata

void log_event(const char *event_description) {
    FILE *log = fopen("application_log.txt", "a");
    if (log) {
        fprintf(log, "Event: %s\n", event_description);
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
        fprintf(stderr, "Error: Failed to create global graphics context.\n");
        exit(1);
    }

    // Load a global font
    global_font = XLoadFont(display, "fixed");
    font_info = XQueryFont(display, global_font);
    if (!font_info) {
        fprintf(stderr, "Error: Failed to load font.\n");
        exit(1);
    }

    log_event("Player initialized successfully.");
}

void cleanup_player(Display *display) {
    // Cleanup resources
    if (font_info) {
        XUnloadFont(display, global_font);
        log_event("Font unloaded.");
    }
    XFreeGC(display, global_gc);
    log_event("Graphics context freed.");
    printf("Resources cleaned up.\n");
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

void draw_player_controls(Display *display, Window window) {
    // Dynamic button layout
    int total_width = (100 * 3) + (20 * 2); // 3 buttons + spacing
    int start_x = (800 - total_width) / 2; // Centered layout (default width = 800)

    draw_button(display, window, start_x, 150, "Play");
    draw_button(display, window, start_x + 120, 150, "Pause");
    draw_button(display, window, start_x + 240, 150, "Stop");

    draw_progress_bar(display, window, 800); // Default width
    draw_volume_control(display, window);
}

void draw_menu(Display *display, Window window) {
    // Draw menu background
    XSetForeground(display, global_gc, BlackPixel(display, DefaultScreen(display)));
    XFillRectangle(display, window, global_gc, 0, 0, 800, 40);

    // Draw menu items
    const char *menu_items[] = {"File", "Edit", "Help"};
    for (int i = 0; i < 3; i++) {
        int text_width = XTextWidth(font_info, menu_items[i], strlen(menu_items[i]));
        int x_position = i * 100 + (100 - text_width) / 2;
        XDrawString(display, window, global_gc, x_position, 25, menu_items[i], strlen(menu_items[i]));
    }
}

void handle_keypress(XKeyEvent *event) {
    KeySym keysym = XLookupKeysym(event, 0);
    if (keysym == XK_space) {
        play();
    } else if (keysym == XK_s) {
        stop();
    } else if (keysym == XK_n) {
        next_track();
    } else if (keysym == XK_p) {
        previous_track();
    } else if (keysym == XK_Up) {
        player_state.volume = fmin(player_state.volume + 0.1f, 1.0f);
    } else if (keysym == XK_Down) {
        player_state.volume = fmax(player_state.volume - 0.1f, 0.0f);
    }
}

void handle_mouse_click(XButtonEvent *event) {
    int x = event->x; // Mouse click X coordinate
    int y = event->y; // Mouse click Y coordinate

    // Calculate button layout dynamically
    int total_button_width = (100 * 3) + (20 * 2); // Total width of 3 buttons with spacing
    int start_x = (800 - total_button_width) / 2; // Center buttons horizontally (default window width = 800)

    // Check if the click is within the button area
    if (y >= 150 && y <= 150 + 30) { // BUTTON_Y and BUTTON_HEIGHT
        if (x >= start_x && x < start_x + 100) {
            play(); // Clicked the "Play" button
        } else if (x >= start_x + 120 && x < start_x + 220) {
            pause(); // Clicked the "Pause" button
        } else if (x >= start_x + 240 && x < start_x + 340) {
            stop(); // Clicked the "Stop" button
        }
    }

    // Check if the click is within the Volume Bar area
    if (y >= 280 && y <= 280 + 5) { // VOLUME_BAR_Y and VOLUME_BAR_HEIGHT
        if (x >= 50 && x <= 50 + 100) { // VOLUME_BAR_WIDTH
            float click_position = (float)(x - 50) / 100.0f; // Normalize to 0.0 - 1.0
            player_state.volume = fmax(0.0f, fmin(1.0f, click_position)); // Clamp volume
            printf("Volume updated to: %.2f\n", player_state.volume);
        }
    }

    // Check if the click is within the Progress Bar area
    if (y >= 250 && y <= 250 + 10) { // PROGRESS_BAR_Y and PROGRESS_BAR_HEIGHT
        if (x >= 50 && x <= 750) { // Dynamic width based on window size (700px usable width)
            float progress_position = (float)(x - 50) / 700.0f; // Normalize to 0.0 - 1.0
            player_state.progress = fmax(0.0f, fmin(1.0f, progress_position)); // Clamp progress
            printf("Progress updated to: %.2f%%\n", player_state.progress * 100);
        }
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

void pause() {
    player_state.playing = 0;
    log_event("Playback paused.");
    printf("Paused.\n");
}

void next_track() {
    log_event("Next track selected.");
    printf("Next track...\n");
}

void previous_track() {
    log_event("Previous track selected.");
    printf("Previous track...\n");
}
