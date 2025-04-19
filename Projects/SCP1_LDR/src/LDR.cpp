#include "LDR.h"

LDR::LDR(uint32_t _pin)
{
    pin = _pin;
    pinMode(pin, INPUT);
}

void LDR::init()
{
}

float LDR::readVoltage()
{
    int readLDR = analogRead(pin);
    float voltage = (readLDR / 4095.0) * 3.3;
    return voltage;
}

uint8_t LDR::readPWM()
{
    int readLDR = analogRead(pin);
    uint8_t _pwmResult = map(readLDR, 0, 4095, 0, 255);
    return _pwmResult;
}
