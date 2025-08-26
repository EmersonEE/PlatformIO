#include <Boton.h>
Boton::Boton(uint8_t _pinB, Tiras &_tira)
    : pinB(_pinB), tira(_tira), lastButtonState(LOW), lastDebounceTime(0)
{
    pinMode(pinB, INPUT);
}

void Boton::onStrip(uint8_t R, uint8_t G, uint8_t B, unsigned long time)
{
    bool currentButtonState = digitalRead(pinB);

    if (currentButtonState != lastButtonState)
    {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY)
    {
        if (currentButtonState == HIGH)
        {
            tira.effectLED(R, G, B, time);
        }
    }

    lastButtonState = currentButtonState;
}

void Boton::offStrip()
{
    // tira.clear(); // Apagar todos los LEDs
    // tira.show();
}