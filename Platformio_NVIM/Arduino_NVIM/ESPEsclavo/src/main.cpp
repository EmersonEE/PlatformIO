#include "HardwareSerial.h"
#include "esp32-hal-gpio.h"
#include "esp32-hal.h"
#include <Arduino.h>
#include <cstdint>
#include <cstdio>

uint8_t pinEntrada = 34;
int buttonState;
int lastButtonState = LOW;
int contador = 0;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 10;
void setup() {
  Serial.begin(115200);
  pinMode(pinEntrada, INPUT);
}

void loop() {
  int reading = digitalRead(pinEntrada);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {

    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        // Serial.println("Detecto EL Opto");
        contador++;
      } else {
        // Serial.println("No Detecto El Opto");
      }
    }
  }
  lastButtonState = reading;
  Serial.print("contador de Tanques: ");
  Serial.println(contador);
  // printf("Hola chundo");
}
