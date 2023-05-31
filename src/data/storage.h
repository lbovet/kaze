#ifndef STORAGE_H
#define STORAGE_H

#include "Arduino.h"
#include "Wire.h"
#include "uEEPROMLib.h"

enum Area
{
    TIMER = 0,
    VOLUME = 32
};

class Storage
{
public:
    Storage()
    {
        Wire.begin();
    }

    void write(Area area, unsigned int address, byte value)
    {
        eeprom.eeprom_write(area + address, value);
    }

    byte read(Area area, unsigned int address)
    {
        return eeprom.eeprom_read(area + address);
    }

private:
    uEEPROMLib eeprom = uEEPROMLib(0x57);
};

Storage storage;

#endif