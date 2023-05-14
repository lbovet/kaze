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

    virtual uint8_t update() override
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

class SerialTask : public Task
{
public:
    SerialTask(Task *one, Task *two)
    {
        this->one = one;
        this->two = two;
    }

    virtual uint8_t update() override
    {
        if (current == 0)
        {
            uint8_t it = one->update();
            if (it == 0)
            {
                current = 1;
                return two->update();
            }
            else
            {
                return it;
            }
        }
        else
        {
            return two->update();
        }
    }
    virtual ~SerialTask()
    {
        delete one;
        delete two;
    }

private:
    uint8_t current = 0;
    Task *one;
    Task *two;
};

class Scheduler
{
public:
    boolean schedule(Task *task, uint8_t category, int interval)
    {
        for (uint8_t i = 0; i < QUEUE_SIZE; i++)
        {
            uint8_t p = (current + i) % QUEUE_SIZE;
            if (tasks[p] == 0)
            {
                tasks[p] = task;
                intervals[p] = interval;
                categories[p] = category;
                if(p == current) {
                    updateCurrent();
                    return true;
                } else {
                    return false;
                }
            } else {
                if(p != current && category != 0 && categories[p] == category) {
                    Serial.println("override");
                    delete tasks[p];
                    tasks[p] = task;
                    intervals[p] = interval;
                    categories[p] = category;
                    return false;
                }
            }
        }
        // queue full
        return false;
    }

    boolean update()
    {
        if (tasks[current] && chrono.hasPassed(intervals[current]))
        {
            while(tasks[current] && !updateCurrent())
            {
                delete tasks[current];
                tasks[current] = 0;
                categories[current] = 0;
                current = (current + 1) % QUEUE_SIZE;
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
        return tasks[current]->update() > 0;
    }

    static const uint8_t QUEUE_SIZE = 8;
    Task *tasks[QUEUE_SIZE] = {0};
    int intervals[QUEUE_SIZE];
    uint8_t categories[QUEUE_SIZE];
    uint8_t current = 0;
    Chrono chrono;
};

#endif