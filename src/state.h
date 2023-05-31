#ifndef STATE_H
#define STATE_H

#include <MemoryFree.h>

#include "input/event.h"
#include "input/orientation.h"
#include "data/time.h"
#include "output/display.h"
#include "output/player.h"
#include "clock.h"
#include "timer.h"
#include "menu.h"
#include "volume.h"

#define DEFAULT_TIMEOUT \
    if (event == TIME)  \
    {                   \
        break;          \
    }                   \
    else                \
    {                   \
        timeout(8000);  \
    }

#define SKIP_TOUCH      \
    if (event == TOUCH) \
    {                   \
        break;          \
    }

#define SKIP_TIME      \
    if (event == TIME) \
    {                  \
        break;         \
    }

#define SKIP_SCROLL                                 \
    if (event == SCROLL_UP || event == SCROLL_DOWN) \
    {                                               \
        break;                                      \
    }

#define TURN_SKIP (event != TURN_DOWN && event != TURN_UP)

class StateMachine
{
    enum State
    {
        CLOCK,
        TIME_SET,
        TIMER,
        MENU,
        WAIT,
        CHIME,
        SNOOZE,
        VOLUME
    };

public:
    StateMachine()
    {
        set(CLOCK);
    }

    void update()
    {
        Event event = bus.next();
        if (event)
        {
            if (event != TIME)
            {
                if (time.hour() < 10)
                    Serial.print(0);
                Serial.print(time.hour());
                Serial.print(F(":"));
                if (time.minute() < 10)
                    Serial.print(0);
                Serial.print(time.minute());
                Serial.print(F(":"));
                if (time.second() < 10)
                    Serial.print(0);
                Serial.print(time.second());
                Serial.print(F(" event "));
                Serial.print(event);
                Serial.print(F(" state "));
                Serial.print(state());
                Serial.print(F(" free "));
                Serial.print(freeMemory());
                Serial.println(F(" B"));
            }
            switch (state())
            {
            case CLOCK:
                switch (event)
                {
                case INIT:
                    clock.show();
                    clock.clearAlarm();
                    timer.progress();
                    acknowledge(event);
                    wait(800);
                    break;
                case TIME:
                    clock.update();
                    timer.progress();
                    break;
                case TURN_DOWN:
                    clock.turn(BOTTOM);
                    timer.disable();
                    wait(2000);
                    acknowledge(event);
                    break;
                case TURN_UP:
                    clock.turn(TOP);
                    wait(2000);
                    acknowledge(event);
                    break;
                case SWIPE_DOWN:
                case SWIPE_UP:
                    if (player.playing())
                    {
                        timer.hide();
                        clock.hideAlarms();
                        push(VOLUME);
                    }
                    break;
                case PRESS:
                    volume.smoothStop();
                    break;
                case TAP:
                    timer.hide();
                    clock.hideAlarms();
                    set(MENU);
                    break;
                case ELAPSED:
                    set(CHIME);
                    acknowledge(event);
                    break;
                case ALARM:
                    if (orientation.current() == TOP)
                    {
                        clock.triggerAlarm();
                        set(CHIME);
                    }
                    acknowledge(event);
                    break;
                default:
                    break;
                }
                break;
            case WAIT:
                switch (event)
                {
                case DELAY:
                    pop(false);
                    break;
                default:
                    break;
                }
                break;
            case TIME_SET:
                DEFAULT_TIMEOUT
                switch (event)
                {
                case INIT:
                    clock.edit();
                    acknowledge(event);
                    break;
                case TAP:
                    if (!clock.next())
                        set(CLOCK);
                    break;
                case PRESS:
                    clock.discard();
                    back();
                    break;
                case SWIPE_UP:
                case SCROLL_UP:
                    clock.up();
                    break;
                case SWIPE_DOWN:
                case SCROLL_DOWN:
                    clock.down();
                    break;
                default:
                    SKIP_TOUCH
                    clock.commit();
                    set(CLOCK, TURN_SKIP);
                    break;
                }
                break;
            case TIMER:
                DEFAULT_TIMEOUT
                switch (event)
                {
                case INIT:
                    timer.open();
                    acknowledge(event);
                    break;
                case TAP:
                case DELAY:
                    timer.activate();
                    set(clock.alarmTriggered() ? SNOOZE : CLOCK);
                    break;
                case SWIPE_UP:
                case SCROLL_UP:
                    timer.up();
                    break;
                case SWIPE_DOWN:
                case SCROLL_DOWN:
                    timer.down();
                    break;
                case PRESS:
                    if (!clock.alarmTriggered())
                        timer.disable();
                    back(clock.alarmTriggered() ? SNOOZE : CLOCK);
                    break;
                default:
                    SKIP_TOUCH
                    timer.disable();
                    set(CLOCK, TURN_SKIP);
                    break;
                }
                break;
            case MENU:
                DEFAULT_TIMEOUT
                switch (event)
                {
                case INIT:
                    menu.open();
                    acknowledge(event);
                    break;
                case SWIPE_UP:
                    menu.up();
                    break;
                case SWIPE_DOWN:
                    menu.down();
                    break;
                case TAP:
                    set((State)menu.select(TIME_SET, TIMER, CLOCK));
                    break;
                case PRESS:
                    menu.disable();
                    back();
                    break;
                default:
                    SKIP_TOUCH
                    SKIP_SCROLL
                    set(CLOCK, TURN_SKIP);
                    break;
                }
                break;
            case CHIME:
                switch (event)
                {
                case INIT:
                    clock.show();
                    timer.disable();
                    if (!player.playing())
                        player.play();
                    acknowledge(event);
                    timeout(10 * 60 * 1000L);
                    break;
                case TIME:
                    clock.update();
                    break;
                case TAP:
                    volume.smoothStop();
                    if (clock.alarmTriggered())
                        set(SNOOZE);
                    else
                        set(CLOCK);
                    break;
                case SWIPE_UP:
                case SWIPE_DOWN:
                    if (clock.alarmTriggered())
                    {
                        volume.smoothStop();
                        set(SNOOZE);
                    }
                    else
                    {
                        clock.hideAlarms();
                        timer.hide();
                        push(VOLUME);
                    }
                    break;
                case ALARM:
                case ELAPSED:
                    break;
                    acknowledge(event);
                default:
                    SKIP_TOUCH
                    SKIP_SCROLL
                    volume.smoothStop();
                    set(CLOCK, TURN_SKIP);
                    break;
                }
                break;
            case SNOOZE:
                switch (event)
                {
                case INIT:
                    clock.show(REPLACE);
                    if (!timer.active())
                        timer.start(true);
                    timer.progress();
                    acknowledge(event);
                    wait(800);
                    break;
                case TIME:
                    clock.update();
                    timer.progress();
                    break;
                case TAP:
                case SWIPE_UP:
                case SWIPE_DOWN:
                    clock.hideAlarms();
                    timer.hide();
                    set(TIMER);
                    break;
                case PRESS:
                    timer.disable();
                    back();
                    break;
                case ALARM:
                case ELAPSED:
                    set(CHIME);
                    acknowledge(event);
                    break;
                case DELAY:
                    break;
                default:
                    SKIP_TOUCH
                    SKIP_SCROLL
                    set(CLOCK, TURN_SKIP);
                    break;
                }
                break;
            case VOLUME:
                DEFAULT_TIMEOUT
                switch (event)
                {
                case INIT:
                    volume.open();
                    acknowledge(event);
                    break;
                case TAP:
                case DELAY:
                    volume.commit();
                    pop();
                    break;
                case SWIPE_UP:
                case SCROLL_UP:
                    volume.up();
                    break;
                case SWIPE_DOWN:
                case SCROLL_DOWN:
                    volume.down();
                    break;
                default:
                    SKIP_TOUCH
                    volume.smoothStop();
                    set(CLOCK, TURN_SKIP);
                    break;
                }
                break;
            }

            switch (event)
            {
            case INIT:
            case TURN_DOWN:
            case TURN_UP:
            case ALARM:
            case ELAPSED:
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
            bus.post(INIT);
        }
    }

    void push(State state, boolean init = true)
    {
        if (init)
        {
            bus.post(INIT);
        }
        stacked = current;
        current = state;
    }

    void pop(boolean init = true)
    {
        current = stacked;
        if (init)
        {
            bus.post(INIT);
        }
    }

    State state()
    {
        return current;
    }

    void acknowledge(Event event)
    {
        bus.clear(event);
    }

    void wait(uint16_t delay)
    {
        bus.post(DELAY, delay);
        push(WAIT, false);
    }

    void timeout(uint32_t delay)
    {
        bus.post(DELAY, delay);
    }

    void back(State state = CLOCK)
    {
        set(state);
        display.show(BACK, BLANK, false, REPLACE);
        wait(500);
    }

    State stacked;
    State current;
};

StateMachine stateMachine;

#endif