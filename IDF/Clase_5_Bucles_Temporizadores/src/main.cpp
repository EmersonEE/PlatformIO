#include <stdio.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_timer.h>

extern "C" void app_main();

void app_main()
{

    const gpio_num_t LED2 = GPIO_NUM_27;
    const gpio_num_t LED = GPIO_NUM_12;
    esp_err_t error;
    error = gpio_set_direction(LED2, GPIO_MODE_OUTPUT);
    if (error != ESP_OK)
    {
        printf("Error al configurar el led2 como salida\n");
    }
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    int estadoLED = 0;
    int64_t ultimoCambio = esp_timer_get_time();

    while (true)
    {

        // ✅ Ejemplo Avanzado: Temporización No Bloqueante con esp_timer_get_time()
        //  Este ejemplo hace parpadear un LED cada segundo sin usar vTaskDelay.
        //  Es útil cuando necesitas hacer varias cosas a la vez.
        int64_t ahora = esp_timer_get_time();

        if (ahora - ultimoCambio >= 1000000LL)
        { // 1 segundo en microsegundos
            estadoLED = !estadoLED;
            gpio_set_level(LED, estadoLED);
            printf("Estado del LED: %s\n", estadoLED ? "ENCENDIDO" : "APAGADO");
            ultimoCambio = ahora;
        }

        // Aca se pueden hacer otras cosas mientras se espera
        // Ejemplo Basico con bloqueo
        for (int i = 0; i < 10; i++)
        {
            printf("Hola MUNDO\n");
            gpio_set_level(LED2, 1);
            vTaskDelay(pdMS_TO_TICKS(500));
            gpio_set_level(LED2, 0);
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
}