#ifndef TOUCH_H
#define TOUCH_H

#include <Wire.h>
#include <Adafruit_MPR121.h>
#include <Chrono.h>

#include "event.h"

#define PRESS_DELAY 1000

class Touch
{
public:
    void begin()
    {
        if (!cap.begin(0x5B, &Wire, 2))
        {
            Serial.println("MPR121 not found, check wiring?");
        }
        cap.setThresholds(0x10, 0x20);
    }

    Event update()
    {
        Event event = NOTHING;
        current = cap.touched();

        if (current != last)
        {
            if (last == 0)
            {
                event = TOUCH;
                chrono.restart();
            }
            else
            {
                if (current == 0)
                {
                    if (!hold && !chrono.hasPassed(PRESS_DELAY))
                    {
                        event = TAP;
                    }
                    hold = false;
                }
                else
                {
                    if (!hold)
                    {
                        hold = true;
                        event = current > last ? SWIPE_UP : SWIPE_DOWN;
                    }
                }
            }
        }
        else
        {
            if (current)
            {
                if (chrono.hasPassed(PRESS_DELAY) && !hold)
                {
                    event = PRESS;
                    hold = true;
                }
            }
        }

        last = current;
        return event;
    }

private:
    Chrono chrono;
    uint16_t last = 0;
    uint16_t current = 0;
    boolean hold = 0;
    Adafruit_MPR121 cap;
};

#endif