#ifndef DISPLAY_H
#define DISPLAY_H

#include <Chrono.h>

#include "matrix.h"
#include "input/orientation.h"

#define BLANK 0xff
#define DIRTY 0xfd
#define SYMBOL 0xfe

const byte symbols[] PROGMEM = {
    0x7c, 0x44, 0x7c, 0x00, 0x44, 0x7c, 0x04, 0x00, 0x5c, 0x54, 0x74, 0x00,
    0x54, 0x54, 0x7c, 0x00, 0x70, 0x10, 0x7c, 0x00, 0x74, 0x54, 0x5c, 0x00,
    0x7c, 0x54, 0x5c, 0x00, 0x60, 0x40, 0x7c, 0x00, 0x7c, 0x54, 0x7c, 0x00,
    0x74, 0x54, 0x7c, 0x00, 0x38, 0x44, 0x82, 0xb2, 0x8a, 0x44, 0x38, 0x00,
    0x00, 0x0c, 0x74, 0x84, 0x86, 0x74, 0x0c, 0x00, 0x00, 0xc6, 0xaa, 0x92,
    0xaa, 0xc6, 0x00, 0x00, 0x06, 0x06, 0x7e, 0x40, 0x46, 0x46, 0x7e, 0x00,
    0x38, 0x28, 0x44, 0xfe, 0x00, 0x54, 0x92, 0x00, 0x00, 0x7c, 0x38, 0x10,
    0x00, 0x7c, 0x38, 0x10, 0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
    0x10, 0x10, 0x10, 0x10, 0x7c, 0x38, 0x10, 0x00, 0x08, 0x1c, 0x3e, 0x08,
    0x08, 0x78, 0x00, 0x00, 0x7c, 0x82, 0x8a, 0x82, 0xa2, 0x82, 0x7c, 0x00,
    0x00, 0x42, 0x66, 0x5a, 0x24, 0x18, 0x00, 0x00, 0x06, 0x08, 0x32, 0xc4,
    0x32, 0x08, 0x06, 0x00, 0x30, 0x48, 0x44, 0x22, 0x44, 0x48, 0x30, 0x00};

enum Symbol
{
    WALL_CLOCK = 5,
    BELL = 6,
    HOURGLASS = 7,
    MUSIC = 8,
    VOLUME = 9,
    NEXT = 10,
    STOP = 11,
    INTERVAL = 12,
    BACK = 13,
    RANDOM = 14,
    SLEEP = 15,
    CHILL = 16,
    LOVE = 17,
    EMPTY = 255
};

enum Transition
{
    REPLACE,
    FADE,
    DISSOLVE,
    SLIDE,
    SLIDE_DOWN,
    SLIDE_UP,
    FALL
};

class Display
{
public:
    void begin()
    {
        matrix1.begin();
        matrix2.begin();
    }

    void show(Symbol symbol, byte right, boolean pad, Transition transition)
    {
        chrono.restart();
        if (iteration > 0)
        {
            if (this->transition == FALL)
            {
                position = requested;
            }
            this->transition = REPLACE;
            update(true);
        }

        mask = 0;
        if (data[1] != symbol)
        {
            data[0] = SYMBOL;
            data[1] = symbol;
            mask |= 0x00ff;
        }
        if (data[2] != right / 10)
        {
            mask |= 0x0f00;
            data[2] = right == BLANK ? BLANK : (right < 10 && !pad ? BLANK : right / 10);
        }
        if (data[3] != right % 10)
        {
            mask |= 0xf000;
            data[3] = right == BLANK ? BLANK : right % 10;
        }

        switch (transition)
        {
        case FALL:
            mask = 0xffff;
            iteration = 48;
            interval = 30;
            break;
        case FADE:
            mask = 0xffff;
            iteration = 14;
            interval = 40;
            break;
        default:
            iteration = 8;
            interval = 40;
        }

        this->transition = transition;
        update();
    }

    void show(byte left, byte right, Transition transition)
    {
        chrono.restart();
        if (iteration > 0)
        {
            if (this->transition == FALL)
            {
                position = requested;
            }
            this->transition = REPLACE;
            update(true);
        }

        mask = 0;
        if (data[0] != left / 10)
        {
            mask |= 0x000f;
            data[0] = left == BLANK ? BLANK : left / 10;
        }
        if (data[1] != left % 10)
        {
            mask |= 0x00f0;
            data[1] = left == BLANK ? BLANK : left % 10;
        }
        if (data[2] != right / 10)
        {
            mask |= 0x0f00;
            data[2] = right == BLANK ? BLANK : right / 10;
        }
        if (data[3] != right % 10)
        {
            mask |= 0xf000;
            data[3] = right == BLANK ? BLANK : right % 10;
        }

        switch (transition)
        {
        case FALL:
            mask = 0xffff;
            iteration = 48;
            interval = 30;
            break;
        case FADE:
            mask = 0xffff;
            iteration = 14;
            interval = 40;
            break;
        default:
            iteration = 8;
            interval = 60;
        }

        this->transition = transition;
        update();
    }

    void setPosition(Position position)
    {
        this->requested = position;
    }

    void setMarkers(byte markers)
    {
        bar = (bar & 0xff00) | markers;
    }

    void setProgress(uint8_t progress)
    {
        this->bar = (bar & 0x00ff) | progress ? bit(16 - progress) : 0;
    }

    void setBrightness(uint8_t brightness)
    {
        matrix1.setBrightness(brightness);
        matrix2.setBrightness(brightness);
    }

    void update(boolean force = false)
    {
        if (!force && (!chrono.hasPassed(interval) || iteration == 0))
        {
            return;
        }
        iteration--;
        chrono.restart();
        byte source;
        for (uint8_t i = 0; i < 16; i++)
        {
            if (bit(i) & mask)
            {
                if (i < 8 && data[0] == SYMBOL)
                {
                    if (data[1] == EMPTY)
                    {
                        source = 0;
                    }
                    else
                    {
                        source = pgm_read_word_near(symbols + data[1] * 8 + i);
                    }
                }
                else
                {
                    if (data[i / 4] == BLANK)
                    {
                        source = 0;
                    }
                    else
                    {
                        if (i == 8 || i==12)
                        {
                            source = 0;
                        }
                        else
                        {
                            source = pgm_read_word_near(symbols + data[i / 4] * 4 + i % 4 - (i > 8 ? 1 : 0));
                        }
                    }
                }

                uint8_t direction = random(2) ? 1 : -1;
                switch (transition)
                {
                case REPLACE:
                    displayBuffer[i] = source;
                    iteration = 0;
                    break;
                case FADE:
                    if(iteration > 8) {
                        for (uint8_t p = random(8); displayBuffer[i] != 0; p+=direction)
                        {
                            if ((displayBuffer[i]) & bit(p % 8))
                            {
                                displayBuffer[i] = displayBuffer[i] & ~bit(p % 8);
                                break;
                            }
                        }
                    } else {
                        for (uint8_t p = random(8); displayBuffer[i] != source; p+=direction)
                        {
                            if ((source ^ displayBuffer[i]) & bit(p % 8))
                            {
                                displayBuffer[i] = (source & bit(p % 8)) ? displayBuffer[i] | bit(p % 8) : displayBuffer[i] & ~bit(p % 8);
                                break;
                            }
                        }
                    }
                    break;
                case DISSOLVE:
                    for (uint8_t p = random(8); displayBuffer[i] != source; p+=direction)
                    {
                        if ((source ^ displayBuffer[i]) & bit(p % 8))
                        {
                            displayBuffer[i] = (source & bit(p % 8)) ? displayBuffer[i] | bit(p % 8) : displayBuffer[i] & ~bit(p % 8);
                            break;
                        }
                    }
                    break;
                case SLIDE:
                    if ((i > 3 && i < 8) || i > 11)
                    {
                        displayBuffer[i] = (displayBuffer[i] << 1) | (source >> iteration);
                    }
                    else
                    {
                        displayBuffer[i] = (displayBuffer[i] >> 1) | (source << iteration);
                    }
                    break;
                case SLIDE_DOWN:
                    displayBuffer[i] = (displayBuffer[i] >> 1) | (source << iteration);
                    break;
                case SLIDE_UP:
                    displayBuffer[i] = (displayBuffer[i] << 1) | (source >> iteration);
                    break;
                case FALL:
                    if (iteration > 24)
                    {
                        byte mask = (1 << ((iteration - 32) / 2 - (random((iteration - 24) / 4) / ((iteration - 24) / 4 - 1)))) - 1;
                        byte moving = displayBuffer[i] & ~mask;
                        moving = moving << 1;
                        displayBuffer[i] = (displayBuffer[i] & mask) | moving;
                    }
                    if (iteration == 24)
                    {
                        position = requested;
                    }
                    if (iteration <= 24)
                    {

                        for (byte j = 0; j < iteration; j++)
                        {
                            byte mask = 0xff >> (j / 3 + random((iteration) / 8 + 1));
                            byte moving = source & ~mask;
                            moving = moving << 1;
                            source = (source & mask) | moving;
                        }
                        displayBuffer[i] = source << (iteration - 16);
                    }
                    break;
                }
            }
            if ((transition != FALL || iteration == 0) && bit(i) & bar)
            {
                displayBuffer[i] = displayBuffer[i] | 1;
            } else {
                displayBuffer[i] = displayBuffer[i] & ~1;
            }
        }
        matrix1.writeDisplay(position ? displayBuffer + 8 : displayBuffer, position);
        matrix2.writeDisplay(position ? displayBuffer : displayBuffer + 8, position);
        for (uint8_t i = 0; i < 16; i++)
        {
            displayBuffer[i] = displayBuffer[i] & 0xfffe;
        }
        if (position != requested && iteration == 0)
        {
            position = requested;
        }
    }

private:
    Chrono chrono;
    byte data[4] = {BLANK, BLANK, BLANK, BLANK};
    uint16_t mask;
    byte displayBuffer[16] = {0};
    Position position = TOP;
    Position requested = TOP;
    uint8_t iteration = 0;
    Transition transition;
    uint8_t interval;
    uint16_t bar = 0;
    Matrix matrix1 = Matrix(0x70);
    Matrix matrix2 = Matrix(0x71);
};

#endif