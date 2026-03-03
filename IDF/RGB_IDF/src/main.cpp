#include <RGB.h>
#include <driver/gpio.h>
#include <driver/ledc.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <math.h>
#include <stdio.h>

extern "C" void app_main();

void app_main()
{

  constexpr gpio_num_t RED = GPIO_NUM_13;
  constexpr gpio_num_t GREEN = GPIO_NUM_12;
  constexpr gpio_num_t BLUE = GPIO_NUM_14;
  RGB ledRGB(RED, GREEN, BLUE);

  while (true)
  {

    for (int i = 0; i < 15; i++)
    {
      /* code */

      ledRGB.seleColor(RGB::BLUE);
      vTaskDelay(pdMS_TO_TICKS(250));
      ledRGB.seleColor(RGB::GREEN);
      vTaskDelay(pdMS_TO_TICKS(250));
      ledRGB.seleColor(RGB::RED);
      vTaskDelay(pdMS_TO_TICKS(250));
      ledRGB.seleColor(RGB::VIOLET);
      vTaskDelay(pdMS_TO_TICKS(250));
      ledRGB.seleColor(RGB::PINK);
      vTaskDelay(pdMS_TO_TICKS(250));
      ledRGB.seleColor(RGB::ORANGE);
      vTaskDelay(pdMS_TO_TICKS(250));
      // vTaskDelay(pdMS_TO_TICKS(5000));
      ledRGB.ledOff();
      vTaskDelay(pdMS_TO_TICKS(200));
    }
  }
}

// RED = 0,
// GREEN,
// BLUE,
// PINK,
// VIOLET,
// ORANGE,
// YELLOW,
// GRIS
