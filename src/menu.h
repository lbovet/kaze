#ifndef MENU_H
#define MENU_H

#include "output/display.h"
#include "output/player.h"
#include "input/orientation.h"
#include "timer.h"

class Menu
{
public:
    Menu(Display *display, Timer *timer, Player *player) : display(display), timer(timer), player(player) {}

    void open()
    {
        if (timer->active())
        {
            symbol = HOURGLASS;
            alarm = 0;
            timer->progress();
        }
        else
        {
            symbol = BELL;
            alarm = 1;
        }
        display->show(symbol, alarm ? alarm : BLANK, false, FADE);
    }

    void up()
    {
        switch (symbol)
        {
        case LOVE:
            symbol = CHILL;
            break;
        case CHILL:
            symbol = SLEEP;
            break;
        case SLEEP:
            symbol = BELL;
            alarm = 1;
            break;
        case BELL:
            if (alarm == 2)
            {
                symbol = HOURGLASS;
                alarm = 0;
                timer->progress();
            }
            else
                alarm++;
            break;
        case HOURGLASS:
            timer->hide();
            symbol = WALL_CLOCK;
            break;
        case WALL_CLOCK:
            symbol = LOVE;
            break;
        default:
            break;
        }
        display->show(symbol, alarm ? alarm : BLANK, false, SLIDE_UP);
    }

    void down()
    {
        switch (symbol)
        {
        case WALL_CLOCK:
            symbol = HOURGLASS;
            timer->progress();
            break;
        case HOURGLASS:
            timer->hide();
            symbol = BELL;
            alarm = 2;
            break;
        case BELL:
            if (alarm == 1)
            {
                symbol = SLEEP;
                alarm = 0;
            }
            else
                alarm--;
            break;
        case SLEEP:
            symbol = CHILL;
            break;
        case CHILL:
            symbol = LOVE;
            break;
        case LOVE:
            symbol = WALL_CLOCK;
            break;
        default:
            break;
        }
        display->show(symbol, alarm ? alarm : BLANK, false, SLIDE_DOWN);
    }

    uint8_t
    select(uint8_t timeSet, uint8_t timer, uint8_t alarm1, uint8_t alarm2, uint8_t back)
    {
        switch (symbol)
        {
        case WALL_CLOCK:
            return timeSet;
        case HOURGLASS:
            return timer;
        case BELL:
            if (alarm == 1)
                return alarm1;
            if (alarm == 2)
                return alarm2;
        default:
            return back;
        }
    }

    boolean disable()
    {
        switch (symbol)
        {
        case HOURGLASS:
            timer->disable();
            return true;
        default:
            return false;
        }
    }

private:
    Symbol symbol = BELL;
    uint8_t alarm = 0;
    Display *display;
    Timer *timer;
    Player *player;
};

#endif