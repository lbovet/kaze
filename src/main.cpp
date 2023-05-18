#include <MemoryFree.h>
#include <Chrono.h>

#include "SD.h"
#include "display.h"
#include "player.h"
#include "touch.h"

Display display;
Player player;
Touch touch;
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
  display.show(ALARM, 0, true, FADE);
  Serial.println(freeMemory());
  touch.begin();
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
      display.show(ALARM, n, true, TURN);
    }
    touch.update();
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
    display.show(ALARM, n, true, SLIDE_UP);
    if (n % 10 == 0)
    {
      player.stop();
    }
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
