#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

    XSelectInput(display, window, ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask);
    XMapWindow(display, window);
    
    XStoreName(display, window, "XOpenPlayer");

    init_player(display, window);

    Atom wmDeleteWindow = XInternAtom(display, "WM_DELETE_WINDOW", False);

    // Store initial window size
    int window_width = 800;
    int window_height = 600;

    while (1) {
        XNextEvent(display, &event);
        if (event.type == Expose) {
            draw_menu(display, window);
            display_welcome_message(display, window);
        } else if (event.type == KeyPress) {
            handle_keypress(&event.xkey);
        } else if (event.type == ButtonPress) {
            handle_mouse_click(&event.xbutton);
        } else if (event.type == ClientMessage) {
            if ((long)event.xclient.data.l[0] == (long)wmDeleteWindow) {
                cleanup_player(display);
                break;
            }
        } else if (event.type == ConfigureNotify) {
            // Handle window resize events here
            window_width = event.xconfigure.width;
            window_height = event.xconfigure.height;

            // Recalculate positions and sizes of UI elements based on new window size
            // For example:
            // - Update button positions based on window width
            // - Update progress bar size based on window width
            // - Adjust volume control size, etc.
            
            // You can now pass the new width and height to any functions that need them.
            printf("Window resized to: %d x %d\n", window_width, window_height);
        }
    }

    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return 0;
}
