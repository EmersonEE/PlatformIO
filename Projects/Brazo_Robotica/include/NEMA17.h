#ifndef __NEMA17_H__
#define __NEMA17_H__

#include <Arduino.h>
class NEMA17
{

public:
    // enum SECUENCIA
    // {
    //     BI_1_BD_1 = 0,
    //     BI_1_BD_2 = 1,
    //     BI_1_BD_3 = 2,
    //     BI_2_BD_1 = 3,
    //     BI_2_BD_2 = 4,
    //     BI_2_BD_3 = 5,
    //     BI_3_BD_1 = 6,
    //     BI_3_BD_2 = 7,
    //     BI_3_BD_3 = 8,
    //     BD_1_BI_1 = 9,
    //     BD_1_BI_2 = 10,
    //     BD_1_BI_3 = 11,
    //     BD_2_BI_1 = 12,
    //     BD_2_BI_2 = 13,
    //     BD_2_BI_3 = 14,
    //     BD_3_BI_1 = 15,
    //     BD_3_BI_2 = 16,
    //     BD_3_BI_3 = 17,

    // };
    NEMA17(uint8_t _pinENA, uint8_t _pinDIR, uint8_t _pinSTEP);
    void vueltaCompleta(uint8_t direction, int totalSteps);
    void mediaVuelta(uint8_t direction, int totalSteps);
    void stepN(long steps, uint8_t direction, int stepDelayUs);
    void rotateDegrees(float degrees, int totalSteps, int stepDelayUs);
    void moveDistance(float height, int totalSteps, int stepDelayUs);
    // void setSecuence(SECUENCIA sel);
    void stopMotor();

private:
    uint8_t pinENA;
    uint8_t pinDIR;
    uint8_t pinSTEP;
    // uint8_t STEPS_PER_REV = 100; // Pasos por revolución física del motor
    // uint8_t MICROSTEPS = 16;    // Microsteps configurados en el driver
    // uint8_t TOTAL_STEPS = STEPS_PER_REV * MICROSTEPS;
    const int STEP_DELAY_US = 500; // Retraso entre pasos (en microsegundos)
};

#endif // __NEMA17_H__