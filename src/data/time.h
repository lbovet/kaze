#ifndef TIME_H
#define TIME_H

#include <uRTCLib.h>

class Time
{
public:
    void begin()
    {
        rtc.set_rtc_address(0x68);
        rtc.set_model(URTCLIB_MODEL_DS3232);
        rtc.refresh();
        rtc.alarmClearFlag(URTCLIB_ALARM_1);
        rtc.alarmClearFlag(URTCLIB_ALARM_2);
    }

    boolean update()
    {
        rtc.refresh();
        if (last != rtc.second())
        {
            last = rtc.second();
            return true;
        }
        return false;
    }

    uint8_t hour(uint8_t alarm = 0)
    {
        switch (alarm)
        {
        case 1:
            return rtc.alarmHour(URTCLIB_ALARM_1);
        case 2:
            return rtc.alarmHour(URTCLIB_ALARM_2);
        default:
            return rtc.hour();
        }
    }

    uint8_t minute(uint8_t alarm = 0)
    {
        switch (alarm)
        {
        case 1:
            return rtc.alarmMinute(URTCLIB_ALARM_1);
        case 2:
            return rtc.alarmMinute(URTCLIB_ALARM_2);
        default:
            return rtc.minute();
        }
    }

    uint8_t second()
    {
        return rtc.second();
    }

    void set(uint8_t hour, uint8_t minute, uint8_t alarm = 0)
    {
        switch (alarm)
        {
        case 1:
            rtc.alarmSet(URTCLIB_ALARM_TYPE_1_FIXED_HMS, 0, minute, hour, 0);
            break;
        case 2:
            rtc.alarmSet(URTCLIB_ALARM_TYPE_2_FIXED_HM, 0, minute, hour, 0);
            break;
        default:
            rtc.set(0, minute, hour, 0, 0, 0, 0);
            break;
        }
        rtc.refresh();
    }

    void deactivateAlarm(uint8_t alarm)
    {
        switch (alarm)
        {
        case 1:
            rtc.alarmDisable(URTCLIB_ALARM_1);
            break;
        case 2:
            rtc.alarmDisable(URTCLIB_ALARM_2);
            break;
        default:
            break;
        }
    }

    boolean alarmSet(uint8_t alarm)
    {
        rtc.refresh();
        switch (alarm)
        {
        case 1:
            Serial.println(rtc.alarmMode(URTCLIB_ALARM_1));
            return rtc.alarmMode(URTCLIB_ALARM_1) == URTCLIB_ALARM_TYPE_1_FIXED_HMS;
            break;
        case 2:
            Serial.println(rtc.alarmMode(URTCLIB_ALARM_2));
            return rtc.alarmMode(URTCLIB_ALARM_2) == URTCLIB_ALARM_TYPE_2_FIXED_HM;
            break;
        default:
            return false;
        }
    }

    boolean checkAlarm()
    {
        if (rtc.alarmTriggered(URTCLIB_ALARM_1))
        {
            rtc.alarmClearFlag(URTCLIB_ALARM_1);
            alarm = 1;
            return true;
        }
        if (rtc.alarmTriggered(URTCLIB_ALARM_2))
        {
            rtc.alarmClearFlag(URTCLIB_ALARM_2);
            alarm = 2;
            return true;
        }
        return false;
    }

    uint8_t lastAlarm()
    {
        return alarm;
    }

private:
    uint8_t last = 255;
    uint8_t alarm = 1;
    uRTCLib rtc;
};

Time time;

#endif