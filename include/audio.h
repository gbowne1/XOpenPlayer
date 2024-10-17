#ifndef AUDIO_H
#define AUDIO_H

#include <X11/Xlib.h>

void play_sine_wave(Display *display, int screen);
void draw_sine_wave(Display *display, Window win, int width, int height, float frequency, float amplitude, float phase);

#endif // AUDIO_H
