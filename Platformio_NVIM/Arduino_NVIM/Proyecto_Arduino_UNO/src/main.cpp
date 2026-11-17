// =========================================================================
// PROYECTO: Sistema de Mision Critica de Grado Industrial (Data Center)
// DISENO Y ARQUITECTURA:
// - Paradigma 100% Asincrono y Cooperativo (Cero llamadas a delay()).
// - Base de tiempo distribuida con millis() y maquinas de estados finitos (FSM).
// - Optimizacion extrema de SRAM para ATmega328P (2 KB) con PROGMEM (macro F()).
// - Driver OLED U8g2 en MODO PAGINA: buffer de video de 128 B (no 1024 B),
//   imprescindible para que la pantalla quepa en la SRAM del ATmega328P.
// - Diseno listo para fabricacion en PCB de KiCad y co-simulacion en Proteus.
// =========================================================================

#include <Arduino.h>            // Nucleo base de Arduino
#include <Wire.h>               // Bus I2C maestro para pantalla OLED
#include <U8g2lib.h>            // Driver OLED SSD1306 en modo pagina (128 B de SRAM)
#include <Keypad.h>             // Manejador no bloqueante de matriz 4x4
#include <Servo.h>              // Control de pulso PWM para servomotores
#include <ArduinoJson.h>        // Serializador y parseador formal de JSON

// =========================================================================
// 1. DEFINICIONES DE PINES Y ASIGNACION DE HARDWARE (PCB READY)
// =========================================================================
// NOTA DE INGENIERIA SENIOR (Resolucion de Colision de Pines y Timers):
// En la especificacion inicial, D5 se asigno simultaneamente a la Columna 3
// del Teclado y a la base del Transistor del Motor DC.
// - D5 es la salida PWM del Timer0 (OC0B). Timer0 NO se ve afectado por
//   Servo.h (que secuestra Timer1) ni por tone() (que secuestra Timer2).
// - Por lo tanto, D5 DEBE reservarse exclusivamente para el PWM del Motor DC.
// - Para resolver el conflicto sin anular ningun periferico, la Columna 3 del
//   teclado se reasigna al pin A1 (Pin Digital 15 del ATmega328P).
// Con esta arquitectura, no hay colision de pines ni conflicto de temporizadores.

#define PIN_LM35        A0      // Sensor de Temperatura LM35 (Entrada Analogica ADC0)
#define PIN_PIR         2       // Sensor de Movimiento PIR HC-SR501 (INT0 - Pin D2)
#define PIN_SERVO       3       // Servomotor Cerrojo SG90 (Control de pulso en D3)
#define PIN_MOTOR_PWM   5       // Motor DC Extractor (Salida PWM Timer0 en D5)
#define PIN_RELE        12      // Modulo Rele Luces de Emergencia (Salida Digital D12)
#define PIN_BUZZER      13      // Buzzer de Alerta Acustica (Salida Digital / tone() D13)

// Configuracion de polaridad del Rele (Modulos comerciales tipicamente activos en LOW)
#define RELE_ACTIVADO   HIGH     // Rele cerrado / Luces encendidas
#define RELE_APAGADO    LOW    // Rele abierto / Luces apagadas

// =========================================================================
// 2. CONSTANTES Y PARAMETROS CRITICOS DEL SISTEMA
// =========================================================================
#define OLED_DIRECCION  0x3C    // Direccion I2C de la OLED (0x3C o 0x3D segun SA0)
#define OLED_ANCHO      128     // Resolucion horizontal del OLED en pixeles
#define OLED_ALTO       64      // Resolucion vertical del OLED en pixeles
#define OLED_FUENTE     u8g2_font_6x10_tr // Fuente 6x10 alojada en PROGMEM (ASCII)

#define TEMP_ARRANQUE   30.0f   // Temperatura (°C) donde el extractor inicia marcha
#define TEMP_MAXIMA     45.0f   // Temperatura (°C) donde el extractor llega al 100% PWM
#define TEMP_CRITICA    40.0f   // Umbral de temperatura critica para alarma de incendio

#define CLAVE_MAESTRA   "1234"  // Clave de seguridad autorizada para apertura
#define MAX_FALLOS      3       // Numero maximo de intentos antes de activar bloqueo
#define TIEMPO_PUERTA   5000UL  // Tiempo que la puerta permanece abierta (5 segundos)
#define TIEMPO_BLOQUEO  30000UL // Tiempo de castigo por fuerza bruta (30 segundos)

#define INTERVALO_JSON  2000UL  // Periodo de envio de telemetria serial JSON (2 segundos)
#define INTERVALO_OLED  350UL   // Refresco de pantalla OLED (evita flickering perceptible)
#define INTERVALO_TEMP  150UL   // Periodo de lectura y filtrado del sensor analogico LM35
#define INTERVALO_RAMPA 40UL    // Tasa de actualizacion de la rampa de aceleracion PWM
#define CADENCIA_BUZZER 300UL   // Intermitencia de alerta: 300ms ON / 300ms OFF

// =========================================================================
// 3. OBJETOS Y VARIABLES GLOBALES DEL SISTEMA
// =========================================================================
// Instancia del controlador de pantalla OLED (hardware I2C, sin pin de reset).
// El sufijo "_1_" habilita el MODO PAGINA: el buffer de video en SRAM es de
// solo 128 bytes (una pagina de 128x8 px). Un framebuffer completo exigiria
// 1024 bytes y el malloc() de Adafruit_SSD1306 fallaba en el ATmega328P.
U8G2_SSD1306_128X64_NONAME_1_HW_I2C pantalla(U8G2_R0, U8X8_PIN_NONE);

// Instancia para el control del servomotor del cerrojo
Servo servomotor;

// Configuracion del Teclado Matricial 4x4
const byte FILAS = 4;
const byte COLUMNAS = 4;
char mapaTeclado[FILAS][COLUMNAS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};
byte pinesFilas[FILAS]    = {11, 10, 9, 8};   // Conectados a D11, D10, D9, D8
byte pinesColumnas[COLUMNAS] = {7, 6, A1, 4}; // Col 3 en A1 para liberar Pin D5 (PWM)
Keypad teclado = Keypad(makeKeymap(mapaTeclado), pinesFilas, pinesColumnas, FILAS, COLUMNAS);

// Banderas de estado del sistema (Variables volatiles para interrupcion)
volatile bool flagMovimientoPIR = false; // Flag modificado unicamente dentro de la ISR

// Variables de estado del HMI y Seguridad
char bufferClave[5] = {0};              // Almacena hasta 4 caracteres + caracter nulo
uint8_t indiceClave = 0;                // Indice del cursor en el buffer de clave
uint8_t contadorFallos = 0;             // Contador de intentos fallidos consecutivos
bool sistemaBloqueado = false;          // Estado de bloqueo por ataque de fuerza bruta
bool accesoConcedido = false;           // Estado de autorizacion de ingreso a la boveda

// Variables de estado de Alarma y Actuadores
bool alarmaTemperatura = false;         // Flag de temperatura por encima de 40 °C
bool alarmaIntrusion = false;           // Flag de deteccion de intruso no autorizado
bool estadoAlarmaGeneral = false;       // OR logico de cualquier condicion de alarma
bool estadoSonoroBuzzer = false;        // Estado actual del oscilador del buzzer (ON/OFF)
bool estadoReleLuces = false;           // Estado logico de las luces de emergencia
uint8_t pwmMotorActual = 0;             // Valor real de PWM aplicado al transistor (0-255)
uint8_t pwmMotorDeseado = 0;            // Valor objetivo calculado por la ley de control
float temperaturaC = 0.0f;              // Temperatura procesada con filtro EMA

// Marcas de tiempo para programacion concurrente (millis())
unsigned long marcaTiempoTemp = 0;      // Control de lectura del LM35
unsigned long marcaTiempoRampa = 0;     // Control de aceleracion suave del motor DC
unsigned long marcaTiempoBuzzer = 0;    // Control de intermitencia del buzzer
unsigned long marcaTiempoJson = 0;      // Control de emision de telemetria JSON
unsigned long marcaTiempoOled = 0;      // Control de refresco visual OLED
unsigned long marcaTiempoServo = 0;     // Control de temporizador de cierre de puerta
unsigned long marcaTiempoBloqueo = 0;   // Control del castigo de 30 segundos por bloqueo

// =========================================================================
// 4. PROTOTIPOS DE FUNCIONES MODULARES (RÚBRICA DE ARQUITECTURA)
// =========================================================================
void manejarTecladoSeguridad(void);
void evaluarClaveIngresada(void);
void resetearBufferClave(void);
void activarAperturaPuerta(void);
void ejecutarCierrePuerta(void);
void procesarTemporizadorPuerta(void);

void procesarSensorTemperatura(void);
void isrSensorMovimiento(void);
void evaluarLogicaIntrusion(void);

void actualizarRampaMotorDC(void);
void controlarAlertaSonora(void);
void controlarIluminacionEmergencia(void);

void actualizarPantallaOLED(void);
void emitirTelemetriaJSON(void);

// =========================================================================
// 5. FUNCION DE CONFIGURACION (SETUP)
// =========================================================================
void setup() {
    // 1. Iniciar puerto serie a 9600 baudios (estandar COMPIM / TeraTerm)
    Serial.begin(9600);

    // 2. Configuracion de Entradas y Salidas Digitales
    pinMode(PIN_BUZZER, OUTPUT);
    digitalWrite(PIN_BUZZER, LOW);

    pinMode(PIN_RELE, OUTPUT);
    digitalWrite(PIN_RELE, RELE_APAGADO);

    pinMode(PIN_MOTOR_PWM, OUTPUT);
    analogWrite(PIN_MOTOR_PWM, 0); // Motor apagado al arranque

    pinMode(PIN_PIR, INPUT);       // HC-SR501 con senal logica TTL

    // 3. Inicializacion del Servomotor electromecanico
    servomotor.attach(PIN_SERVO);
    servomotor.write(0);           // Cerrojo cerrado inicialmente (0 grados)

    // 4. Inicializacion de la Pantalla OLED SSD1306 (I2C, driver U8g2 modo pagina)
    // Verificacion explicita de presencia fisica: U8g2 no propaga el NACK del
    // bus en begin(), por lo que se sondea antes con Wire para mantener la
    // alerta de telemetria OLED_I2C_INIT_FAILED.
    Wire.begin();
    Wire.beginTransmission(OLED_DIRECCION);
    bool oledPresente = (Wire.endTransmission() == 0);

    // U8g2 trabaja con la direccion I2C en formato de 8 bits (0x3C << 1 = 0x78)
    pantalla.setI2CAddress(OLED_DIRECCION << 1);

    if (!oledPresente || !pantalla.begin()) {
        // En caso de fallo de hardware en el bus I2C, emitir alerta por puerto serie
        Serial.println(F("{\"error\":\"OLED_I2C_INIT_FAILED\"}"));
    } else {
        pantalla.setFont(OLED_FUENTE);
        pantalla.setFontMode(1); // Texto transparente (sin caja de fondo)
        pantalla.firstPage();
        do {
            pantalla.setCursor(19, 28);
            pantalla.print(F("DATA CENTER OPS"));
            pantalla.setCursor(16, 44);
            pantalla.print(F("INICIALIZANDO..."));
        } while (pantalla.nextPage());
    }

    // 5. Configuracion de Interrupcion Externa para Sensor PIR (INT0 en Pin D2)
    // Se utiliza CHANGE para registrar tanto el inicio como el fin de la deteccion
    attachInterrupt(digitalPinToInterrupt(PIN_PIR), isrSensorMovimiento, CHANGE);
}

// =========================================================================
// 6. BUCLE PRINCIPAL (LOOP COOPERATIVO NO BLOQUEANTE)
// =========================================================================
void loop() {
    // Despachador ciclico de tareas asincronas guiadas por eventos y temporizadores
    manejarTecladoSeguridad();          // Gestion de HMI y entrada de clave
    procesarTemporizadorPuerta();      // Control de auto-cierre del cerrojo
    procesarSensorTemperatura();      // Lectura ADC y filtrado de temperatura
    evaluarLogicaIntrusion();         // Evaluacion de seguridad basada en PIR
    actualizarRampaMotorDC();         // Modulacion PWM suave del extractor
    controlarAlertaSonora();          // Generacion de patrones acusticos 1kHz
    controlarIluminacionEmergencia();  // Control del modulo de rele
    actualizarPantallaOLED();          // Refresco de telemetria en OLED sin jitter
    emitirTelemetriaJSON();           // Transmision serial de datos en JSON limpio
}

// =========================================================================
// 7. SUBSISTEMA A: GESTION DE TECLADO Y ACCESO DE SEGURIDAD (HMI)
// =========================================================================

// Tarea para leer teclas de forma no bloqueante y gestionar el estado de bloqueo
void manejarTecladoSeguridad(void) {
    // Si el sistema esta bloqueado por exceso de intentos, verificar si expiro el castigo
    if (sistemaBloqueado) {
        if (millis() - marcaTiempoBloqueo >= TIEMPO_BLOQUEO) {
            sistemaBloqueado = false;
            contadorFallos = 0;
            resetearBufferClave();
        } else {
            return; // Bloqueo activo: se descarta cualquier pulsacion en el teclado
        }
    }

    char teclaPresionada = teclado.getKey(); // Lectura asincrona instantanea
    if (teclaPresionada == NO_KEY) {
        return; // No se ha presionado ninguna tecla en este ciclo
    }

    // Clasificacion del caracter ingresado
    if (teclaPresionada >= '0' && teclaPresionada <= '9') {
        // Ingreso de digitos numericos para la clave (maximo 4 caracteres)
        if (indiceClave < 4) {
            bufferClave[indiceClave++] = teclaPresionada;
            bufferClave[indiceClave] = '\0'; // Mantener terminador nulo valido
        }
    } else if (teclaPresionada == '*') {
        // Tecla de cancelacion / borrado de buffer
        resetearBufferClave();
    } else if (teclaPresionada == '#') {
        // Tecla de confirmacion / validacion de clave
        if (indiceClave > 0) {
            evaluarClaveIngresada();
        }
    }
}

// Compara el buffer ingresado con la clave maestra autorizada
void evaluarClaveIngresada(void) {
    if (strcmp(bufferClave, CLAVE_MAESTRA) == 0) {
        // Acceso Correcto: conceder entrada y abrir cerrojo
        contadorFallos = 0;
        activarAperturaPuerta();
    } else {
        // Clave Invalida: registrar fallo y verificar posible ataque
        contadorFallos++;
        accesoConcedido = false;
        if (contadorFallos >= MAX_FALLOS) {
            sistemaBloqueado = true;
            marcaTiempoBloqueo = millis(); // Registrar inicio del periodo de castigo
        }
    }
    resetearBufferClave(); // Limpiar buffer de entrada tras la validacion
}

// Limpia el buffer de clave de memoria de forma segura
void resetearBufferClave(void) {
    memset(bufferClave, 0, sizeof(bufferClave));
    indiceClave = 0;
}

// Abre el cerrojo mediante el servomotor y programa el temporizador de auto-cierre
void activarAperturaPuerta(void) {
    accesoConcedido = true;
    alarmaIntrusion = false;       // Cancelar falsa alarma si fue autorizada
    servomotor.write(90);          // Desplazar cerrojo a 90 grados (Abierto)
    marcaTiempoServo = millis();   // Disparar temporizador de auto-cierre
}

// Devuelve el cerrojo a la posicion de bloqueo
void ejecutarCierrePuerta(void) {
    accesoConcedido = false;
    servomotor.write(0);           // Cerrojo en 0 grados (Cerrado)
}

// Controla el cierre automatico transcurridos 5 segundos
void procesarTemporizadorPuerta(void) {
    if (accesoConcedido) {
        if (millis() - marcaTiempoServo >= TIEMPO_PUERTA) {
            ejecutarCierrePuerta();
        }
    }
}

// =========================================================================
// 8. SUBSISTEMA B: LECTURA Y EVALUACION DE SENSORES
// =========================================================================

// Rutina de Servicio de Interrupcion (ISR) para el Sensor PIR
void isrSensorMovimiento(void) {
    flagMovimientoPIR = (digitalRead(PIN_PIR) == HIGH);
}

// Evalua condiciones de intrusion con base en el sensor PIR y credenciales
void evaluarLogicaIntrusion(void) {
    // Si se detecta movimiento mientras el acceso NO ha sido autorizado formalmente,
    // se cataloga como evento de intrusion no autorizada en el Data Center.
    if (flagMovimientoPIR && !accesoConcedido) {
        alarmaIntrusion = true;
    } else if (accesoConcedido) {
        // Si la persona ingreso con clave autorizada, su presencia es valida
        alarmaIntrusion = false;
    }
}

// Muestrea el LM35, aplica filtro digital EMA y calcula el objetivo PWM del extractor
void procesarSensorTemperatura(void) {
    unsigned long instanteActual = millis();
    if (instanteActual - marcaTiempoTemp < INTERVALO_TEMP) {
        return; // Periodo de muestreo no alcanzado
    }
    marcaTiempoTemp = instanteActual;

    // Lectura del canal analogico ADC0
    int lecturaADC = analogRead(PIN_LM35);

    // Conversion a milivoltios y luego a grados Celsius (LM35 = 10 mV/°C)
    // Con Vref = 5.0V: Voltaje = lectura * (5000 mV / 1024) -> Temp = Voltaje / 10
    float lecturaInstantanea = (lecturaADC * 500.0f) / 1024.0f;

    // Filtro Digital Pasa-Bajas: Media Movil Exponencial (EMA) para supresion de ruido
    if (temperaturaC <= 0.1f) {
        temperaturaC = lecturaInstantanea; // Condicion inicial
    } else {
        temperaturaC = (0.75f * temperaturaC) + (0.25f * lecturaInstantanea);
    }

    // Evaluacion del umbral critico de alarma termica (> 40.0 °C)
    alarmaTemperatura = (temperaturaC > TEMP_CRITICA);

    // Calculo del PWM objetivo para el motor DC (Extractor)
    // Por debajo de 30 °C: Extractor apagado (0)
    // Entre 30 °C y 45 °C: Escalado proporcional (0 a 255)
    // Por encima de 45 °C: Maxima potencia (255)
    if (temperaturaC <= TEMP_ARRANQUE) {
        pwmMotorDeseado = 0;
    } else if (temperaturaC >= TEMP_MAXIMA) {
        pwmMotorDeseado = 255;
    } else {
        float factor = (temperaturaC - TEMP_ARRANQUE) / (TEMP_MAXIMA - TEMP_ARRANQUE);
        pwmMotorDeseado = (uint8_t)(factor * 255.0f);
    }
}

// =========================================================================
// 9. SUBSISTEMA B: CONTROL DE ACTUADORES Y POTENCIA
// =========================================================================

// Rampa suave de aceleracion y desaceleracion PWM para proteger la etapa de potencia
void actualizarRampaMotorDC(void) {
    unsigned long instanteActual = millis();
    if (instanteActual - marcaTiempoRampa < INTERVALO_RAMPA) {
        return;
    }
    marcaTiempoRampa = instanteActual;

    // Incremento o decremento gradual del duty cycle
    const uint8_t PASO_RAMPA = 5;
    if (pwmMotorActual < pwmMotorDeseado) {
        if ((uint16_t)pwmMotorActual + PASO_RAMPA >= pwmMotorDeseado) {
            pwmMotorActual = pwmMotorDeseado;
        } else {
            pwmMotorActual += PASO_RAMPA;
        }
    } else if (pwmMotorActual > pwmMotorDeseado) {
        if (pwmMotorActual <= PASO_RAMPA || (pwmMotorActual - PASO_RAMPA) <= pwmMotorDeseado) {
            pwmMotorActual = pwmMotorDeseado;
        } else {
            pwmMotorActual -= PASO_RAMPA;
        }
    }

    // Aplicar senal PWM en Pin D5 (Timer0)
    analogWrite(PIN_MOTOR_PWM, pwmMotorActual);
}

// Genera un patron de alarma acustica de 1kHz intermitente (300ms ON / 300ms OFF)
// Implementado 100% sin delay() mediante comparacion de temporizadores
void controlarAlertaSonora(void) {
    // La alarma general se activa si hay temperatura critica, intrusion o bloqueo HMI
    estadoAlarmaGeneral = (alarmaTemperatura || alarmaIntrusion || sistemaBloqueado);

    if (!estadoAlarmaGeneral) {
        // Si no existen condiciones de alarma, garantizar silencio
        if (estadoSonoroBuzzer) {
            noTone(PIN_BUZZER);
            estadoSonoroBuzzer = false;
        }
        return;
    }

    // Gestion del oscilador con cadencia de 300 ms
    unsigned long instanteActual = millis();
    if (instanteActual - marcaTiempoBuzzer >= CADENCIA_BUZZER) {
        marcaTiempoBuzzer = instanteActual;
        estadoSonoroBuzzer = !estadoSonoroBuzzer;

        if (estadoSonoroBuzzer) {
            tone(PIN_BUZZER, 1000); // Emite onda cuadrada de 1000 Hz
        } else {
            noTone(PIN_BUZZER);     // Silencio en el periodo bajo
        }
    }
}

// Activa el modulo de rele para iluminacion de emergencia en eventos criticos
void controlarIluminacionEmergencia(void) {
    // Las luces se encienden si hay alarma o durante el acceso autorizado a la boveda
    bool activarLuces = (estadoAlarmaGeneral || accesoConcedido);

    if (activarLuces != estadoReleLuces) {
        estadoReleLuces = activarLuces;
        digitalWrite(PIN_RELE, estadoReleLuces ? RELE_ACTIVADO : RELE_APAGADO);
    }
}

// =========================================================================
// 10. INTERFAZ VISUAL: PANTALLA OLED SSD1306 (U8G2 MODO PAGINA)
// -------------------------------------------------------------------------
// U8g2 en modo pagina dibuja la pantalla barriendo 8 paginas de 128x8 px:
// el buffer de video en SRAM es de solo 128 bytes, frente a los 1024 bytes
// del framebuffer completo de Adafruit_SSD1306 (que el malloc() no podia
// satisfacer en el ATmega328P). El bucle firstPage()/nextPage() limpia y
// transfiere cada pagina de forma automatica, sin parpadeo percibible.
// NOTA: las coordenadas Y de setCursor() son la LINEA BASE del texto.
// =========================================================================
void actualizarPantallaOLED(void) {
    unsigned long instanteActual = millis();
    if (instanteActual - marcaTiempoOled < INTERVALO_OLED) {
        return; // Control de tasa de refresco
    }
    marcaTiempoOled = instanteActual;

    // U8g2 no imprime flotantes directamente: se formatea el valor a texto
    char textoTemperatura[8];
    dtostrf(temperaturaC, 4, 1, textoTemperatura); // Ej: "23.4"

    // Cuenta regresiva del castigo HMI (se calcula solo si hay bloqueo activo)
    unsigned long segundosRestantes = 0;
    if (sistemaBloqueado) {
        unsigned long tiempoTranscurrido = millis() - marcaTiempoBloqueo;
        segundosRestantes = (TIEMPO_BLOQUEO > tiempoTranscurrido) ?
                            ((TIEMPO_BLOQUEO - tiempoTranscurrido) / 1000UL) : 0;
    }

    pantalla.setFont(OLED_FUENTE);
    pantalla.setFontMode(1); // Texto transparente (sin caja de fondo)

    // Barrido de paginas: cada iteracion transfiere 8 filas al SSD1306
    pantalla.firstPage();
    do {
        // Encabezado con Temperatura y PWM
        pantalla.setCursor(0, 8);
        pantalla.print(F("TEMP: "));
        pantalla.print(textoTemperatura);
        pantalla.print(F("C"));

        pantalla.setCursor(78, 8);
        pantalla.print(F("PWM:"));
        pantalla.print(pwmMotorActual);

        // Estado del PIR y del Rele
        pantalla.setCursor(0, 20);
        pantalla.print(F("PIR: "));
        pantalla.print(flagMovimientoPIR ? F("DETECTADO") : F("NORMAL"));

        pantalla.setCursor(88, 20);
        pantalla.print(F("R:"));
        pantalla.print(estadoReleLuces ? F("ON") : F("OFF"));

        // Estado de Seguridad y Acceso
        pantalla.setCursor(0, 32);
        pantalla.print(F("ACCESO: "));
        if (sistemaBloqueado) {
            pantalla.print(F("BLOQUEADO"));
        } else if (accesoConcedido) {
            pantalla.print(F("AUTORIZADO"));
        } else {
            pantalla.print(F("DENEGADO"));
        }

        // Estado del Sistema de Alarma
        pantalla.setCursor(0, 44);
        pantalla.print(F("ALERTA: "));
        if (alarmaTemperatura) {
            pantalla.print(F("!ALTA TEMP!"));
        } else if (alarmaIntrusion) {
            pantalla.print(F("!INTRUSION!"));
        } else if (sistemaBloqueado) {
            pantalla.print(F("!BLOQUEO HMI!"));
        } else {
            pantalla.print(F("NORMAL"));
        }

        // Entrada de clave con asteriscos para privacidad
        pantalla.setCursor(0, 56);
        pantalla.print(F("CLAVE: ["));
        for (uint8_t i = 0; i < indiceClave; i++) {
            pantalla.print('*');
        }
        for (uint8_t i = indiceClave; i < 4; i++) {
            pantalla.print(' ');
        }
        pantalla.print(F("]"));

        // Mensaje de cuenta regresiva si el sistema esta castigado
        if (sistemaBloqueado) {
            pantalla.setCursor(78, 56);
            pantalla.print(F("T-"));
            pantalla.print(segundosRestantes);
            pantalla.print(F("s"));
        }
    } while (pantalla.nextPage());
}

// =========================================================================
// 11. SUBSISTEMA C: SERIALIZACION E INGESTA JSON (TELEMETRIA CRITICA)
// =========================================================================
void emitirTelemetriaJSON(void) {
    unsigned long instanteActual = millis();
    if (instanteActual - marcaTiempoJson < INTERVALO_JSON) {
        return; // Periodo de emision de 2 segundos
    }
    marcaTiempoJson = instanteActual;

    // Asignacion de documento JSON estatico en la pila (SRAM segura, sin heap)
    StaticJsonDocument<200> docJson;

    // Empaquetado formal de variables
    docJson[F("temp_c")]     = round(temperaturaC * 10.0f) / 10.0f;
    docJson[F("pir")]        = flagMovimientoPIR ? 1 : 0;
    docJson[F("motor_pwm")]  = pwmMotorActual;
    docJson[F("rele")]       = estadoReleLuces ? 1 : 0;
    docJson[F("alarma")]     = estadoAlarmaGeneral ? 1 : 0;

    if (sistemaBloqueado) {
        docJson[F("acceso")] = F("BLOQUEADO");
    } else if (accesoConcedido) {
        docJson[F("acceso")] = F("AUTORIZADO");
    } else {
        docJson[F("acceso")] = F("DENEGADO");
    }

    // Serializacion compacta directamente hacia el bus UART
    serializeJson(docJson, Serial);
    Serial.println(); // Delimitador estricto de salto de linea '\n'
}
