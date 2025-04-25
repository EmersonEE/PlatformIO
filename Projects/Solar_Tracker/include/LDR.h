#ifndef __LDR_H__
#define __LDR_H__

#include <Arduino.h>

class LDR
{
public:
    LDR(uint8_t pin, float vref = 3.3); // Permitir cambiar Vref
    void init();
    uint16_t readRaw();                                              // Leer valor crudo (0-4095)
    uint8_t readMapped(uint16_t minRaw = 0, uint16_t maxRaw = 4095); // Mapeo personalizable
    float readVoltage();                                             // Voltaje con Vref ajustable
    void setVref(float vref);                                        // Cambiar voltaje de referencia
    float getLDRAverage(uint8_t ldrNum, uint8_t ldrNum2);
    float readVoltage2(uint8_t selLDR);                                             // Voltaje con Vref ajustable


private:
    uint8_t pinLDR;
    float vref;
};

#endif // __LDR_H__