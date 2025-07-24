#ifndef __SERVOIDF_H__
#define __SERVOIDF_H__

#include <stdio.h>
#include <inttypes.h>
#include <driver/ledc.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

class Servo
{
private:
    ledc_channel_t channel;
    gpio_num_t gpio;

    static constexpr ledc_mode_t SPEED_MODE = LEDC_LOW_SPEED_MODE;
    static constexpr ledc_timer_t TIMER_NUM = LEDC_TIMER_0;
    static constexpr ledc_timer_bit_t DUTY_RESOLUTION = LEDC_TIMER_16_BIT;
    static constexpr uint32_t FREQUENCY = 50;
    static constexpr ledc_clk_cfg_t CLK_CONFIG = LEDC_AUTO_CLK;

    static constexpr uint32_t SERVO_MIN_PULSEWIDTH = 500;   // in µs
    static constexpr uint32_t SERVO_MAX_PULSEWIDTH = 2500;  // in µs
    static constexpr uint8_t SERVO_MAX_DEGREE = 180;

    long map(int x, long in_min, long in_max, long out_min, long out_max);

public:
    Servo(ledc_channel_t _channel, gpio_num_t _gpio);
    void init();
    void setAngle(int angle);
    ~Servo();
};

#endif // __SERVOIDF_H__
