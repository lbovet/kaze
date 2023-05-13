#ifndef SLIDE_H
#define SLIDE_H

#include "animation.h"

class SlideOut : public OutAnimation
{
public:
    SlideOut(byte buffer[], boolean direction = true, int size = 8) : OutAnimation(buffer, size)
    {
        this->direction = direction;
    }
    uint8_t update() override
    {
        if (this->iteration-- > 0)
        {
            for (byte i = 0; i < size; i++)
            {
                this->animBuffer[i] = this->direction ? this->animBuffer[i] >> 1 : this->animBuffer[i] << 1;
                this->targetBuffer[i] = this->animBuffer[i];
            }
        }
        return iteration;
    }
    virtual ~SlideOut() {}

private:
    boolean direction;
};

class SlideIn : public InAnimation
{
public:
    SlideIn(byte buffer[], byte *source, boolean direction = true, int size = 8) : InAnimation(buffer, source, size)
    {
        this->direction = direction;
    }
    uint8_t update() override
    {
        if (this->iteration-- > 0)
        {
            for (byte i = 0; i < size; i++)
            {
                this->animBuffer[i] = this->direction ? this->source[i] << iteration : this->source[i] >> iteration;
                this->targetBuffer[i] = this->targetBuffer[i] | this->animBuffer[i];
            }
        }
        return iteration;
    }
    virtual ~SlideIn() {}

private:
    boolean direction;
};

#endif