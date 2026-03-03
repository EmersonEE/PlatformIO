#include "ESP8266WiFi.h"
#include "HardwareSerial.h"
#include "WiFiManager.h"

#include <wificonf.h>
bool ret;

WIFI_CONF::WIFI_CONF() {}
void WIFI_CONF::confinit(bool resetConfig) {
  WiFiManager wm;

  if (resetConfig == true)
    wm.resetSettings();
  if (resetConfig == false)
    Serial.println("No se resetea");
  // wm.resetSettings();

  ret = wm.autoConnect("ESP8266WIFI", "12345678");
  if (!ret) {
    Serial.println("Failed to conenect");
  } else {
    Serial.println("Connected SIUUUUUUUUUUUU!");
  }
}

void WIFI_CONF::showIP() {

  Serial.print("IP Address \t");
  Serial.println(WiFi.localIP());
}
