#include <MemoryFree.h>
#include <Chrono.h>

#include "SD.h"
#include "display.h"
#include "player.h"
#include "touch.h"
#include "time.h"
#include "event.h"
#include "orientation.h"

EventBus event;
Display display;
Player player;
Touch touch;
Orientation orientation;
Time time;
Chrono longDelay, shortDelay;

void setup()
{
  Serial.begin(9600);
  Serial.println(F("go"));
  display.begin();
  display.setBar(0x02 | 0x20);
  player.begin();
  touch.begin();
  time.begin();
  longDelay.restart();
  Serial.println(freeMemory());
}

void loop()
{
  if (shortDelay.hasPassed(100))
  {
    shortDelay.restart();
    touch.update();

    if (orientation.update())
    {
        Serial.println(orientation.value());
        display.setTurned(orientation.value());
        display.show(time.hour(), time.minute(), FALL);
    }
  }

  if (longDelay.hasPassed(500))
  {
    Serial.println(freeMemory());
    longDelay.restart();
    if(time.update())
    {
      display.setBar(0x02 | 0x20, 8 - ((short)(time.second() * 8) / 60));
      display.show(time.hour(), time.minute(), SLIDE);
    }
  }

  display.update();
}
