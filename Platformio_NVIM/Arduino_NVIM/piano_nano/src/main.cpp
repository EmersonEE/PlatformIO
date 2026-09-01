#include <Arduino.h>
#include "midi.h"
const byte PIN_BUZZER = 10;

const byte botones[] = {2, 3, 4, 5, 6, 7, 8, 9};
const byte leds[] = {A7, A6, A5, A4, A3, A2, A1, A0};

const int frecuencias[] = {
  262, 294, 330, 349, 392, 440, 494, 523
};

const byte NUM_NOTAS = 8;



void apagarLeds()
{
  for (byte i = 0; i < NUM_NOTAS; i++) {
    digitalWrite(leds[i], LOW);
  }
}

void setup()
{
  pinMode(PIN_BUZZER, OUTPUT);

  for (byte i = 0; i < NUM_NOTAS; i++) {
    pinMode(botones[i], INPUT_PULLUP);
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }
}

void loop()
{
  // Botones conectados entre el pin y GND.
  bool boton1 = digitalRead(botones[0]) == LOW;
  bool boton2 = digitalRead(botones[1]) == LOW;

  // Reproduce la canción al presionar simultáneamente
  // los botones conectados a los pines 2 y 3.
  if (boton1 && boton2) {
    apagarLeds();

    digitalWrite(leds[0], HIGH);
    digitalWrite(leds[1], HIGH);

    delay(30);  // Antirrebote

    if (digitalRead(botones[0]) == LOW &&
        digitalRead(botones[1]) == LOW) {
      midi(PIN_BUZZER);
    }

    apagarLeds();

    // Esperar a que ambos botones sean liberados.
    while (digitalRead(botones[0]) == LOW ||
           digitalRead(botones[1]) == LOW) {
      delay(10);
    }

    return;
  }

  bool notaActiva = false;

  for (byte i = 0; i < NUM_NOTAS; i++) {
    bool presionado = digitalRead(botones[i]) == LOW;

    digitalWrite(leds[i], presionado ? HIGH : LOW);

    // El buzzer solo puede reproducir una nota simultáneamente.
    if (presionado && !notaActiva) {
      tone(PIN_BUZZER, frecuencias[i]);
      notaActiva = true;
    }
  }

  if (!notaActiva) {
    noTone(PIN_BUZZER);
  }
}
