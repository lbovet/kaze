#ifndef EVENT_H
#define EVENT_H

#include <Arduino.h>
#include <Chrono.h>

enum Event {
    NOTHING = 0,
    INIT,
    TAP,
    PRESS,
    SWIPE_UP,
    SWIPE_DOWN,
    SCROLL_UP,
    SCROLL_DOWN,
    DELAY,
    TURN_UP,
    TURN_DOWN,
    TIME,
    ALARM
};

class EventBus
{
public:
    void post(Event event, short delay = 0)
    {
        state = state | bit(event);
        if(delay)
        {
            this->delay = delay;
            chrono.restart();
        }
    }

    Event next() {
        for(Event event = NOTHING; event <= ALARM; event = (Event)((int)event+1))
        {
            if(state & bit(event)) {
                if(event != DELAY || chrono.hasPassed(delay))
                    return event;
            }
        }
        return NOTHING;
    }

    void acknowledge(Event event)
    {
        state = state & ~bit(event);
    }
private:
    Chrono chrono;
    short delay;
    uint16_t state = {0};
};

#endif