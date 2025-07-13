#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main(void)
{
    // Definición de pines
    constexpr gpio_num_t BOTON_PIN = GPIO_NUM_34;
    constexpr gpio_num_t LED_PIN = GPIO_NUM_27;
    // Variables de estado
    uint8_t estadoBotonAnterior = 0;
    uint8_t estadoLED = 0; // Apagado inicialmente
    esp_err_t error;

    // Configuración del botón (entrada con pull-down)
    error = gpio_set_direction(BOTON_PIN, GPIO_MODE_INPUT);
    if (error != ESP_OK)
    {
        printf("Error al configurar la dirección del GPIO\n");
        return;
    }
    gpio_set_pull_mode(BOTON_PIN, GPIO_PULLDOWN_ONLY);

    // Configuración del LED (salida)
    error = gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    if (error != ESP_OK)
    {
        printf("Error al configurar la dirección del GPIO\n");
        return;
    }

    while (true)
    {
        int estadoBotonActual = gpio_get_level(BOTON_PIN);
        // Detección de flanco ascendente (botón presionado)
        if (estadoBotonActual == 1 && estadoBotonAnterior == 0)
        {
            vTaskDelay(pdMS_TO_TICKS(20)); // Antirrebote suave
            estadoLED = !estadoLED;        // Alternar estado del LED
            gpio_set_level(LED_PIN, estadoLED);
            printf("Estado del LED: %s\n", estadoLED ? "ENCENDIDO" : "APAGADO");
        }
        // Actualizar estado anterior del botón
        estadoBotonAnterior = estadoBotonActual;
        // Pequeño retardo para evitar sobrecarga del procesador
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}