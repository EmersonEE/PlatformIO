#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>

#define PULSADOR 9
#define PULSADOR_ANUNCIO 7

SoftwareSerial softSerial(10, 11);
DFRobotDFPlayerMini reproductor;

bool reproduciendo = false;

const byte leds = 12;

unsigned long tiempoAnterior = 0;
unsigned long tiempoInicio = 0;

byte estadoAnimacion = 0;

// Debounce pulsador principal
bool estadoPulsador = LOW;
bool ultimaLectura = HIGH;
unsigned long ultimoTiempoDebounce = 0;
const unsigned long debounceDelay = 30;

// Anuncio no bloqueante
bool estadoAnuncio = HIGH;
bool ultimaLecturaAnuncio = HIGH;
unsigned long ultimoTiempoDebounceAnuncio = 0;
unsigned long ultimoTiempoAnuncio = 0;
const unsigned long cooldownAnuncio = 500;

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
    pinMode(leds, OUTPUT);
    while (true) {
      digitalWrite(leds, HIGH);
      delay(100);
      digitalWrite(leds, LOW);
      delay(100);
    }
  }

  delay(1000);

  reproductor.volume(27);
  reproductor.enableLoopAll();

  tiempoInicio = millis();
}

void loop() {
  unsigned long ahora = millis();

  // --- Pulsador principal con debounce ---
  bool lectura = digitalRead(PULSADOR);

  if (lectura != ultimaLectura) {
    ultimoTiempoDebounce = ahora;
    ultimaLectura = lectura;
  }

  if (ahora - ultimoTiempoDebounce >= debounceDelay) {
    if (lectura != estadoPulsador) {
      estadoPulsador = lectura;

      if (lectura == HIGH) {
        reproductor.start();
        reproduciendo = true;
        tiempoInicio = ahora;
        estadoAnimacion = 0;
        Serial.println("Reproduciendo");
      } else {
        reproductor.pause();
        reproduciendo = false;
        digitalWrite(leds, LOW);
        Serial.println("Pausado");
      }
    }
  }

  if (reproduciendo) {
    animacionCorazon();
  }

  // --- Pulsador de anuncio con debounce y cooldown no bloqueante ---
  bool lecturaAnuncio = digitalRead(PULSADOR_ANUNCIO);

  if (lecturaAnuncio != ultimaLecturaAnuncio) {
    ultimoTiempoDebounceAnuncio = ahora;
    ultimaLecturaAnuncio = lecturaAnuncio;
  }

  if (ahora - ultimoTiempoDebounceAnuncio >= debounceDelay) {
    if (lecturaAnuncio == LOW && estadoAnuncio == HIGH &&
        ahora - ultimoTiempoAnuncio >= cooldownAnuncio) {
      reproductor.advertise(1);
      ultimoTiempoAnuncio = ahora;
      Serial.println("Anuncio");
    }
    estadoAnuncio = lecturaAnuncio;
  }
}
