#ifndef SWITCH_H
#define SWITCH_H

#include "task.h"

class Switchable
{
public:
    virtual void setSwitch(boolean value) = 0;
};

class SwitchTask : public Task
{
public:
    SwitchTask(Switchable* switchable, boolean value)
    {
        this->switchable = switchable;
        this->value = value;
    }

    virtual uint8_t update() override
    {
        switchable->setSwitch(value);
        return 0;
    }
    virtual ~SwitchTask() {};
private:
    Switchable* switchable;
    boolean value;
};

#endif