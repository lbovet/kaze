#ifndef TOUCH_H
#define TOUCH_H

#include <Wire.h>
#include <Adafruit_MPR121.h>
#include <Chrono.h>

#include "event.h"

#define PRESS_DELAY 800
#define TAP_DELAY 40
#define BOUNCE_DELAY 600
#define QUIET_DELAY 300
#define SCROLL_DELAY 300

#define sgn(x) (((x) < 0) ? -1 : ((x) > 0))

class Touch
{
public:
    void begin()
    {
        if (!cap.begin(0x5B, &Wire, 2))
        {
            Serial.println(F("E MPR121"));
        }
        cap.setThresholds(7, 4);
    }

    Event update()
    {
        Event event = NOTHING;
        current = cap.touched();

        if (current != last)
        {
            tapping = false;
            int8_t newPosition = 0;
            if (last == 0)
            {
                if (chrono.elapsed() < BOUNCE_DELAY)
                {
                    bounce = true;
                }
                event = TOUCH;
                scroll = false;
                ref = current;
                chrono.restart();
            }
            else
            {
                newPosition = distance(ref, current);
                if (current == 0)
                {
                    if (!hold && chrono.hasPassed(TAP_DELAY))
                    {
                        if(bounce)
                            bounce = false;
                        else
                            tapping = true;
                    }
                    hold = false;
                }
                else
                {
                    if (sgn(newPosition) != sgn(lastPosition))
                    {
                        chrono.restart();
                        scroll = false;
                        event = newPosition > lastPosition ? SWIPE_UP : SWIPE_DOWN;
                        hold = true;
                    }
                }
            }
            lastPosition = newPosition;
        }
        else
        {
            if (current)
            {
                if (chrono.hasPassed(PRESS_DELAY))
                {
                    if (!hold && ref == current)
                    {
                        event = PRESS;
                        hold = true;
                    }
                }
                if (lastPosition && chrono.hasPassed(SCROLL_DELAY))
                {
                    if(scroll || chrono.hasPassed(PRESS_DELAY)) {
                        scroll = true;
                        chrono.restart();
                        event = lastPosition > 0 ? SCROLL_UP : SCROLL_DOWN;
                    }
                }
            } else {
                if(tapping && chrono.hasPassed(QUIET_DELAY)) {
                    event = TAP;
                    tapping = false;
                }
            }
        }

        last = current;
        return event;
    }

private:
    int8_t distance(uint16_t ref, uint16_t value)
    {
        int8_t refStart = -1;
        int8_t refEnd = -1;
        int8_t valueStart = -1;
        int8_t valueEnd = -1;

        for (uint16_t i = 0; i < 16; i++)
        {
            if (bit(i) & ref)
            {
                refEnd = i;
                if (refStart == 0xff)
                    refStart = i;
            }
            if (bit(i) & value)
            {
                valueEnd = i;
                if (valueStart == 0xff)
                    valueStart = i;
            }
        }
        int8_t lower = valueStart - refStart;
        int8_t upper = valueEnd - refEnd;
        if (abs(lower) == abs(upper))
        {
            return 0;
        }
        else
        {
            return abs(lower) > abs(upper) ? lower : upper;
        }
    }

    Chrono chrono;
    boolean bounce = false;
    boolean tapping = false;
    uint16_t ref = 0;
    uint16_t last = 0;
    uint16_t current = 0;
    boolean hold = false;
    boolean scroll = false;
    int8_t lastPosition = 0;
    Adafruit_MPR121 cap;
};

Touch touch;

#endif