#ifndef MATRIX_H
#define MATRIX_H

#include <Wire.h>
#include <Adafruit_I2CDevice.h>

#define SWAP(a, b) (a ^= b, b = a ^ b, a ^= b)

#define HT16K33_BLINK_CMD 0x80       ///< I2C register for BLINK setting
#define HT16K33_BLINK_DISPLAYON 0x01 ///< I2C value for steady on
#define HT16K33_BLINK_OFF 0          ///< I2C value for steady off
#define HT16K33_BLINK_2HZ 1          ///< I2C value for 2 Hz blink
#define HT16K33_BLINK_1HZ 2          ///< I2C value for 1 Hz blink
#define HT16K33_BLINK_HALFHZ 3       ///< I2C value for 0.5 Hz blink

#define HT16K33_CMD_BRIGHTNESS 0xE0 ///< I2C register for BRIGHTNESS setting

class Matrix
{
public:
    byte displayBuffer[8] = {0};

    Matrix(uint8_t address)
    {
        i2c_dev = new Adafruit_I2CDevice(address, &Wire);
    }

    void begin()
    {
        i2c_dev->begin();

        // turn on oscillator
        byte buffer[1] = {0x21};
        i2c_dev->write(buffer, 1);

        clear();
        writeDisplay();
        blinkRate(HT16K33_BLINK_OFF);
        setBrightness(1);
    }

    void setBrightness(uint8_t b)
    {
        if (b > 15)
            b = 15; // limit to max brightness
        byte buffer = HT16K33_CMD_BRIGHTNESS | b;
        i2c_dev->write(&buffer, 1);
    }

    void blinkRate(uint8_t b)
    {
        if (b > 3)
            b = 0; // turn off if not sure
        byte buffer = HT16K33_BLINK_CMD | HT16K33_BLINK_DISPLAYON | (b << 1);
        i2c_dev->write(&buffer, 1);
    }

    void writeDisplay()
    {
        byte buffer[17];
        buffer[0] = 0x00;

        for (byte i = 0; i < 8; i++)
        {
            buffer[1 + 2 * i] = (displayBuffer[i] >> 1) | (displayBuffer[i] << 7);
        }

        i2c_dev->write(buffer, 17);
    }

    void clear()
    {
        for (byte i = 0; i < 8; i++)
        {
            displayBuffer[i] = 0;
        }
    }

    void rotate()
    {
        SWAP(displayBuffer[0], displayBuffer[7]);
        SWAP(displayBuffer[1], displayBuffer[6]);
        SWAP(displayBuffer[2], displayBuffer[5]);
        SWAP(displayBuffer[3], displayBuffer[4]);
        for (byte i = 0; i < 8; i++)
        {
            displayBuffer[i] = reverse(displayBuffer[i]);
        }
    }

private:
    byte reverse(byte b)
    {
        b = (b & 0b11110000) >> 4 | (b & 0b00001111) << 4;
        b = (b & 0b11001100) >> 2 | (b & 0b00110011) << 2;
        b = (b & 0b10101010) >> 1 | (b & 0b01010101) << 1;
        return b;
    }

    Adafruit_I2CDevice *i2c_dev;
};

#endif