#include "PWM.h"

PWM::PWM(uint8_t _pin) {
    pin = _pin;
}

void PWM::init(uint8_t channel, uint32_t frequency, uint8_t resolution) {
#if defined(ESP32)
    ledChannel = channel;
    ledcSetup(ledChannel, frequency, resolution); 
    ledcAttachPin(pin, ledChannel);              
#else
    pinMode(pin, OUTPUT);
#endif
}

void PWM::ledPWM(uint8_t pwmValue) {
#if defined(ESP32)
    ledcWrite(ledChannel, pwmValue); 
#else
    analogWrite(pin, pwmValue);
#endif
}