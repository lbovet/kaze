#ifndef CLOCK_H
#define CLOCK_H

#include "output/display.h"
#include "data/time.h"
#include "input/orientation.h"

class Clock
{
public:
    Clock(Display *display, Time *time) : display(display), time(time) {}

    void show()
    {
        updateBar();
        display->show(time->hour(), time->minute(), FADE);
    }

    void turn(Position position)
    {
        this->position = position;
        display->setPosition(position);
        updateBar();
        display->show(time->hour(), time->minute(), FALL);
    }

    void update()
    {
        display->show(time->hour(), time->minute(), SLIDE);
    }

private:
    void updateBar()
    {
        if (position == BOTTOM)
        {
            display->setBar(0x02 | 0x20, 8 - ((short)(time->second() * 8) / 60));
        }
        else
        {
            display->setBar(0, 0);
        }
    }
    Position position;
    Display *display;
    Time *time;
};

#endif