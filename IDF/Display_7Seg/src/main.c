#include <stdio.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const gpio_num_t diplay_pins[] = {21, 22, 23, 19, 18, 5, 2};

const uint8_t numeros_catodo[10][7] = {
    {1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 0, 0, 0, 0},
    {1, 1, 0, 1, 1, 0, 1},
    {1, 1, 1, 1, 0, 0, 1},
    {0, 1, 1, 0, 0, 1, 1},
    {1, 0, 1, 1, 0, 1, 1},
    {0, 0, 1, 1, 1, 1, 1},
    {1, 1, 1, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 0, 1, 1}

};
const uint8_t numeros_anodo[10][7] = {
    {0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 1, 1, 1, 1},
    {0, 0, 1, 0, 0, 1, 0},
    {0, 0, 0, 0, 1, 1, 0},
    {1, 0, 0, 1, 1, 0, 0},
    {0, 1, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0}

};

static void showDigit(int digit)
{
    if (digit < 0 || digit > 9)
        return;
    for (int i = 0; i < 7; i++)
    {
        gpio_set_level(diplay_pins[i], numeros_catodo[digit][i]);
    }
}

static void hw()
{
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << diplay_pins[0]) |
                        (1ULL << diplay_pins[1]) |
                        (1ULL << diplay_pins[2]) |
                        (1ULL << diplay_pins[3]) |
                        (1ULL << diplay_pins[4]) |
                        (1ULL << diplay_pins[5]) |
                        (1ULL << diplay_pins[6]),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE};
    gpio_config(&io_conf);
}
void app_main()
{
    hw();
    while (true)
    {
        for (int i = 0; i <= 9; i++)
        {
            showDigit(i);
            printf("Digito %d\n", i);
            vTaskDelay(pdMS_TO_TICKS(250));
        }
    }
}