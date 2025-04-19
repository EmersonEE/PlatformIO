#include "MIC.h"

MIC::MIC(uint8_t _pinMIC)
{
    pinMIC = _pinMIC;
    pinMode(pinMIC, INPUT);
}

uint8_t MIC::readMIC()
{
    return map(analogRead(pinMIC), 0, 4095, 0, 255);
}

float MIC::readMICmA()
{
    return (analogRead(pinMIC) * 3.3 / 4095.0 )- 1.23 ;
}