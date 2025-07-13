#include "HardwareSerial.h"
#include <Arduino.h>
// #define A0 = A2;

int valor = 0;
int pwmValue = 0;
float voltage = 0;

void setup()
{
  Serial.begin(115200);
  pinMode(A2, INPUT);
}

float voltaje(int valor, float voltajeReferencia = 5.0)
{
  return valor * (5.0 / 1023.0);
}

uint8_t returnPWM(uint32_t sensorRead, uint8_t rangeValue = 255)
{
  return map(sensorRead, 0, 1023, 0, rangeValue);
}

void loop()
{
  voltage = voltaje(valor);
  pwmValue = returnPWM(analogRead(A2));
  Serial.print("PWM Obtenido: ");
  Serial.println(pwmValue);
  Serial.print("Voltage Leido : ");
  Serial.println(voltage);
  Serial.print("Valor Leido : ");
  Serial.println(valor);
  delay(10);
}
