#ifndef VOLUME_H
#define VOLUME_H

#include "output/display.h"
#include "output/player.h"
#include "data/storage.h"

#define START_DELAY 2000
#define STOP_DELAY 20

class Volume
{
public:
    void open()
    {
        starting = false;
        display.show(SPEAKER, current, false, FADE);
    }

    void update()
    {
        if (updated)
        {
            player.setVolume(current);
            updated = false;
        }
        if (starting && chrono.hasPassed(START_DELAY))
        {
            chrono.restart();
            player.setVolume(++current);
            if (current >= target)
            {
                starting = false;
            }
        }
        if (stopping && chrono.hasPassed(STOP_DELAY))
        {
            chrono.restart();
            player.setVolume(--current);
            if (current == 0)
            {
                player.stop();
                stopping = false;
            }
        }
    }

    void up()
    {
        if (current < 20)
            display.show(SPEAKER, ++current, false, SLIDE_UP);
        updated = true;
    }

    void down()
    {
        if (current > 1)
            display.show(SPEAKER, --current, false, SLIDE_DOWN);
        updated = true;
    }

    void smoothStop()
    {
        stopping = true;
        starting = false;
    }

    void start(Music music)
    {
        this->music = music;
        current = target = storage.read(VOLUME, music);
        updated = true;
    }

    void smoothStart(Music music)
    {
        this->music = music;
        player.setVolume(0);
        target = storage.read(VOLUME, music);
        current = 0;
        starting = true;
        stopping = false;
    }

    void commit()
    {
        storage.write(VOLUME, music, current);
    }

private:
    Chrono chrono;
    int8_t current;
    int8_t target;
    Music music;
    boolean updated;
    bool stopping = false;
    bool starting = false;
};

Volume volume;

#endif
