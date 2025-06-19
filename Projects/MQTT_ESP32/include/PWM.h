#ifndef __PWM_H__
#define __PWM_H__

#include <Arduino.h>
class PWM{

    public:
        PWM(uint8_t _pin);
        void init(uint8_t channel, uint32_t frequency, uint8_t resolution);
        void ledPWM(uint8_t pwmValue);
    private:
        uint8_t pin;
        uint8_t ledChannel;
    

};
#endif // __PWM_H__