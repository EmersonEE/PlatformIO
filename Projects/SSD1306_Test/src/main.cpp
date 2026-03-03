#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "image.h"
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup()
{

  Serial.begin(115200); // Optional: for debugging

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED not found");
    while (true)
      ; // Stop if OLED not detected
  }

  display.clearDisplay();

  // Draw bitmap
  display.drawBitmap(
      0, 0,
      epd_bitmap_9109fc31_1267_4d6c_af11_5651855e302d,
      128, 64,
      WHITE);

  display.display();
}

void loop()
{
}
