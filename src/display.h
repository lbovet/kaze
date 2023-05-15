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
    SLIDE_UP,
    FALL_IN
};

enum Category
{
    MANDATORY = 0,
    OPTIONAL
};

class Display
{
public:
    void begin()
    {
        matrix1.begin();
        matrix2.begin();
        left = matrix1.displayBuffer;
        right = matrix2.displayBuffer;
    }

    void symbol(Symbol symbol, Position position, Transition transition)
    {
        byte buffer[8];
        for (byte j = 0; j < 8; j++)
        {
            buffer[j] = pgm_read_word_near(symbols + symbol * 8 + j);
        }

        Task *task;

        byte **targetBuffer = (position == LEFT ? &left : &right);

        switch (transition)
        {
        case FADE:
            task = new ParallelTask(
                new FadeOut(targetBuffer),
                new FadeIn(targetBuffer, buffer));
            break;
        case FALL_IN:
            task = new FallIn(targetBuffer, buffer);
            break;
        case SLIDE_DOWN:
        case SLIDE_UP:
            task = new ParallelTask(
                new SlideOut(targetBuffer, transition == SLIDE_DOWN),
                new SlideIn(targetBuffer, buffer, transition == SLIDE_DOWN));
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

        byte **targetBuffer = position < 2 ? &left : &right;
        byte offset = (position % 2) * 4 + (position < 2 ? 0 : 1);

        Task *task;

        switch (transition)
        {
        case FADE:
            task = new ParallelTask(
                new FadeOut(targetBuffer, 3, offset),
                new FadeIn(targetBuffer, buffer, 3, offset));
            break;
        case FALL_IN:
            task = new FallIn(targetBuffer, buffer, 3, offset);
            break;
        case SLIDE_DOWN:
        case SLIDE_UP:
            task = new ParallelTask(
                new SlideOut(targetBuffer, transition == SLIDE_DOWN, 3, offset),
                new SlideIn(targetBuffer, buffer, transition == SLIDE_DOWN, 3, offset));
        }
        addTask(task);
    }

    void schedule(uint8_t category = 0, int interval = 60)
    {
        if (preparedTask)
        {
            scheduler.schedule(preparedTask, category, interval);
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
        if (turned != this->turned)
        {
            Task *out = new ParallelTask(
                new FallOut(&left),
                new FallOut(&right));
            addTask(new SerialTask(out, new SwitchTask(turned)));
            schedule(MANDATORY);
        }
    }

    void doTurn()
    {
        if (turned != switchValue)
        {
            turned = switchValue;
            matrix1.setRotation(turned);
            matrix2.setRotation(turned);
            if (turned)
            {
                left = matrix2.displayBuffer;
                right = matrix1.displayBuffer;
            }
            else
            {
                left = matrix1.displayBuffer;
                right = matrix2.displayBuffer;
            }
        }
    }

    void update()
    {
        if (scheduler.update())
        {
            doTurn();
            matrix1.writeDisplay();
            matrix2.writeDisplay();
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

    Task *preparedTask = 0;
    boolean initialized = false;
    boolean turned = false;
    Matrix matrix1 = Matrix(0x70);
    Matrix matrix2 = Matrix(0x71);
    byte *left;
    byte *right;
    Scheduler scheduler;
};

#endif