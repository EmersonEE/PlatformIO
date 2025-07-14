#include <stdio.h>
#include <iostream>
#include <driver/adc.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_err.h>
#include <esp_log.h>

extern "C" void app_main();
float map_float(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void app_main()
{
    static const char *TAG = "ADC_ERRO_HANDLER";
    esp_err_t error;
    // Congigurar ancho de ADC (12bits=)

    error = adc1_config_width(ADC_WIDTH_BIT_12);
    if (error != ESP_OK)
    {
        ESP_LOGE(TAG, "Error al configurar el ancho del ADC: %s", esp_err_to_name(error));
        return;
    }
    ESP_LOGI(TAG, "Ancho del ADC Configurado Exitosamente");

    // Configurar atenuacion(rango completo)
    error = adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_12);
    if (error != ESP_OK)
    {
        ESP_LOGE(TAG, "Error al Configurar la Atenuacion del ADC: %s", esp_err_to_name(error));
        return;
    }
    ESP_LOGI(TAG, "Atenuacion configurada exitosamente");

    int valorAnterior = 0;

    while (true)
    {
        // statement
        int valor = adc1_get_raw(ADC1_CHANNEL_5);
        if (valor != valorAnterior)
        {

            printf("Valor del ADC: %d\n", valor);
        }
        valorAnterior = valor;
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}