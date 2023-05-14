#ifndef DISPLAY_H
#define DISPLAY_H

#include "display/matrix.h"
#include "display/animation/fade.h"
#include "display/animation/fall.h"
#include "display/animation/slide.h"
#include "display/task.h"
#include "display/switch.h"

const byte symbols[] PROGMEM = {
    0x7c, 0x44, 0x7c, 0x00, 0x44, 0x7c, 0x04, 0x00, 0x5c, 0x54, 0x74, 0x00,
    0x54, 0x54, 0x7c, 0x00, 0x70, 0x10, 0x7c, 0x00, 0x74, 0x54, 0x5c, 0x00,
    0x7c, 0x54, 0x5c, 0x00, 0x60, 0x40, 0x7c, 0x00, 0x7c, 0x54, 0x7c, 0x00,
    0x74, 0x54, 0x7c, 0x00, 0x38, 0x44, 0x82, 0xb2, 0x8a, 0x44, 0x38, 0x00,
    0x00, 0x0c, 0x74, 0x84, 0x86, 0x74, 0x0c, 0x00, 0x00, 0xc6, 0xaa, 0x92,
    0xaa, 0xc6, 0x00, 0x00, 0x06, 0x06, 0x7e, 0x40, 0x46, 0x46, 0x7e, 0x00,
    0x38, 0x28, 0x44, 0xfe, 0x00, 0x54, 0x92, 0x00, 0x00, 0x7c, 0x38, 0x10,
    0x00, 0x7c, 0x38, 0x10, 0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
    0x10, 0x10, 0x10, 0x10, 0x7c, 0x38, 0x10, 0x00, 0x08, 0x1c, 0x3e, 0x08,
    0x08, 0x78, 0x00, 0x00, 0x7c, 0x82, 0x8a, 0x82, 0xa2, 0x82, 0x7c, 0x00,
    0x00, 0x42, 0x66, 0x5a, 0x24, 0x18, 0x00, 0x00, 0x06, 0x08, 0x32, 0xc4,
    0x32, 0x08, 0x06, 0x00, 0x30, 0x48, 0x44, 0x22, 0x44, 0x48, 0x30, 0x00};

enum Symbol
{
    CLOCK = 5,
    ALARM = 6,
    TIMER = 7,
    MUSIC = 8,
    VOLUME = 9,
    NEXT = 10,
    STOP = 11,
    INTERVAL = 12,
    BACK = 13,
    RANDOM = 14,
    SLEEP = 15,
    CHILL = 16,
    LOVE = 17
};

enum Position
{
    LEFT,
    RIGHT
};

enum Transition
{
    FADE,
    SLIDE_DOWN,
    SLIDE_UP
};

class Display
{
public:
    void begin()
    {
        matrix1.begin();
        matrix2.begin();
    }

    void symbol(Symbol symbol, Position position, Transition transition)
    {
        byte buffer[8];
        for (byte j = 0; j < 8; j++)
        {
            buffer[j] = pgm_read_word_near(symbols + symbol * 8 + j);
        }

        Task *task;

        switch (transition)
        {
        case FADE:
            task = new ParallelTask(
                new FadeOut(getMatrix(position)->displayBuffer),
                new FadeIn(getMatrix(position)->displayBuffer, buffer));
            break;
        case SLIDE_DOWN:
        case SLIDE_UP:
            task = new ParallelTask(
                new SlideOut(getMatrix(position)->displayBuffer, transition == SLIDE_DOWN),
                new SlideIn(getMatrix(position)->displayBuffer, buffer, transition == SLIDE_DOWN));
            break;
        }

        addTask(task);
    }

    void digit(uint8_t number, uint8_t position, Transition transition)
    {
        byte buffer[4];
        for (byte j = 0; j < 4; j++)
        {
            buffer[j] = pgm_read_word_near(symbols + number * 4 + j);
        }

        byte *targetBuffer = getMatrix(position < 2 ? LEFT : RIGHT)->displayBuffer + (position % 2) * 4;

        Task *task;

        switch (transition)
        {
        case FADE:
            task = new ParallelTask(
                new FadeOut(targetBuffer, 4),
                new FadeIn(targetBuffer, buffer, 4));
            break;
        case SLIDE_DOWN:
        case SLIDE_UP:
            task = new ParallelTask(
                new SlideOut(targetBuffer, transition == SLIDE_DOWN, 4),
                new SlideIn(targetBuffer, buffer, transition == SLIDE_DOWN, 4));
        }
        addTask(task);
    }

    void schedule(boolean force = false, int interval = 60)
    {
        if (preparedTask)
        {
            scheduler.schedule(preparedTask, force, interval);
            preparedTask = 0;
        }
    }

    void setBrightness(uint8_t brightness)
    {
        matrix1.setBrightness(brightness);
        matrix2.setBrightness(brightness);
    }

    void setTurned(boolean turned)
    {
        if(turned != this->turned) {
            Task *before = new ParallelTask(
                new FallOut(getMatrix(LEFT)->displayBuffer),
                new FallOut(getMatrix(RIGHT)->displayBuffer));
            Task *after = new ParallelTask(
                new FallIn(getMatrix(LEFT)->displayBuffer, getMatrix(RIGHT)->displayBuffer),
                new FallIn(getMatrix(RIGHT)->displayBuffer, getMatrix(LEFT)->displayBuffer));
            Task *task;
            task = new SerialTask(new SerialTask(before, new SwitchTask(&this->turned, turned)), after);
            addTask(task);
            schedule(true);
        }
    }

    void update()
    {
        if (scheduler.update())
        {
            if(turned) {
                matrix1.rotate();
                matrix2.rotate();
            }
            matrix1.writeDisplay();
            matrix2.writeDisplay();
            if (turned)
            {
                matrix1.rotate();
                matrix2.rotate();
            }
        }
    }

private:
    void addTask(Task *task)
    {
        if (preparedTask)
        {
            preparedTask = new ParallelTask(preparedTask, task);
        }
        else
        {
            preparedTask = task;
        }
    }
    Matrix *getMatrix(Position position)
    {
        if (!turned)
        {
            return position == LEFT ? &matrix1 : &matrix2;
        }
        else
        {
            return position == LEFT ? &matrix2 : &matrix1;
        }
    }
    Task *preparedTask = 0;
    boolean initialized = false;
    boolean turned = false;
    Matrix matrix1 = Matrix(0x70);
    Matrix matrix2 = Matrix(0x71);
    Scheduler scheduler;
};

#endif