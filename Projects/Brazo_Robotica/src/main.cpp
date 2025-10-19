#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include "data.h"
#include <MQTT.h>
#include "NEMA17.h"
#include "Secuencias.h"
MQTTClient clienteMQTT;
WiFiMulti wifiMulti;
WiFiClient net;

unsigned long tiempoAnterior = 0;
unsigned long ultimoReintento = 0;
const unsigned long intervaloReintento = 5000; // 5 segundos
Secuencias secu;
uint8_t EN_PIN = 16;
uint8_t DIR_PIN = 2;
uint8_t STEP_PIN = 4;
uint8_t pinENBase = 23;
uint8_t pinDIRBase = 21;
uint8_t pinSTEPBase = 22;
uint8_t pinENEslabon1 = 19;
uint8_t pinSTEPEslabon1 = 18;
uint8_t pinDIREslabon1 = 17;
uint8_t pinENEslabon2 = 27;
uint8_t pinSTEPEslabon2 = 14;
uint8_t pinDIREslabon2 = 13;
float grados = 0;
float gradosEslabon1 = 0;
float gradosEslabon2 = 0;
// Configuración del motor
const int STEPS_PER_REV = 100; // Pasos por revolución física del motor
const int MICROSTEPS = 16;     // Microsteps configurados en el driver
const int TOTAL_STEPS = STEPS_PER_REV * MICROSTEPS;
int conteovueltas = 0;
// Sensibilidad del joystick
const int DEADZONE = 500;      // Zona muerta en torno al centro (0-4095 ADC)
const int STEP_DELAY_US = 500; // Retraso entre pasos (en microsegundos)
const int STEP_DELAY_US_HIGHT = 250;
// Objetos
float altura = 0;
NEMA17 motorAltura(EN_PIN, DIR_PIN, STEP_PIN);
NEMA17 motorBase(pinENBase, pinDIRBase, pinSTEPBase);
NEMA17 motorEslabon1(pinENEslabon1, pinDIREslabon1, pinSTEPEslabon1);
NEMA17 motorEslabon2(pinENEslabon2, pinDIREslabon2, pinSTEPEslabon2);

void MensajeMQTT(String topic, String payload)
{
  Serial.print("Topic: ");
  Serial.print(topic);
  Serial.print(" | Mensaje: ");
  Serial.println(payload);
  if (topic == "control/motor1")
  {
    Serial.println("Motor 1: " + payload);
    if (payload == "RIGHT")
    {
      Serial.println("Puto");
      grados = 360;
    }
    else if (payload == "LEFT")
    {
      Serial.println("Mierda");
      grados = -360;
    }
    motorBase.rotateDegrees(grados, TOTAL_STEPS, STEP_DELAY_US);

    Serial.println(grados);
  }
  // Motor Primer Eslabon
  else if (topic == "control/motor2")
  {
    if (payload == "RIGHT")
    {
      gradosEslabon1 = 45;
    }
    else if (payload == "LEFT")
    {
      gradosEslabon1 = -45;
    }
    motorEslabon1.rotateDegrees(gradosEslabon1, TOTAL_STEPS, STEP_DELAY_US);
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
    motorAltura.moveDistance(altura, TOTAL_STEPS, STEP_DELAY_US);
  }
  // Motor Segundo Eslabon
  else if (topic == "control/motor4")
  {
    if (payload == "RIGHT")
    {
      gradosEslabon2 = 180;
    }
    else if (payload == "LEFT")
    {
      gradosEslabon2 = -180;
    }
    motorEslabon2.rotateDegrees(gradosEslabon2, TOTAL_STEPS, STEP_DELAY_US);
  }
  else if (topic = "ESP/Secuencias")
  {
    Serial.println(payload);
    int secuenciaSel = payload.toInt();
    secu.setSecuencia(secuenciaSel);
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
}

void loop()
{

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