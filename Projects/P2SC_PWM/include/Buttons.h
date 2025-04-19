#ifndef __BUTTONS_H__
#define __BUTTONS_H__

#include <Arduino.h>

class Buttons
{
public:
    
    Buttons(uint8_t _pinB1, uint8_t _pinB2, uint8_t _pinB3, uint8_t _pinB4, uint8_t _pinPot);


    void init();

    uint8_t readPot();
    bool buttonOnPressed();
    bool button30Pressed();
    bool button60Pressed();
    bool button90Pressed();

private:
    uint8_t pinB1, pinB2, pinB3, pinB4, pinPot;
    uint8_t lastStateB1, lastStateB2, lastStateB3, lastStateB4; 
};

#endif // __BUTTONS_H__