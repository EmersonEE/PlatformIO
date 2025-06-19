#if defined(ESP32)
#include <WiFi.h>
#include <WiFiMulti.h>
#include <PantallitaXD.h>
#include "MSM.h"
#include "PWM.h"
PantallitaXD pantalla;
WiFiMulti wifiMulti;
#else
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#endif
PWM pwm(27);
#include <MQTT.h>
#include "data.h" // Aquí debes definir: ssid_1, password_1, etc.
#define LONGITUD_CARNET 9 // Longitud esperada del carnet
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
  // Validación más robusta
  if (payload.length() != LONGITUD_CARNET)
  {
    pantalla.showCarnetError(payload);
    Serial.printf("Longitud inválida: %d, esperado: %d\n", payload.length(), LONGITUD_CARNET);
    return;
  }

  // Verificar si es numérico (dependiendo de requisitos)
  for (char c : payload)
  {
    if (!isdigit(c))
    {
      pantalla.showCarnetError(payload);
      Serial.println("Contenido no numérico");
      return;
    }
  }

  if (payload.equals(payload_anterior))
  {
    Serial.println("Mensaje repetido, ignorado");
    return;
  }

  payload_anterior = payload;
  pantalla.showCarnet(payload);
  Serial.println("Pantalla actualizada: " + payload);
}

void brillo(const String &payload)
{
  // pantalla.showCarnet(payload);
  pwm.ledPWM(String(payload).toInt());
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
  else if (topic == "/brillo")
  {
    brillo(payload);
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
    // Feedback visual mejorado
    static uint8_t counter = 0;
    Serial.print(".");
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(250 + (counter++ % 3) * 100); // Patrón de espera variable
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("\nFallo en conexión WiFi");
    ESP.restart(); // O manejar de otra manera
    return;
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
  clienteMQTT.subscribe("/brillo");
}

void setup()
{
  Serial.begin(115200);
  pantalla.init();
  pantalla.showMessage("Iniciando...");
  pwm.init(0, 5000, 8); // Inicializa PWM en el canal 0, frecuencia 5000 Hz, resolución 8 bits

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
    // clienteMQTT.publish("/brillo", String(valor));
  }
}
