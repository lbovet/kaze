#ifndef TOUCH_H
#define TOUCH_H

#include <Wire.h>
#include "Adafruit_MPR121.h"

#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

class Touch
{
public:
    void begin()
    {
        if (!cap.begin(0x5B, &Wire, 2))
        {
            Serial.println("MPR121 not found, check wiring?");
        }
    }

    void update()
    {
        current = cap.touched();

        for (uint8_t i = 0; i < 12; i++)
        {
            // it if *is* touched and *wasnt* touched before, alert!
            if ((current & _BV(i)) && !(last & _BV(i)))
            {
                Serial.print(i);
                Serial.println(" touched");
                Serial.print(cap.filteredData(i));
                Serial.print(" ");
                Serial.println(cap.baselineData(i));
            }
            // if it *was* touched and now *isnt*, alert!
            if (!(current & _BV(i)) && (last & _BV(i)))
            {
                Serial.print(i);
                Serial.println(" released");
            }
        }

        last = current;
    }

private:
    uint16_t last = 0;
    uint16_t current = 0;
    Adafruit_MPR121 cap;
};

#endif