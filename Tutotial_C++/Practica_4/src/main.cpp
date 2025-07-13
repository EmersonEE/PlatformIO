#include "HardwareSerial.h"
#include "pins_arduino.h"
#include <Arduino.h>

// Condicionales y buvles

void condicionalIF(int condi);
void cicloFOR(int conteo);
void condicionalSWITCH(int modo);
void tarea(unsigned long time, int pinLED, int numeroParpade, int sensorReas);
int sensor = A0;
void cicloWHILE(int comparar, int contador);
void setup() {
  pinMode(sensor, OUTPUT);
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
}
void loop() {
  condicionalIF(30);
  condicionalSWITCH(2);
  cicloFOR(5);
  cicloWHILE(0, 10);
  tarea(100, LED_BUILTIN, 3, sensor);
}

void condicionalIF(int condi) {
  if (condi < 30) {
    Serial.println("Hace Calor");
  } else if (condi < 15) {
    Serial.println("Hace Frio");

  } else {
    Serial.println("Temperatura Normal");
  }
}

void condicionalSWITCH(int modo) {

  switch (modo) {
  case 1:
    Serial.println("Modo Manual");
    break;
  case 2:
    Serial.println("Modo Automatico");
    break;
  case 3:
    Serial.println("Modo Configuracion");
    break;
  default:
    Serial.println("Modo Desconocido");
  }
}
void cicloFOR(int conteo) {
  for (int i = 0; i < conteo; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
  }
}
void cicloWHILE(int comparar, int contador) {
  while (contador < 10) {
    Serial.println(contador);
    contador++;
  }
}

void tarea(unsigned long time, int pinLED, int numeroParpade, int sensorReas) {
  if (sensorReas == 1) {

    for (int i = 0; i < numeroParpade; i++) {
      digitalWrite(pinLED, HIGH);
      delay(time);
      digitalWrite(pinLED, LOW);
      delay(time);
    }

  } else {
    digitalWrite(pinLED, LOW);
  }
}
// Tarea hacer una clase para que el led parpadee 3 veces al presionarl el boton
