#include "PWMLED.h"

PWMLED::PWMLED(uint8_t _pin) {
    pin = _pin;
}

void PWMLED::init(uint8_t channel, uint32_t frequency, uint8_t resolution) {
    ledChannel = channel;
    ledcSetup(ledChannel, frequency, resolution); 
    ledcAttachPin(pin, ledChannel);              
}

void PWMLED::ledPWM(uint8_t pwmValue) {
    ledcWrite(ledChannel, pwmValue); 
}