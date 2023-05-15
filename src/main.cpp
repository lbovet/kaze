#include <MemoryFree.h>
#include <Chrono.h>

#include "display.h"
#include "player.h"

Display display;
Player player;
Chrono chrono;

void setup()
{
  Serial.begin(9600);
  Serial.println("go");
  display.begin();
  delay(1000);
  chrono.restart();
  Serial.println(freeMemory());
  player.begin();
  Serial.println(freeMemory());
  display.symbol(ALARM, LEFT, FADE);
  display.digit(0, 2, FADE);
  display.digit(0, 3, FADE);
  display.schedule();
  Serial.println(freeMemory());
  player.play();
}

long sensor;
boolean orientation;
boolean previousOrientation;
uint8_t n = 0;

void loop()
{
  if (chrono.hasPassed(100))
  {
    chrono.restart();
    Serial.println(freeMemory());
  }

  display.update();
/*
  if (chrono.hasPassed(1000))
  {
    chrono.restart();
    n++;
    if (n > 59)
      n = 0;
    display.digit(n % 10, 3, SLIDE_UP);
    if (n % 10 == 0)
    {
      display.digit(n / 10, 2, SLIDE_DOWN);
    }
    display.schedule(SCROLL);
    Serial.println(freeMemory());
  }


  while(abs(sensor) < 50) {
    if(analogRead(A0) < 512)
      sensor --;
    else
      sensor ++;
  }
  orientation = sensor > 0;
  sensor = 0;

  if(orientation != previousOrientation) {
    previousOrientation = orientation;
    display.setTurned(orientation);
    display.symbol(ALARM, LEFT, FALL_IN);
    display.digit(n % 10, 3, FALL_IN);
    display.digit(n / 10, 2, FALL_IN);
    display.schedule(TURN);
  }*/

}
