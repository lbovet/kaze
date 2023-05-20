#ifndef ORIENTATION_H
#define ORIENTATION_H

#include <Arduino.h>

enum Position
{
    TOP,
    BOTTOM
};

class Orientation
{
public:
    boolean update()
    {
        if (abs(sensor) < 10)
        {
            if (analogRead(A0) < 512)
                sensor--;
            else
                sensor++;
        }
        else
        {
            Position newValue = sensor < 0 ? TOP : BOTTOM;
            sensor = 0;
            if (newValue != value)
            {
                value = newValue;
                return true;
            }
        }
        return false;
    }
    Position current()
    {
        return value;
    }

private:
    Position value = TOP;
    int16_t sensor;
};

#endif