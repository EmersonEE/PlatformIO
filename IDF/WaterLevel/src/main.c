#include "esp_log_level.h"
#include "freertos/projdefs.h"
#include "hal/gpio_types.h"
#include "soc/gpio_num.h"
#include "stdbool.h"
#include "sys/types.h"
#include <driver/gpio.h>
#include <esp_err.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <stdint.h>
#include <stdio.h>

static const char *TAG = "GPIO_INPUT";
static const char *TAGO = "GPIO_OUTPUT";

typedef struct {
  gpio_num_t pin;
  uint8_t state;
  gpio_config_t config;
} digital_output_t;
#define NUM_OUTPUTS 5
digital_output_t outputs[NUM_OUTPUTS] = {
    {GPIO_NUM_4, 0, {0}},  {GPIO_NUM_18, 0, {0}}, {GPIO_NUM_19, 0, {0}},
    {GPIO_NUM_23, 0, {0}}, {GPIO_NUM_25, 0, {0}},
};
esp_err_t configure_outpus(void) {
  gpio_config_t ou_conf = {.intr_type = GPIO_INTR_DISABLE,
                           .mode = GPIO_MODE_OUTPUT,
                           .pin_bit_mask =
                               (1ULL << GPIO_NUM_4) | (1ULL << GPIO_NUM_18) |
                               (1ULL << GPIO_NUM_19) | (1ULL << GPIO_NUM_23) |
                               (1ULL << GPIO_NUM_25),
                           .pull_down_en = 0,
                           .pull_up_en = 0};
  esp_err_t ret = gpio_config(&ou_conf);
  if (ret != ESP_OK) {
    ESP_LOGE(TAGO, "Error configurando GPIOs: %s", esp_err_to_name(ret));
  } else {
    ESP_LOGI(TAGO, "GPIOs configurados correctamente");
  }
  return ret;
}
typedef struct {
  gpio_num_t pin;
  uint8_t state;
  uint8_t prev_state;
  gpio_config_t config;

} digital_input_t;
#define NUM_IMPUTS 4

digital_input_t inputs[NUM_IMPUTS] = {
    {GPIO_NUM_34, 0, 0, {0}},

    {GPIO_NUM_35, 0, 0, {0}},

    {GPIO_NUM_36, 0, 0, {0}},

    {GPIO_NUM_39, 0, 0, {0}},
};

esp_err_t configure_inputs(void) {
  gpio_config_t io_conf = {.intr_type = GPIO_INTR_DISABLE,
                           .mode = GPIO_MODE_INPUT,
                           .pin_bit_mask =
                               (1ULL << GPIO_NUM_34) | (1ULL << GPIO_NUM_35) |
                               (1ULL << GPIO_NUM_36) | (1ULL << GPIO_NUM_39),
                           .pull_down_en = GPIO_PULLDOWN_DISABLE,
                           .pull_up_en = GPIO_PULLUP_DISABLE};
  esp_err_t ret = gpio_config(&io_conf);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Error al configurar GPIOs: %s", esp_err_to_name(ret));
  }
  return ret;
}

void read_inputs(void) {
  for (int i = 0; i < NUM_IMPUTS; i++) {
    inputs[i].prev_state = inputs[i].state;
    inputs[i].state = gpio_get_level(inputs[i].pin);
    if (inputs[i].state != inputs[i].prev_state) {
      ESP_LOGI(TAG, "Cambio en pin %d: de %d", inputs[i].pin,
               inputs[i].prev_state);
    }
  }
}
/*
void set_state_gpio(uint8_t state_1, uint8_t state_2, uint8_t state_3,
                    uint8_t state_4, uint8_t state_5) {
  gpio_set_level(outputs[0].pin, state_1);
  gpio_set_level(outputs[1].pin, state_2);
  gpio_set_level(outputs[2].pin, state_3);
  gpio_set_level(outputs[3].pin, state_4);
  gpio_set_level(outputs[4].pin, state_5);
}*/
void set_state_gpio(uint8_t state_1, uint8_t state_2, uint8_t state_3,
                    uint8_t state_4, uint8_t state_5) {
  uint8_t states[] = {state_1, state_2, state_3, state_4, state_5};
  for (int i = 0; i < NUM_OUTPUTS; i++) {
    gpio_set_level(outputs[i].pin, states[i]);
    outputs[i].state = states[i];
  }
  ESP_LOGI(TAGO,
           " Estado de los pines: Led1: %d, Led2: %d, Led3: %d, Led4: %d, "
           "Led5: %d",
           outputs[0].state, outputs[1].state, outputs[2].state,
           outputs[3].state, outputs[4].state);
}
typedef enum {
  ALL_LOW,
  ALL_HIGH,
  MIXED,
  ERROR,
  LEVEL_1,
  LEVEL_2,
  LEVEL_3,
  LEVEL_4
} comparison_result_t;

comparison_result_t compare_inputs(void) {
  uint8_t count_high = 0;

  for (int i = 0; i < NUM_IMPUTS; i++) {
    if (inputs[i].state == 0) {
      count_high++;
    }
    if (gpio_get_level(inputs[i].pin) != inputs[i].state) {
      return ERROR;
    }
  }
  if (count_high == 0)
    return ALL_LOW;
  // if (count_high == NUM_IMPUTS)
  //  return ALL_HIGH;
  if (count_high == 1)
    return LEVEL_1;
  if (count_high == 2)
    return LEVEL_2;
  if (count_high == 3)
    return LEVEL_3;
  if (count_high == 4)
    return LEVEL_4;
  return MIXED;
}
void input_task(void *pvParameters) {

  configure_inputs();
  configure_outpus();
  while (true) {
    read_inputs();
    comparison_result_t result = compare_inputs();
    switch (result) {
    case ALL_LOW:
      ESP_LOGI(TAG, "Todos los inputs bajos: estado idle");
      set_state_gpio(1, 0, 0, 0, 0);
      break;
      // case ALL_HIGH:
      //   ESP_LOGI(TAG, "Todos los inputs altos");
      //   break;
    case MIXED:
      ESP_LOGI(TAG, "Inputs mixtos");
      break;
    case ERROR:
      ESP_LOGI(TAG, "Error en comparcion de inputs");
      break;
    case LEVEL_1:
      ESP_LOGI(TAG, "Nivel 1");
      set_state_gpio(0, 1, 0, 0, 0);
      break;
    case LEVEL_2:
      ESP_LOGI(TAG, "Nivel 2");
      set_state_gpio(0, 1, 1, 0, 0);
      break;
    case LEVEL_3:
      ESP_LOGI(TAG, "Nivel 3");
      set_state_gpio(0, 1, 1, 1, 0);
      break;
    case LEVEL_4:
      ESP_LOGI(TAG, "Nivel 4");
      set_state_gpio(0, 1, 1, 1, 1);
      break;
    default:
      ESP_LOGI(TAG, "No hay nada que comparar");
    }
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}
void app_main() {

  ESP_LOGI(TAG, "Iniciando aplicacion de lectura de inputs digitales");

  xTaskCreate(input_task, "Inputs Task", 2048, NULL, 5, NULL);
  while (true) {
    vTaskDelay(pdMS_TO_TICKS(1000));
    ESP_LOGI(TAG, "Main Loop Activo");
  }
}
