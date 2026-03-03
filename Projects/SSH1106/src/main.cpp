#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "image.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUM_OUTPUTS 5
#define NUM_INPUTS 4

uint8_t outputPins[NUM_OUTPUTS] = {19, 18, 4, 25, 26};
uint8_t inputPins[NUM_INPUTS] = {36, 39, 34, 35};

uint8_t inputState[NUM_INPUTS];

typedef enum
{
  ALL_LOW,
  LEVEL_1,
  LEVEL_2,
  LEVEL_3,
  LEVEL_4,
  ERROR_STATE
} comparison_result_t;

comparison_result_t lastResult = ERROR_STATE;

void configurePins()
{
  for (int i = 0; i < NUM_INPUTS; i++)
  {
    pinMode(inputPins[i], INPUT); // si son 34–39
  }

  for (int i = 0; i < NUM_OUTPUTS; i++)
  {
    pinMode(outputPins[i], OUTPUT);
    digitalWrite(outputPins[i], LOW);
  }
}

void readInputs()
{
  for (int i = 0; i < NUM_INPUTS; i++)
  {
    inputState[i] = digitalRead(inputPins[i]);
  }
}

comparison_result_t compareInputs()
{
  bool s0 = (inputState[0] == LOW);
  bool s1 = (inputState[1] == LOW);
  bool s2 = (inputState[2] == LOW);
  bool s3 = (inputState[3] == LOW);

  if (!s0 && !s1 && !s2 && !s3)
    return ALL_LOW;

  if (s0 && !s1 && !s2 && !s3)
    return LEVEL_1;

  if (s0 && s1 && !s2 && !s3)
    return LEVEL_2;

  if (s0 && s1 && s2 && !s3)
    return LEVEL_3;

  if (s0 && s1 && s2 && s3)
    return LEVEL_4;

  return ERROR_STATE;
}

void setOutputs(uint8_t s1, uint8_t s2, uint8_t s3, uint8_t s4, uint8_t s5)
{
  uint8_t states[NUM_OUTPUTS] = {s1, s2, s3, s4, s5};

  for (int i = 0; i < NUM_OUTPUTS; i++)
  {
    digitalWrite(outputPins[i], states[i]);
  }
}
void titleProject()
{
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(32, 5);
  display.print("WATER");
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(32, 25);
  display.print("LEVEL");

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(2, 45);
  display.print("CONTROLLER");
  display.display();
}
void drawPercentage(uint8_t percent)
{
  display.clearDisplay();

  char buffer[6];
  sprintf(buffer, "%d%%", percent);

  // Título
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(45, 5);
  display.print("Nivel");

  display.setTextSize(4);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(buffer, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 28);
  display.print(buffer);

  display.display();
}

void drawError()
{
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(28, 20);
  display.print("ERROR");

  display.setTextSize(1);
  display.setCursor(15, 50);
  display.print("Secuencia invalida");

  display.display();
}

void setup()
{
  Serial.begin(115200);
  for (int i = 0; i < NUM_INPUTS; i++)
  {
    inputState[i] = HIGH;
  }

  configurePins();
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("Error iniciando OLED");
    while (true)
      ;
  }

  display.clearDisplay();

  // Splash screen
  display.drawBitmap(0, 0, epd_bitmap_9109fc31_1267_4d6c_af11_5651855e302d, 128, 64, SSD1306_WHITE);
  display.display();

  delay(3000);
  titleProject();
  delay(3000);
  Serial.println("Sistema iniciado");
}

void loop()
{
  readInputs();
  comparison_result_t result = compareInputs();

  if (result != lastResult)
  {
    lastResult = result;

    switch (result)
    {
    case ALL_LOW:
      Serial.println("Nivel 0%");
      setOutputs(1, 0, 0, 0, 0);
      drawPercentage(0);
      break;

    case LEVEL_1:
      Serial.println("Nivel 25%");
      setOutputs(0, 1, 0, 0, 0);
      drawPercentage(25);
      break;

    case LEVEL_2:
      Serial.println("Nivel 50%");
      setOutputs(0, 1, 1, 0, 0);
      drawPercentage(50);
      break;

    case LEVEL_3:
      Serial.println("Nivel 75%");
      setOutputs(0, 1, 1, 1, 0);
      drawPercentage(75);
      break;

    case LEVEL_4:
      Serial.println("Nivel 100%");
      setOutputs(0, 1, 1, 1, 1);
      drawPercentage(100);
      break;

    case ERROR_STATE:
      Serial.println("ERROR DE SECUENCIA");
      setOutputs(0, 0, 0, 0, 0);
      drawError();
      break;
    }
  }

  delay(100);
}
