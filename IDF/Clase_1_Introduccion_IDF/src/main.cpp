#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

int contador = 0;
extern "C" void app_main();
void app_main()
{

    printf("!Hola Mundo desde ESP-IDF con C++!");
    while (true)
    {
        printf("Iteracion: %d\n", contador);
        contador++;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}