#ifndef PWMLED_H
#define PWMLED_H

#include <Arduino.h>

class PWMLED {
public:
    PWMLED(uint8_t _pin);
    void init(uint8_t channel, uint32_t frequency, uint8_t resolution);
    void ledPWM(uint8_t pwmValue);

private:
    uint8_t pin;
    uint8_t ledChannel;
};

#endif // PWMLED_H