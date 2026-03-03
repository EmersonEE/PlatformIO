#include "drivers.h"
#include "c_types.h"
#include "core_esp8266_features.h"
#include "sys/_intsup.h"
#include "sys/_types.h"
#include <cstdint>
#include <sys/types.h>

Driver::Driver(uint8_t pinLed) {
  this->pinLed = pinLed;
  pinMode(pinLed, OUTPUT);
}

void Driver::ledOn() { digitalWrite(pinLed, HIGH); }
void Driver::ledOff() { digitalWrite(pinLed, LOW); }

void Driver::blinkLed(unsigned long time) {
  digitalWrite(pinLed, HIGH);
  delay(time);
  digitalWrite(pinLed, LOW);
  delay(time);
}

void Driver::repeatBlink(uint8_t repeat, unsigned long time) {

  for (int i = 0; i < repeat; i++) {
    Driver::blinkLed(time);
  }
}
