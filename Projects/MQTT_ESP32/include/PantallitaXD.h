#ifndef __PANTALLITAXD_H__
#define __PANTALLITAXD_H__

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class PantallitaXD
{
    public:
    PantallitaXD();
    void init();
    void showMessage(const String message);
    void showCarnet(const String message);
    void showCarnetError(const String message);
    void showAsistenciaYa(const String message);
    private:
    #define SCREEN_WIDTH 128
    #define SCREEN_HEIGHT 64
    #define OLED_RESET -1
};

#endif // __PANTALLITAXD_H__