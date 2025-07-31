#include <stdio.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void app_main()
{

    gpio_num_t LED_27 = GPIO_NUM_27;
    gpio_num_t LED_14 = GPIO_NUM_14;
    gpio_num_t LED_12 = GPIO_NUM_12;
    // CONTROLAR SOLO UN PIN
    // gpio_config_t io_conf = {
    //     .pin_bit_mask = (1ULL << LED_27),
    //     .mode = GPIO_MODE_OUTPUT,
    //     .pull_down_en = GPIO_PULLDOWN_DISABLE,
    //     .pull_up_en = GPIO_PULLUP_DISABLE,
    //     .intr_type = GPIO_INTR_DISABLE};
    // gpio_config(&io_conf);

    // while (true)
    // {
    //     gpio_set_level(LED_27, 1);
    //     vTaskDelay(pdMS_TO_TICKS(500));
    //     gpio_set_level(LED_27, 0);
    //     vTaskDelay(pdMS_TO_TICKS(500));
    // }

    // CONTROLAR VARIOS PINES

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_27) | (1ULL << LED_14) | (1ULL << LED_12),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&io_conf);
    while (true)
    {
        gpio_set_level(LED_27, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_set_level(LED_14, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_set_level(LED_12, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_set_level(LED_12, 0);
        gpio_set_level(LED_14, 0);
        gpio_set_level(LED_27, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}