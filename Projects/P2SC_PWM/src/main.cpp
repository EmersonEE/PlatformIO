#include <Arduino.h>
#include "DEF.h"
#include "Buttons.h"
#include "MotorController.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

Motor motorAspas(MOTOR_ASPAS);
Motor motorBase(PIN_A, PIN_B, PIN_PWM);
Buttons botones(BOTON_ENCENDIDO, BOTON_LOWSPEED, BOTON_MEDIUMSPEED, BOTON_HIGHSPEED, PIN_POT);
TaskHandle_t Tarea0;

bool encendido = false;
void loop0(void *parameter)
{
    for (;;)
    {
        motorBase.baseLeft(botones.readPot());
        delay(1000);
        motorBase.baseRight(botones.readPot());
        delay(1000);
    }
}

void setup()
{
    Serial.begin(115200);
    motorAspas.initAspas(0, FRECUENCY, RESOLUTION);
    motorBase.initBase(1, FRECUENCY, RESOLUTION);
    botones.init();
    xTaskCreatePinnedToCore(loop0, "Tarea_0", 1000, NULL, 1, &Tarea0, 0); // Core 0
}

void loop()
{
    if (botones.buttonOnPressed())
    {
        if (encendido)
        {
            motorAspas.aspas(Motor::STOP);
            encendido = false;
        }
        else
        {
            motorAspas.aspas(Motor::LOW_SPEED);
            encendido = true;
        }
    }
    if (botones.button30Pressed())
    {
        motorAspas.aspas(Motor::LOW_SPEED);
        encendido = true;
    }
    else if (botones.button60Pressed())
    {
        motorAspas.aspas(Motor::MEDIUM_SPEED);
        encendido = true;
    }
    else if (botones.button90Pressed())
    {
        motorAspas.aspas(Motor::HIGH_SPEED);
        encendido = true;
    }
}
