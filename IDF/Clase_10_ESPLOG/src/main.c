#include <stdio.h>
#include <driver/gpio.h>
#include <esp_err.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
static const char *TAG = "My_APP";
void app_main()
{

    esp_log_level_set(TAG, ESP_LOG_DEBUG);

    ESP_LOGI(TAG, "INICIANDO APlicacion ESP32");
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << GPIO_NUM_12),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE};
    esp_err_t error = gpio_config(&io_conf);
    if (error != ESP_OK)
    {
        ESP_LOGE(TAG, "Error al configurar GPIO: %s", esp_err_to_name(error));
        return;
    }
    ESP_LOGI(TAG, "GPIO Configurado Correctamente");

    while (true)
    {
        error = gpio_set_level(GPIO_NUM_12, 1);
        if (error != ESP_OK)
        {
            ESP_LOGE(TAG, "Error al establecer nivel alto: %s", esp_err_to_name(error));
            return;
        }
        ESP_LOGD(TAG, "GPIO 2 en alto");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        error = gpio_set_level(GPIO_NUM_12, 0);
        if (error != ESP_OK)
        {
            ESP_LOGE(TAG, "Error al establecer nivel bajo: %s", esp_err_to_name(error));
            return;
        }
        ESP_LOGD(TAG, "GPIO 2 en bajo");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}