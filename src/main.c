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
                break;
            }
        }
    }

    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return 0;
}
