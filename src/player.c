#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "../include/player.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_player(Display *display, Window window) {
    display_welcome_message(display, window);
    // Initialize audio/video libraries here
}

void handle_keypress(XKeyEvent *event) {
    switch (event->keycode) {
        case 65: // Spacebar
            play(); // You may need to map these keycodes to specific keys
            break;
        case 39: // 'S' key
            stop();
            break;
        case 57: // 'N' key
            next_track();
            break;
        case 55: // 'P' key
            previous_track();
            break;
        // Add cases for other controls
    }
}

void play() {
    printf("Playing...\n");
}

void pause() {
    printf("Paused.\n");
}

void stop() {
    printf("Stopped.\n");
}

void next_track() {
    printf("Next track...\n");
}

void previous_track() {
    printf("Previous track...\n");
}

void increase_volume() {
    printf("Increasing volume...\n");
}

void decrease_volume() {
    printf("Decreasing volume...\n");
}

void toggle_fullscreen(Window *window) {
    (void)window;
}

void display_welcome_message(Display *display, Window window) {
    // Create a graphics context
    GC gc = XCreateGC(display, window, 0, NULL);
    XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display))); // Set text color

    const char *welcome_text = "Welcome to XOpenPlayer!";
    int text_length = strlen(welcome_text);
    
    // Load a standard font
    Font font = XLoadFont(display, "fixed"); // Use a standard font like "fixed"
    XSetFont(display, gc, font);

    // Calculate text width
    int text_width = XTextWidth(XQueryFont(display, font), welcome_text, text_length);
    
    int x_position = (800 - text_width) / 2; // Center the text based on the window width

    XDrawString(display, window, gc, x_position, 100, welcome_text, text_length);

    // Cleanup
    XUnloadFont(display, font);
    XFreeGC(display, gc); // Free the graphics context
}



