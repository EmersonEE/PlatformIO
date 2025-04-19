#include <Arduino.h>
#include "MIC.h"

#define MICRO_BUENO 34
#define MICRO_MALO 35
MIC micrBueno(MICRO_BUENO);
void setup()
{
  Serial.begin(115200);

}

void loop()
{
  Serial.println(micrBueno.readMIC());
  
}