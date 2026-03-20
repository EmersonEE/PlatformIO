#include <AccelStepper.h>
#include <Arduino.h>

// Configuración del motor: (Tipo de interfaz, Pin STEP, Pin DIR)
AccelStepper stepper(1, 22, 21);

// Definiciones del motor
const int pasosPorRevolucion =
    1600; // Ajusta según tu motor (NEMA 17 suele ser 200)
bool moviendoAdelante = true;

void setup() {
  // Pin de habilitación (si tu driver lo requiere, como el A4988)
  pinMode(23, OUTPUT);
  digitalWrite(23, LOW);

  // Configuración de velocidad y aceleración
  stepper.setMaxSpeed(1000);    // Pasos por segundo
  stepper.setAcceleration(500); // Pasos por segundo al cuadrado

  // Iniciamos el primer movimiento
  stepper.moveTo(pasosPorRevolucion);
}

void loop() {
  // Si el motor llegó a su destino
  if (stepper.distanceToGo() == 0) {
    delay(1000); // Pausa de 1 segundo entre giros

    if (moviendoAdelante) {
      // Si estaba en 200, ahora vuelve a 0 (360 grados atrás)
      stepper.moveTo(0);
      moviendoAdelante = false;
    } else {
      // Si estaba en 0, ahora va a 200 (360 grados adelante)
      stepper.moveTo(pasosPorRevolucion);
      moviendoAdelante = true;
    }
  }

  // ¡Muy importante! Esta función debe llamarse constantemente
  stepper.run();
}
