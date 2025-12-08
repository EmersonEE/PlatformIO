
#include "freertos/projdefs.h"
#include "hal/ledc_types.h"
#include "soc/clk_tree_defs.h"
#include "soc/gpio_num.h"
#include <driver/ledc.h>
#include <esp_err.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdbool.h>
#include <stdio.h>

void app_main() {

  ledc_timer_config_t ledc_timer = {.speed_mode = LEDC_LOW_SPEED_MODE,
                                    .duty_resolution = LEDC_TIMER_13_BIT,
                                    .timer_num = LEDC_TIMER_0,
                                    .freq_hz = 5000,
                                    .clk_cfg = LEDC_AUTO_CLK};
  ledc_timer_config(&ledc_timer);

  ledc_channel_config_t ledc_channel = {.gpio_num = GPIO_NUM_26,
                                        .speed_mode = LEDC_LOW_SPEED_MODE,
                                        .channel = LEDC_CHANNEL_1,
                                        .timer_sel = LEDC_TIMER_0,
                                        .duty = 0};
  ledc_channel_config(&ledc_channel);

  int duty = 0;
  int delta = 10;

  while (true) {

    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    duty += delta;
    if (duty >= 8191 || duty <= 0) {

      delta = -delta;
    }
    vTaskDelay(pdMS_TO_TICKS(20));
  }
}
