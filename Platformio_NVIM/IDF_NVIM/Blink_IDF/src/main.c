#include "driver/gpio.h"
#include "freertos/projdefs.h"
#include "hal/gpio_types.h"
#include "soc/gpio_num.h"
#include "stdbool.h"
#include <driver/ledc.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define LED GPIO_NUM_14

void app_main() {

  while (true) {

    gpio_config_t gpioOut = {.pin_bit_mask = (1ULL << LED),
                             .mode = GPIO_MODE_OUTPUT,
                             .pull_down_en = GPIO_PULLDOWN_DISABLE,
                             .pull_up_en = GPIO_PULLUP_DISABLE,
                             .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&gpioOut);

    gpio_set_level(LED, 1);
    vTaskDelay(pdMS_TO_TICKS(1000));
    gpio_set_level(LED, 0);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
