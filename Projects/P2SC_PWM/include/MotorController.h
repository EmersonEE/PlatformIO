#ifndef __MOTORCONTROLLER_H__
#define __MOTORCONTROLLER_H__
#include <Arduino.h>

class Motor
{

public:

    enum SPEED {
        STOP =0,
        LOW_SPEED = 76,
        MEDIUM_SPEED = 153,
        HIGH_SPEED = 230,
        MAX_SPEED = 255
    };
    // Constructor para el motor de las elices
    Motor(uint8_t _pinMotorA);

    // Constructor para el motor de la base
    Motor(uint8_t _pinMotorB, uint8_t _pinMotorC, uint8_t _pinSpeed);

    // #define LED_CHANNEL_ASPAS 0
    // #define FRECUENCY 5000
    // #define RESOLUTION 8
    void initAspas(uint8_t channel, uint32_t frequency, uint8_t resolution);

    void initBase(uint8_t channel, uint32_t frequency, uint8_t resolution);

    void aspas(SPEED Speed);
    void baseRight(uint8_t setSpeed);
    void baseLeft(uint8_t setSpeed);

private:
    uint8_t pinMotorA;
    uint8_t pinMotorB;
    uint8_t pinMotorC;
    uint8_t pinSpeed;
    uint8_t ledChannel;
    bool isThreePinMotor;
};

#endif ////__MOTORCONTROLLER_H__