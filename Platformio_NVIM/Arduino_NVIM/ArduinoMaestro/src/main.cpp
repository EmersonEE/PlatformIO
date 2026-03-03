#include "pins_arduino.h"
#include <Arduino.h>
#include <EasyButton.h>
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, 0);
}
void cicloContador(int largo, unsigned long time) {
  for (int i = 0; i < largo; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(time);
    digitalWrite(LED_BUILTIN, LOW);
    delay(time);
  }
}

void loop() {
  cicloContador(10, 1000);
  cicloContador(10, 500);
  cicloContador(10, 50);
  cicloContador(10, 20);
  cicloContador(5, 1500);
  cicloContador(10, 100);
}
