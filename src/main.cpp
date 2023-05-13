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
  delay(1000);
  display.begin();
  display.showSymbol(ALARM, LEFT, FADE);
  display.showDigit(0, 2, SLIDE_UP);
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
    if (n > 9)
      n = 0;
    display.showDigit(n, 3, SLIDE_UP);
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
  */
}
