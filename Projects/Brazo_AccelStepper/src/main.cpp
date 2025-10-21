#include <Arduino.h>
#include <AccelStepper.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <MQTT.h>
#include "data.h"
#include "NEMA17.h"
MQTTClient clienteMQTT;
WiFiMulti wifiMulti;
WiFiClient net;

#define EN_ESLABON_1 19
#define DIR_ESLABON_1 17
#define STEP_ESLABON_1 18

#define EN_ESLABON_2 27
#define DIR_ESLABON_2 13
#define STEP_ESLABON_2 14

#define EN_BASE 23
#define DIR_BASE 21
#define STEP_BASE 22

#define EN_ALTURA 16
#define DIR_ALTURA 2
#define STEP_ALTURA 4

AccelStepper motorBase(AccelStepper::DRIVER, STEP_BASE, DIR_BASE);
// AccelStepper motorAltura(AccelStepper::DRIVER, STEP_ALTURA, DIR_ALTURA);
// NEMA17 motorAltura(EN_PIN, DIR_PIN, STEP_PIN);

NEMA17 motorAltura(EN_ALTURA, DIR_ALTURA, STEP_ALTURA);
AccelStepper motorEslabonUno(AccelStepper::DRIVER, STEP_ESLABON_1, DIR_ESLABON_1);
AccelStepper motorEslabonDos(AccelStepper::DRIVER, STEP_ESLABON_2, DIR_ESLABON_2);

const int PASOS_MOVIMIENTO = 1600;
const int PASOS_H = 3200;
const int PASOS_MOVIMIENTO_MEDIA = 800;

const int PASOS_MOVIMIENTO_CUARTO = 400;

const int VEL_MAX = 1000;
const int ACEL = 500;
const int VEL_MAX_H = 500;
const int ACEL_H = 250;
const int STEPS_PER_REV = 100; // Pasos por revolución física del motor
const int MICROSTEPS = 16;     // Microsteps configurados en el driver
const int TOTAL_STEPS = STEPS_PER_REV * MICROSTEPS;
const int STEP_DELAY_US = 500; // Retraso entre pasos (en microsegundos)
// === Variables ===
char comando = ' ';
bool enMovimiento = false;
bool movimientoBase = false;
bool movimientoAltura = false;
bool movimientoEslabonUno = false;
bool movimientoEslabonDos = false;

float grados = 0;
float gradosEslabon1 = 0;
float gradosEslabon2 = 0;
float altura = 0;
unsigned long tiempoAnterior = 0;
unsigned long ultimoReintento = 0;
const unsigned long intervaloReintento = 5000; // 5 segundos
void MensajeMQTT(String topic, String payload)

{
    motorBase.stop();
    // motorAltura.stop();

    Serial.print("Topic: ");
    Serial.print(topic);
    Serial.print(" | Mensaje: ");
    Serial.println(payload);
    if (topic == "control/motor1")
    {
        Serial.println("Motor 1: " + payload);
        if (payload == "RIGHT")
        {
            grados = 1;
        }
        else if (payload == "LEFT")
        {
            grados = -1;
        }
        // int gradosP = payload.toInt();
        Serial.println("→ Moviendo motorBase +1600 pasos");
        motorBase.move(grados * PASOS_MOVIMIENTO);
        movimientoBase = true;
        movimientoAltura = false;
        movimientoEslabonUno = false;
        movimientoEslabonDos = false;
        enMovimiento = true;
        Serial.println(grados);
    }
    // Motor Primer Eslabon
    else if (topic == "control/motor2")
    {
        if (payload == "RIGHT")
        {
            gradosEslabon1 = 1;
        }
        else if (payload == "LEFT")
        {
            gradosEslabon1 = -1;
        }

        motorEslabonUno.move(gradosEslabon1 * PASOS_MOVIMIENTO_CUARTO);
        movimientoBase = false;
        movimientoAltura = false;
        movimientoEslabonUno = true;
        movimientoEslabonDos = false;
        enMovimiento = true;
    }
    // Motor de Altura
    else if (topic == "control/motor3")
    {
        Serial.println("Motor 3: " + payload);
        if (payload == "UP")
        {
            altura = -10;
        }
        else if (payload == "DOWN")
        {
            altura = 10;
        }
        Serial.println(altura);
        Serial.println("→ Moviendo motorAltura +1600 pasos");
        // motorAltura.move(altura * PASOS_H);
        motorAltura.moveDistance(altura, TOTAL_STEPS, STEP_DELAY_US);

        movimientoBase = false;
        movimientoAltura = true;
        movimientoEslabonUno = false;
        movimientoEslabonDos = false;
        enMovimiento = true;
    }
    // Motor Segundo Eslabon
    else if (topic == "control/motor4")
    {
        if (payload == "RIGHT")
        {
            gradosEslabon2 = 1;
        }
        else if (payload == "LEFT")
        {
            gradosEslabon2 = -1;
        }
        motorEslabonDos.move(gradosEslabon2 * PASOS_MOVIMIENTO_CUARTO);
        movimientoBase = false;
        movimientoAltura = false;
        movimientoEslabonUno = false;
        movimientoEslabonDos = true;
        enMovimiento = true;
    }
    else if (topic == "control/motor5")
    {

        if (payload == "RIGHT")
        {
            gradosEslabon2 = 1;
        }
        else if (payload == "LEFT")
        {
            gradosEslabon2 = -1;
        }
        Serial.println("→ Moviendo  Los dos Eslabones al mismo Tiempo +1600 pasos");
        motorEslabonDos.move(gradosEslabon2 * PASOS_MOVIMIENTO_CUARTO);
        motorEslabonUno.move(gradosEslabon2 * PASOS_MOVIMIENTO_CUARTO);
        movimientoBase = false;
        movimientoAltura = false;
        movimientoEslabonUno = true;
        movimientoEslabonDos = true;
        enMovimiento = true;
    }
    else if (topic == "ESP/Secuencias")
    {
        Serial.println(payload);
        int secuenciaSel = payload.toInt();
        // secu.setSecuencia(secuenciaSel);
    }
    else
    {
        Serial.println("Tópico no manejado");
    }
}

void conectar()
{
    const unsigned long timeoutWiFi = 30000; // 30 segundos timeout
    unsigned long inicio = millis();

    Serial.print("Conectando con WiFi...");
    while (wifiMulti.run() != WL_CONNECTED && millis() - inicio < timeoutWiFi)
    {
        static uint8_t counter = 0;
        Serial.print(".");
        delay(200);
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("\nFallo en conexión WiFi");
        ESP.restart();
        return;
    }

    Serial.println("\nConectado a WiFi: " + WiFi.SSID());
    Serial.println("IP local: " + WiFi.localIP().toString());

    Serial.print("Conectando a MQTT...");
    while (!clienteMQTT.connect(NombreESP))
    {
        Serial.print("*");
        delay(200);
    }
    Serial.println("\nConectado a MQTT");

    clienteMQTT.subscribe("control/motor1");
    clienteMQTT.subscribe("control/motor2");
    clienteMQTT.subscribe("control/motor3");
    clienteMQTT.subscribe("control/motor4");
    clienteMQTT.subscribe("control/motor5");

    clienteMQTT.subscribe("ESP/Secuencias");
}

void setup()
{
    Serial.begin(115200);
    wifiMulti.addAP(ssid_1, password_1);
    wifiMulti.addAP(ssid_2, password_2);
    WiFi.mode(WIFI_STA);

    clienteMQTT.begin(BrokerMQTT, net);
    clienteMQTT.onMessage(MensajeMQTT);

    conectar();
    pinMode(EN_BASE, OUTPUT);
    pinMode(EN_ALTURA, OUTPUT);
    digitalWrite(EN_BASE, LOW);
    digitalWrite(EN_ALTURA, LOW);
    pinMode(EN_ESLABON_1, OUTPUT);
    pinMode(EN_ESLABON_2, OUTPUT);
    digitalWrite(EN_ESLABON_1, LOW);
    digitalWrite(EN_ESLABON_2, LOW);

    motorBase.setMaxSpeed(VEL_MAX);
    motorBase.setAcceleration(ACEL);
    // motorAltura.setMaxSpeed(VEL_MAX_H);
    // motorAltura.setAcceleration(ACEL_H);
    motorEslabonUno.setAcceleration(ACEL);
    motorEslabonUno.setMaxSpeed(VEL_MAX);
    motorEslabonDos.setAcceleration(ACEL);
    motorEslabonDos.setMaxSpeed(VEL_MAX);
}

void verificarMovimientoCompletado()
{
    bool baseCompleto = (motorBase.distanceToGo() == 0);
    // bool alturaCompleto = (motorAltura.distanceToGo() == 0);
    bool eslabonUnoCompleto = (motorEslabonUno.distanceToGo() == 0);
    bool eslabonDosCompleto = (motorEslabonDos.distanceToGo() == 0);

    if (movimientoBase && baseCompleto)
    {
        Serial.println("✔ Motor Base completó su movimiento");
        movimientoBase = false;
    }

    // if (movimientoAltura && alturaCompleto)
    // {
    //     Serial.println("✔ Motor Altura completó su movimiento");
    //     movimientoAltura = false;
    // }
    if (movimientoEslabonUno && eslabonUnoCompleto)
    {
        Serial.println("✔ Motor Eslabon Uno completó su movimiento");
        movimientoEslabonUno = false;
    }
    if (movimientoEslabonDos && eslabonDosCompleto)
    {
        Serial.println("✔ Motor Eslabon Dos completó su movimiento");
        movimientoEslabonDos = false;
    }

    // Si ningún motor está en movimiento, actualizar estado general
    if (!movimientoBase && !movimientoAltura && !movimientoEslabonUno && !movimientoEslabonDos && enMovimiento)
    {
        enMovimiento = false;
        Serial.println("--- Todos los movimientos completados ---");
    }
}

void loop()
{
    // // === Leer comandos desde Serial ===
    // if (Serial.available() > 0)
    // {
    //     comando = Serial.read();
    //     procesarComando(comando);
    // }

    // === Ejecutar movimiento ===
    if (enMovimiento)
    {
        if (movimientoBase)
        {
            motorBase.run();
        }
        // if (movimientoAltura)
        // {
        //     motorAltura.run();
        // }
        if (movimientoEslabonUno)
        {
            motorEslabonUno.run();
        }
        if (movimientoEslabonDos)
        {
            motorEslabonDos.run();
        }
        // Verificar si los movimientos han terminado
        verificarMovimientoCompletado();
    }
    clienteMQTT.loop();
    delay(10);

    if (!clienteMQTT.connected())
    {
        if (millis() - ultimoReintento > intervaloReintento)
        {
            ultimoReintento = millis();
            conectar();
        }
        return;
    }
}