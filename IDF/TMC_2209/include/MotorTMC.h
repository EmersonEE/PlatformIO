#ifndef __MOTORTMC_H__
#define __MOTORTMC_H__

#include <driver/gpio.h>
#include <esp_log.h>
#include <esp_err.h>
#include <stdint.h>
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
class MotorTMC
{
private:
    gpio_num_t enPin, stepPin, dirPin;

public:
    MotorTMC(gpio_num_t _enPin, gpio_num_t _stepPin, gpio_num_t _dirPin);
    void init();
    void movRight();
    void movLeft();
    void testMotor();

    ~MotorTMC();
};

#endif // __MOTORTMC_H__