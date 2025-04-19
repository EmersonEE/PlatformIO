// #include "LDR.h"

// LDR::LDR(uint8_t _pinLDR)
// {
//     pinLDR = _pinLDR;
//     pinMode(pinLDR, INPUT);
// }

// uint8_t LDR::readLDR()
// {
//     return map(analogRead(pinLDR), 0, 4095, 0, 255);
// }

// float LDR::readVoltage()
// {
//     return (3.30) * analogRead(pinLDR) / 4095.0;
// }

// ldr.cpp
#include "LDR.h"

LDR::LDR(uint8_t pin, float vref) : pinLDR(pin), vref(vref)
{
    pinMode(pinLDR, INPUT);
}

uint16_t LDR::readRaw()
{
    return analogRead(pinLDR); // Lectura directa (0-4095)
}

uint8_t LDR::readMapped(uint16_t minRaw, uint16_t maxRaw)
{
    uint16_t raw = readRaw();
    raw = constrain(raw, minRaw, maxRaw); // Limitar al rango esperado
    return map(raw, minRaw, maxRaw, 0, 255);
}

float LDR::readVoltage()
{
    return vref * readRaw() / 4095.0;
}

float LDR::readVoltage2(uint8_t selLDR)
{
    return vref * analogRead(selLDR) / 4095.0;
}

void LDR::setVref(float newVref)
{
    vref = newVref;
}

float LDR::getLDRAverage(uint8_t ldrNum, uint8_t ldrNum2)
{
    float value1 = readVoltage2(ldrNum);
    float value2 = readVoltage2(ldrNum2);
    return (value1 + value2) / 2;
}
// float getLDRAverageUp();
// float getLDRAverageDown();
// float getLDRAverageLeft();
// float getLDRAverageRight();
