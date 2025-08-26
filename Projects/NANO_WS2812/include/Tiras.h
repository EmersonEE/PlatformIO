#ifndef __TIRAS_H__
#define __TIRAS_H__
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class Tiras
{
private:
    Adafruit_NeoPixel tira;
    uint8_t numP, ledP;

public:
    Tiras(uint8_t _numP, uint8_t _ledP);
    void init();
    void effectLED(uint8_t R, uint8_t G, uint8_t B, unsigned long time);
};


#endif // __TIRAS_H__