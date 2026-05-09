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

const char *host = "192.168.1.217";
const int port = 80;
const unsigned long interval = 5000;
unsigned long previousMillis = 0;
unsigned long lastSampleTime = 0;
void enviarDatos(float bpm) {
  WiFiClient client;

  // 🔌 Conectar al servidor
  if (!client.connect(host, port)) {
    Serial.println("❌ Error conexión TCP al servidor");
    return;
  }

  // 📝 Construir la URL apuntando al archivo correcto
  // Nota: Cambiamos 'api/registros.php' por 'api/recibir_datos_esp32.php'
  char url[150];
  snprintf(url, sizeof(url),
           "/heart_rate/api/recibir_datos_esp32.php?id_paciente=1&bpm=%.0f",
           bpm);

  Serial.printf("📤 Enviando petición a: %s\n", url);

  // 🚀 Enviar petición GET
  client.printf("GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", url,
                host);

  //  Esperar respuesta
  unsigned long timeout = millis();
  while (!client.available()) {
    if (millis() - timeout > 3000) {
      Serial.println(" Timeout esperando respuesta del servidor");
      client.stop();
      return;
    }
  }

  // 📖 Leer respuesta para verificar si se guardó
  // Buscamos la palabra "Guardado" en la respuesta
  String respuesta = client.readString();
  if (respuesta.indexOf("Guardado") != -1) {
    Serial.println("✅ ¡Éxito! Datos guardados en el servidor.");
    // Opcional: Parpadear LED verde si tienes uno
  } else {
    Serial.println("⚠️ Respuesta del servidor (posible error):");
    Serial.println(
        respuesta.substring(0, 100)); // Imprime los primeros 100 caracteres
  }

  client.stop();
}
bool detectarPulso(int signalValue) {
  static float baseline = 500;
  baseline = baseline * 0.995 + signalValue * 0.005; // Línea base adaptativa
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

      if (delta > 300 && delta < 2000) { // 30-200 BPM
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

  // Estabilizar ADC
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
  // Muestreo a ~100Hz
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
