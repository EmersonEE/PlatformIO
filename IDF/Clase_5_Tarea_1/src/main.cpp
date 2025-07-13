#include <stdio.h>
#include <driver/gpio.h>
#include <esp_timer.h>
#include <inttypes.h> // Para los macros PRIuX
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
extern "C" void app_main();
void app_main()
{
    int contador = 0;
    int64_t ultimoCambio = esp_timer_get_time();

    while (true)
    {
        int64_t ahora = esp_timer_get_time();

        if (ahora - ultimoCambio >= 2000000LL)
        { // 1 segundo en microsegundos
            contador++;
            printf("Valor de Contador: %d\n", contador);
            ultimoCambio = ahora;
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}