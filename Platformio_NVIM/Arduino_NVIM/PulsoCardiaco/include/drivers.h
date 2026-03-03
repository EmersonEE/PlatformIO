#ifndef __DRIVERS__
#define __DRIVERS__
#include "c_types.h"
#include "sys/_intsup.h"
#include "sys/_types.h"
#include <Arduino.h>
#include <cstdint>
#include <sys/types.h>
class Driver {
public:
  Driver(uint8_t pinLed);
  void ledOn();
  void ledOff();
  void blinkLed(unsigned long time);
  void repeatBlink(uint8_t repeat, unsigned long time);

private:
  uint8_t pinLed;
};
#endif // !__DRIVERS__
