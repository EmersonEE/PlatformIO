#include <PID.h>

PID::PID(float pKp, float pKi, float pKd, float pInt_saturation) : kp(pKd), ki(pKi), kd(pKd), in_saturation(pInt_saturation)
{
    acum_integral = 0;
    first_run = 1;
}
// PID.cpp - Implementar nuevos métodos
void PID::setOutputLimits(float min, float max)
{
    outputMin = min;
    outputMax = max;
}

void PID::setTarget(float pTarget)
{
    target = pTarget;
}

float PID::update(float input, float dt)
{
    if (first_run)
    {
        prev_error = target - input;
        first_run = 0;
    }

    float error = target - input;
    float prop = error;

    // Integral
    acum_integral += (error + prev_error) * 0.5f * dt;

    // Anti-windup
    if (acum_integral > in_saturation)
    {
        acum_integral = in_saturation;
    }
    else if (acum_integral < -in_saturation)
    {
        acum_integral = -in_saturation;
    }

    // Derivativo
    float derivativo = (error - prev_error) / dt;
    prev_error = error;

    // Calcular salida
    float output = kp * prop + ki * acum_integral + kd * derivativo;

    // Limitar salida
    if (output > outputMax)
        output = outputMax;
    if (output < outputMin)
        output = outputMin;

    return output;
}

void PID::reset()
{
    ki = 0;
    prev_error = 0;
    first_run = 1;
}
