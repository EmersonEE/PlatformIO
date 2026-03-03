#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Wire.h>

#define I2C_ADDRESS 0x3C
Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, &Wire, /* reset=*/-1);

void setup() {
  display.begin(I2C_ADDRESS, true); // true = OLED
  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("Hola desde PlatformIO!");
  display.println("SH1106 1.3\"");
  display.display();
  delay(1500);
}
