#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>

#define PULSADOR 9
#define PULSADOR_ANUNCIO 7

SoftwareSerial softSerial(10, 11);
DFRobotDFPlayerMini reproductor;

bool estadoAnterior = LOW;
bool reproduciendo = false;

const byte leds = 12;

unsigned long tiempoAnterior = 0;
unsigned long tiempoInicio = 0;

byte estadoAnimacion = 0;

void animacionCorazon() {

  unsigned long ahora = millis();

  switch (estadoAnimacion) {

  case 0:
    digitalWrite(leds, HIGH);
    tiempoAnterior = ahora;
    estadoAnimacion = 1;
    break;

  case 1:
    if (ahora - tiempoAnterior >= 120) {
      digitalWrite(leds, LOW);
      tiempoAnterior = ahora;
      estadoAnimacion = 2;
    }
    break;

  case 2:
    if (ahora - tiempoAnterior >= 100) {
      digitalWrite(leds, HIGH);
      tiempoAnterior = ahora;
      estadoAnimacion = 3;
    }
    break;

  case 3:
    if (ahora - tiempoAnterior >= 120) {
      digitalWrite(leds, LOW);
      tiempoAnterior = ahora;
      estadoAnimacion = 4;
    }
    break;

  case 4:
    if (ahora - tiempoAnterior >= 500) {

      if (ahora - tiempoInicio > 8000) {
        estadoAnimacion = 10;
        tiempoInicio = ahora;
      } else {
        estadoAnimacion = 0;
      }
    }
    break;

  case 10:
    digitalWrite(leds, HIGH);
    tiempoAnterior = ahora;
    estadoAnimacion = 11;
    break;

  case 11:
    if (ahora - tiempoAnterior >= 70) {
      digitalWrite(leds, LOW);
      tiempoAnterior = ahora;
      estadoAnimacion = 12;
    }
    break;

  case 12:
    if (ahora - tiempoAnterior >= 70) {
      digitalWrite(leds, HIGH);
      tiempoAnterior = ahora;
      estadoAnimacion = 13;
    }
    break;

  case 13:
    if (ahora - tiempoAnterior >= 70) {
      digitalWrite(leds, LOW);
      tiempoAnterior = ahora;
      estadoAnimacion = 14;
    }
    break;

  case 14:
    if (ahora - tiempoAnterior >= 70) {
      digitalWrite(leds, HIGH);
      tiempoAnterior = ahora;
      estadoAnimacion = 15;
    }
    break;

  case 15:
    if (ahora - tiempoAnterior >= 70) {
      digitalWrite(leds, LOW);
      tiempoAnterior = ahora;
      estadoAnimacion = 0;
    }
    break;
  }
}

void setup() {

  softSerial.begin(9600);
  Serial.begin(9600);

  pinMode(PULSADOR, INPUT_PULLUP);
  pinMode(PULSADOR_ANUNCIO, INPUT_PULLUP);
  pinMode(leds, OUTPUT);

  digitalWrite(leds, LOW);

  if (!reproductor.begin(softSerial, true, false)) {
    while (true)
      ;
  }

  delay(1000);

  reproductor.volume(27);
  reproductor.enableLoopAll();

  tiempoInicio = millis();
}

void loop() {

  bool estadoActual = digitalRead(PULSADOR);

  if (estadoActual == HIGH && estadoAnterior == LOW) {

    reproductor.start();
    reproduciendo = true;

    tiempoInicio = millis();
    estadoAnimacion = 0;

    Serial.println("Reproduciendo");
  }

  if (estadoActual == LOW && estadoAnterior == HIGH) {

    reproductor.pause();
    reproduciendo = false;

    digitalWrite(leds, LOW);

    Serial.println("Pausado");
  }

  estadoAnterior = estadoActual;

  if (reproduciendo) {
    animacionCorazon();
  }

  if (digitalRead(PULSADOR_ANUNCIO) == LOW) {
    reproductor.advertise(1);
    delay(500);
  }
}
