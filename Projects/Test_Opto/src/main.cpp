#include <Arduino.h>
const int sensorPin = 15;
const int ledPin = 4;
int valor = 0;
int estadoAnterior = 0;
unsigned long tiempoUltimoCambio = 0;
unsigned long debounceDelay = 50; // 50 ms de anti-rebote

int conteo = 0;
int conteoAnterior = 0;

void setup()
{
  Serial.begin(115200);
  pinMode(sensorPin, INPUT);
  pinMode(ledPin, OUTPUT);
}

void loop()
{
  int lectura = digitalRead(sensorPin);
  unsigned long tiempoActual = millis();

  if (lectura != estadoAnterior && (tiempoActual - tiempoUltimoCambio) > debounceDelay)
  {
    tiempoUltimoCambio = tiempoActual;

    if (lectura == HIGH)
    {
      conteo++;
      Serial.println("Ficha Detectada");
      digitalWrite(ledPin, HIGH);
    }
    else
    {
      digitalWrite(ledPin, LOW);
    }

    estadoAnterior = lectura;
  }

  if (conteo != conteoAnterior)
  {
    Serial.print("Fichas Contadas: ");
    Serial.println(conteo);
    conteoAnterior = conteo;
  }
}
