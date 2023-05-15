#ifndef SWITCH_H
#define SWITCH_H

#include "task.h"

boolean switchValue = false;

class SwitchTask : public Task
{
public:
    SwitchTask(boolean value)
    {
        this->value = value;
    }

    virtual uint8_t update() override
    {
        switchValue = value;
        return 0;
    }
    virtual ~SwitchTask() {};
private:
    boolean value;
};

#endif