#include <SD.h>
#include <MemoryFree.h>
#include <Chrono.h>

#include "display.h"

Display display;
Chrono chrono;

void setup()
{
  Serial.begin(9600);
  Serial.println("go");
  display.begin();
  delay(1000);
  display.symbol(ALARM, LEFT, SLIDE_UP);
  display.digit(0, 2, FADE);
  display.digit(0, 3, FADE);
  display.schedule();
  chrono.restart();
}

long sensor;
boolean orientation;
boolean previousOrientation;
uint8_t n = 0;

void loop()
{
  display.update();

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
    display.schedule();
    Serial.println(freeMemory());
  }

/*
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
  }*/
}
