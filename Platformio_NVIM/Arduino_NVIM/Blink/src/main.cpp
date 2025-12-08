#include "esp32-hal.h"
#include <Arduino.h>
#include <LED.h>
#define LED1 2
#define LED2 25
#define LED3 26
#define LED4 27
LEDS leds(LED1, LED2, LED3, LED4);
LEDS led22(LED1, LED2, LED3, LED4);
void setup() {
  leds.init();
  Serial.begin(115200);
}

void loop() {

  for (int i = 0; i < 10; i++) {
    leds.OnAll();
    delay(500);
    leds.OffAll();
    delay(600);
  }
  if (int i = 0) {
    leds.SecuenciaCinco();
    delay(500);
    leds.SecuenciaCuatro();
    delay(400);
  }
}
