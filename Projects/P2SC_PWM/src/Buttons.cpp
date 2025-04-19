#include "Buttons.h"

// Constructor
Buttons::Buttons(uint8_t _pinB1, uint8_t _pinB2, uint8_t _pinB3, uint8_t _pinB4, uint8_t _pinPot)
{
    pinB1 = _pinB1;
    pinB2 = _pinB2;
    pinB3 = _pinB3;
    pinB4 = _pinB4;
    pinPot = _pinPot;

    lastStateB1 = HIGH;
    lastStateB2 = HIGH;
    lastStateB3 = HIGH;
    lastStateB4 = HIGH;
}

void Buttons::init()
{
    pinMode(pinB1, INPUT);
    pinMode(pinB2, INPUT);
    pinMode(pinB3, INPUT);
    pinMode(pinB4, INPUT);
    pinMode(pinPot, INPUT);
}

uint8_t Buttons::readPot()
{
    int readPot = analogRead(pinPot);
    return map(readPot, 0, 4095, 0, 255);
}

bool Buttons::buttonOnPressed()
{
    bool currentState = digitalRead(pinB1);
    if (currentState == LOW && lastStateB1 == HIGH)
    {
        lastStateB1 = currentState;
        return true;
    }
    lastStateB1 = currentState;
    return false;
}

bool Buttons::button30Pressed()
{
    bool currentState = digitalRead(pinB2);
    if (currentState == LOW && lastStateB2 == HIGH)
    {
        lastStateB2 = currentState;
        return true;
    }
    lastStateB2 = currentState;
    return false;
}

bool Buttons::button60Pressed()
{
    bool currentState = digitalRead(pinB3);
    if (currentState == LOW && lastStateB3 == HIGH)
    {
        lastStateB3 = currentState;
        return true;
    }
    lastStateB3 = currentState;
    return false;
}

bool Buttons::button90Pressed()
{
    bool currentState = digitalRead(pinB4);
    if (currentState == LOW && lastStateB4 == HIGH)
    {
        lastStateB4 = currentState;
        return true;
    }
    lastStateB4 = currentState;
    return false;
}