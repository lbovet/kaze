#ifndef ACTION_H
#define ACTION_H

#include "input/event.h"
#include "data/time.h"
#include "output/display.h"
#include "output/player.h"

class Timer
{
public:
    Timer(Display *display, Player *player, Time *time) : display(display), player(player), time(time) {}

    void open()
    {
        display->show(HOURGLASS, value ? value : minutes, false, FADE);
        display->setBar(0,0);
    }

    void close(boolean activate)
    {
        if(activate) {
            value = minutes;
        } else {
            value = 0;
        }
    }

    uint8_t remaining()
    {
        return value;
    }

    void change(Direction up)
    {
        if(value) {
            minutes = value;
            value = 0;
        }
        display->show(HOURGLASS, minutes += (up ? 1 : -1), false, up ? SLIDE_UP : SLIDE_DOWN);
    }

private:
    uint8_t minutes = 15;
    uint8_t value = 0;
    Display *display;
    Player *player;
    Time *time;
};

#endif