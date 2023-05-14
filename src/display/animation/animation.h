#ifndef ANIMATION_H
#define ANIMATION_H

#include <Arduino.h>

#include "../task.h"

class Animation : public Task
{
public:
    Animation(byte **buffer, byte size = 8, byte offset = 0)
    {
        this->size = size;
        this->targetBuffer = buffer;
        this->offset = offset;
    }

    virtual ~Animation() {}

protected:
    byte iteration = 8;
    byte size;
    byte offset;
    byte **targetBuffer;
    byte animBuffer[8] = {0};
};

class InAnimation : public Animation
{
public:
    InAnimation(byte **buffer, byte size = 8, byte offset = 0) : Animation(buffer, size, offset) {}
    InAnimation(byte **buffer, byte *source, byte size = 8, byte offset = 0) : Animation(buffer, size, offset)
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
    OutAnimation(byte** buffer, byte size = 8, byte offset = 0) : Animation(buffer, size, offset) {}

    virtual void init()
    {
        if(!initialized) {
            Serial.println("starter");
            memcpy(this->animBuffer + offset, *targetBuffer + offset, size);
            initialized = true;
        }
    }
    virtual ~OutAnimation() {}
private:
    boolean initialized = false;
};

#endif