#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "../include/player.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Constants for UI layout
#define BUTTON_WIDTH 100
#define BUTTON_HEIGHT 30
#define BUTTON_SPACING 20
#define BUTTON_Y 150
#define MENU_HEIGHT 40
#define MENU_ITEM_WIDTH 100
#define PROGRESS_BAR_HEIGHT 10
#define PROGRESS_BAR_Y 250
#define VOLUME_BAR_WIDTH 100
#define VOLUME_BAR_HEIGHT 5
#define VOLUME_BAR_Y 280

// Structure to hold UI state
static struct {
    int playing;
    float volume;
    float progress;
} player_state = {0, 0.5f, 0.0f};

void draw_button(Display *display, Window window, GC gc, int x, int y, const char *label) {
    // Draw button background
    XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
    XFillRectangle(display, window, gc, x, y, BUTTON_WIDTH, BUTTON_HEIGHT);
    
    // Draw button border
    XSetForeground(display, gc, WhitePixel(display, DefaultScreen(display)));
    XDrawRectangle(display, window, gc, x, y, BUTTON_WIDTH, BUTTON_HEIGHT);
    
    // Draw button label
    int text_len = strlen(label);
    Font font = XLoadFont(display, "fixed");
    XSetFont(display, gc, font);
    int text_width = XTextWidth(XQueryFont(display, font), label, text_len);
    XDrawString(display, window, gc, x + (BUTTON_WIDTH - text_width) / 2, 
                y + BUTTON_HEIGHT - 10, label, text_len);
    XUnloadFont(display, font);
}

void draw_progress_bar(Display *display, Window window, GC gc) {
    int window_width = 800; // Using your default window width
    int progress_width = window_width - 100; // Margins on both sides
    int progress_x = 50; // Starting position
    
    // Draw progress bar background
    XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
    XFillRectangle(display, window, gc, progress_x, PROGRESS_BAR_Y, 
                   progress_width, PROGRESS_BAR_HEIGHT);
    
    // Draw progress
    XSetForeground(display, gc, WhitePixel(display, DefaultScreen(display)));
    XFillRectangle(display, window, gc, progress_x, PROGRESS_BAR_Y,
                   (int)(progress_width * player_state.progress), PROGRESS_BAR_HEIGHT);
}

void draw_volume_control(Display *display, Window window, GC gc) {
    int x = 50;
    
    // Draw volume bar background
    XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
    XFillRectangle(display, window, gc, x, VOLUME_BAR_Y, 
                   VOLUME_BAR_WIDTH, VOLUME_BAR_HEIGHT);
    
    // Draw current volume level
    XSetForeground(display, gc, WhitePixel(display, DefaultScreen(display)));
    XFillRectangle(display, window, gc, x, VOLUME_BAR_Y,
                   (int)(VOLUME_BAR_WIDTH * player_state.volume), VOLUME_BAR_HEIGHT);
    
    // Draw volume label
    const char *label = "Volume";
    Font font = XLoadFont(display, "fixed");
    XSetFont(display, gc, font);
    XDrawString(display, window, gc, x, VOLUME_BAR_Y - 5, label, strlen(label));
    XUnloadFont(display, font);
}

void draw_player_controls(Display *display, Window window) {
    GC gc = XCreateGC(display, window, 0, NULL);
    
    // Calculate center position for buttons
    int window_width = 800;
    int total_width = (BUTTON_WIDTH * 3) + (BUTTON_SPACING * 2);
    int start_x = (window_width - total_width) / 2;
    
    // Draw control buttons
    draw_button(display, window, gc, start_x, BUTTON_Y, "Play");
    draw_button(display, window, gc, start_x + BUTTON_WIDTH + BUTTON_SPACING, BUTTON_Y, "Pause");
    draw_button(display, window, gc, start_x + (BUTTON_WIDTH + BUTTON_SPACING) * 2, BUTTON_Y, "Stop");
    
    // Draw progress and volume controls
    draw_progress_bar(display, window, gc);
    draw_volume_control(display, window, gc);
    
    XFreeGC(display, gc);
}

void init_player(Display *display, Window window) {
    player_state.playing = 0;
    player_state.volume = 0.5f;
    player_state.progress = 0.0f;
    display_welcome_message(display, window);
}

void draw_menu(Display *display, Window window) {
    GC gc = XCreateGC(display, window, 0, NULL);
    
    // Draw menu background
    XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
    XFillRectangle(display, window, gc, 0, 0, 800, MENU_HEIGHT);
    
    // Load font and draw menu items
    Font font = XLoadFont(display, "fixed");
    XSetFont(display, gc, font);
    XSetForeground(display, gc, WhitePixel(display, DefaultScreen(display)));
    
    const char *menu_items[] = {"File", "Edit", "Help"};
    for (int i = 0; i < 3; i++) {
        int text_width = XTextWidth(XQueryFont(display, font), menu_items[i], strlen(menu_items[i]));
        int x_position = i * MENU_ITEM_WIDTH + (MENU_ITEM_WIDTH - text_width) / 2;
        XDrawString(display, window, gc, x_position, 25, menu_items[i], strlen(menu_items[i]));
    }
    
    // Draw player controls after menu
    draw_player_controls(display, window);
    
    XUnloadFont(display, font);
    XFreeGC(display, gc);
}

void display_welcome_message(Display *display, Window window) {
    GC gc = XCreateGC(display, window, 0, NULL);
    Font font = XLoadFont(display, "fixed");
    XSetFont(display, gc, font);

    const char *welcome_message = "Welcome to XOpenPlayer!";
    int text_len = strlen(welcome_message);
    int text_width = XTextWidth(XQueryFont(display, font), welcome_message, text_len);

    XSetForeground(display, gc, WhitePixel(display, DefaultScreen(display)));
    XDrawString(display, window, gc, (WINDOW_WIDTH - text_width) / 2, 100, welcome_message, text_len);

    XUnloadFont(display, font);
    XFreeGC(display, gc);
}

void cleanup_player(Display *display) {
    (void)display;  // Suppress the unused parameter warning
    printf("Cleaning up resources...\n");
    // Clean-up code (if any)
}

void handle_keypress(XKeyEvent *event) {
    switch (event->keycode) {
        case KEY_PLAY:
            play();
            break;
        case KEY_STOP:
            stop();
            break;
        case KEY_NEXT:
            next_track();
            break;
        case KEY_PREV:
            previous_track();
            break;
        case KEY_VOL_UP:
            if (player_state.volume < 1.0f) {
                player_state.volume += 0.1f;
            }
            break;
        case KEY_VOL_DOWN:
            if (player_state.volume > 0.0f) {
                player_state.volume -= 0.1f;
            }
            break;
        default:
            break;
    }
}

void handle_mouse_click(XButtonEvent *event) {
    int x = event->x;
    int y = event->y;

    // Handle clicks on the Play, Pause, Stop buttons
    if (y >= BUTTON_Y && y <= BUTTON_Y + BUTTON_HEIGHT) {
        if (x >= 50 && x <= 50 + BUTTON_WIDTH) {
            play();
        } else if (x >= 50 + BUTTON_WIDTH + BUTTON_SPACING && x <= 50 + 2 * BUTTON_WIDTH + BUTTON_SPACING) {
            pause();
        } else if (x >= 50 + 2 * (BUTTON_WIDTH + BUTTON_SPACING) && x <= 50 + 3 * BUTTON_WIDTH + 2 * BUTTON_SPACING) {
            stop();
        }
    }
}

void play() {
    player_state.playing = 1;
    printf("Playing...\n");
}

void stop() {
    player_state.playing = 0;
    player_state.progress = 0.0f;
    printf("Stopped.\n");
}

void pause() {
    player_state.playing = 0;
    printf("Paused.\n");
}

void next_track() {
    printf("Next track...\n");
    // Implement actual track switching if needed
}

void previous_track() {
    printf("Previous track...\n");
    // Implement actual track switching if needed
}

