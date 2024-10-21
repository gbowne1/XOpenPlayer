#ifndef PLAYER_H
#define PLAYER_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>

void init_player(Display *display, Window window);
void handle_keypress(XKeyEvent *event);
void play();
void pause();
void stop();
void next_track();
void previous_track();
void increase_volume();
void decrease_volume();
void toggle_fullscreen(Window *window);
void display_welcome_message(Display *display, Window window);

#endif // PLAYER_H
