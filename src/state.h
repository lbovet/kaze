#ifndef STATE_H
#define STATE_H

#include "event.h"
#include "display.h"
#include "player.h"
#include "time.h"

enum State
{
    IDLE
};

class StateMachine
{
public:
    StateMachine(Display *display, Player *player, Time *time) : display(display), player(player), time(time) {}

    boolean update(Event event)
    {
        boolean consumed = true;

        switch (state())
        {
        case IDLE:
            switch (event)
            {
            case INIT:
                display->show(time->hour(), time->minute(), FADE);
                break;
            case TIME:
                display->setBar(0x02 | 0x20, 8 - ((short)(time->second() * 8) / 60));
                display->show(time->hour(), time->minute(), SLIDE);
                break;
            case TURN_DOWN:
                display->setTurned(true);
                display->show(time->hour(), time->minute(), FALL);
                break;
            case TURN_UP:
                display->setTurned(false);
                display->show(time->hour(), time->minute(), FALL);
                break;
            default:
                consumed = false;
            }
            break;
        default:
            consumed = false;
        }
        return consumed;
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

    Display *display;
    Player *player;
    Time *time;
};

#endif