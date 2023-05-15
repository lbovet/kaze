#ifndef FADE_H
#define FADE_H

#include "animation.h"

class FadeIn : public InAnimation
{
public:
    FadeIn(byte **buffer, byte *source, byte size = 8, byte offset = 0) : InAnimation(buffer, source, size, offset)
    {
        mask = new byte[size] {0};
    };

    uint8_t update() override
    {
        if (this->iteration > 0)
        {
            iteration--;
            for (byte i = 0; i < size; i++)
            {
                byte p = 1 << random(8);
                for (byte j = 0; j < 8 && (this->mask[i] == (this->mask[i] | p)); j++)
                {
                    p = (p << 1) | (p >> 7); // rotate
                }
                this->mask[i] = this->mask[i] | p;
                this->animBuffer[i] = this->animBuffer[i] | (this->source[i] & p);
                (*this->targetBuffer)[i+offset] = (*this->targetBuffer)[i+offset] | this->animBuffer[i];
            }
        }
        return iteration;
    }
    virtual ~FadeIn() {
        delete [] mask;
    }

private:
    byte *mask;
};

class FadeOut : public OutAnimation
{
public:
    FadeOut(byte **buffer, byte size = 8, byte offset = 0) : OutAnimation(buffer, size, offset){
        this->mask = new byte[size] {0};
    };

    uint8_t update() override
    {
        init();
        if (this->iteration > 0)
        {
            iteration--;
            for (byte i = 0; i < size; i++)
            {
                byte p = 1 << random(8);
                for (byte j = 0; j < 8 && (this->mask[i] == (this->mask[i] | p)); j++)
                {
                    p = (p << 1) | (p >> 7); // rotate
                }
                this->mask[i] = this->mask[i] | p;
                this->animBuffer[i] = this->animBuffer[i] & ~p;
                (*this->targetBuffer)[i+offset] = this->animBuffer[i];
            }
        }
        return iteration;
    }

    virtual ~FadeOut() {
        delete [] mask;
    }

private:
    byte * mask;
};

#endif