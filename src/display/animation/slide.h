#ifndef SLIDE_H
#define SLIDE_H

#include "animation.h"

class SlideOut : public OutAnimation
{
public:
    SlideOut(byte **buffer, boolean direction = true, byte size = 8, byte offset = 0) : OutAnimation(buffer, size, offset)
    {
        this->direction = direction;
    }

    uint8_t update() override
    {
        init();
        if (this->iteration-- > 0)
        {
            for (byte i = offset; i < offset + size; i++)
            {
                this->animBuffer[i] = this->direction ? this->animBuffer[i] >> 1 : this->animBuffer[i] << 1;
                (*this->targetBuffer)[i] = this->animBuffer[i];
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
    SlideIn(byte** buffer, byte *source, boolean direction = true, byte size = 8, byte offset = 0) : InAnimation(buffer, source, size, offset)
    {
        this->direction = direction;
    }
    uint8_t update() override
    {
        if (this->iteration-- > 0)
        {
            for (byte i = offset; i < offset + size; i++)
            {
                this->animBuffer[i] = this->direction ? this->source[i - offset] << iteration : this->source[i - offset] >> iteration;
                (*this->targetBuffer)[i] = (*this->targetBuffer)[i] | this->animBuffer[i];
            }
        }
        return iteration;
    }
    virtual ~SlideIn() {}

private:
    boolean direction;
};

#endif