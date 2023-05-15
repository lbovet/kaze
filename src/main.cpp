#include <MemoryFree.h>
#include <Chrono.h>

#include "SD.h"
#include "display.h"
#include "player.h"

Display display;
Player player;
Chrono chrono, chrono2;

void setup()
{
  Serial.begin(9600);
  Serial.println(F("go"));
  display.begin();
  delay(500);
  Serial.println(freeMemory());
  player.begin();
  Serial.println(freeMemory());
  display.symbol(ALARM, LEFT, FADE);
  display.digit(0, 2, FADE);
  display.digit(0, 3, FADE);
  display.schedule();
  Serial.println(freeMemory());
  chrono.restart();
}

long sensor;
boolean orientation;
boolean previousOrientation;
uint8_t n = 0;

void loop()
{
  if (chrono2.hasPassed(100))
  {
    chrono2.restart();
    Serial.println(freeMemory());

    if (orientation != previousOrientation)
    {
      previousOrientation = orientation;
      Serial.println(orientation);

      display.setTurned(orientation);
      display.symbol(ALARM, LEFT, FALL_IN);
      display.digit(n % 10, 3, FALL_IN);
      display.digit(n / 10, 2, FALL_IN);
      display.schedule(TURN);
    }
  }

  display.update();

  if (chrono.hasPassed(1000))
  {
    chrono.restart();
    n++;
    if (n > 59)
      n = 0;
    if (n==1) {
      player.play();
    }
    display.digit(n % 10, 3, SLIDE_UP);
    if (n % 10 == 0)
    {
      display.digit(n / 10, 2, SLIDE_DOWN);
      player.stop();
    }
    display.schedule(SCROLL);
  }

  if(abs(sensor) < 50) {
    if(analogRead(A0) < 512)
      sensor --;
    else
      sensor ++;
  } else {
    sensor = 0;
  }
  orientation = sensor > 0;
}
