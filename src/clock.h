#ifndef CLOCK_H
#define CLOCK_H

#include "output/display.h"
#include "data/time.h"
#include "input/orientation.h"

enum ClockMode
{
    SHOW_CLOCK,
    SET_CLOCK,
    SET_ALARM_1,
    SET_ALARM_2
};

class Clock
{
public:
    Clock(Display *display, Time *time, Orientation *orientation) : display(display), time(time), orientation(orientation) {}

    void show()
    {
        mode = SHOW_CLOCK;
        if(position == TOP)
            display->setMarkers(alarmBits);
        else
            display->setMarkers(0);
        display->show(time->hour(), time->minute(), FADE);
    }

    void turn(Position position)
    {
        this->position = position;
        display->setPosition(position);
        if (position == TOP)
            display->setMarkers(alarmBits);
        else
            display->setMarkers(0);
        display->show(time->hour(), time->minute(), FALL);
    }

    void update()
    {
        display->show(time->hour(), time->minute(), SLIDE);
    }

    void setClock()
    {
        hour = time->hour();
        minute = time->minute();
        mode = SET_CLOCK;
    }

    void setAlarm(uint8_t alarm)
    {
        hour = time->hour(alarm);
        minute = time->minute(alarm);
        if (alarm == 1)
            mode = SET_ALARM_1;
        else
            mode = SET_ALARM_2;
    }

    void edit()
    {
        editHour = true;
        display->show(hour, minute, FADE);
        display->blinkLeft();
    }

    void up()
    {
        if (editHour)
        {
            if (hour < 24)
            {
                hour++;
            }
        }
        else
        {
            if (minute < 59)
            {
                minute++;
            }
        }
        display->show(hour, minute, SLIDE_UP);
    }

    void down()
    {
        if (editHour)
        {
            if (hour > 0)
            {
                hour--;
            }
        }
        else
        {
            if (minute > 0)
            {
                minute--;
            }
        }
        display->show(hour, minute, SLIDE_DOWN);
    }

    boolean next()
    {
        if (editHour)
        {
            display->blinkRight();
            editHour = false;
            return true;
        }
        else
        {
            commit();
            return false;
        }
    }

    void commit()
    {
        switch (mode)
        {
        case SET_ALARM_1:
            alarmBits |= 1;
            break;
        case SET_ALARM_2:
            alarmBits |= 2;
            break;
        default:
            break;
        }
        Serial.println("commit");
    }

    void disable(uint8_t alarm)
    {
        if (alarmBits == 3)
            alarmBits &= ~alarm;
        else
            alarmBits = alarm == 1 ? 2 : 1;
    }

    void discard()
    {
        switch (mode)
        {
        case SET_ALARM_1:
            disable(1);
            break;
        case SET_ALARM_2:
            disable(2);
            break;
        default:
            break;
        }
        Serial.println("discard");
    }

    byte alarms()
    {
        return this->alarmBits;
    }

    void hideAlarms()
    {
        display->setMarkers(0);
    }

private:
    byte alarmBits = 1;
    ClockMode mode;
    boolean editHour;
    uint8_t hour;
    uint8_t minute;
    Position position;
    Display *display;
    Time *time;
    Orientation *orientation;
};

#endif