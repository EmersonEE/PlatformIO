#include "PWM.h"

PWM::PWM(uint8_t _pin) {
    pin = _pin;
}

void PWM::init(uint8_t channel, uint32_t frequency, uint8_t resolution) {
    ledChannel = channel;
    ledcSetup(ledChannel, frequency, resolution); 
    ledcAttachPin(pin, ledChannel);              
}

void PWM::ledPWM(uint8_t pwmValue) {
    ledcWrite(ledChannel, pwmValue); 
}