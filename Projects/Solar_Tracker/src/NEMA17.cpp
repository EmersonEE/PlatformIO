#include "NEMA17.h"
/**************************************************************************/
/*!
    @file     NEMA17.h
    @brief    Constructor.
    @details  Pines de control para el motor NEMA 17.
*/
/**************************************************************************/
/**************************************************************************/
/*!
    @brief    Colocar los pines a utilizar.
    @param    _pinENA       Pin conectado al ENABLE.
    @param    _pinDIR       Pin conectado a DIRECCION
    @param    _pinSTEP      Pin conectado a STEP.
*/
/**************************************************************************/
NEMA17::NEMA17(uint8_t _pinENA, uint8_t _pinDIR, uint8_t _pinSTEP)
{
    pinENA = _pinENA;
    pinDIR = _pinDIR;
    pinSTEP = _pinSTEP;
    pinMode(pinSTEP, OUTPUT);
    pinMode(pinDIR, OUTPUT);
    pinMode(pinENA, OUTPUT);
    digitalWrite(pinENA, LOW);
}
// void NEMA17::direcction(){}

void NEMA17::moveRight()
{
    digitalWrite(pinDIR, LOW);
    digitalWrite(pinSTEP, HIGH);
    delay(5);
    digitalWrite(pinSTEP, LOW);
    delay(5);
}

void NEMA17::moveUp()
{
    digitalWrite(pinDIR, LOW);
    digitalWrite(pinSTEP, HIGH);
    delay(5);
    digitalWrite(pinSTEP, LOW);
    delay(5);
}

void NEMA17::moveLeft()
{
    digitalWrite(pinDIR, HIGH);
    digitalWrite(pinSTEP, HIGH);
    delay(5);
    digitalWrite(pinSTEP, LOW);
    delay(5);
}

void NEMA17::moveDown()
{
    digitalWrite(pinDIR, HIGH);
    digitalWrite(pinSTEP, HIGH);
    delay(5);
    digitalWrite(pinSTEP, LOW);
    delay(5);
}

void NEMA17::testMotorM1()
{
    for (int i = 0; i < stepsPerRevolutionM1; i++)
    {
        moveRight();
    }
    delay(2000);
    for (int i = 0; i < stepsPerRevolutionM1; i++)
    {
        moveLeft();
    }
    delay(2000);
}

void NEMA17::testMotorM2()
{
    for (int i = 0; i < stepsPerRevolutionM2; i++)
    {
        moveUp();
    }
    delay(2000);
    for (int i = 0; i < stepsPerRevolutionM2; i++)
    {
        moveDown();
    }
    delay(2000);
}

void NEMA17::motorStop(uint8_t conteo)
{
    if ((conteo <= motorStopA || conteo <= motorStooBaseN) || (conteo >= motorStopB || conteo >= motorStopBase))
    {
        digitalWrite(pinENA, HIGH);
    }
    else
    {
        digitalWrite(pinENA, LOW);
    }
}

/**************************************************************************/
/*!
    @file     NEMA17.h
    @brief    Implementación del control del motor NEMA17 basado en sensores.
    @details  Este método utiliza las lecturas de un sensor para mover el motor
              en un eje vertical u horizontal dependiendo de la configuración.
*/
/**************************************************************************/
/**************************************************************************/
/*!
    @brief    Mueve y detiene el motor NEMA17 según las lecturas del sensor.
    @param    sensor       Valor del sensor utilizado para determinar el movimiento.
    @param    min          Número mínimo de pasos permitidos.
    @param    max          Número máximo de pasos permitidos.
    @param    pasos        Referencia del contador de pasos del motor.
    @param    sel          Indicador para el eje de movimiento (true = Vertical,
                           false = Horizontal).
    @param    direction    Puntero que indica la dirección del movimiento.
*/
/**************************************************************************/
void NEMA17::moveAndStopMotor(int sensor, int min, int max, int &pasos, bool sel, int *direcction)
{
    if (sensor > 175 && pasos < max)
    {
        if (sel == true)
        {
            moveUp();
            *direcction = 24;
        }
        else
        {
            moveRight();
            *direcction = 26;
        }
        pasos++;
        motorStop(pasos);
    }
    else if (sensor < 75 && pasos > min)
    {
        if (sel == true)
        {
            moveDown();
            *direcction = 25;
        }
        else
        {
            moveLeft();
            *direcction = 27;
        }
        pasos--;
        motorStop(pasos);
    }
    else
    {
        motorStop(pasos);
    }
}
/**************************************************************************/
/*!
    @file     NEMA17.h
    @brief    Archivo de implementación para el control del motor NEMA17.
    @details  Contiene métodos para mover y detener el motor basado en datos
              de sensores LDR.
*/
/**************************************************************************/
/**************************************************************************/
/*!
    @brief    Mover y detener el motor NEMA17 según lecturas de sensores LDR.
    @param    average1        Promedio de lecturas del sensor 1.
    @param    average2        Promedio de lecturas del sensor 2.
    @param    calibrationMin  Límite mínimo de calibración para el movimiento.
    @param    calibrationMax  Límite máximo de calibración para el movimiento.
    @param    min             Número mínimo de pasos permitidos.
    @param    max             Número máximo de pasos permitidos.
    @param    pasos           Referencia del contador de pasos del motor.
    @param    sel             Selección para el eje de movimiento.
    @param    direction       Puntero para indicar la dirección del movimiento.
*/
/**************************************************************************/
void NEMA17::moveAndStopMotorLDR(float &average1, float &average2, float calibrationMin, float calibrationMax, int min, int max, int &pasos, bool sel, int *direcction)
{
    if ((average1 - average2 > calibrationMin) && pasos > min)
    {
        if (sel == true)
        {
            moveUp();
            *direcction = 24;
        }
        else
        {
            moveLeft();
            *direcction = 27;
        }
        pasos--;
        motorStop(pasos);
    }
    else if ((average2 - average1 > calibrationMax) && pasos < max)
    {
        if (sel == true)
        {
            moveDown();
            *direcction = 25;
        }
        else
        {
            moveRight();
            *direcction = 26;
        }
        pasos++;
        motorStop(pasos);
    }
    else
    {
        motorStop(pasos);
    }
}

void NEMA17::moveMotorsWithPID(float outputX, float outputY, int &pasosX, int &pasosY, int *direcction)
{
    // Eje X
    if (outputX < -0.1f && pasosX > -360)
    { // Mover izquierda
        moveLeft();
        pasosX--;
        *direcction = 27;
    }
    else if (outputX > 0.1f && pasosX < 360)
    { // Mover derecha
        moveRight();
        pasosX++;
        *direcction = 26;
    }

    // Eje Y
    if (outputY < -0.1f && pasosY > 0)
    { // Mover abajo
        moveDown();
        pasosY--;
        *direcction = 25;
    }
    else if (outputY > 0.1f && pasosY < 300)
    { // Mover arriba
        moveUp();
        pasosY++;
        *direcction = 24;
    }

    // Aplicar límites
    motorStop(pasosX);
    motorStop(pasosY);
}
