#ifndef __WIFICONF__
#define __WIFICONF__
#include <Arduino.h>
#include <DNSServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiManager.h>
class WIFI_CONF {
public:
  WIFI_CONF();
  void confinit(bool resetConfig);
  void showIP();

private:
};

#endif // !__WIFICONF__
