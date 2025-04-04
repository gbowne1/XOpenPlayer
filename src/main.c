#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/player.h"

// Enum for standardized error codes
enum ErrorCode {
    ERR_DISPLAY = 1,
    ERR_WINDOW_CREATION
};

// Function to handle errors
void handle_error(int error_code, Display *display) {
    switch (error_code) {
        case ERR_DISPLAY:
            fprintf(stderr, "Error: Unable to open X display. Check your DISPLAY environment variable.\n");
            break;
        case ERR_WINDOW_CREATION:
            fprintf(stderr, "Error: Failed to create the X window.\n");
            break;
        default:
            fprintf(stderr, "An unknown error occurred.\n");
    }
    if (display) {
        XCloseDisplay(display);
    }
    exit(error_code);
}

// Logging function
void log_event(const char *message) {
    FILE *log = fopen("application_log.txt", "a");
    if (log) {
        fprintf(log, "Event: %s\n", message);
        fclose(log);
    } else {
        fprintf(stderr, "Warning: Unable to open log file for writing.\n");
    }
}

int main() {
    Display *display = NULL;
    Window window;
    XEvent event;

    // Open the display
    display = XOpenDisplay(NULL);
    if (!display) {
        handle_error(ERR_DISPLAY, NULL);
    }
    log_event("X display opened successfully.");

    int screen = DefaultScreen(display);

    // Create a simple window
    window = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10, 800, 600, 1,
                                 BlackPixel(display, screen), WhitePixel(display, screen));
    if (!window) {
        handle_error(ERR_WINDOW_CREATION, display);
    }
    log_event("Window created successfully.");

    // Select input events for the window
    XSelectInput(display, window, ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask);
    XMapWindow(display, window);
    XStoreName(display, window, "XOpenPlayer");

    // Initialize the player
    init_player(display, window);
    log_event("Player initialized.");

    // Support for closing the window
    Atom wmDeleteWindow = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wmDeleteWindow, 1);

    // Store initial window size
    int window_width = 800, window_height = 600;

    while (1) {
        XNextEvent(display, &event);
        if (event.type == Expose) {
            draw_menu(display, window);
            display_welcome_message(display, window);
            log_event("Expose event handled.");
        } else if (event.type == KeyPress) {
            handle_keypress(&event.xkey);
            log_event("KeyPress event handled.");
        } else if (event.type == ButtonPress) {
            handle_mouse_click(&event.xbutton);
            log_event("ButtonPress event handled.");
        } else if (event.type == ClientMessage) {
            if ((long)event.xclient.data.l[0] == (long)wmDeleteWindow) {
                log_event("ClientMessage received: Closing window.");
                cleanup_player(display);
                break;
            }
        } else if (event.type == ConfigureNotify) {
            // Handle window resize events
            window_width = event.xconfigure.width;
            window_height = event.xconfigure.height;
            printf("Window resized to: %d x %d\n", window_width, window_height);
            log_event("ConfigureNotify (resize) event handled.");
        }
    }

    // Cleanup resources
    log_event("Cleaning up resources.");
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    log_event("Application terminated successfully.");
    return 0;
}
