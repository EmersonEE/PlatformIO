// const int stepsPerRevolution = 2400;
#include <Arduino.h>

#define VRX_PIN 39 // ESP32 pin GPIO39 (ADC3) conectado al pin VRX
#define VRY_PIN 36 // ESP32 pin GPIO36 (ADC0) conectado al pin VRY
#define SW_PIN 4
#define STEP 16
#define DIR 17
#define ENA 18

bool configuracion = false;         // Estado del ciclo while
int valorX = 0;                     // Valor del eje X
int valorY = 0;                     // Valor del eje Y
int valorSW = HIGH;                 // Estado actual del botón
int lastSWState = HIGH;             // Último estado registrado del botón
unsigned long lastDebounceTime = 0; // Tiempo de la última lectura del botón
unsigned long debounceDelay = 50;   // Retardo para eliminar rebotes

void setup()
{
  Serial.begin(115200);
  pinMode(SW_PIN, INPUT_PULLUP);
  pinMode(STEP, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(ENA, OUTPUT);
  digitalWrite(ENA, LOW);
}

void loop()
{
  valorX = map(analogRead(VRX_PIN), 0, 4095, 0, 255);
  valorY = map(analogRead(VRY_PIN), 0, 4095, 0, 255);

  int reading = digitalRead(SW_PIN);

  if (reading != lastSWState)
  {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    if (reading != valorSW)
    {
      valorSW = reading;

      if (valorSW == LOW)
      {
        configuracion = !configuracion;
        if (configuracion)
        {
          Serial.println("Entrando en modo configuración");
        }
        else
        {
          Serial.println("Saliendo de modo configuración");
        }
      }
    }
  }
  lastSWState = reading;
  if (configuracion)
  {
    Serial.println("Dentro del modo configuración");
    if (valorX > 150)
    {
      digitalWrite(DIR, HIGH);
      digitalWrite(STEP, HIGH);
      delay(5);
      digitalWrite(STEP, LOW);
      delay(5);
      Serial.println("Moviendo Derecha");
    }
    else if (valorX < 50)
    {
      digitalWrite(DIR, LOW);
      digitalWrite(STEP, HIGH);
      delay(5);
      digitalWrite(STEP, LOW);
      delay(5);
      Serial.println("Moviendo Izquierda");
    }
  }
}