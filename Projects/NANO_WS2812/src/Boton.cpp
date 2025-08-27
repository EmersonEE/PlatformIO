#include <Boton.h>

#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel tiraled(60, 9, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel tiraled2(60, 10, NEO_GRB + NEO_KHZ800);

// Boton::Boton(uint8_t _pinB, Tiras &_tira)
//     : pinB(_pinB), tira(_tira), lastButtonState(LOW), lastDebounceTime(0)
Boton::Boton(uint8_t _pinB)
    : pinB(_pinB), lastButtonState(LOW), lastDebounceTime(0)
{
    pinMode(pinB, INPUT);
}
void Boton::begin()
{
    tiraled.begin();
    tiraled2.begin();
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
            Boton::offStrip();
            for (int i = 0; i < 60; i++)
            {
                tiraled.clear();
                tiraled.setPixelColor(i, tiraled.Color(R, G, B));
                tiraled.show();
                delay(time);
            }
            Boton::offStrip();
        }
    }

    lastButtonState = currentButtonState;
}

void Boton::onDoubleStrip(uint8_t R, uint8_t G, uint8_t B, unsigned long time)
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
            Boton::offStrip();
            for (int i = 0; i < 60; i++)
            {
                tiraled.clear();
                tiraled2.clear();
                tiraled.setPixelColor(i, tiraled.Color(R, G, B));
                tiraled2.setPixelColor(i, tiraled2.Color(R, G, B));
                tiraled.show();
                tiraled2.show();
                delay(time);
            }
            Boton::offStrip();
        }
    }

    lastButtonState = currentButtonState;
}

void Boton::offStrip()
{
    tiraled.clear();
    tiraled.show();
    tiraled2.clear();
    tiraled2.show();
}