#ifndef LIGHT_H
#define LIGHT_H

#include <Arduino.h>

class LightSensor {

public:
    uint8_t value() {
        return analogRead(A1) / 64;
    }
};

LightSensor lightSensor;

#endif