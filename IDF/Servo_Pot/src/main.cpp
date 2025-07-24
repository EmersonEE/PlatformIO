#include <stdio.h>
#include <inttypes.h>
#include <driver/adc.h>
#include <driver/ledc.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "ServoIDF.h"
extern "C" void app_main();

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void app_main()
{
    Servo servo(LEDC_CHANNEL_0, GPIO_NUM_18);
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11);
    servo.init();

    int valorAnterior = -1;

    while (true)
    {
        int valor_adc = adc1_get_raw(ADC1_CHANNEL_5);
        int angulo = map(valor_adc, 0, 4095, 0, 180); // Ángulo de 0° a 180°

        // if (angulo != valorAnterior)
        // {
        //     servo.setAngle(angulo);
        //     printf("Angulo: %d\n",angulo);
        //     valorAnterior = angulo;
        // }
        // vTaskDelay(pdMS_TO_TICKS(50));
        servo.setAngle(0);
        vTaskDelay(pdMS_TO_TICKS(250));
        servo.setAngle(45);
        vTaskDelay(pdMS_TO_TICKS(250));
        servo.setAngle(90);
        vTaskDelay(pdMS_TO_TICKS(250));
        servo.setAngle(135);
        vTaskDelay(pdMS_TO_TICKS(250));
        servo.setAngle(180);
        vTaskDelay(pdMS_TO_TICKS(250));
           servo.setAngle(135);
        vTaskDelay(pdMS_TO_TICKS(250));
        servo.setAngle(90);
        vTaskDelay(pdMS_TO_TICKS(250));
        servo.setAngle(45);
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}
