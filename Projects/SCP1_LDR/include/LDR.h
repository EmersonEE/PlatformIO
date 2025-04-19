#ifndef LDR_H
#define LDR_H

#include <Arduino.h>

class LDR
{
private:
    uint32_t pin;

public:
    LDR(uint32_t _pin);
    void init();
    float readVoltage();
    uint8_t readPWM();
};

#endif