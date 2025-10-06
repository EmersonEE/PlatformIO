#include <driver/gpio.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdbool.h>
#include <stdio.h>

void app_main() {

  gpio_config_t io = {.intr_type = GPIO_INTR_DISABLE,
                      .mode = GPIO_MODE_OUTPUT,
                      .pin_bit_mask = (1ULL << 27),
                      .pull_down_en = GPIO_PULLDOWN_DISABLE,
                      .pull_up_en = GPIO_PULLUP_DISABLE};
  gpio_config(&io);
  while (true) {
  }
}
