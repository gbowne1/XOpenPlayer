#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "../include/player.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void handle_keypress(XKeyEvent *event) {
    switch (event->keycode) {
        case 65: // Spacebar
            if (player_state.playing) {
                pause();
            } else {
                play();
            }
            break;
        case 39: stop(); break; // 'S' key
        case 57: next_track(); break; // 'N' key
        case 55: previous_track(); break; // 'P' key
        case 111: // Up arrow
            player_state.volume = (player_state.volume + 0.1f > 1.0f) ? 1.0f : player_state.volume + 0.1f;
            printf("Volume: %.1f\n", player_state.volume);
            break;
        case 116: // Down arrow
            player_state.volume = (player_state.volume - 0.1f < 0.0f) ? 0.0f : player_state.volume - 0.1f;
            printf("Volume: %.1f\n", player_state.volume);
            break;
    }
}

void handle_mouse_click(XButtonEvent *event) {
    // Menu clicks
    if (event->y < MENU_HEIGHT) {
        int menu_index = event->x / MENU_ITEM_WIDTH;
        switch (menu_index) {
            case 0: printf("File menu clicked\n"); break;
            case 1: printf("Edit menu clicked\n"); break;
            case 2: printf("Help menu clicked\n"); break;
        }
        return;
    }
    
    // Progress bar clicks
    if (event->y >= PROGRESS_BAR_Y && event->y <= PROGRESS_BAR_Y + PROGRESS_BAR_HEIGHT) {
        float click_position = (float)(event->x - 50) / (800 - 100);
        if (click_position >= 0.0f && click_position <= 1.0f) {
            player_state.progress = click_position;
            printf("Seeking to: %.1f%%\n", player_state.progress * 100);
        }
        return;
    }
    
    // Volume bar clicks
    if (event->y >= VOLUME_BAR_Y && event->y <= VOLUME_BAR_Y + VOLUME_BAR_HEIGHT) {
        float click_position = (float)(event->x - 50) / VOLUME_BAR_WIDTH;
        if (click_position >= 0.0f && click_position <= 1.0f) {
            player_state.volume = click_position;
            printf("Volume set to: %.1f\n", player_state.volume);
        }
        return;
    }
    
    // Playback control buttons
    if (event->y >= BUTTON_Y && event->y <= BUTTON_Y + BUTTON_HEIGHT) {
        int start_x = (800 - (BUTTON_WIDTH * 3 + BUTTON_SPACING * 2)) / 2;
        if (event->x >= start_x && event->x <= start_x + BUTTON_WIDTH) {
            play();
        } else if (event->x >= start_x + BUTTON_WIDTH + BUTTON_SPACING && 
                   event->x <= start_x + (BUTTON_WIDTH * 2 + BUTTON_SPACING)) {
            pause();
        } else if (event->x >= start_x + (BUTTON_WIDTH * 2 + BUTTON_SPACING * 2) && 
                   event->x <= start_x + (BUTTON_WIDTH * 3 + BUTTON_SPACING * 2)) {
            stop();
        }
    }
}

void play() {
    player_state.playing = 1;
    printf("Playing...\n");
}

void pause() {
    player_state.playing = 0;
    printf("Paused.\n");
}

void stop() {
    player_state.playing = 0;
    player_state.progress = 0.0f;
    printf("Stopped.\n");
}

void next_track() {
    player_state.progress = 0.0f;
    printf("Next track...\n");
}

void previous_track() {
    player_state.progress = 0.0f;
    printf("Previous track...\n");
}

void display_welcome_message(Display *display, Window window) {
    GC gc = XCreateGC(display, window, 0, NULL);
    XSetForeground(display, gc, WhitePixel(display, DefaultScreen(display)));
    XSetBackground(display, gc, BlackPixel(display, DefaultScreen(display)));
    
    const char *welcome_text = "Welcome to XOpenPlayer!";
    int text_length = strlen(welcome_text);
    Font font = XLoadFont(display, "fixed");
    XSetFont(display, gc, font);
    
    int text_width = XTextWidth(XQueryFont(display, font), welcome_text, text_length);
    int x_position = (800 - text_width) / 2;
    XDrawString(display, window, gc, x_position, 100, welcome_text, text_length);
    
    XUnloadFont(display, font);
    XFreeGC(display, gc);
}