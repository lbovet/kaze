#ifndef FALL_H
#define FALL_H

#include "animation.h"

class FallOut : public OutAnimation
{
public:
    FallOut(byte **buffer, byte size = 8, byte offset = 0) : OutAnimation(buffer, size, offset)
    {
        this->iteration = 24;
    }
    uint8_t update() override
    {
        init();
        if (this->iteration > 0)
        {
            iteration--;
            for (byte i = offset; i < offset + size; i++)
            {
                byte mask = (1 << ((iteration - 8) / 2 - (random(iteration / 4) / (iteration / 4 - 1)))) - 1;
                byte moving = this->animBuffer[i] & ~mask;
                moving = moving << 1;
                this->animBuffer[i] = (this->animBuffer[i] & mask) | moving;
                (*this->targetBuffer)[i] = this->animBuffer[i];
            }
        }
        return iteration;
    }
    virtual ~FallOut() {}

private:
};

class FallIn : public InAnimation
{
public:
    FallIn(byte **buffer, byte *source, uint8_t size = 8, uint8_t offset = 0) : InAnimation(buffer, source, size, offset)
    {
        this->iteration = 24;
    }
    uint8_t update() override
    {
        if (this->iteration > 0)
        {
            iteration--;
            for (byte i = 0; i < size; i++)
            {
                this->animBuffer[i] = source[i];
                for (byte j = 0; j < iteration; j++)
                {
                    byte mask = 0xff >> (j / 3 + random(iteration / 8 + 1));
                    byte moving = this->animBuffer[i] & ~mask;
                    moving = moving << 1;
                    this->animBuffer[i] = (this->animBuffer[i] & mask) | moving;
                }

                (*this->targetBuffer)[i+offset] = this->animBuffer[i] << (iteration - 16);
            }
        }
        return iteration;
    }
    virtual ~FallIn() {}
};

#endif