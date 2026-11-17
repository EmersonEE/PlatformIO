// ============================================================
// Proyecto: Sistema de Mision Critica de Grado Industrial
// Board: Arduino Uno R3 | Framework: Arduino | PIO
// Reglas: SIN delay(), todo con millis(). PCB-ready.
// ============================================================
#include <Arduino.h>            // Base Arduino
#include <Wire.h>               // Bus I2C para OLED
#include <Adafruit_GFX.h>       // Graficos base OLED
#include <Adafruit_SSD1306.h>   // Driver OLED SSD1306
#include <Keypad.h>             // Teclado matricial 4x4
#include <Servo.h>              // Servomotor SG90
#include <ArduinoJson.h>        // JSON limpio garantizado

// ---------- 1. DEFINICIONES DE PINES (PCB final) ----------
// NOTA INGENIERIA: tu mapa original pedia Motor en D5 y a la vez
// Columna Keypad en D5 -> CONFLICTO. Correccion aplicada:
// Motor PWM se mueve a D9 (PWM hardware). Fila Keypad D9 -> A2.
// Todo lo demas queda igual a tu arquitectura.
#define PIN_LM35      A0        // OUT LM35 -> A0 (analogico)
#define PIN_PIR       2         // OUT PIR -> D2 (INT0)
#define PIN_SERVO     3         // Senal servo -> D3
#define PIN_MOTOR     9         // Base transistor (PWM) -> D9
#define PIN_RELE      12        // Senal rele -> D12
#define PIN_BUZZER    13        // Buzzer -> D13

// Rele tipico de 1 canal: activo en LOW. Si tu modulo es HIGH, cambia a HIGH.
#define RELE_ON   LOW
#define RELE_OFF  HIGH

// ---------- 2. CONSTANTES DEL SISTEMA ----------
#define OLED_ADDR     0x3C      // Direccion I2C SSD1306
#define OLED_W        128       // Ancho pixeles
#define OLED_H        64        // Alto pixeles (usa 32 si tu panel es 128x32)
#define TEMP_EXT_ON   30.0f     // Umbral arranque extractor (C)
#define TEMP_EXT_MAX  45.0f     // Temp para 100% PWM (C)
#define TEMP_ALARMA   40.0f     // Umbral alarma termica (C)
#define CLAVE_OK      "1234"    // Clave maestra (4 digitos)
#define MAX_INTENTOS  3         // Intentos antes de bloqueo
#define T_SERVO_OPEN  5000UL    // Tiempo puerta abierta (ms)
#define T_BLOQUEO     30000UL   // Tiempo bloqueo por fuerza bruta (ms)
#define T_JSON        2000UL    // Periodo telemetria JSON (ms)
#define T_OLED        500UL     // Refresco OLED (ms, evita parpadeo)
#define T_TEMP        200UL     // Muestreo LM35 (ms)
#define T_RAMPA       50UL      // Paso rampa PWM (ms)
#define T_BUZZER      300UL     // Intermitencia buzzer (ms)

// ---------- 3. OBJETOS Y VARIABLES GLOBALES ----------
Adafruit_SSD1306 oled(OLED_W, OLED_H, &Wire, -1); // Sin pin RESET dedicado
Servo cerrojo;                                     // Servomotor puerta

const byte ROWS = 4, COLS = 4;                     // Teclado 4x4
char teclas[ROWS][COLS] = {                        // Mapa fisico
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte pinFilas[ROWS] = {11, 10, A2, 8};             // D9 original -> A2 (libera PWM)
byte pinCols[COLS]  = {7, 6, 5, 4};                // Sin cambios
Keypad teclado = Keypad(makeKeymap(teclas), pinFilas, pinCols, ROWS, COLS);

volatile bool flagPIR = false;      // Seteado en ISR, leido en loop
char  claveBuf[5] = {0};            // Buffer 4 digitos + '\0'
uint8_t idxClave = 0;               // Posicion escritura buffer
uint8_t intentosFallidos = 0;       // Contador fuerza bruta
bool  bloqueado = false;            // Estado bloqueo
bool  accesoAutorizado = false;     // Puerta abierta
bool  intrusion = false;            // PIR + no autorizado
bool  alarmaTemp = false;           // Temp > 40C
bool  alarmaActiva = false;         // OR de alarmas
bool  buzzerOn = false;             // Estado actual buzzer
uint8_t motorActual = 0;            // PWM real (rampa suave)
uint8_t motorObjetivo = 0;          // PWM deseado
bool  releOn = false;               // Estado luces emergencia
float tempC = 0.0f;                 // Temperatura filtrada

unsigned long tTemp = 0, tRampa = 0, tBuzzer = 0;   // Timers tareas
unsigned long tJson = 0, tOled = 0;                 // Timers tareas
unsigned long tServo = 0, tBloqueo = 0;             // Timers eventos
unsigned long tBuzzerToggle = 0;                    // Toggle 300ms

// ---------- 4. PROTOTIPOS (modularizacion rubrica) ----------
void   tareaTeclado(void);           // HMI no bloqueante
void   tareaTemperatura(void);       // LM35 + EMA + actuacion
void   tareaPIR(void);               // Logica intrusion
void   tareaMotor(void);             // Rampa PWM suave
void   tareaBuzzer(void);            // Intermitente 1kHz/300ms
void   tareaRele(void);              // Luces emergencia
void   tareaServo(void);             // Auto-cierre puerta
void   tareaOLED(void);              // Telemetria sin parpadeo
void   tareaJSON(void);              // Trama JSON \n
void   validarClave(void);           // Compara buffer vs CLAVE_OK
void   limpiarClave(void);           // Resetea buffer
void   abrirPuerta(void);            // Servo 90 + timer
void   cerrarPuerta(void);           // Servo 0
void   isrPIR(void);                 // ISR D2

// ============================================================
// SETUP
// ============================================================
void setup() {
  Serial.begin(9600);                       // COMPIM 9600 baudios
  pinMode(PIN_BUZZER, OUTPUT);              // Buzzer salida
  digitalWrite(PIN_BUZZER, LOW);            // Apagado inicial
  pinMode(PIN_RELE, OUTPUT);                // Rele salida
  digitalWrite(PIN_RELE, RELE_OFF);         // Luces OFF inicial
  pinMode(PIN_MOTOR, OUTPUT);               // Transistor motor
  analogWrite(PIN_MOTOR, 0);                // Motor detenido
  pinMode(PIN_PIR, INPUT);                  // HC-SR501 OUT digital
  cerrojo.attach(PIN_SERVO);                // Servo a D3
  cerrojo.write(0);                         // Cerrojo cerrado (0 grados)
  if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) { // Init OLED 0x3C
    Serial.println(F("{\"error\":\"oled_init_fail\"}")); // Reporta y sigue
  } else {
    oled.clearDisplay();                    // Limpia VRAM
    oled.setTextSize(1);                    // Fuente 6x8
    oled.setTextColor(SSD1306_WHITE);       // Solo blanco en SSD1306
    oled.setCursor(0, 0);                   // Origen
    oled.println(F("DATACENTER OK"));       // Splash
    oled.display();                         // Vuelca a pantalla
  }
  attachInterrupt(digitalPinToInterrupt(PIN_PIR), isrPIR, CHANGE); // ISR PIR
}

// ============================================================
// LOOP: despachador cooperativo, cero delay()
// ============================================================
void loop() {
  tareaTeclado();       // Poll teclado (no bloquea)
  tareaTemperatura();   // Muestreo + flags alarma + target PWM
  tareaPIR();           // Evalua intrusion
  tareaMotor();         // Rampa hacia objetivo
  tareaBuzzer();        // Patron sonoro sin bloquear
  tareaRele();          // Rele segun alarma
  tareaServo();         // Auto-cierre
  tareaOLED();          // Refresh 500ms
  tareaJSON();          // Envio 2000ms + '\n'
}

// ============================================================
// FUNCIONES DE TECLADO / SEGURIDAD
// ============================================================
void tareaTeclado(void) {
  if (bloqueado) {                                    // Ventana de bloqueo
    if (millis() - tBloqueo >= T_BLOQUEO) {           // Expiro castigo
      bloqueado = false;                              // Libera HMI
      intentosFallidos = 0;                           // Resetea contador
      limpiarClave();                                 // Limpia buffer
    } else return;                                    // Ignora teclas
  }
  char k = teclado.getKey();                          // No bloqueante ('\0' si nada)
  if (!k) return;                                     // Sin tecla, sale
  if (k >= '0' && k <= '9') {                         // Digito valido
    if (idxClave < 4) {                               // Espacio libre
      claveBuf[idxClave++] = k;                       // Guarda digito
      claveBuf[idxClave] = '\0';                      // Cierra string
    }
  } else if (k == '*') {                              // Borrar
    limpiarClave();                                   // Vacía buffer
  } else if (k == '#') {                              // Confirmar
    validarClave();                                   // Compara clave
  }
  // Teclas A,B,C,D se ignoran (reservadas futuro).
}

void validarClave(void) {
  if (strcmp(claveBuf, CLAVE_OK) == 0) {              // Clave correcta
    intentosFallidos = 0;                             // Resetea fallos
    abrirPuerta();                                    // Servo 90 + timer
  } else {                                            // Clave erronea
    intentosFallidos++;                               // Suma intento
    accesoAutorizado = false;                         // Sigue denegado
    if (intentosFallidos >= MAX_INTENTOS) {           // Fuerza bruta
      bloqueado = true;                               // Activa bloqueo
      tBloqueo = millis();                            // Marca inicio
    }
  }
  limpiarClave();                                     // Siempre limpia tras #
}

void limpiarClave(void) {
  memset(claveBuf, 0, sizeof(claveBuf));              // Cero buffer
  idxClave = 0;                                       // Cursor a 0
}

void abrirPuerta(void) {
  accesoAutorizado = true;                            // Flag acceso
  intrusion = false;                                  // Falsa intrusion previa se cancela
  cerrojo.write(90);                                  // Abre cerrojo
  tServo = millis();                                  // Inicia auto-cierre
}

void cerrarPuerta(void) {
  accesoAutorizado = false;                           // Vuelve a denegado
  cerrojo.write(0);                                   // Cierra cerrojo
}

void tareaServo(void) {
  if (accesoAutorizado && (millis() - tServo >= T_SERVO_OPEN)) // 5s pasaron
    cerrarPuerta();                                   // Auto-cierre
}

// ============================================================
// FUNCIONES DE SENSORES
// ============================================================
void isrPIR(void) {
  flagPIR = (digitalRead(PIN_PIR) == HIGH);           // Lee estado real (CHANGE)
}

void tareaPIR(void) {
  // Intrusion = movimiento Y puerta cerrada (no autorizado).
  // Si la puerta esta abierta por clave valida, no es intrusion.
  intrusion = (flagPIR && !accesoAutorizado);
}

void tareaTemperatura(void) {
  unsigned long ahora = millis();                     // Base tiempo
  if (ahora - tTemp < T_TEMP) return;                 // Muestrea cada 200ms
  tTemp = ahora;                                      // Recarga timer
  int adc = analogRead(PIN_LM35);                     // 0..1023
  float volt = adc * (5.0f / 1024.0f);                // A voltios (Vref 5V)
  float inst = volt * 100.0f;                         // LM35: 10mV/C
  if (tempC == 0.0f) tempC = inst;                    // Primera muestra
  else tempC = 0.8f * tempC + 0.2f * inst;            // Filtro EMA (anti-ruido)
  alarmaTemp = (tempC > TEMP_ALARMA);                 // Flag >40C
  if (tempC <= TEMP_EXT_ON) motorObjetivo = 0;         // Bajo umbral: off
  else {                                              // Control proporcional
    float m = (tempC - TEMP_EXT_ON) * 255.0f / (TEMP_EXT_MAX - TEMP_EXT_ON);
    motorObjetivo = (uint8_t)constrain(m, 0.0f, 255.0f); // Satura 0..255
  }
}

// ============================================================
// FUNCIONES DE ACTUADORES
// ============================================================
void tareaMotor(void) {
  unsigned long ahora = millis();                     // Base tiempo
  if (ahora - tRampa < T_RAMPA) return;               // Paso cada 50ms
  tRampa = ahora;                                     // Recarga
  if (motorActual < motorObjetivo) {                  // Debe acelerar
    uint8_t d = motorObjetivo - motorActual;          // Diferencia
    motorActual += (d > 5 ? 5 : d);                   // Rampa +5 suave
  } else if (motorActual > motorObjetivo) {           // Debe frenar
    uint8_t d = motorActual - motorObjetivo;          // Diferencia
    motorActual -= (d > 5 ? 5 : d);                   // Rampa -5 suave
  }
  analogWrite(PIN_MOTOR, motorActual);                // PWM al transistor
}

void tareaBuzzer(void) {
  alarmaActiva = (alarmaTemp || intrusion || bloqueado); // OR de alarmas
  if (!alarmaActiva) {                                // Sin alarma
    if (buzzerOn) { noTone(PIN_BUZZER); buzzerOn = false; } // Apaga una vez
    return;
  }
  unsigned long ahora = millis();                     // Base tiempo
  if (ahora - tBuzzerToggle >= T_BUZZER) {            // Cada 300ms alterna
    tBuzzerToggle = ahora;                            // Recarga
    buzzerOn = !buzzerOn;                             // Invierte estado
    if (buzzerOn) tone(PIN_BUZZER, 1000);             // 1kHz ON
    else noTone(PIN_BUZZER);                          // OFF
  }
}

void tareaRele(void) {
  bool debeEncender = (alarmaActiva || intrusion);    // Luces en alarma/movimiento
  if (debeEncender != releOn) {                       // Solo conmuta al cambiar
    releOn = debeEncender;                            // Actualiza estado
    digitalWrite(PIN_RELE, releOn ? RELE_ON : RELE_OFF); // Actua rele
  }
}

// ============================================================
// OLED (telemetria sin parpadeo: refresh 500ms)
// ============================================================
void tareaOLED(void) {
  unsigned long ahora = millis();                     // Base tiempo
  if (ahora - tOled < T_OLED) return;                 // Limita refresco
  tOled = ahora;                                      // Recarga
  oled.clearDisplay();                                // Limpia frame
  oled.setCursor(0, 0);                               // Fila 0
  oled.print(F("T:")); oled.print(tempC, 1);          // Temp 1 decimal
  oled.print(F("C M:")); oled.print(motorActual);     // PWM motor
  oled.setCursor(0, 10);                              // Fila 1
  oled.print(F("PIR:"));                              // Estado PIR
  oled.print(flagPIR ? F("MOV") : F("---"));
  oled.print(F(" R:"));                               // Rele
  oled.print(releOn ? F("ON ") : F("OFF"));
  oled.setCursor(0, 20);                              // Fila 2
  oled.print(F("ACC:"));                              // Acceso
  if (bloqueado) oled.print(F("BLOQUEADO"));
  else if (accesoAutorizado) oled.print(F("AUTORIZADO"));
  else oled.print(F("DENEGADO"));
  oled.setCursor(0, 30);                              // Fila 3
  if (alarmaTemp) oled.print(F("!ALTA TEMP"));
  else if (intrusion) oled.print(F("!INTRUSION"));
  else oled.print(F("SISTEMA OK"));
  oled.setCursor(0, 42);                              // Fila 4: eco clave
  oled.print(F("KEY:"));
  for (uint8_t i = 0; i < idxClave; i++) oled.print('*'); // No muestra digitos
  if (bloqueado) {                                    // Cuenta regresiva
    oled.setCursor(0, 52);
    oled.print(F("DESBLOQ:"));
    oled.print((T_BLOQUEO - (millis() - tBloqueo)) / 1000);
    oled.print(F("s"));
  }
  oled.display();                                     // Vuelca una sola vez
}

// ============================================================
// JSON (trama limpia terminada en \n, cada 2s)
// ============================================================
void tareaJSON(void) {
  unsigned long ahora = millis();                     // Base tiempo
  if (ahora - tJson < T_JSON) return;                 // Periodo 2s
  tJson = ahora;                                      // Recarga
  StaticJsonDocument<192> doc;                        // RAM medida para Uno
  doc[F("temp_c")] = round(tempC * 10.0f) / 10.0f;    // 1 decimal
  doc[F("pir")] = flagPIR ? 1 : 0;                    // 0/1 numerico
  doc[F("motor_pwm")] = motorActual;                  // 0..255
  doc[F("rele")] = releOn ? 1 : 0;                    // 0/1
  doc[F("alarma")] = alarmaActiva ? 1 : 0;            // 0/1
  if (bloqueado) doc[F("acceso")] = F("BLOQUEADO");   // Estado acceso
  else if (accesoAutorizado) doc[F("acceso")] = F("AUTORIZADO");
  else doc[F("acceso")] = F("DENEGADO");
  serializeJson(doc, Serial);                         // JSON compacto
  Serial.println();                                   // Delimitador \n
}
