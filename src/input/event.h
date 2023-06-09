#ifndef EVENT_H
#define EVENT_H

#include <Arduino.h>
#include <Chrono.h>

enum Event
{
    NOTHING = 0,
    DELAY,
    INIT,
    TAP,
    PRESS,
    SWIPE_UP,
    SWIPE_DOWN,
    SCROLL_UP,
    SCROLL_DOWN,
    TOUCH,
    TURN_UP,
    TURN_DOWN,
    TIME,
    ELAPSED,
    ALARM
};

enum Direction
{
    DOWN,
    UP
};

#define LAST_EVENT ALARM

class EventBus
{
public:
    void post(Event event, uint32_t delay = 0)
    {
        if (event != NOTHING)
        {
            state = state | bit(event);
            if (delay)
            {
                this->delay = delay;
                chrono.restart();
            }
        }
    }

    Event next()
    {
        for (Event event = NOTHING; event <= LAST_EVENT; event = (Event)((int)event + 1))
        {
            if (state & bit(event))
            {
                if (event != DELAY || chrono.hasPassed(delay))
                    return event;
            }
        }
        return NOTHING;
    }

    void clear(Event event)
    {
        state = state & ~bit(event);
    }

private:
    Chrono chrono;
    uint32_t delay;
    uint16_t state = 0;
};

EventBus bus;

#endif