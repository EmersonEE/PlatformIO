#ifndef MIC_H
#define MIC_H

#include <Arduino.h>
class MIC
{
public:
    MIC(uint8_t _pinMIC);
    uint8_t readMIC();
    float readMICmA();

private:
    uint8_t pinMIC;
};

#endif