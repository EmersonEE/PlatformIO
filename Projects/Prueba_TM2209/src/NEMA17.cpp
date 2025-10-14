#include "NEMA17.h"
/**************************************************************************/
/*!
    @file     NEMA17.h
    @brief    Constructor.
    @details  Pines de control para el motor NEMA 17.
*/
/**************************************************************************/
/**************************************************************************/
/*!
    @brief    Colocar los pines a utilizar.
    @param    _pinENA       Pin conectado al ENABLE.
    @param    _pinDIR       Pin conectado a DIRECCION
    @param    _pinSTEP      Pin conectado a STEP.
*/
/**************************************************************************/
NEMA17::NEMA17(uint8_t _pinENA, uint8_t _pinDIR, uint8_t _pinSTEP)
{
    pinENA = _pinENA;
    pinDIR = _pinDIR;
    pinSTEP = _pinSTEP;
    pinMode(pinSTEP, OUTPUT);
    pinMode(pinDIR, OUTPUT);
    pinMode(pinENA, OUTPUT);
    digitalWrite(pinENA, LOW);
}
/// @brief Ejecuta el número especificado de pasos en la dirección dada.
///
/// Este método mueve el motor paso a paso la cantidad de veces definida por `totalSteps`.
/// Es el método principal para el movimiento controlado del motor.
///
/// @param direction La dirección del movimiento. Generalmente, **HIGH** o **LOW**
///                  (o `1` o `0`) para el pin DIR.
/// @param totalSteps El número total de pasos (pulsos) que debe ejecutar el motor.
///                   Para una vuelta completa ($360^{\circ}$), este valor depende
///                   de la configuración de micropasos del driver (ej: 200 para paso completo).
void NEMA17::vueltaCompleta(uint8_t direction, int totalSteps)
{
    digitalWrite(pinDIR, direction); // Establece dirección
    for (int i = 0; i < totalSteps; i++)
    {
        digitalWrite(pinSTEP, LOW);
        delayMicroseconds(2);
        digitalWrite(pinSTEP, HIGH);
        delayMicroseconds(STEP_DELAY_US);
    }
}
void NEMA17::mediaVuelta(uint8_t direction, int totalSteps)
{
}
void NEMA17::stopMotor()
{
}

void NEMA17::stepN(long steps, uint8_t direction, int stepDelayUs)
{
    digitalWrite(pinDIR, direction);
    digitalWrite(pinENA, LOW);
    for (long i = 0; i < steps; i++)
    {
        digitalWrite(pinSTEP, HIGH);
        delayMicroseconds(2);
        digitalWrite(pinSTEP, LOW);
        delayMicroseconds(stepDelayUs);
    }
}

void NEMA17::rotateDegrees(float degrees, int totalSteps, int stepDelayUs)
{
    if (degrees == 0.0f)
        return;
    bool negative = (degrees < 0.0f);
    float absdeg = fabs(degrees);
    float stepsPerDegree = (float)totalSteps / 360.0f;
    long steps = lround(absdeg * stepsPerDegree); // redondear al tenero mas cercano

    uint8_t dir = negative ? HIGH : LOW;
    stepN(steps, dir, stepDelayUs);
}

void NEMA17::moveDistance(float height, int totalSteps, int stepDelayUs)
{
    if (height == 0.0f)
        return;
    const float pasos_por_mm = 1600.0 / 8.05;
    long altura = lround(height * pasos_por_mm);
    bool negative = (height < 0.0f);
    uint8_t dir = negative ? HIGH : LOW;
    stepN(abs(altura), dir, stepDelayUs);
}
