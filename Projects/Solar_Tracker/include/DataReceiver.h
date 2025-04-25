#ifndef __DATARECEIVER_H__
#define __DATARECEIVER_H__

#include <Arduino.h>
#include "LDR.h"

class DataReceiver
{
public:
    DataReceiver(LDR &ldr1, LDR &ldr2, LDR &ldr3, LDR &ldr4);
    void update();
    uint8_t getBrightestLDR();
    float getLDRVoltage(uint8_t ldrNum);
    float getLDRAverageUp();
    float getLDRAverageDown();
    float getLDRAverageLeft();
    float getLDRAverageRight();
    float getLDRAverageCenter();

private:
    LDR *ldrs[4];          // Array de punteros a LDRs
    uint16_t rawValues[4]; // Almacena lecturas actuales
};

#endif // __DATARECEIVER_H__