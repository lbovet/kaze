#include <MemoryFree.h>
#include <Chrono.h>

#include "SD.h"
#include "output/display.h"
#include "output/player.h"
#include "input/touch.h"
#include "input/time.h"
#include "input/event.h"
#include "input/orientation.h"
#include "clock.h"
#include "timer.h"
#include "machine.h"

Display display;
Player player;
Time time;

Touch touch;
Orientation orientation;

Timer timer(&display, &player, &time);
Clock clock(&display, &time);

EventBus bus;
StateMachine stateMachine(&bus, &clock, &timer);

Chrono longDelay, shortDelay;

void setup()
{
  Serial.begin(9600);
  Serial.println(F("go"));
  time.begin();
  display.begin();
  player.begin();
  touch.begin();
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
    // Serial.println(freeMemory());
    longDelay.restart();
    if (time.update())
    {
      bus.post(TIME);
    }
  }

  stateMachine.update();
  display.update();
}
