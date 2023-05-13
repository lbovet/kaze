#ifndef TASK_H
#define TASK_H

#include <Arduino.h>
#include <Chrono.h>

class Task
{
public:
    virtual uint8_t update() = 0;
    virtual ~Task() {}
};

class ParallelTask : public Task
{
public:
    ParallelTask(Task *one, Task *two)
    {
        this->one = one;
        this->two = two;
    }

    virtual uint8_t update()
    {
        uint8_t i1 = one->update();
        uint8_t i2 = two->update();
        return max(i1, i2);
    }
    virtual ~ParallelTask()
    {
        delete one;
        delete two;
    }

private:
    Task *one;
    Task *two;
};

class Scheduler
{
public:
    boolean schedule(Task *task, int interval)
    {
        if (current)
        {
            if (next)
                delete next;
            next = task;
            nextinterval = interval;
            return false;
        }
        else
        {
            current = task;
            currentinterval = interval;
            updateCurrent();
            return true;
        }
    }

    boolean update()
    {
        if (current && chrono.hasPassed(currentinterval))
        {
            boolean finished = !updateCurrent();
            if (finished)
            {
                delete current;
                if (next)
                {
                    current = next;
                    currentinterval = nextinterval;
                    next = 0;
                    updateCurrent();
                }
                else
                {
                    current = 0;
                }
            }
            return true;
        }
        else
        {
            return false;
        }
    }

private:
    boolean updateCurrent()
    {
        chrono.restart();
        return current->update() > 0;
    }

    Task *current = 0;
    int currentinterval;
    Task *next = 0;
    int nextinterval;

    Chrono chrono;
};

#endif