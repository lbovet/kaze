#ifndef STATE_H
#define STATE_H

#include "event.h"
#include "display.h"
#include "player.h"
#include "time.h"

enum State
{
    IDLE,
    ANIMATION,
    TIME_SET
};

class StateMachine
{
public:
    StateMachine(EventBus *bus, Display *display, Player *player, Time *time) : bus(bus), display(display), player(player), time(time) {}

    void update()
    {
        Event event = bus->next();
        if (event)
        {
            if(event != TIME) {
                Serial.print("event ");
                Serial.print(event);
                Serial.print(" state ");
                Serial.println(state());
            }
            switch (state())
            {
            case IDLE:
                switch (event)
                {
                case INIT:
                    display->show(time->hour(), time->minute(), FADE);
                    push(ANIMATION);
                    bus->post(DELAY, 500);
                    bus->acknowledge(event);
                    break;
                case TIME:
                    display->show(time->hour(), time->minute(), SLIDE);
                    break;
                case TURN_DOWN:
                    display->setTurned(true);
                    display->setBar(0x02 | 0x20, 8 - ((short)(time->second() * 8) / 60));
                    display->show(time->hour(), time->minute(), FALL);
                    push(ANIMATION);
                    bus->post(DELAY, 2000);
                    bus->acknowledge(event);
                    break;
                case TURN_UP:
                    display->setTurned(false);
                    display->setBar(0,0);
                    display->show(time->hour(), time->minute(), FALL);
                    push(ANIMATION);
                    bus->post(DELAY, 2000);
                    bus->acknowledge(event);
                    break;
                case TAP:
                    display->show(CLOCK, time->minute(), true, FADE);
                    set(TIME_SET);
                default:
                    break;
                }
                break;
            case ANIMATION:
                switch (event)
                {
                case DELAY:
                    pop();
                    break;
                default:
                    break;
                }
                break;
            case TIME_SET:
                switch (event)
                {
                case TAP:
                    set(IDLE);
                    bus->post(INIT);
                    break;
                case SWIPE_UP:
                    display->show(CLOCK, time->minute()+1, true, SLIDE_UP);
                    break;
                case SWIPE_DOWN:
                    display->show(CLOCK, time->minute() -1, true, SLIDE_DOWN);
                    break;
                default:
                    break;
                }
            default:
                break;
            }

            switch(event) {
                case DELAY:
                case TIME:
                case TOUCH:
                case TAP:
                case PRESS:
                case SWIPE_UP:
                case SWIPE_DOWN:
                case SCROLL_UP:
                case SCROLL_DOWN:
                    bus->acknowledge(event);
                default:
                    break; // explicit acknowledge
            }
        }
    }

private:
    void set(State state)
    {
        current = state;
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
    State stacked;
    State current = IDLE;

    EventBus *bus;
    Display *display;
    Player *player;
    Time *time;
};

#endif