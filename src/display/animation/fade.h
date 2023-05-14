#ifndef FADE_H
#define FADE_H

#include "animation.h"

class FadeIn : public InAnimation
{
public:
    FadeIn(byte **buffer, byte *source, byte size = 8, byte offset = 0) : InAnimation(buffer, source, size, offset){};

    uint8_t update() override
    {
        if (this->iteration > 0)
        {
            iteration--;
            for (byte i = offset; i < offset + size; i++)
            {
                byte p = 1 << random(8);
                for (byte j = 0; j < 8 && (this->mask[i] == (this->mask[i] | p)); j++)
                {
                    p = (p << 1) | (p >> 7); // rotate
                }
                this->mask[i] = this->mask[i] | p;
                this->animBuffer[i] = this->animBuffer[i] | (this->source[i - offset] & p);
                (*this->targetBuffer)[i] = (*this->targetBuffer)[i] | this->animBuffer[i];
            }
        }
        return iteration;
    }
    virtual ~FadeIn() {}

private:
    byte mask[8] = {0};
};

class FadeOut : public OutAnimation
{
public:
    FadeOut(byte **buffer, byte size = 8, byte offset = 0) : OutAnimation(buffer, size, offset){};

    uint8_t update() override
    {
        init();
        if (this->iteration > 0)
        {
            iteration--;
            for (byte i = offset; i < offset + size; i++)
            {
                byte p = 1 << random(8);
                for (byte j = 0; j < 8 && (this->mask[i] == (this->mask[i] | p)); j++)
                {
                    p = (p << 1) | (p >> 7); // rotate
                }
                this->mask[i] = this->mask[i] | p;
                this->animBuffer[i] = this->animBuffer[i] & ~p;
                (*this->targetBuffer)[i] = this->animBuffer[i];
            }
        }
        return iteration;
    }

    virtual ~FadeOut() {}

private:
    byte mask[8] = {0};
};

#endif