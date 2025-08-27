#ifndef __BOTON_H__
#define __BOTON_H__

#include <Arduino.h>
#include <Tiras.h>

class Boton
{
private:
    uint8_t pinB;
    // Tiras &tira;
    bool lastButtonState;
    unsigned long lastDebounceTime;
    static const unsigned long DEBOUNCE_DELAY = 50;

public:
    // Boton(uint8_t _pinB, Tiras &_tira);
    Boton(uint8_t _pinB);
    void begin();
    void onStrip(uint8_t R, uint8_t G, uint8_t B, unsigned long time);
    void onDoubleStrip(uint8_t R, uint8_t G, uint8_t B, unsigned long time);

    void offStrip();
};

#endif // __BOTON_H__