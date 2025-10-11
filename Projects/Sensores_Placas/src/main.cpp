#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ACS712.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_INA219 ina219;

ACS712 currentSensor(34, 3.3, 4095, 185);
int calibration_factor = 120;
// const int sensorIN = 34;
// int mVperAmp = 185;
// int watt = 0;
// double voltageACS = 0;
// double VRMS = 0;
// double ampsRMS = 0;

float sensibilidad = 0.185;
float I = 0.00;
float ajuste = 0.05;

void setup(void)
{
  Serial.begin(115200);
  currentSensor.autoMidPoint();
  while (!Serial)
  {
    delay(1);
  }

  Serial.println("Hello!");

  if (!ina219.begin())
  {
    Serial.println("Failed to find INA219 chip");
    while (1)
    {
      delay(10);
    }
  }
  Serial.println("Measuring voltage and current with INA219 ...");

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println("Hello, world!");
  display.display();
  display.clearDisplay();
}

// float getVPP()
// {
//   float result;
//   int readValue;
//   int maxValue = 0;
//   int minValue = 4096;
//   uint32_t start_time = millis();

//   while ((millis() - start_time) < 1000)
//   {
//     readValue = analogRead(sensorIN);
//     if (readValue > maxValue)
//     {
//       maxValue = readValue;
//     }
//     if (readValue < minValue)
//     {
//       minValue = readValue;
//     }
//   }
//   return result = ((maxValue - minValue) * 3.3) / 4096.0;
// }

float readCurrent()
{
  float average = 0;
  for (int i = 0; i < 100; i++)
  {
    average += currentSensor.mA_DC();
    delay(1);
  }
  float mA = (abs(average / 100.0));
  if (mA <= 5)
    mA = 0;

  // Print values for debugging
  Serial.print("Current: ");
  Serial.print(mA);
  Serial.println(" mA");

  return mA;
}
void loop(void)
{
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float power_mW = 0;
  float current = readCurrent();
  // voltageACS = getVPP();
  // VRMS = (voltageACS / 2.0) * 0.707;
  // ampsRMS = ((VRMS * 1000) / mVperAmp) - 0.3;
  // watt = (ampsRMS * 240 / 1.2);

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 5);
  display.println("Bus Voltage: ");
  display.setCursor(80, 5);
  display.println(busvoltage);
  display.setCursor(110, 5);
  display.println(" V");
  display.setCursor(0, 17);
  display.println("Shunt Voltg: ");
  display.setCursor(80, 17);
  display.println(shuntvoltage);
  display.setCursor(110, 17);
  display.println(" mV");
  display.setCursor(0, 29);
  display.println("Current: ");
  display.setCursor(80, 29);
  display.println(current_mA);
  display.setCursor(110, 29);
  display.println(" mA");
  display.setCursor(0, 41);
  display.println("Power: ");
  display.setCursor(80, 41);
  display.println(power_mW);
  display.setCursor(110, 41);
  display.println(" mW");
  display.setCursor(0, 53);
  display.println("Voltaje: ");
  display.setCursor(80, 53);
  display.println(current);
  display.setCursor(110, 53);
  display.println(" mA");
  display.display();
  display.clearDisplay();
  // Serial.print("Bus Voltage:   ");
  // Serial.print(busvoltage);
  // Serial.println(" V");
  // Serial.print("Shunt Voltage: ");
  // Serial.print(shuntvoltage);
  // Serial.println(" mV");
  // Serial.print("Load Voltage:  ");
  // Serial.print(loadvoltage);
  // Serial.println(" V");
  // Serial.print("Current:       ");
  // Serial.print(current_mA);
  // Serial.println(" mA");
  // Serial.print("Power:         ");
  // Serial.print(power_mW);
  // Serial.println(" mW");
  // Serial.println("");

  delay(1000);
}
