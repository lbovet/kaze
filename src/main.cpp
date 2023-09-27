#include <Chrono.h>

#include "SD.h"
#include "data/time.h"
#include "output/display.h"
#include "output/player.h"
#include "input/touch.h"
#include "input/event.h"
#include "input/orientation.h"
#include "input/light.h"
#include "clock.h"
#include "timer.h"
#include "state.h"
#include "menu.h"
#include "volume.h"

Chrono longDelay, shortDelay, touchDelay;

void setup()
{
  Serial.begin(9600);
  Serial.println(F("go"));
  time.begin();
  display.begin();
  player.begin();
  touch.begin();
  clock.begin();
  delay(100);
}

void loop()
{
  if (touchDelay.hasPassed(70))
  {
    bus.post(touch.update());
    touchDelay.restart();
  }

  if (shortDelay.hasPassed(100))
  {
    shortDelay.restart();
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
    display.setBrightness(lightSensor.value());
    player.update();
  }

  stateMachine.update();
  display.update();
  volume.update();
}
