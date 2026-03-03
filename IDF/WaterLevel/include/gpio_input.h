#ifndef gpio_input__
#define gpio_input__
#include "soc/gpio_num.h"
#include <cstdint>
#include <driver/gpio.h>
#include <esp_err.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <sys/types.h>
typedef struct {
  gpio_num_t pin;
  uint8_t state;
  uint8_t prev_state;
  gpio_config_t config;

} digital_input_t;
class gpio_input {
public:
private:
};

#endif // !gpio_input__
