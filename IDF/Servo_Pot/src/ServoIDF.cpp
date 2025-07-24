#include "ServoIDF.h"

Servo::Servo(ledc_channel_t _channel, gpio_num_t _gpio)
{
    channel = _channel;
    gpio = _gpio;
}

Servo::~Servo() {}

long Servo::map(int x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void Servo::init()
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode = SPEED_MODE,
        .duty_resolution = DUTY_RESOLUTION,
        .timer_num = TIMER_NUM,
        .freq_hz = FREQUENCY,
        .clk_cfg = CLK_CONFIG};

    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .gpio_num = gpio,
        .speed_mode = SPEED_MODE,
        .channel = channel,
        .timer_sel = TIMER_NUM,
        .duty = 0,
        .hpoint = 0};

    ledc_channel_config(&ledc_channel);
}

void Servo::setAngle(int angle)
{
    int pulso_us = map(angle, 0, SERVO_MAX_DEGREE, SERVO_MIN_PULSEWIDTH, SERVO_MAX_PULSEWIDTH);
    int duty = (pulso_us * (1 << DUTY_RESOLUTION)) / 20000;

    ledc_set_duty(SPEED_MODE, channel, duty);
    ledc_update_duty(SPEED_MODE, channel);
}
