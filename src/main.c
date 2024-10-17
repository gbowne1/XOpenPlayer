#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include "../include/audio.h"

int main(int argc, char **argv)
{
    Display *display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }

    int screen = DefaultScreen(display);

    printf("Press Enter to play a sine wave, Ctrl+C to quit...\n");

    printf("Press any key to close the window...\n");

    play_sine_wave(display, screen);

    XCloseDisplay(display);
    return 0;
}

