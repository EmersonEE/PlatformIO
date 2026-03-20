#include "HardwareSerial.h"
#include "pins_arduino.h"
#include <Arduino.h>
#define FINAL_CARRERA 8
#define LDR 9
#define IN1 12
#define IN2 11
#define ENA 10
void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(FINAL_CARRERA, INPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(LDR, INPUT);
  Serial.begin(115200);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);
}

void loop() {

  //  if (digitalRead(FINAL_CARRERA) == 0) {
  //
  //    Serial.println("Final de Carrera Activado");
  //    digitalWrite(IN1, HIGH);
  //    digitalWrite(IN2, LOW);
  //    analogWrite(ENA, 150);
  //  } else {
  //    Serial.println("FInal de carrera apgado");
  //  }
  //  if (digitalRead(LDR) == 1) {
  //
  //    digitalWrite(IN1, LOW);
  //    digitalWrite(IN2, HIGH);
  //    analogWrite(ENA, 150);
  //    Serial.println("LDR Activado");
  //  } else {
  //    Serial.println("LDR NO|");
  //  }
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 250);
  delay(1500);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, 250);
  delay(1000);
}
