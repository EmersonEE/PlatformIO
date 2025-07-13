#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main(void)
{
    const gpio_num_t BOTON_PIN = GPIO_NUM_34;
    const gpio_num_t LED_PIN = GPIO_NUM_27;

    gpio_set_direction(BOTON_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BOTON_PIN, GPIO_PULLDOWN_ONLY);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    int estadoBotonAnterior = 0;
    int clickCount = 0;
    uint8_t estadoLED =0;
    TickType_t ultimoClick = 0;

    while (true)
    {
        int estadoBotonActual = gpio_get_level(BOTON_PIN);

        // Detectar flanco ascendente (presionar botón)
        if (estadoBotonActual == 1 && estadoBotonAnterior == 0)
        {
            TickType_t ahora = xTaskGetTickCount();
            if (ahora - ultimoClick < pdMS_TO_TICKS(600))
            {
                clickCount++;
            }
            else
            {
                clickCount = 1;
            }
            ultimoClick = ahora;

            printf("Click #%d\n", clickCount);

            // Lógica condicional para detectar cantidad de clicks
            if (clickCount == 1)
            {
                // Un solo clic: alternar LED
                estadoLED = !estadoLED; // Alternar estado del LED
                gpio_set_level(LED_PIN, estadoLED);
                printf("Estado del LED: %s\n", estadoLED ? "ENCENDIDO" : "APAGADO");
            }
            else if (clickCount == 2)
            {
                // Doble clic: parpadeo rápido 3 veces
                for (int i = 0; i < 8; i++)
                {
                    gpio_set_level(LED_PIN, 1);
                    vTaskDelay(pdMS_TO_TICKS(100));
                    gpio_set_level(LED_PIN, 0);
                    vTaskDelay(pdMS_TO_TICKS(100));
                }
                printf("Parpadeo rápido!\n");
            }
            else if (clickCount >= 3)
            {
                // Tres clics: parpadeo continuo
                printf("Modo parpadeo continuo...\n");
                while (true)
                {
                    gpio_set_level(LED_PIN, 1);
                    vTaskDelay(pdMS_TO_TICKS(200));
                    gpio_set_level(LED_PIN, 0);
                    vTaskDelay(pdMS_TO_TICKS(200));
                    // Salir si hay otro clic
                    if (gpio_get_level(BOTON_PIN))
                    {
                        gpio_set_level(LED_PIN, 0);
                        printf("Modo parpadeo terminado.\n");
                        clickCount = 0;
                        break;
                    }
                }
            }
        }

        estadoBotonAnterior = estadoBotonActual;
        vTaskDelay(pdMS_TO_TICKS(50)); // Evitar saturación del CPU
    }
}