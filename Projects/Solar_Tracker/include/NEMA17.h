#ifndef __NEMA17_H__
#define __NEMA17_H__


#include <Arduino.h>
class NEMA17
{

public:
    NEMA17(uint8_t _pinENA, uint8_t _pinDIR, uint8_t _pinSTEP);
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();
    void testMotorM1();
    void testMotorM2();
    void motorStop(uint8_t conteo);
    void moveAndStopMotor(int sensor, int min, int max, int &pasos, bool sel, int *direcction);
    void moveAndStopMotorLDR(float &average1, float &average2, float calibration, float calibrationMax, int min, int max, int &pasos, bool sel, int *direcction);
    void moveMotorsWithPID(float outputX, float outputY, int &pasosX, int &pasosY, int *direcction) ;
private:
    int stepsPerRevolutionM1 = 2400;
    int stepsPerRevolutionM2 = 2400;
    int motorStopA = 0;
    int motorStopB = 301;
    int motorStopBase = 361;
    int motorStooBaseN = -361;
    uint8_t pinENA;
    uint8_t pinDIR;
    uint8_t pinSTEP;
};


#endif // __NEMA17_H__