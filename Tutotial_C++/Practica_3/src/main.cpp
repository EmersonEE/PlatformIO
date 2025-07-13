#include "HardwareSerial.h"
#include <Arduino.h>

// Creando clases para Arduino
//

// Declracion de constante
//
const int ledPin = 13;

void encenderLED(int pin, unsigned long time);

float promedio(float a, float b) { return (a + b) / 2; }

// Crear funcion de enum class
int sumar(int a, int b) { return a + b; }

void setup() { encenderLED(ledPin, 2000); }
void loop() {}

void encenderLED(int pin, unsigned long time) {

  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  delay(time);
  digitalWrite(pin, LOW);
  Serial.begin(115200);
  Serial.println("LED Encendido");
  int resultado = sumar(5, 3);
  Serial.print("Resutlado");
  Serial.println(resultado);
  float pro = promedio(5, 8);
  Serial.print("Resultado Promedio");
  Serial.println(pro);
}

// Tarea modificar funcion para que apague el led t segundos
// Tarea crear funcion para poder sacar el promedio de dos numeros
