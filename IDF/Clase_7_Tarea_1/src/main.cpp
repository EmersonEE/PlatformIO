#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/ledc.h>
#include <driver/adc.h>
#include <esp_err.h>
#include <esp_log.h>

extern "C" void app_main();

void app_main()
{
    static const char *TAG = "ERROR_PWM";
    static const char *TAG_2 = "ERROR_ADC";

    esp_err_t error;

    // Configurando PWM

    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_12_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK};
    error = ledc_timer_config(&ledc_timer);
    if (error != ESP_OK)
    {
        ESP_LOGE(TAG, "Error al configurar el timer del PWM: %s", esp_err_to_name(error));
        return;
    }
    ESP_LOGI(TAG, "Timer configurado Perfectamente");

    ledc_channel_config_t ledc_channel = {
        .gpio_num = GPIO_NUM_12,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0};
    error = ledc_channel_config(&ledc_channel);
    if (error != ESP_OK)
    {
        ESP_LOGE(TAG, "Error al configurar el Channel del PWM: %s", esp_err_to_name(error));
        return;
    }
    ESP_LOGI(TAG, "Channel configurado Perfectamente");

    // Configurando ADC
    error = adc1_config_width(ADC_WIDTH_BIT_12);

    error = adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_12);
    int valorAnterior = 0;
    while (true)
    {

        int duty = adc1_get_raw(ADC1_CHANNEL_5);
        if (duty != valorAnterior)
        {

            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            printf("Valor del Duty: %d\n", duty);
        }
        valorAnterior = duty;
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}