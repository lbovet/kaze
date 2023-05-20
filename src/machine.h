#ifndef MACHINE_H
#define MACHINE_H

#include "input/event.h"
#include "input/time.h"
#include "output/display.h"
#include "output/player.h"
#include "timer.h"

enum State
{
    CLOCK,
    TIMER,
    WAIT
};

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
                    wait(500);
                    acknowledge(event);
                    break;
                case TIME:
                    clock->update();
                    break;
                case TURN_DOWN:
                    clock->turn(true);
                    wait(2000);
                    acknowledge(event);
                    break;
                case TURN_UP:
                    clock->turn(false);
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
                switch (event)
                {
                case INIT:
                    timer->open();
                    acknowledge(event);
                    break;
                case TAP:
                    timer->close();
                    set(CLOCK);
                    break;
                case SWIPE_UP:
                    timer->change(true);
                    break;
                case SWIPE_DOWN:
                    timer->change(false);
                    break;
                default:
                    break;
                }
            default:
                break;
            }

            switch (event)
            {
            case DELAY:
            case TIME:
            case TOUCH:
            case TAP:
            case PRESS:
            case SWIPE_UP:
            case SWIPE_DOWN:
            case SCROLL_UP:
            case SCROLL_DOWN:
                acknowledge(event);
            default:
                break; // explicit acknowledge
            }
        }
    }

private:
    void set(State state)
    {
        current = state;
        bus->post(INIT);
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

    State stacked;
    State current;

    EventBus *bus;
    Clock *clock;
    Timer *timer;
};

#endif