#include <math.h>
#include <unistd.h>
#include <X11/extensions/Xrandr.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>
#include "../include/audio.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void play_sine_wave(Display *display, int screen)
{
    int width = 800;  // Set a default width
    int height = 600; // Set a default height
    int depth = DefaultDepth(display, screen);
    unsigned long black = BlackPixel(display, screen);
    unsigned long white = WhitePixel(display, screen);

    XSetWindowAttributes attr;
    attr.event_mask = ExposureMask | KeyPressMask | StructureNotifyMask | SubstructureNotifyMask;
    attr.background_pixel = black;
    attr.border_pixel = black;

    // Get the visual for the window
    Visual *visual = DefaultVisual(display, screen);

    // Create the window
    Window win = XCreateWindow(display,
                               RootWindow(display, screen),
                               0, 0, width, height, 0,
                               depth, InputOutput,
                               visual,
                               CWEventMask | CWBackPixel | CWBorderPixel,
                               &attr);

    // Set window manager hints
    XSizeHints hints;
    hints.flags = PMinSize | PMaxSize;
    hints.min_width = 200;
    hints.min_height = 150;
    hints.max_width = DisplayWidth(display, screen);
    hints.max_height = DisplayHeight(display, screen);
    XSetWMNormalHints(display, win, &hints);

    // Set window title
    XStoreName(display, win, "XOpenPlayer");

    // Handle window manager protocols
    Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    Atom wm_protocols = XInternAtom(display, "WM_PROTOCOLS", False);
    XSetWMProtocols(display, win, &wm_delete_window, 1);

    XMapWindow(display, win);

    float frequency = 440.0f; // Hz
    float amplitude = height / 2 - 10; // Pixel value
    float phase = 0.0f;

    while (1) {
        XEvent event;
        XNextEvent(display, &event);

        switch (event.type) {
            case Expose:
                draw_sine_wave(display, win, width, height, frequency, amplitude, phase);
                break;
            case KeyPress:
                XDestroyWindow(display, win);
                return;
            case ClientMessage:
                if (event.xclient.message_type == wm_protocols &&
                    (Atom)event.xclient.data.l[0] == wm_delete_window) {
                    XDestroyWindow(display, win);
                    return;
                }
                break;
            case ConfigureNotify:
                width = event.xconfigure.width;
                height = event.xconfigure.height;
                amplitude = height / 2 - 10;
                draw_sine_wave(display, win, width, height, frequency, amplitude, phase);
                break;
        }
    }
}

void draw_sine_wave(Display *display, Window win, int width, int height, float frequency, float amplitude, float phase)
{
    GC gc = XCreateGC(display, win, 0, NULL);
    unsigned long white = WhitePixel(display, DefaultScreen(display));
    XSetForeground(display, gc, white);
    XDrawLine(display, win, gc, 0, height / 2, width, height / 2);

    for (int x = 1; x < width; x++) {
        float y = sin(phase + (float)x * M_PI * frequency / width) * amplitude + height / 2;
        XDrawPoint(display, win, gc, x, (int)y);
    }

    XFreeGC(display, gc);
}
