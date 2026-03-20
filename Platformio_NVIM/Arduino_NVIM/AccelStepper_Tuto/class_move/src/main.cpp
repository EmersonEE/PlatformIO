#include <AccelStepper.h>
#include <Arduino.h>

AccelStepper stepper(1, 22, 21);

const int pasosPorRevolucion = 1600;
const int gradosPrueba[4] = {45, 90, -45, -90};
int indiceActual = 0; // Para saber qué ángulo toca mover

void setup() {
  pinMode(23, OUTPUT);
  digitalWrite(23, LOW);

  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(500);
  Serial.begin(115200);

  int pasos = (gradosPrueba[indiceActual] * pasosPorRevolucion) / 360;
  stepper.move(pasos);
}

void loop() {
  stepper.run();

  if (stepper.distanceToGo() == 0) {
    Serial.println("Destino alcanzado. Esperando...");

    delay(1000);

    indiceActual++;

    if (indiceActual >= 4) {
      indiceActual = 0;
    }

    int grados = gradosPrueba[indiceActual];
    long pasos = (long)grados * pasosPorRevolucion / 360;

    Serial.print("Moviendo a grados: ");
    Serial.println(grados);

    stepper.move(pasos);
  }
}
