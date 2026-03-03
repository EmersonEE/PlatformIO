#include "esp_attr.h"
#include "hal/gpio_types.h"
#include "portmacro.h"
#include "reent.h"
#include "stdbool.h"
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stddef.h>

#define GPIO_LED 13
#define GPIO_LED_PIN_SEL (1ULL << GPIO_LED)
#define GPIO_BUTTON 36
#define GPIO_BUTTON_PIN_SEL (1ULL << GPIO_BUTTON)
#define ESP_INTR_FLAG_DEFAULT 0
static void button_handler(void *arg);
static void init_hw(void) {
  gpio_config_t io_conf;
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pin_bit_mask = GPIO_LED_PIN_SEL;
  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
  gpio_config(&io_conf);
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pin_bit_mask = GPIO_BUTTON_PIN_SEL;
  io_conf.intr_type = GPIO_INTR_NEGEDGE;
  io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
  gpio_config(&io_conf);

  gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
  gpio_isr_handler_add(GPIO_BUTTON, button_handler, NULL);
}
static TickType_t next = 0;
static bool led_state = false;

static void IRAM_ATTR button_handler(void *arg) {
  TickType_t now = xTaskGetTickCountFromISR();

  if (now > next) {
    led_state = !led_state;
    gpio_set_level(GPIO_LED, led_state);
    next = now + 500 / portTICK_PERIOD_MS;
  }
}

void app_main() {

  init_hw();
  vTaskSuspend(NULL);
}
