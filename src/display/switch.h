#ifndef SWITCH_H
#define SWITCH_H

#include "task.h"

class SwitchTask : public Task
{
public:
    SwitchTask(boolean* dest, boolean value)
    {
        this->dest = dest;
        this->value = value;
    }
    virtual uint8_t update() override
    {
        *dest = value;
        return 0;
    }
    virtual ~SwitchTask() {};
private:
    boolean* dest;
    boolean value;
};

#endif