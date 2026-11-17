
#include <LiquidCrystal.h>
#include <Servo.h>
#include <Arduino.h>
// ===== PANTALLA LCD (Pines 2, 3, 4, 5) =====
// RS=20, E=21, D4=5, D5=4, D6=3, D7=2
LiquidCrystal lcd(20, 21, 5, 4, 3, 2);

Servo miServo;

// ===== SENSORES Y ACTUADORES =====
const byte trigPin  = 22; 
const byte echoPin  = 23; 
const byte servoPin = 7;  // Servo en el pin 7

// ===== BOTONES =====
const byte botonPin   = 6;  // Botón Manual
const byte rfPin      = 8;  // Botón Simulador RF
const byte irPlatoPin = 9;  // Sensor de Plato

// ===== LED Y BUZZER =====
const byte ledPin    = 10; 
const byte buzzerPin = 11; 

// ===== CALIBRACIÓN =====
const int PLATO_LLENO = 16; 
const int PLATO_VACIO = 21; 

// ===== VARIABLES =====
unsigned long ultimaMedicion = 0;
const unsigned long intervaloMedicion = 1000;
long duracion = 0;
float distancia = -1;
int porcentajeComida = 0;
bool dispensacionAutomaticaPendiente = false;
bool esperandoQueSeLlene = false;
int contadorDepositoVacio = 0; 

float medirDistancia() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(15); 
  digitalWrite(trigPin, LOW);
  duracion = pulseIn(echoPin, HIGH, 50000UL);
  if (duracion == 0) return -1;
  return duracion * 0.0343 / 2.0;
}

int calcularPorcentaje(float d) {
  if (d < 0) return porcentajeComida;
  if (d <= PLATO_LLENO) return 100;
  if (d >= PLATO_VACIO) return 10;
  int porcentaje = 100 - ((d - PLATO_LLENO) * 90 / (PLATO_VACIO - PLATO_LLENO));
  return constrain(porcentaje, 10, 100);
}

void mostrarEnLCD(int porcentaje, bool platoPresente) {
  lcd.setCursor(0, 0);
  lcd.print("Nivel: ");
  lcd.print(porcentaje);
  lcd.print("%      "); 
  lcd.setCursor(0, 1);
  if (platoPresente) lcd.print("Plato OK        ");
  else lcd.print("¡PON EL PLATO!  ");
}

void avisoBuzzer(unsigned long duracionMs) {
  digitalWrite(buzzerPin, HIGH);
  delay(duracionMs);
  digitalWrite(buzzerPin, LOW);
}

void dispensarComida(bool platoPresente) {
  if (!platoPresente) {
    lcd.clear(); lcd.print("¡PON EL PLATO!"); avisoBuzzer(600); delay(1000); return;
  }
  lcd.clear(); lcd.print("Dispensando...");
  digitalWrite(ledPin, HIGH); avisoBuzzer(150);
  miServo.write(90); delay(1500); miServo.write(0);
  digitalWrite(ledPin, LOW); avisoBuzzer(150);
  esperandoQueSeLlene = true; dispensacionAutomaticaPendiente = false; contadorDepositoVacio = 0; 
  ultimaMedicion = millis() - intervaloMedicion;
}

void setup() {
  pinMode(trigPin, OUTPUT); pinMode(echoPin, INPUT);
  pinMode(irPlatoPin, INPUT_PULLUP); 
  pinMode(botonPin, INPUT_PULLUP); 
  pinMode(rfPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT); 
  pinMode(buzzerPin, OUTPUT);
  
  miServo.attach(servoPin); 
  miServo.write(0);
  
  lcd.begin(16, 2); 
  lcd.print("Iniciando..."); 
  delay(1200); 
  lcd.clear();
}

void loop() {
  bool platoPresente = (digitalRead(irPlatoPin) == LOW);

  if (millis() - ultimaMedicion >= intervaloMedicion) {
    ultimaMedicion = millis();
    distancia = medirDistancia();
    if (distancia >= 0) {
      porcentajeComida = calcularPorcentaje(distancia);
      if (distancia >= PLATO_VACIO) {
        contadorDepositoVacio++;
        if (contadorDepositoVacio >= 3 && !esperandoQueSeLlene) dispensacionAutomaticaPendiente = true;
      } else { contadorDepositoVacio = 0; }
      if (distancia <= PLATO_LLENO) esperandoQueSeLlene = false;
    }
    mostrarEnLCD(porcentajeComida, platoPresente);
  }

  if (digitalRead(botonPin) == LOW) { 
    delay(50); 
    if (digitalRead(botonPin) == LOW) { 
      dispensarComida(platoPresente); 
      while (digitalRead(botonPin) == LOW) delay(10); 
    } 
  }
  
  if (digitalRead(rfPin) == LOW) { 
    delay(50); 
    if (digitalRead(rfPin) == LOW) { 
      dispensarComida(platoPresente); 
      while (digitalRead(rfPin) == LOW) delay(10); 
    } 
  }
  
  if (dispensacionAutomaticaPendiente) dispensarComida(platoPresente);
}

