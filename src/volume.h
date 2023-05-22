#ifndef VOLUME_H
#define VOLUME_H

#include "output/player.h"

#define START_DELAY 2000
#define STOP_DELAY 50

class Volume
{
public:
    void open()
    {
        display.show(SPEAKER, current, false, FADE);
    }

    void update()
    {
        if(!starting && !player.playing() && current != 10) {
            current = 10;
            updated = true;
        }
        if(updated)
        {
            player.setVolume(current);
            updated = false;
        }
        if(starting && chrono.hasPassed(START_DELAY)) {
            chrono.restart();
            player.setVolume(++current);
            if (current >= 10)
            {
                starting = false;
            }
        }
        if(stopping && chrono.hasPassed(STOP_DELAY)) {
            chrono.restart();
            player.setVolume(--current);
            if(current == 0)
            {
                player.stop();
                stopping = false;
            }
        }
    }

    void up()
    {
        if(current < 20)
            display.show(SPEAKER, ++current, false, SLIDE_UP);
        updated = true;
    }

    void down()
    {
        if(current > 1)
            display.show(SPEAKER, --current, false, SLIDE_DOWN);
        updated = true;
    }

    void smoothStop()
    {
        stopping = true;
        starting = false;
    }

    void smoothStart()
    {
        player.setVolume(0);
        current = 0;
        starting = true;
        stopping = false;
    }

    void commit()
    {
    }
private:
    Chrono chrono;
    int8_t current = 10;
    boolean updated;
    bool stopping = false;
    bool starting = false;
};

Volume volume;

#endif
