#include <Wire.h>
#include <Arduino.h>
// ============================================
// CONFIGURACIÓN DE PINES
// ============================================
// Puente H L298N
#define MOTOR_A_IN1 7
#define MOTOR_A_IN2 6
#define MOTOR_B_IN3 5
#define MOTOR_B_IN4 4
#define MOTOR_A_ENA 9   // PWM
#define MOTOR_B_ENB 10  // PWM

// ============================================
// DIRECCIÓN I2C DEL MPU6050
// ============================================
#define MPU6050_ADDR 0x68

// Registros del MPU6050
#define MPU6050_PWR_MGMT_1   0x6B
#define MPU6050_GYRO_CONFIG  0x1B
#define MPU6050_ACCEL_CONFIG 0x1C
#define MPU6050_ACCEL_XOUT_H 0x3B
#define MPU6050_TEMP_OUT_H   0x41
#define MPU6050_GYRO_XOUT_H  0x43

// ============================================
// VARIABLES GLOBALES DEL SENSOR
// ============================================
int16_t accel_x_raw, accel_y_raw, accel_z_raw;
int16_t gyro_x_raw, gyro_y_raw, gyro_z_raw;
int16_t temperatura_raw;

float accel_x, accel_y, accel_z;
float gyro_x, gyro_y, gyro_z;      

float angulo_accel;
float angulo_gyro;
float angulo_filtrado; 

// Offsets de calibración (se calculan en setup)
float gyro_x_offset = 0;
float gyro_y_offset = 0;
float gyro_z_offset = 0;

// ============================================
// FILTRO DE MEDIA MÓVIL
// ============================================
#define VENTANA_MA 10
float buffer_angulo[VENTANA_MA];
int indice_buffer = 0;
float suma_buffer = 0;

// ============================================
// CONTROL PID (Valores de inicio PDF Pag 22)
// ============================================
// ¡IMPORTANTE! Empezamos con valores bajos para calibrar
float Kp = 20.0;   // Fase 1: Ajustar solo Kp hasta que oscile constante
float Ki = 0.0;    // Fase 3: Dejar en 0 al inicio
float Kd = 0.0;    // Fase 2: Dejar en 0 al inicio

float setpoint = 0.0;        
float error = 0;
float error_previo = 0;      
float integral = 0;          
float derivada = 0;
float salida_pid = 0;        

float integral_max = 50.0;
float integral_min = -50.0;

// ============================================
// CONTROL DE TIEMPO
// ============================================
unsigned long tiempo_previo = 0;
float dt = 0;

#define ALPHA 0.95

// ============================================
// LÍMITE DE VELOCIDAD DE MOTORES
// ============================================
#define VELOCIDAD_MAXIMA 255
#define VELOCIDAD_MINIMA 30   // Si zumba pero no se mueve, subir a 40 o 50
#define ANGULO_CAIDA 45.0     




// ============================================
// FUNCIONES MPU6050 (Sin cambios mayores)
// ============================================

void escribirRegistro(uint8_t reg, uint8_t valor) {
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(reg);
  Wire.write(valor);
  Wire.endTransmission();
}
void inicializarMPU6050() {
  escribirRegistro(MPU6050_PWR_MGMT_1, 0x00);
  delay(100);
  escribirRegistro(MPU6050_GYRO_CONFIG, 0x00);
  escribirRegistro(MPU6050_ACCEL_CONFIG, 0x00);
  delay(100);
}



void leerRegistros(uint8_t reg, uint8_t cantidad, uint8_t *buffer) {
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)MPU6050_ADDR, cantidad);
  for(uint8_t i = 0; i < cantidad; i++) {
    buffer[i] = Wire.read();
  }
}

void leerMPU6050() {
  uint8_t buffer[14];
  leerRegistros(MPU6050_ACCEL_XOUT_H, 14, buffer);
  
  accel_x_raw = (buffer[0] << 8) | buffer[1];
  accel_y_raw = (buffer[2] << 8) | buffer[3];
  accel_z_raw = (buffer[4] << 8) | buffer[5];
  // temperatura se salta
  gyro_x_raw = (buffer[8] << 8) | buffer[9];
  gyro_y_raw = (buffer[10] << 8) | buffer[11];
  gyro_z_raw = (buffer[12] << 8) | buffer[13];
  
  accel_x = accel_x_raw / 16384.0;
  accel_y = accel_y_raw / 16384.0;
  accel_z = accel_z_raw / 16384.0;
  
  gyro_x = (gyro_x_raw / 131.0) - gyro_x_offset;
  gyro_y = (gyro_y_raw / 131.0) - gyro_y_offset;
  gyro_z = (gyro_z_raw / 131.0) - gyro_z_offset;
}

void calibrarGiroscopio() {
  const int num_muestras = 1000;
  float suma_x = 0, suma_y = 0, suma_z = 0;
  for(int i = 0; i < num_muestras; i++) {
    leerMPU6050();
    // Sumamos raw/131.0 pero SIN offset aún, porque lo estamos calculando
    // (Pequeña corrección: leerMPU6050 ya resta el offset, así que aquí 
    // debemos leer manualmente o temporalmente poner offsets en 0)
    // Para simplificar, asumimos que offsets son 0 al llamar esto en setup.
    suma_x += (gyro_x_raw / 131.0); 
    suma_y += (gyro_y_raw / 131.0);
    suma_z += (gyro_z_raw / 131.0);
    delay(3);
  }
  gyro_x_offset = suma_x / num_muestras;
  gyro_y_offset = suma_y / num_muestras;
  gyro_z_offset = suma_z / num_muestras;
}

float calcularAnguloAcelerometro() {
  return atan2(accel_y, accel_x) * 180.0 / PI;
}

float filtroMediaMovil(float nuevo_valor) {
  suma_buffer -= buffer_angulo[indice_buffer];
  buffer_angulo[indice_buffer] = nuevo_valor;
  suma_buffer += nuevo_valor;
  indice_buffer = (indice_buffer + 1) % VENTANA_MA;
  return suma_buffer / VENTANA_MA;
}

// CORRECCIÓN AQUÍ: Invertí HIGH/LOW respecto a tu código original
// para corregir el problema de "giro contrario"
void moverAdelante(int velocidad) {
  // Motor A Adelante (INVERTIDO)
  digitalWrite(MOTOR_A_IN1, LOW);  // Antes era HIGH
  digitalWrite(MOTOR_A_IN2, HIGH); // Antes era LOW
  analogWrite(MOTOR_A_ENA, velocidad);
  
  // Motor B Adelante (INVERTIDO)
  digitalWrite(MOTOR_B_IN3, LOW);  // Antes era HIGH
  digitalWrite(MOTOR_B_IN4, HIGH); // Antes era LOW
  analogWrite(MOTOR_B_ENB, velocidad);
}

void moverAtras(int velocidad) {
  // Motor A Atrás (INVERTIDO)
  digitalWrite(MOTOR_A_IN1, HIGH); // Antes era LOW
  digitalWrite(MOTOR_A_IN2, LOW);  // Antes era HIGH
  analogWrite(MOTOR_A_ENA, velocidad);
  
  // Motor B Atrás (INVERTIDO)
  digitalWrite(MOTOR_B_IN3, HIGH); // Antes era LOW
  digitalWrite(MOTOR_B_IN4, LOW);  // Antes era HIGH
  analogWrite(MOTOR_B_ENB, velocidad);
}

void detenerMotores() {
  digitalWrite(MOTOR_A_IN1, LOW);
  digitalWrite(MOTOR_A_IN2, LOW);
  analogWrite(MOTOR_A_ENA, 0);
  
  digitalWrite(MOTOR_B_IN3, LOW);
  digitalWrite(MOTOR_B_IN4, LOW);
  analogWrite(MOTOR_B_ENB, 0);
}
// ============================================
// CONTROL PID - LÓGICA CORREGIDA
// ============================================
float calcularPID(float medicion, float delta_tiempo) {
  // CAMBIO CRÍTICO: El error debe ser (medición - setpoint) o (setpoint - medición)
  // dependiendo de cómo definas "Adelante". 
  // Si Ángulo Positivo es "caer al frente", queremos PID Positivo para "mover al frente".
  // Si Setpoint=0, Error = medicion - setpoint = +Grados.
  // P = Kp * (+Grados) = +Velocidad. 
  // CONTROLAR_MOTORES(+Velocidad) -> MOVER_ADELANTE.
  error = medicion - setpoint; 
  
  float P = Kp * error;
  
  integral += error * delta_tiempo;
  if(integral > integral_max) integral = integral_max;
  if(integral < integral_min) integral = integral_min;
  float I = Ki * integral;
  
  derivada = (error - error_previo) / delta_tiempo;
  float D = Kd * derivada;
  
  error_previo = error;
  
  return P + I + D;
}

// ============================================
// CONTROL DE MOTORES - DIRECCIÓN INVERTIDA
// ============================================
void controlarMotores(float velocidad) {
  if(velocidad > VELOCIDAD_MAXIMA) velocidad = VELOCIDAD_MAXIMA;
  if(velocidad < -VELOCIDAD_MAXIMA) velocidad = -VELOCIDAD_MAXIMA;
  
  if(abs(velocidad) < VELOCIDAD_MINIMA) {
    detenerMotores();
    return;
  }
  
  int pwm = abs((int)velocidad);
  
  // Lógica simple: Positivo = Adelante, Negativo = Atrás
  if(velocidad > 0) {
    moverAdelante(pwm);
  } else {
    moverAtras(pwm);
  }
}



void setup() {
  Serial.begin(115200);
  Serial.println("=================================");
  Serial.println("ROBOT AUTOBALANCEADO - USAC 2025");
  Serial.println("=================================");
  
  pinMode(MOTOR_A_IN1, OUTPUT);
  pinMode(MOTOR_A_IN2, OUTPUT);
  pinMode(MOTOR_B_IN3, OUTPUT);
  pinMode(MOTOR_B_IN4, OUTPUT);
  pinMode(MOTOR_A_ENA, OUTPUT);
  pinMode(MOTOR_B_ENB, OUTPUT);
  
  detenerMotores();

  Wire.begin();
  Wire.setClock(400000); 
  
  Serial.println("Inicializando MPU6050...");
  inicializarMPU6050();
  
  Serial.println("Calibrando giroscopio (mantén quieto el robot)...");
  calibrarGiroscopio();

  for(int i = 0; i < VENTANA_MA; i++) {
    buffer_angulo[i] = 0;
  }
  
  leerMPU6050();
  angulo_accel = calcularAnguloAcelerometro();
  angulo_gyro = angulo_accel;
  angulo_filtrado = angulo_accel;
  
  Serial.println("Sistema listo!");
  Serial.println("Coloca el robot vertical y espera 2 segundos...");
  delay(2000);
  
  tiempo_previo = millis();
}

void loop() {
  unsigned long tiempo_actual = millis();
  dt = (tiempo_actual - tiempo_previo) / 1000.0; 
  tiempo_previo = tiempo_actual;

  leerMPU6050();
  
  angulo_accel = calcularAnguloAcelerometro();
  angulo_gyro = angulo_gyro + gyro_y * dt;

  // Filtro Complementario
  angulo_filtrado = ALPHA * (angulo_filtrado + gyro_y * dt) + (1 - ALPHA) * angulo_accel;

  // Filtro Media Móvil
  angulo_filtrado = filtroMediaMovil(angulo_filtrado);

  salida_pid = calcularPID(angulo_filtrado, dt);
  
  if(abs(angulo_filtrado) > ANGULO_CAIDA) {
    detenerMotores();
  } else {
    controlarMotores(salida_pid);
  }
  
  // Debug
  if(tiempo_actual % 100 < 10) { 
    Serial.print("Angulo: ");
    Serial.print(angulo_filtrado, 2);
    Serial.print(" | PID: ");
    Serial.println(salida_pid, 2);
  }
  
  delay(10); // Pequeño delay para estabilidad del loop
}
