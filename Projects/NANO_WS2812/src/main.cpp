#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <Adafruit_NeoPixel.h>
#include "Tiras.h"
#include "Boton.h"

#define BOTON_T1 A0
#define BOTON_T2 A1
#define TIME 500

SemaphoreHandle_t xSerialSemaphore;

void TaskBoton1(void *pvParameters);
void TaskBoton2(void *pvParameters);

Boton botonT1(BOTON_T1);
Boton botonT2(BOTON_T2);
void setup()
{
  botonT1.begin();
  botonT1.offStrip();
  botonT2.begin();
  botonT2.offStrip();
  Serial.begin(115200);

  if (xSerialSemaphore == NULL)
  {
    xSerialSemaphore = xSemaphoreCreateMutex();
    if ((xSerialSemaphore) != NULL)
      xSemaphoreGive((xSerialSemaphore));
  }

  xTaskCreate(
      TaskBoton1,
      "Boton1",
      128,
      NULL,
      1,
      NULL);
  xTaskCreate(
      TaskBoton2,
      "Boton2",
      128,
      NULL,
      1,
      NULL);
}

void loop()
{
}

void TaskBoton1(void *pvParameters __attribute__((unused)))
{
  while (true)
  {

    botonT1.onStrip(0, 255, 0, TIME);
  }
  vTaskDelay(pdMS_TO_TICKS(10));
}

void TaskBoton2(void *pvParameters __attribute__((unused)))
{
  while (true)
  {
    botonT2.onDoubleStrip(0, 255, 0, TIME);
  }
  vTaskDelay(pdMS_TO_TICKS(10));
}