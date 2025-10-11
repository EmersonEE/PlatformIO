#include <Arduino.h>

// Definición de pines
const int EN_PIN = 23;   // Pin de habilitación (Enable)
const int STEP_PIN = 22; // Pin de paso (Step)
const int DIR_PIN = 21;  // Pin de dirección (Direction)

// Parámetros del motor
const int STEPS_PER_REVOLUTION = 200; // Pasos por revolución (ajusta según tu motor)
const int MICROSTEPS = 16;           // Microsteps configurados en el TMC2209
const int SPEED_DELAY = 500;         // Retardo en microsegundos entre pasos (ajusta para velocidad)

void setup() {
  // Configurar los pines como salidas
  pinMode(EN_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);

  // Habilitar el driver (bajo activa el TMC2209)
  digitalWrite(EN_PIN, LOW);

  // Iniciar comunicación serial para depuración (opcional)
  Serial.begin(115200);
}
// Función para mover el motor un número determinado de pasos
void moveMotor(int steps) {
  for (int i = 0; i < steps; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(SPEED_DELAY);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(SPEED_DELAY);
  }
}
void loop() {
  // Girar en una dirección
  digitalWrite(DIR_PIN, HIGH); // Establecer dirección (HIGH o LOW según el sentido)
  Serial.println("Girando en dirección 1");
  moveMotor(1 * STEPS_PER_REVOLUTION * MICROSTEPS); // 1 revolución completa
  delay(1000); // Esperar 1 segundo

  // Girar en la dirección opuesta
  digitalWrite(DIR_PIN, LOW); // Cambiar dirección
  Serial.println("Girando en dirección 2");
  moveMotor(1 * STEPS_PER_REVOLUTION * MICROSTEPS); // 1 revolución completa
  delay(1000); // Esperar 1 segundo
}

