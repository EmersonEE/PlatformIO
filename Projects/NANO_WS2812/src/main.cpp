#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <Adafruit_NeoPixel.h>
#include "Tiras.h"
#include "Boton.h"
#define LED_PIN 10
#define PIN_LED_T2 9
#define NUM_PIXELS_T2 60
#define NUM_PIXELS 60
#define BOTON_T1 A0
#define BOTON_T2 A1
#define TIME 500
SemaphoreHandle_t xSerialSemaphore;

void TaskBoton1(void *pvParameters);
void TaskBoton2(void *pvParameters);

Tiras tira1(NUM_PIXELS, LED_PIN);
Tiras tira2(NUM_PIXELS_T2, PIN_LED_T2);
Boton botonT1(BOTON_T1, tira1);
Boton botonT2(BOTON_T2, tira2);
void setup()
{
  tira1.init();
  tira2.init();
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

  // tira1.effectLED(0, 255, 0, 250);

  // tira2.effectLED(0, 255, 0, 500);
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

    botonT2.onStrip(0, 255, 0, TIME);
  }
  vTaskDelay(pdMS_TO_TICKS(10));
}