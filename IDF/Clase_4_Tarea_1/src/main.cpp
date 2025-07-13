
#include <stdio.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

extern "C" void app_main();
void app_main()
{

    constexpr gpio_num_t LED = GPIO_NUM_27;
    constexpr gpio_num_t Boton = GPIO_NUM_34;

    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_direction(Boton, GPIO_MODE_INPUT);
    gpio_set_pull_mode(Boton, GPIO_PULLDOWN_ONLY);

    uint8_t estadoBotonAnterior = 0;
    uint8_t clickCount = 0;
    uint8_t estadoLED = 0;
    TickType_t ultimoClick = 0;
    bool botonFuePresionado = true;
    uint8_t accion = 0;
    while (true)
    {
        int estadoActualBoton = gpio_get_level(Boton);

        if (estadoActualBoton == 1 && estadoBotonAnterior == 0)
        {
            TickType_t ahora = xTaskGetTickCount();

            printf("Inicio %" PRIu32 "\n", ahora);
            botonFuePresionado = true;
            while (botonFuePresionado == true)
            {
                if (gpio_get_level(Boton) == 0)
                {
                    botonFuePresionado = false;
                    TickType_t final = xTaskGetTickCount();
                    printf("Final %" PRIu32 "\n", final);
                    uint32_t tiempo = final - ahora;
                    printf("Tiempo: %" PRIu32 "\n", tiempo);
                    if (tiempo < 100)
                    {
                        accion = 1;
                    }
                    if (tiempo >= 100)
                    {
                        accion = 2;
                    }
                    break;
                }
            }
            if (accion == 1)
            {
                estadoLED = !estadoLED;
                gpio_set_level(LED, estadoLED);
                printf("Estado del LED: %s\n", estadoLED ? "ENCENDIDO" : "APAGADO");
            }
            else if (accion == 2)
            {
                for (int i = 0; i < 8; i++)
                {
                    gpio_set_level(LED, 1);
                    vTaskDelay(pdMS_TO_TICKS(100));
                    gpio_set_level(LED, 0);
                    vTaskDelay(pdMS_TO_TICKS(100));
                }
                accion = 0;
                printf("Parpadeo rápido!\n");
            }
        }
        estadoBotonAnterior = estadoActualBoton;
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}