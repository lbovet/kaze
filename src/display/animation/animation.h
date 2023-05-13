#ifndef ANIMATION_H
#define ANIMATION_H

#include <Arduino.h>

#include "../task.h"

class Animation : public Task
{
public:
    Animation(byte buffer[], int size = 8)
    {
        this->size = size;
        this->targetBuffer = buffer;
    }
    virtual ~Animation() {}

protected:
    byte iteration = 8;
    byte size;
    byte *targetBuffer;
    byte animBuffer[8] = {0};
};

class InAnimation : public Animation
{
public:
    InAnimation(byte buffer[], byte *source, int size = 8) : Animation(buffer, size)
    {
        for (byte i = 0; i < size; i++)
        {
            this->source[i] = source[i];
        }
    }
    virtual ~InAnimation() {}

protected:
    byte source[8];
};

class OutAnimation : public Animation
{
public:
    OutAnimation(byte buffer[], int size = 8) : Animation(buffer, size)
    {
        memcpy(this->animBuffer, buffer, size);
    }
    virtual ~OutAnimation() {}
};

#endif