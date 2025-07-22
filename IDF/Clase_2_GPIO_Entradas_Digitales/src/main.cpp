#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
// Manejando Errores
extern "C" void app_main();

void app_main()
{

  const gpio_num_t boton = GPIO_NUM_34;
  esp_err_t error;
  error = gpio_set_direction(boton, GPIO_MODE_INPUT);
  if (error != ESP_OK)
  {
    printf("Error al configurar la direccion del GPIO\n");
    return;
  }
  else
  {
    printf("Todo Correcto ");
  }

  error = gpio_set_pull_mode(boton, GPIO_PULLDOWN_ONLY);
  if (error != ESP_OK)
  {
    printf("Error al configurar el pull mode del GPIO\n");
    return;
  }
  else
  {
    printf("Todo Correcto");
  }
  int estadoAnterior = 0;
  while (true)
  {
    int valor = gpio_get_level(boton);
    if (valor == 1 && estadoAnterior != valor)
    {
      printf("Boton Presionado\n");
      estadoAnterior = valor;
    }
    else if (valor == 0 && estadoAnterior != valor)
    {
      printf("Boton sin Presionar\n");
      estadoAnterior = valor;
      vTaskDelay(pdMS_TO_TICKS(200));
    }
  }
}
