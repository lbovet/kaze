#ifndef STATE_H
#define STATE_H

#include "input/event.h"
#include "data/time.h"
#include "output/display.h"
#include "output/player.h"
#include "clock.h"
#include "timer.h"

enum State
{
    CLOCK,
    TIMER,
    WAIT
};

#define TIMEOUT        \
    if (event == TIME) \
    {                  \
        break;         \
    }                  \
    else               \
    {                  \
        timeout(8000); \
    }

#define SKIP_TOUCH                 \
    if (event == TOUCH)          \
    {                            \
        break;                   \
        Serial.println("break"); \
    }

#define TURN_SKIP (event != TURN_DOWN && event != TURN_UP)

class StateMachine
{
public:
    StateMachine(EventBus *bus, Clock *clock, Timer *timer) : bus(bus), clock(clock), timer(timer)
    {
        set(CLOCK);
    }

    void update()
    {
        Event event = bus->next();
        if (event)
        {
            if (event != TIME)
            {
                Serial.print("event ");
                Serial.print(event);
                Serial.print(" state ");
                Serial.println(state());
            }
            switch (state())
            {
            case CLOCK:
                switch (event)
                {
                case INIT:
                    clock->show();
                    wait(800);
                    acknowledge(event);
                    break;
                case TIME:
                    clock->update();
                    timer->progress();
                    break;
                case TURN_DOWN:
                    clock->turn(BOTTOM);
                    wait(2000);
                    acknowledge(event);
                    break;
                case TURN_UP:
                    clock->turn(TOP);
                    wait(2000);
                    acknowledge(event);
                    break;
                case TAP:
                    set(TIMER);
                default:
                    break;
                }
                break;
            case WAIT:
                switch (event)
                {
                case DELAY:
                    pop();
                    break;
                default:
                    break;
                }
                break;
            case TIMER:
                TIMEOUT
                switch (event)
                {
                case INIT:
                    timer->open();
                    acknowledge(event);
                    break;
                case TAP:
                case DELAY:
                    timer->activate();
                    set(CLOCK, TURN_SKIP);
                    break;
                case SWIPE_UP:
                case SCROLL_UP:
                    timer->change(UP);
                    break;
                case SWIPE_DOWN:
                case SCROLL_DOWN:
                    timer->change(DOWN);
                    break;
                default:
                    SKIP_TOUCH
                    timer->discard();
                    set(CLOCK, TURN_SKIP);
                    break;
                }
                break;
            default:
                break;
            }

            switch (event)
            {
            case INIT:
            case TURN_DOWN:
            case TURN_UP:
            case ALARM:
                break; // requires explicit acknowledge
            default:
                acknowledge(event); // swallow event in all cases
            }
        }
    }

private:
    void set(State state, boolean init = true)
    {
        current = state;
        if (init)
        {
            bus->post(INIT);
        }
    }

    void push(State state)
    {
        stacked = current;
        current = state;
    }

    void pop()
    {
        current = stacked;
    }

    State state()
    {
        return current;
    }

    void acknowledge(Event event)
    {
        bus->clear(event);
    }

    void wait(uint16_t delay)
    {
        bus->post(DELAY, delay);
        push(WAIT);
    }

    void timeout(uint16_t delay)
    {
        bus->post(DELAY, delay);
    }

    State stacked;
    State current;

    EventBus *bus;
    Clock *clock;
    Timer *timer;
};

#endif