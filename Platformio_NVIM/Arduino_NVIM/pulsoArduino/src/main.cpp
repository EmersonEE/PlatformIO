#include <Arduino.h>
const int pulsePin = A3;
int threshold = 512; // Se ajustará solo
unsigned long lastBeatTime = 0;
bool pulseDetected = false;

// Promedio de BPM
const int numLecturas = 5;
int lecturasBPM[numLecturas];
int indiceLectura = 0;
int sumaBPM = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("Coloca el dedo y mantenlo quieto...");
}

void loop() {
  int sensorValue = analogRead(pulsePin);

  int signalMax = 0;
  int signalMin = 1023;

  // Capturamos el pico en una ventana pequeña para ajustar el umbral
  threshold = (threshold * 0.9) + (sensorValue * 0.1);

  // Detección lógica: El latido debe estar un poco por encima del promedio
  if (sensorValue > (threshold + 5) && !pulseDetected) {
    unsigned long currentMillis = millis();
    unsigned long duration = currentMillis - lastBeatTime;

    if (duration > 300 && duration < 2000) { // Filtro entre 30 y 200 BPM
      int bpmInstantaneo = 60000 / duration;
      lastBeatTime = currentMillis;
      pulseDetected = true;
      digitalWrite(LED_BUILTIN, HIGH);

      // Promedio móvil
      sumaBPM = sumaBPM - lecturasBPM[indiceLectura];
      lecturasBPM[indiceLectura] = bpmInstantaneo;
      sumaBPM = sumaBPM + lecturasBPM[indiceLectura];
      indiceLectura = (indiceLectura + 1) % numLecturas;

      Serial.print("BPM: ");
      Serial.println(sumaBPM / numLecturas);
    }
  }

  if (sensorValue < threshold) {
    pulseDetected = false;
    digitalWrite(LED_BUILTIN, LOW);
  }

  delay(10);
}
