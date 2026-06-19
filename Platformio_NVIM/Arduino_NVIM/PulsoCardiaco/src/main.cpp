#include "HardwareSerial.h"
#include "drivers.h"
#include "wificonf.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>

Driver ledOk(LED_BUILTIN);
WIFI_CONF wf;

#define SENSOR_PIN A0
const long fingerThreshold = 150;
const long signalMax = 1000;

const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
bool peakDetected = false;

float signalFiltered = 0;
const float alphaSignal = 0.9;

float beatsPerMinute = 0;
int beatAvg = 0;
float lastValidBPM = 0;

const char *host = "192.168.1.136";
const int port = 80;
const unsigned long interval = 5000;
unsigned long previousMillis = 0;
unsigned long lastSampleTime = 0;
int currentPatientId = 1; // Por defecto 1
String currentToken = "";
bool isSessionActive = false;
unsigned long lastSessionCheck = 0;
void verificarSesion() {
  if (millis() - lastSessionCheck < 3000)
    return;
  lastSessionCheck = millis();

  Serial.println("🔍 Consultando sesión al servidor...");

  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("❌ No se pudo conectar al servidor para verificar sesión");
    return;
  }

  client.printf("GET /heart_rate/api/check_session.php HTTP/1.1\r\n"
                "Host: %s\r\n"
                "Connection: close\r\n"
                "User-Agent: ESP8266\r\n\r\n",
                host);

  unsigned long timeout = millis();
  while (!client.available() && millis() - timeout < 3000) {
    yield();
  }

  if (!client.available()) {
    Serial.println("⏱ Timeout esperando respuesta del servidor");
    client.stop();
    return;
  }

  String response = "";
  while (client.available()) {
    response += client.readString();
  }
  client.stop();

  Serial.println("📥 Respuesta RAW del servidor:");
  Serial.println(response.substring(0, 200));
  Serial.println("---");

  int jsonStart = response.indexOf("{");
  int jsonEnd = response.lastIndexOf("}");

  if (jsonStart == -1 || jsonEnd == -1) {
    Serial.println("❌ No se encontró JSON válido en la respuesta");
    return;
  }

  String jsonBody = response.substring(jsonStart, jsonEnd + 1);
  Serial.printf("📄 JSON extraído: %s\n", jsonBody.c_str());

  if (jsonBody.indexOf("\"activa\":true") != -1 ||
      jsonBody.indexOf("\"activa\": true") != -1) {

    isSessionActive = true;
    Serial.println("✅ Sesión ACTIVA detectada");

    int idKey = jsonBody.indexOf("\"id_paciente\"");
    if (idKey != -1) {
      int idStart = jsonBody.indexOf(":", idKey) + 1;
      while (idStart < jsonBody.length() &&
             (jsonBody[idStart] == ' ' || jsonBody[idStart] == '\t')) {
        idStart++;
      }
      int idEnd = idStart;
      while (idEnd < jsonBody.length() && isDigit(jsonBody[idEnd])) {
        idEnd++;
      }
      String idStr = jsonBody.substring(idStart, idEnd);
      currentPatientId = idStr.toInt();
      Serial.printf("👤 ID Paciente extraído: %d\n", currentPatientId);
    }

    int tokenKey = jsonBody.indexOf("\"token\"");
    if (tokenKey != -1) {
      int tokenStart = jsonBody.indexOf("\"", tokenKey + 7) + 1;
      int tokenEnd = jsonBody.indexOf("\"", tokenStart);
      if (tokenStart > 0 && tokenEnd > tokenStart) {
        currentToken = jsonBody.substring(tokenStart, tokenEnd);
        Serial.printf("🔐 Token extraído: %s...\n",
                      currentToken.substring(0, 8).c_str());
      }
    }

    if (currentPatientId > 0 && currentToken.length() > 10) {
      Serial.printf("✅✅ Sesión lista: Paciente #%d, Token válido\n",
                    currentPatientId);
    } else {
      Serial.println(
          "⚠️ Sesión activa pero faltan datos (ID o token incompletos)");
      isSessionActive = false;
    }

  } else {

    isSessionActive = false;
    currentToken = "";
    currentPatientId = 1;
    Serial.println("⏸ Sesión NO activa o no encontrada");

    if (jsonBody.indexOf("\"activa\":false") != -1) {
      Serial.println("   → El servidor dice: activa=false");
    } else if (jsonBody.length() < 10) {
      Serial.println(
          "   → Respuesta JSON muy corta, posible error en servidor");
    }
  }
}
void enviarDatos(float bpmPromedio) {
  Serial.println("\n=== 📤 INICIO ENVÍO ===");

  verificarSesion();

  if (!isSessionActive) {
    Serial.println("⏸ NO se envían datos: sesión inactiva");
    Serial.println("=== 📤 FIN ENVÍO (cancelado) ===\n");
    return;
  }

  Serial.printf("✅ Sesión válida: Paciente #%d\n", currentPatientId);

  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("❌ Error: No se pudo conectar al servidor (TCP)");
    return;
  }

  char url[250];
  snprintf(
      url, sizeof(url),
      "/heart_rate/api/"
      "recibir_datos_esp32.php?id_paciente=%d&bpm=%.0f&token=%s&lecturas=%d",
      currentPatientId, bpmPromedio, currentToken.c_str());

  Serial.printf("🔗 URL a enviar: %s\n", url);

  client.printf("GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", url,
                host);

  unsigned long timeout = millis();
  while (!client.available() && millis() - timeout < 3000) {
    yield();
  }

  if (client.available()) {
    String resp = client.readString();
    Serial.printf("📥 Respuesta HTTP: %s\n", resp.substring(0, 150).c_str());

    if (resp.indexOf("Guardado") != -1 ||
        resp.indexOf("\"message\":\"Guardado\"") != -1) {
      Serial.println("✅✅ ÉXITO: Datos guardados en base de datos");
    } else if (resp.indexOf("error") != -1 || resp.indexOf("Error") != -1) {
      Serial.println("❌ El servidor reportó un error");
    } else {
      Serial.println("⚠️ Respuesta del servidor no reconocida");
    }
  } else {
    Serial.println("⚠️ No se recibió respuesta del servidor");
  }

  client.stop();
  Serial.println("=== 📤 FIN ENVÍO ===\n");
}
bool detectarPulso(int signalValue) {
  static float baseline = 500;
  baseline = baseline * 0.995 + signalValue * 0.005;
  int deviation = signalValue - baseline;

  const int peakThreshold = 25;

  if (deviation > peakThreshold && !peakDetected &&
      (millis() - lastBeat > 300)) {
    peakDetected = true;
    return true;
  }
  if (deviation < peakThreshold * 0.5)
    peakDetected = false;
  return false;
}

void loopSensor() {
  int raw = analogRead(SENSOR_PIN);
  signalFiltered = alphaSignal * signalFiltered + (1.0 - alphaSignal) * raw;

  if (signalFiltered > fingerThreshold && signalFiltered < signalMax) {
    if (detectarPulso(signalFiltered)) {
      long delta = millis() - lastBeat;
      lastBeat = millis();

      if (delta > 300 && delta < 2000) {
        beatsPerMinute = 60000.0 / (float)delta;

        rates[rateSpot++] = constrain((byte)beatsPerMinute, 30, 200);
        rateSpot %= RATE_SIZE;

        beatAvg = 0;
        for (byte i = 0; i < RATE_SIZE; i++)
          beatAvg += rates[i];
        beatAvg /= RATE_SIZE;

        lastValidBPM = (beatAvg > 0) ? beatAvg : beatsPerMinute;
        Serial.printf("💓 Latido: %.0f BPM (Delta: %lums)\n", lastValidBPM,
                      delta);
      }
    }
  } else {
    beatsPerMinute = 0;
    beatAvg = 0;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(SENSOR_PIN, INPUT);

  for (int i = 0; i < 20; i++) {
    analogRead(SENSOR_PIN);
    delay(5);
  }
  signalFiltered = analogRead(SENSOR_PIN);

  wf.confinit(false);
  wf.showIP();
  Serial.println("✅ ESP8266 listo. Coloca el dedo.");
}

void loop() {
  if (millis() - lastSampleTime >= 10) {
    lastSampleTime = millis();
    loopSensor();
  }

  if (millis() - previousMillis >= interval) {
    previousMillis = millis();

    if (lastValidBPM >= 30 && lastValidBPM <= 200) {
      enviarDatos(lastValidBPM);
    } else {
      Serial.println("⚠️ Sin BPM válido para enviar");
    }
  }
  yield();
}
