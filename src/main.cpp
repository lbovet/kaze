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
StateMachine stateMachine(&display, &player, &time);
Touch touch;
Orientation orientation;

Chrono longDelay, shortDelay;

void setup()
{
  Serial.begin(9600);
  Serial.println(F("go"));
  display.begin();
  player.begin();
  touch.begin();
  time.begin();
  bus.post(INIT);
}

void loop()
{
  if (shortDelay.hasPassed(100))
  {
    shortDelay.restart();
    touch.update();
    if (orientation.update())
    {
      bus.post(orientation.value() ? TURN_DOWN : TURN_UP);
    }
  }

  if (longDelay.hasPassed(500))
  {
    Serial.println(freeMemory());
    longDelay.restart();
    if (time.update())
    {
      bus.post(TIME);
    }
  }

  Event event = bus.next();
  if (stateMachine.update(event))
  {
    bus.acknowledge(event);
  }
  display.update();
}
