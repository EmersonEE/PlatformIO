#include <Arduino.h>
#include <AccelStepper.h>

// Definición de pines para el ESP32
#define STEP_PIN 27
#define DIR_PIN  14
// NOTA: El pin EN (Enable) del TMC2209 está conectado físicamente a GND (siempre habilitado).

// Interfaz para drivers tipo Step/Dir: AccelStepper::DRIVER (1 = STEP, DIR)
AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

// Cantidad de pasos para la prueba.
// Si el TMC2209 está en microstepping (por ejemplo 1/16 con 200 pasos/rev típicos de NEMA 17):
// 200 pasos * 16 microsteps = 3200 pasos por vuelta completa.
// Ajusta este valor según la configuración de los pines MS1/MS2 de tu driver.
const long STEPS_PER_CYCLE = 1600; 

// Variables para control de pausas entre giros
unsigned long lastMoveTime = 0;
const unsigned long PAUSE_BETWEEN_MOVES_MS = 1000;
bool movingForward = true;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    // Espera hasta 3 segundos la conexión serie
  }

  Serial.println(F("\n==========================================="));
  Serial.println(F("    TEST MOTOR NEMA 17 + TMC2209 + ESP32    "));
  Serial.println(F("==========================================="));
  Serial.println(F("Configuración de pines:"));
  Serial.printf("  - STEP Pin   : GPIO %d\n", STEP_PIN);
  Serial.printf("  - DIR Pin    : GPIO %d\n", DIR_PIN);
  Serial.println(F("  - ENABLE Pin : Conectado a GND (Activo LOW)"));
  Serial.println(F("==========================================="));

  // Velocidad máxima (pasos por segundo) y Aceleración (pasos por segundo^2)
  // Con rampas de aceleración evitamos que el motor pierda pasos o se bloquee por inercia
  stepper.setMaxSpeed(1000.0);       // Pasos por segundo máximos
  stepper.setAcceleration(500.0);    // Aceleración suave

  // Iniciar primer movimiento
  Serial.printf("Iniciando giro horario (%ld pasos)...\n", STEPS_PER_CYCLE);
  stepper.moveTo(STEPS_PER_CYCLE);
}

void loop() {
  // stepper.run() debe ejecutarse continuamente lo más rápido posible dentro del loop
  stepper.run();

  // Si llegó a la posición objetivo
  if (stepper.distanceToGo() == 0) {
    if (lastMoveTime == 0) {
      lastMoveTime = millis();
      Serial.println(F("Objetivo alcanzado. Esperando pausa..."));
    }

    // Espera un segundo antes de invertir el giro
    if (millis() - lastMoveTime >= PAUSE_BETWEEN_MOVES_MS) {
      if (movingForward) {
        Serial.printf("Invirtiendo sentido: Girando antihorario (%ld pasos)...\n", -STEPS_PER_CYCLE);
        stepper.moveTo(-STEPS_PER_CYCLE);
        movingForward = false;
      } else {
        Serial.printf("Invirtiendo sentido: Girando horario (%ld pasos)...\n", STEPS_PER_CYCLE);
        stepper.moveTo(STEPS_PER_CYCLE);
        movingForward = true;
      }
      lastMoveTime = 0; // Reiniciar temporizador
    }
  }
}
