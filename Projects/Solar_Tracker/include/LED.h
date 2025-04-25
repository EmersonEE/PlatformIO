#ifndef __LED_H__
#define __LED_H__

#include <Arduino.h>
class LED
{

public:
    LED(uint8_t _pinLED1, uint8_t _pinLED2);
    void ledConf();
    void ledLDR();
    void ledsOff();

private:
    uint8_t pinLED1;
    uint8_t pinLED2;
};

#endif // __LED_H__