#include "OLED.h"
#include "PWMLED.h"
#include "LDR.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DEFP.h"

LDR myLDR(LDR_PIN);
PWMLED myLED(LED_PWM);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// void setup()
// {
//   myLDR.init();
//   myLED.init(0, 5000, 8);
// }

// void loop()
// {
//   //  int a = map(value, fromLow, fromHigh, toLow, toHigh);
//   float volatje = myLDR.readVoltage();
//   // float pwm = myLDR.readPWM();
//   myLED.ledPWM(myLDR.readPWM());
// }

OLED::OLED()
{
}

void OLED::begin()
{
    Serial.begin(115200);
    myLDR.init();
    myLED.init(LED_CHANNEL, FRECUENCY, RESOLUTION);
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Don't proceed, loop forever
    }
    display.clearDisplay();
}

void OLED::oledVoltage()
{

    float voltaje = myLDR.readVoltage();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 28);
    display.println("Voltaje : ");
    display.setCursor(32, 28);
    display.println(voltaje);
    Serial.print("Voltaje: ");
    Serial.println(voltaje);
    display.display();
}

void OLED::oledPWM()
{
    uint32_t pwmValue = myLDR.readPWM();
    myLED.ledPWM(pwmValue);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 56);
    display.println("Voltaje : ");
    display.setCursor(32, 56);
    display.println(pwmValue);
    Serial.print("PWM: ");
    Serial.println(pwmValue);
    display.display();
}
