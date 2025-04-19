#include "MotorController.h"
#include "PWMLED.h"

Motor::Motor(uint8_t _pinMotorA)
{
    pinMotorA = _pinMotorA;
    isThreePinMotor = false;
}

Motor::Motor(uint8_t _pinMotorB, uint8_t _pinMotorC, uint8_t _pinSpeed)
{
    pinMotorC = _pinMotorC;
    pinMotorB = _pinMotorB;
    pinSpeed = _pinSpeed;
    isThreePinMotor = true;
}

void Motor::initAspas(uint8_t channel, uint32_t frequency, uint8_t resolution)
{
    ledChannel = channel;
    ledcSetup(ledChannel, frequency, resolution);
    ledcAttachPin(pinMotorA, ledChannel);
}
void Motor::aspas(SPEED Speed)
{
    ledcWrite(ledChannel, Speed);
}
void Motor::initBase(uint8_t channel, uint32_t frequency, uint8_t resolution)
{
    ledChannel = channel;
    ledcSetup(ledChannel, frequency, resolution);
    ledcAttachPin(pinSpeed, ledChannel);
    pinMode(pinMotorB, OUTPUT);
    pinMode(pinMotorC, OUTPUT);

    digitalWrite(pinMotorB, LOW);
    digitalWrite(pinMotorC, LOW);
}

void Motor::baseRight(uint8_t setSpeed)
{

    digitalWrite(pinMotorB, HIGH);
    digitalWrite(pinMotorC, LOW);
    ledcWrite(ledChannel, setSpeed);
}

void Motor::baseLeft(uint8_t setSpeed)
{
    digitalWrite(pinMotorB, LOW);
    digitalWrite(pinMotorC, HIGH);
    ledcWrite(ledChannel, setSpeed);
}