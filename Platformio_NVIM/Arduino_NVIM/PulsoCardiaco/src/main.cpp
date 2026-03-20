#include "HardwareSerial.h"
#include "drivers.h"
#include "wificonf.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <MAX30105.h>
#include <Wire.h>
#include <cstdlib>
#include <execution>
#include <heartRate.h>

Driver ledOk(LED_BUILTIN);
WIFI_CONF wf;
MAX30105 particleSensor;

const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;

float irFiltered = 0;
const float alpha = 0.9;

float beatsPerMinute;
int beatAvg;

const char *host = "192.168.1.136";
const int port = 80;

const unsigned long interval = 5000;
const long fingerThreshold = 50000;

unsigned long previousMillis = 0;

struct SensorData {
  long irValue;
  float bpm;
  int avgBpm;
};

float procesarBPM(float bpmRaw, long irValue) {

  if (irValue < fingerThreshold) {
    return -1; // inválido
  }

  float bpm = bpmRaw;

  if (bpm < 50 || bpm > 120) {
    return -1;
  }

  static float bpmPrev = 70;

  if (abs(bpm - bpmPrev) > 15) {
    bpm = bpmPrev;
  }

#define N 5
  static float buffer[N] = {70, 70, 70, 70, 70};
  static int idx = 0;

  buffer[idx++] = bpm;
  idx %= N;

  float suma = 0;
  for (int i = 0; i < N; i++) {
    suma += buffer[i];
  }

  bpm = suma / N;

  static float bpmFiltrado = 70;
  float alpha = 0.2;

  bpmFiltrado = alpha * bpm + (1 - alpha) * bpmFiltrado;

  bpmPrev = bpmFiltrado;

  return bpmFiltrado;
}

SensorData leerSensor() {

  SensorData data;

  long irValue = particleSensor.getIR();

  irFiltered = alpha * irFiltered + (1 - alpha) * irValue;

  if (checkForBeat(irFiltered)) {

    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 220 && beatsPerMinute > 30) {

      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE;

      beatAvg = 0;

      for (byte x = 0; x < RATE_SIZE; x++)
        beatAvg += rates[x];

      beatAvg /= RATE_SIZE;
    }
  }

  data.irValue = irFiltered;
  data.bpm = beatsPerMinute;
  data.avgBpm = beatAvg;

  return data;
}

void maxInit() {

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30102 no encontrado");
    while (1)
      ;
  }

  Serial.println("Colocar dedo en el sensor");

  byte ledBrightness = 60;
  byte sampleAverage = 4;
  byte ledMode = 2;
  int sampleRate = 100;
  int pulseWidth = 411;
  int adcRange = 4096;

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate,
                       pulseWidth, adcRange);

  particleSensor.setPulseAmplitudeRed(0x1F);
  particleSensor.setPulseAmplitudeGreen(0);
}

void setup() {
  Serial.begin(115200);
  randomSeed(micros());

  wf.confinit(false);
  wf.showIP();

  Serial.println("ESP8266 iniciado");
  maxInit();
}

void enviarDatos(float bpm) {

  WiFiClient client;

  if (!client.connect(host, port)) {
    Serial.println("Error de conexión");
    return;
  }

  char url[120];

  snprintf(url, sizeof(url), "/heart_rate/index.php?bpm=%.2f", bpm);

  Serial.println(url);

  client.printf("GET %s HTTP/1.1\r\n"
                "Host: %s\r\n"
                "Connection: close\r\n\r\n",
                url, host);

  unsigned long timeout = millis();

  while (!client.available()) {
    if (millis() - timeout > 5000) {
      Serial.println("Timeout");
      client.stop();
      return;
    }
  }

  while (client.available()) {
    Serial.write(client.read());
  }

  client.stop();
}

SensorData sensor;

void loop() {

  sensor = leerSensor();

  float bpmFiltrado = procesarBPM(sensor.avgBpm, sensor.irValue);

  if (bpmFiltrado < 0) {
    return;
  }

  if (millis() - previousMillis >= interval) {

    previousMillis = millis();

    Serial.print("BPM Filtrado: ");
    Serial.println(bpmFiltrado);

    enviarDatos(bpmFiltrado);
  }
}
