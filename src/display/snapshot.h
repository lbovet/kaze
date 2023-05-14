#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include "task.h"

class SnapshotTask : public Task
{
public:
    SnapshotTask(byte** left, byte** right)
    {
        this->leftSource = left;
        this->rightSource = right;
    }
    virtual uint8_t update() override
    {
        memcpy(left, *leftSource, 8);
        memcpy(right, *leftSource, 8);
        return 0;
    }

    virtual ~SnapshotTask(){};

    byte left[8];
    byte right[8];
private:
    byte** leftSource;
    byte** rightSource;
};

#endif