#ifndef __PID_H__
#define __PID_H__
#include <Arduino.h>

class PID
{
public:
    PID(float pKp, float pKi, float pkd, float pInt_saturation = 2000);
    float update(float error, float dt);
    void setKp(float pKp) { kp = pKp; }
    void setKi(float pKi) { ki = pKi; }
    void setKd(float pKd) { kd = pKd; }
    void reset();
    void setOutputLimits(float min, float max);
    void setTarget(float target);

private:
    float kp, ki, kd;
    float prev_error;
    float acum_integral;
    float in_saturation;
    uint8_t first_run;
    float outputMin = -1.0f;
    float outputMax = 1.0f;
    float target = 0.0f;
};

#endif // __PID_H__

/*
Proporcional   e = t(target) - m(measure)

Integral

Derivada error

output =   e*Kp   +  Ki*∫<0 >t e*dt   +    de*dkd/dt
             P              I                 D
d
*/

// PID.h - Añadir estos métodos
// class PID
// {
// public:
//     // ... métodos existentes ...
//     void setOutputLimits(float min, float max);
//     void setTarget(float target);

// private:
//     // ... variables existentes ...
//     float outputMin = -1.0f;
//     float outputMax = 1.0f;
//     float target = 0.0f;
// };