#ifndef ORIENTATION_H
#define ORIENTATION_H

#include <Arduino.h>

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
            bool newValue = sensor > 0;
            sensor = 0;
            if (newValue != current)
            {
                current = newValue;
                return true;
            }
        }
        return false;
    }
    boolean value()
    {
        return current;
    }

private:
    bool current = 0;
    int16_t sensor;
};

#endif