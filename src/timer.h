#ifndef ACTION_H
#define ACTION_H

#include <Chrono.h>
#include "output/display.h"
#include "output/player.h"

class Timer
{
public:
    void open()
    {
        display.show(HOURGLASS, running ? value() : minutes, false, DISSOLVE);
    }

    void activate()
    {
        if (!running)
        {
            chrono.restart();
        }
        running = true;
    }

    void disable()
    {
        running = false;
        hide();
    }

    boolean update()
    {
        if (running && chrono.elapsed() / 1000 > minutes * 60)
        {
            running = false;
            return true;
        }
        else
        {
            return false;
        }
    }

    boolean active()
    {
        return running;
    }

    void progress()
    {
        if (running)
        {
            unsigned long elapsed = chrono.elapsed() / 1000;
            unsigned long ratio = 7 * (minutes * 60 - elapsed) / (minutes * 60) + 1;
            display.setProgress(max(0, ratio));
        }
        else
        {
            display.setProgress(0);
        }
    }

    void hide()
    {
        display.setProgress(0);
    }

    void up()
    {
        if (running)
        {
            minutes = value();
        }
        running = false;
        if(minutes < 59)
            display.show(HOURGLASS, ++minutes, false, SLIDE_UP);
    }

    void down()
    {
        if (running)
        {
            minutes = value();
        }
        running = false;
        if(minutes > 1)
            display.show(HOURGLASS, --minutes, false, SLIDE_DOWN);
    }

private:
    uint8_t value()
    {
        unsigned long elapsed = chrono.elapsed() / 1000;
        if (elapsed > minutes * 60)
        {
            return 0;
        }
        else
        {
            return minutes - (elapsed / 60);
        }
    }
    Chrono chrono;
    boolean running;
    uint8_t minutes = 15;
};

Timer timer;

#endif