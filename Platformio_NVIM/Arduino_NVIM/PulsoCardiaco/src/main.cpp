#include "drivers.h"
#include "wificonf.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>

Driver ledOk(LED_BUILTIN);
WIFI_CONF wf;

const char *host = "192.168.1.136";
const int port = 80;
const unsigned long interval = 5000;

unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);
  randomSeed(micros());

  wf.confinit(false);
  wf.showIP();

  Serial.println("ESP8266 iniciado");
}

void loop() {

  if (millis() - previousMillis >= interval) {
    previousMillis = millis();

    int a = random(1, 100);
    int b = random(60, 120);

    WiFiClient client;

    if (!client.connect(host, port)) {
      Serial.println("Error de conexión");
      return;
    }

    char url[120];
    snprintf(url, sizeof(url), "/heart_rate/index.php?id=%d&heart_rate=%d", a,
             b);

    Serial.println(url);

    client.printf("GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",
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
}
