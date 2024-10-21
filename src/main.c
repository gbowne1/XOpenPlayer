#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/player.h"

int main() {
    Display *display;
    Window window;
    XEvent event;

    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Unable to open X display\n");
        exit(1);
    }

    int screen = DefaultScreen(display);
    window = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10, 800, 600, 1,
                                 BlackPixel(display, screen), WhitePixel(display, screen));
    
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask);
    XMapWindow(display, window);
    
    // Set the window title
    XStoreName(display, window, "XOpenPlayer");

    init_player(display, window);

    Atom wmDeleteWindow = XInternAtom(display, "WM_DELETE_WINDOW", False);

    while (1) {
        XNextEvent(display, &event);
        if (event.type == Expose) {
            display_welcome_message(display, window);
        } else if (event.type == KeyPress) {
            handle_keypress(&event.xkey);
        } else if (event.type == ClientMessage) {
            // Handle window close event
            if ((long)event.xclient.data.l[0] == (long)wmDeleteWindow) {
                break; // Exit the loop
            }
        }
    }

    XDestroyWindow(display, window); // Clean up window
    XCloseDisplay(display); // Close the display connection
    return 0;
}
