#ifndef __L298N_H__
#define __L298N_H__

#include <stdio.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_err.h>
#include <esp_log.h>

class L298N
{
private:
    esp_err_t error;
    const char *TAG = "L298N";
    gpio_num_t IN1;
    gpio_num_t IN2;
    gpio_num_t EN;

public:
    L298N(gpio_num_t IN1, gpio_num_t IN2, gpio_num_t EN);
    void motorConfig();
    void motorRight();
    void motorLeft();
    void motorForward();
    void motorReverse();
    void motorStop();
};

#endif // __L298N_H__