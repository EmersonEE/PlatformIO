#include <Arduino.h>

class LED {
public:
  LED(uint8_t _ledPin);
  void on();
  void off();

private:
  uint8_t ledPin;
};
