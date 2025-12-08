#include <stdio.h>
#include <driver/ledc.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_err.h>
#include <esp_log.h>

extern "C" void app_main();
void app_main()
{
    static const char *TAG = "PWM_ERROR_HANDLER";
    esp_err_t error;
    // Configurando el timer
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_12_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK};
    error = ledc_timer_config(&ledc_timer);
    if (error != ESP_OK)
    {
        ESP_LOGE(TAG, "Error al configurar el timer LEDC: %s", esp_err_to_name(error));
        return;
    }
    ESP_LOGI(TAG, "Timer LEDC configurado exitosamente");

    ledc_channel_config_t ledc_channel = {
        .gpio_num = GPIO_NUM_26,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint =0
    };
    error = ledc_channel_config(&ledc_channel);
    if (error != ESP_OK)
    {
        ESP_LOGE(TAG, "Error al configurar el timer LEDC: %s", esp_err_to_name(error));
        return;
    }
    ESP_LOGI(TAG, "Timer LEDC configurado exitosamente");
    int duty = 0;
    int delta = 10;
    while (true)
    {
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        duty += delta;
        if (duty >= 8191 || duty <= 0)
        {
            delta = -delta;
        }
        printf("Valor del Duty: %d\n", duty);
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}