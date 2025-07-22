#ifndef __RGB_H__
#define __RGB_H__

#include <stdio.h>
#include <driver/ledc.h>
#include <inttypes.h>
#include <stdlib.h>
#include <time.h>

class RGB
{

public:
    enum COLOR
    {
        RED = 0,
        GREEN,
        BLUE,
        PINK,
        VIOLET,
        ORANGE,
        YELLOW,
        GRIS

    };
    RGB(uint8_t _pinRed, uint8_t _pinGreen, uint8_t _pinBlue);
    void init();
    void seleColor(COLOR sel);
    void setColor(uint8_t red, uint8_t green, uint8_t blue);
    void ledOff();
    void randomColor();

private:
    uint8_t pinRed;
    uint8_t pinGreen;
    uint8_t pinBlue;
};

#endif // __RGB_H__