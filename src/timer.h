#ifndef ACTION_H
#define ACTION_H

#include <Chrono.h>
#include "output/display.h"
#include "output/player.h"

class Timer
{
public:
    Timer(Display *display, Player *player) : display(display), player(player) {}

    void open()
    {
        display->show(HOURGLASS, active ? value() : minutes, false, FADE);
        active = false;
    }

    void activate()
    {
        active = true;
        chrono.restart();
    }

    void discard()
    {
        active = false;
    }

    boolean update()
    {
        Serial.println(active);
        Serial.println(chrono.elapsed() / 1000);
        Serial.println(minutes * 60);

        if(active && chrono.elapsed() / 1000 > minutes * 60)
        {
            Serial.println("ALARM");
            active = false;
            return true;
        } else {
            return false;
        }
    }

    void progress() {
        if(active) {
            unsigned long elapsed = chrono.elapsed() / 1000;
            unsigned long ratio = 7 * (minutes * 60 - elapsed) / (minutes * 60) + 1;
            Serial.println(ratio);
            display->setProgress(max(0, ratio));
        } else {
            display->setProgress(0);
        }
    }

    void change(Direction up)
    {
        if(active) {
            minutes = value();
        }
        display->show(HOURGLASS, minutes += (up ? 1 : -1), false, up ? SLIDE_UP : SLIDE_DOWN);
    }

private:
    uint8_t value()
    {
        unsigned long elapsed = chrono.elapsed() / 1000;
        if (elapsed > minutes * 60)
        {
            return 0;
        }
        else {
            return minutes - (elapsed / 60);
        }
    }
    Chrono chrono;
    boolean active;
    uint8_t minutes = 15;
    Display *display;
    Player *player;
};

#endif