#ifndef ACTION_H
#define ACTION_H

#include "input/event.h"
#include "input/time.h"
#include "output/display.h"
#include "output/player.h"

class Timer
{
public:
    Timer(Display *display, Player *player, Time *time) : display(display), player(player), time(time) {}

    void open()
    {
        display->show(HOURGLASS, time->minute(), true, FADE);
    }

    void close()
    {
    }

    void change(boolean up)
    {
        display->show(HOURGLASS, time->minute() + (up ? 1 : -1), true, up ? SLIDE_UP : SLIDE_DOWN);
    }

private:
    Display *display;
    Player *player;
    Time *time;
};

#endif