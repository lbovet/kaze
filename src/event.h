#ifndef EVENT_H
#define EVENT_H

#include <Arduino.h>
#include <Chrono.h>

enum Event {
    IDLE = 0,
    TAP,
    UP,
    DOWN,
    PRESS,
    DELAY,
    TURN,
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
        for(Event event = IDLE; event <= ALARM; event = (Event)((int)event+1))
        {
            if(state & bit(event)) {
                if(event != DELAY || chrono.hasPassed(delay))
                    return event;
            }
        }
        return IDLE;
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