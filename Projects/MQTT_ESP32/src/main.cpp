#if defined(ESP32)
#include <WiFi.h>
#include <WiFiMulti.h>
#include <PantallitaXD.h>
#include "MSM.h"
PantallitaXD pantalla;
WiFiMulti wifiMulti;
#else
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#endif

#include <MQTT.h>
#include "data.h" // Aquí debes definir: ssid_1, password_1, etc.

WiFiClient net;
MQTTClient clienteMQTT;

MSM mensajes;

int pot = 33;
int lectura = 0;
unsigned long tiempoAnterior = 0;
unsigned long ultimoReintento = 0;
const unsigned long intervaloReintento = 5000; // 5 segundos
String payload_anterior = "";
int ledazul = 14;

void manejarMensajeSuscribirse(const String &payload)
{
  if (payload == "0")
  {
    digitalWrite(ledazul, HIGH);
    Serial.println("LED azul ENCENDIDO");
    delay(1500);
  }
  else if (payload == "1")
  {
    digitalWrite(ledazul, LOW);
    Serial.println("LED azul APAGADO");
  }
  else
  {
    Serial.println("Payload inválido en /leds");
  }
}

void manejarMensajePantalla(const String &payload)
{

  if (payload != payload_anterior && payload.length() ==9)
  {
    Serial.println(payload);
    payload_anterior = payload;
    pantalla.showCarnet(payload);
  }
  else if (payload.length() <= 8 || payload.length() > 9)
  {
    
    pantalla.showCarnetError(payload);
  }
  else
  {
    Serial.println("Mensaje repetido, no se actualiza pantalla");
    return;
  }
}

void MensajeMQTT(String topic, String payload)
{
  Serial.print("Topic: ");
  Serial.print(topic);
  Serial.print(" | Mensaje: ");
  Serial.println(payload);

  if (topic == "/leds")
  {
    manejarMensajeSuscribirse(payload);
  }
  else if (topic == "/pantalla")
  {
    manejarMensajePantalla(payload);
  }
  else
  {
    Serial.println("Tópico no manejado");
  }
}

void conectar()
{
  Serial.print("Conectando con WiFi...");
  while (wifiMulti.run() != WL_CONNECTED)
  {
    Serial.print(".");
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }

  Serial.println("\nConectado a WiFi: " + WiFi.SSID());
  Serial.println("IP local: " + WiFi.localIP().toString());

  Serial.print("Conectando a MQTT...");
  while (!clienteMQTT.connect(NombreESP))
  {
    Serial.print("*");
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
  }
  Serial.println("\nConectado a MQTT");

  // ✅ Suscripciones
  clienteMQTT.subscribe("/leds");
  clienteMQTT.subscribe("/pantalla");
}

void setup()
{
  Serial.begin(115200);
  pantalla.init();
  pantalla.showMessage("Iniciando...");

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ledazul, OUTPUT);
  digitalWrite(ledazul, LOW);
  pinMode(pot, INPUT);

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

  if (millis() - tiempoAnterior > 1000)
  {
    tiempoAnterior = millis();
    int valor = map(analogRead(pot), 0, 4095, 0, 100);
    clienteMQTT.publish("/saludo", String(valor));
  }
}
