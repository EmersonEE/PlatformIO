#include "JOYSTICK.h"
// PinX - PinY - PinSW
JOYSTICK::JOYSTICK(uint8_t _pinX, uint8_t _pinY, uint8_t _pinSW)
    : pinX(_pinX), pinY(_pinY), pinSW(_pinSW)
{
    pinMode(pinX, INPUT);
    pinMode(pinY, INPUT);
    pinMode(pinSW, INPUT_PULLUP);

    lastButtonState = HIGH;
    lastDebounceTime = 0;
}

int JOYSTICK::readX()
{
    return analogRead(pinX);
}

int JOYSTICK::readY()
{
    return analogRead(pinY);
}

bool JOYSTICK::isPressed()
{
    return digitalRead(pinSW) == LOW;
}

// Detectar si el botón fue presionado (transición)
bool JOYSTICK::wasPressed()
{
    int reading = digitalRead(pinSW);
    bool pressed = false;

    if (reading != lastSWState)
    {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay)
    {
        if (reading != lastButtonState)
        {
            lastButtonState = reading;
            if (lastButtonState == LOW)
            {
                pressed = true;
            }
        }
    }

    lastSWState = reading;
    return pressed;
}

bool JOYSTICK::wasReleased()
{
    int reading = digitalRead(pinSW);

    if (reading != lastButtonState)
    {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay)
    {
        if (reading != lastButtonState)
        {
            lastButtonState = reading;

            if (lastButtonState == HIGH)
            {
                return true;
            }
        }
    }

    return false;
}

int JOYSTICK::mapX(int min, int max)
{
    return map(analogRead(pinX), 0, 4095, min, max);
}

int JOYSTICK::mapY(int min, int max)
{
    return map(analogRead(pinY), 0, 4095, min, max);
}