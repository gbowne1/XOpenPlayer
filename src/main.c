
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
#include <X11/Xatom.h>
#include <X11/keysym.h>
/* #include <X11/Xresource.h>
 * #include <X11/extensions/Xrandr.h>
 * #include <X11/extensions/Xcursor.h>
 * #include <X11/extensions/Xinerama.h>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "player.h"
#include "util.h"

/* Enum for standardized error codes */
enum ErrorCode {
    SUCCESS = 0,
    ERR_UNKNOWN = -1,
    ERR_DISPLAY = 1,
    ERR_WINDOW_CREATION = 2,
};

/* Function to handle errors */
void handle_error(int error_code, Display *display) {
    switch (error_code) {
        case ERR_DISPLAY:
            fprintf(stderr, "FAIL: Unable to open X display\n");
            fprintf(stderr, "    : Check DISPLAY env var\n");
            break;
        case ERR_WINDOW_CREATION:
            fprintf(stderr, "FAIL: create the X window\n");
            fprintf(stderr, "FAIL: ERR_WINDOW_CREATION\n");
            break;
        default:
            fprintf(stderr, "FAIL: unknown error\n");
    }
    if (display) {
        XCloseDisplay(display);
    }
    exit(error_code);
}

int main(void) {
    Display *display = NULL;
    Window window;
    XEvent event;
    XSizeHints *size_hints;
    Atom wmDeleteWindow;
    int screen, window_width, window_height;

    /* Open the display */
    display = XOpenDisplay(NULL);
    if (!display) {
        handle_error(ERR_DISPLAY, NULL);
    }
    log_event("X11 display opened");

    screen = DefaultScreen(display);

    /* Create a simple window */
    window = XCreateSimpleWindow(display, RootWindow(display, screen),
                                 10, 10, 800, 600, 1,
                                 BlackPixel(display, screen),
                                 WhitePixel(display, screen));

    if (!XMapWindow(display, window)) {
    handle_error(ERR_WINDOW_CREATION, display);
    }
    log_event("X11 Window created");

    size_hints = XAllocSizeHints();
    if (size_hints) {
        size_hints->flags = PMinSize | PBaseSize;
        size_hints->min_width = 400;
        size_hints->min_height = 300;
        size_hints->base_width = 800;
        size_hints->base_height = 600;
        XSetWMNormalHints(display, window, size_hints);
        XFree(size_hints);
    }

    /* Select input events for the window */
    XSelectInput(display, window,
             ExposureMask | KeyPressMask | ButtonPressMask |
             ButtonReleaseMask | PointerMotionMask | StructureNotifyMask);

    XMapWindow(display, window);
    XStoreName(display, window, "XOpenPlayer");

    /* Initialize the player */
    init_player(display, window);
    log_event("Player initialized.");

    load_tracks("tracks.txt");
    log_event("Tracks loaded.");

    /* Support for closing the window */
    wmDeleteWindow = XInternAtom(display,
                                      "WM_DELETE_WINDOW", False);

    XSetWMProtocols(display, window, &wmDeleteWindow, 1);

    /* Store initial window size */
    window_width = 800;
    window_height = 600;

    while (1) {
        XNextEvent(display, &event);
        if (event.type == Expose) {
            XClearWindow(display, window);
            draw_menu(display, window, window_width);
            display_welcome_message(display, window, window_width);
            draw_player_controls(display, window, window_width);
            display_track_info(display, window, window_width);
            log_event("Expose event");
        } else if (event.type == KeyPress) {
            handle_keypress(&event.xkey, display, window, window_width);
            log_event("KeyPress event");
        } else if (event.type == ButtonPress) {
            handle_mouse_click(&event.xbutton, window_width);
            log_event("ButtonPress event");
        } else if (event.type == ClientMessage) {
            if ((long)event.xclient.data.l[0] == (long)wmDeleteWindow) {
                log_event("ClientMessage received: Closing window");
                cleanup_player(display);
                break;
            }
        } else if (event.type == ConfigureNotify) {
            window_width = event.xconfigure.width;
            window_height = event.xconfigure.height;
            handle_resize(window_width, window_height);

            log_event("ConfigureNotify (resize) event");
        } else if (event.type == ButtonRelease) {
            handle_mouse_release(&event.xbutton);
            log_event("ButtonRelease event");
        } else if (event.type == MotionNotify) {
            handle_mouse_motion(&event.xmotion, window_width);
            log_event("MotionNotify event");
        }
    }

    /* Cleanup resources */
    log_event("Cleaning up resources.");
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    log_event("Application terminated successfully.");
    return 0;
}

