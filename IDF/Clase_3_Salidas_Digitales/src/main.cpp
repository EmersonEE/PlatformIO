#include <stdio.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

extern "C" void app_main();

void app_main()
{
    const gpio_num_t LED = GPIO_NUM_27;
    esp_err_t error;
    error = gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    if (error != ESP_OK)
    {
        printf("Error Al Configurar el GPIO como salida\n");
        return;
    }

    while (true)
    {
        error = gpio_set_level(LED, 1);
        if (error != ESP_OK)
        {
            printf("Error al encender el LED\n");
        }
        printf("LED ENCENIDO\n");
        vTaskDelay(pdMS_TO_TICKS(500));

        error = gpio_set_level(LED, 0);
        if (error != ESP_OK)
        {
            printf("Error al Apagar el LED\n");
        }
        printf("LED APAGADO\n");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}