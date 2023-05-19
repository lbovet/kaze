#include <MemoryFree.h>
#include <Chrono.h>

#include "SD.h"
#include "display.h"
#include "player.h"
#include "touch.h"
#include "time.h"
#include "event.h"
#include "orientation.h"
#include "state.h"

EventBus bus;
Display display;
Player player;
Time time;
StateMachine stateMachine(&bus, &display, &player, &time);
Touch touch;
Orientation orientation;

Chrono longDelay, shortDelay;

void setup()
{
  Serial.begin(9600);
  Serial.println(F("go"));
  time.begin();
  display.begin();
  player.begin();
  touch.begin();
  bus.post(INIT);
  delay(100);
}

void loop()
{
  if (shortDelay.hasPassed(100))
  {
    shortDelay.restart();
    bus.post(touch.update());
    if (orientation.update())
    {
      bus.post(orientation.value() ? TURN_DOWN : TURN_UP);
    }
  }

  if (longDelay.hasPassed(1000))
  {
    //Serial.println(freeMemory());
    longDelay.restart();
    if (time.update())
    {
      bus.post(TIME);
    }
  }

  stateMachine.update();
  display.update();
}
