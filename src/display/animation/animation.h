#ifndef ANIMATION_H
#define ANIMATION_H

#include <Arduino.h>

#include "../task.h"

class Animation : public Task
{
public:
    Animation(byte **buffer, byte size = 8, byte offset = 0)
    {
        this->animBuffer = new byte[size] {0};
        this->size = size;
        this->targetBuffer = buffer;
        this->offset = offset;
    }

    virtual ~Animation() {
        delete [] this->animBuffer;
    }

protected:
    byte iteration = 8;
    byte size;
    byte offset;
    byte **targetBuffer;
    byte *animBuffer;
};

class InAnimation : public Animation
{
public:
    InAnimation(byte **buffer, byte *source, byte size = 8, byte offset = 0) : Animation(buffer, size, offset)
    {
        this->source = new byte[size];
        for (byte i = 0; i < size; i++)
        {
            this->source[i] = source[i];
        }
    }

    virtual ~InAnimation()
    {
        delete[] source;
    }

protected:
    byte *source;
};

class OutAnimation : public Animation
{
public:
    OutAnimation(byte **buffer, byte size = 8, byte offset = 0) : Animation(buffer, size, offset) {}

    virtual void init()
    {
        if (!initialized)
        {
            memcpy(this->animBuffer, *targetBuffer + offset, size);
            initialized = true;
        }
    }
    virtual ~OutAnimation() {}

private:
    boolean initialized = false;
};

#endif