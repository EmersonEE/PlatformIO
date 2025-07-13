#include <Arduino.h>

// Tipos de Variables

uint8_t led = 18;
int temperatura = 25;
float voltaje = 5.0;
boolean estado = true;
char letra = 'A';
byte brillo = 128;

void setup() {
  Serial.begin(115200);

  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
}
void loop() {

  Serial.print("temperatura: ");
  Serial.println(temperatura);

  Serial.print("voltaje: ");
  Serial.println(voltaje);

  Serial.print("estado: ");
  Serial.println(estado);
  Serial.print("letra: ");
  Serial.println(letra);
  Serial.print("brillo: ");
  Serial.println(brillo);
  delay(1000);
}
