#include <Chrono.h>

#include "SD.h"
#include "data/time.h"
#include "output/display.h"
#include "output/player.h"
#include "input/touch.h"
#include "input/event.h"
#include "input/orientation.h"
#include "clock.h"
#include "timer.h"
#include "state.h"
#include "menu.h"

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
      bus.post(orientation.current() == BOTTOM ? TURN_DOWN : TURN_UP);
    }
  }

  if (longDelay.hasPassed(1000))
  {
    longDelay.restart();
    if (time.update())
    {
      bus.post(TIME);
    }
    if (time.checkAlarm())
    {
      bus.post(ALARM);
    }
    if (timer.update())
    {
      bus.post(ELAPSED);
    }
  }

  stateMachine.update();
  display.update();
}
