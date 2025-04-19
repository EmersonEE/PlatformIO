#include <Arduino.h>
#include "OLED.h"

OLED resultados;

void setup()
{
  resultados.begin();
}

void loop()
{
  resultados.oledVoltage();
  resultados.oledPWM();
  delay(10);
}