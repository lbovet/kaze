#ifndef CLOCK_H
#define CLOCK_H

#include "output/display.h"
#include "input/time.h"

class Clock
{
public:
    Clock(Display *display, Time *time) : display(display), time(time) {}

    void show()
    {
        display->show(time->hour(), time->minute(), FADE);
    }

    void turn(boolean orientation)
    {
        display->setTurned(orientation);
        if (orientation)
        {
            display->setBar(0x02 | 0x20, 8 - ((short)(time->second() * 8) / 60));
        }
        display->show(time->hour(), time->minute(), FALL);
    }

    void update()
    {
        display->show(time->hour(), time->minute(), SLIDE);
    }

private:
    Display *display;
    Time *time;
};

#endif