#ifndef STORAGE_H
#define STORAGE_H

#include "Arduino.h"
#include "Wire.h"
#include "uEEPROMLib.h"

enum Area
{
    TIMER = 0,
    VOLUME = 32,
    TRACK = 64
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

    void write(Area area, unsigned int address, byte* data, unsigned int size)
    {
        eeprom.eeprom_write(area + address, data, size);
    }

    void read(Area area, unsigned int address, byte *data, unsigned int size)
    {
        eeprom.eeprom_read(area + address, data, size);
    }

private:
    uEEPROMLib eeprom = uEEPROMLib(0x57);
};

Storage storage;

#endif