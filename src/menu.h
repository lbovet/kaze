#ifndef MENU_H
#define MENU_H

#include "output/display.h"
#include "output/player.h"
#include "input/orientation.h"
#include "timer.h"
#include "clock.h"
#include "volume.h"

class Menu
{
public:
    void open()
    {
        if (timer.active())
        {
            symbol = HOURGLASS;
            alarm = 0;
            timer.progress();
        }
        else
        {
            symbol = BELL;
            if (clock.alarms() & 1)
                alarm = 1;
            else
                alarm = 2;
        }
        display.show(symbol, alarm ? alarm : BLANK, false, FADE);
    }

    void up()
    {
        switch (symbol)
        {
        case HEART:
            symbol = MEDITATION;
            break;
        case MEDITATION:
            symbol = MOON;
            break;
        case MOON:
            symbol = BELL;
            alarm = 1;
            break;
        case BELL:
            if (alarm == 2)
            {
                symbol = HOURGLASS;
                alarm = 0;
                timer.progress();
            }
            else
                alarm++;
            break;
        case HOURGLASS:
            timer.hide();
            symbol = WALL_CLOCK;
            break;
        case WALL_CLOCK:
            symbol = HEART;
            break;
        default:
            break;
        }
        display.show(symbol, alarm ? alarm : BLANK, false, SLIDE_UP);
    }

    void down()
    {
        switch (symbol)
        {
        case WALL_CLOCK:
            symbol = HOURGLASS;
            timer.progress();
            break;
        case HOURGLASS:
            timer.hide();
            symbol = BELL;
            alarm = 2;
            break;
        case BELL:
            if (alarm == 1)
            {
                symbol = MOON;
                alarm = 0;
            }
            else
                alarm--;
            break;
        case MOON:
            symbol = MEDITATION;
            break;
        case MEDITATION:
            symbol = HEART;
            break;
        case HEART:
            symbol = WALL_CLOCK;
            break;
        default:
            break;
        }
        display.show(symbol, alarm ? alarm : BLANK, false, SLIDE_DOWN);
    }

    uint8_t
    select(uint8_t timeSet, uint8_t timer, uint8_t back)
    {
        switch (symbol)
        {
        case BELL:
            clock.setAlarm(alarm);
            return timeSet;
        case WALL_CLOCK:
            clock.setClock();
            return timeSet;
        case HOURGLASS:
            return timer;
        default:
            Music music;
            switch (symbol)
            {
            case MOON:
                music = SLEEP;
                break;
            case MEDITATION:
                music = MASSAGE;
                break;
            case HEART:
                music = LOVE;
                break;
            default:
                return back;
            }
            volume.start(music);
            player.play(music);
            return back;
        }
    }

    void disable()
    {
        switch (symbol)
        {
        case HOURGLASS:
            timer.disable();
            break;
        case BELL:
            clock.disable(alarm);
            break;
        default:
            break;
        }
    }

private:
    Symbol symbol = BELL;
    uint8_t alarm = 0;
};

Menu menu;

#endif