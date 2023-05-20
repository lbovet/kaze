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
        display->show(time->hour(), time->minute(), FADE);
    }

    void turn(Position position)
    {
        this->position = position;
        display->setPosition(position);
        display->show(time->hour(), time->minute(), FALL);
    }

    void update()
    {
        display->show(time->hour(), time->minute(), SLIDE);
    }

private:
    Position position;
    Display *display;
    Time *time;
};

#endif