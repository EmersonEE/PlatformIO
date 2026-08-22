/*
 * omarchy-esp32-leds - version simple
 * Solo muestra UN color solido en toda la tira, tomado de keyboard.rgb.
 *
 * Endpoint:
 *   GET /color?hex=RRGGBB   -> aplica ese color solido a toda la tira
 *   GET /status             -> color actual en JSON
 */

#include <Arduino.h>
#include <FastLED.h>
#include <WebServer.h>
#include <WiFi.h>

#define WIFI_SSID "CLARO_h9hU3j"
#define WIFI_PASSWORD "7474FB19FD"

#define LED_PIN 5
#define NUM_LEDS 52
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define BRIGHTNESS 225

CRGB leds[NUM_LEDS];
WebServer server(80);
String currentHex = "000000";

void applySolidColor(uint8_t r, uint8_t g, uint8_t b) {
  fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
  FastLED.show();
}

bool parseHex(const String &hex, uint8_t &r, uint8_t &g, uint8_t &b) {
  if (hex.length() != 6)
    return false;
  for (unsigned int i = 0; i < hex.length(); i++) {
    if (!isxdigit((unsigned char)hex[i]))
      return false;
  }
  long value = strtol(hex.c_str(), NULL, 16);
  r = (value >> 16) & 0xFF;
  g = (value >> 8) & 0xFF;
  b = value & 0xFF;
  return true;
}

void handleColor() {
  if (!server.hasArg("hex")) {
    server.send(400, "text/plain", "Falta parametro ?hex=RRGGBB");
    return;
  }

  String hex = server.arg("hex");
  hex.replace("#", "");
  hex.trim();
  hex.toLowerCase();

  uint8_t r, g, b;
  if (!parseHex(hex, r, g, b)) {
    server.send(400, "text/plain", "Formato invalido, usa RRGGBB");
    return;
  }

  applySolidColor(r, g, b);
  currentHex = hex;

  Serial.printf("Color aplicado: #%02X%02X%02X\n", r, g, b);
  server.send(200, "text/plain", "OK " + hex);
}

void handleStatus() {
  String json =
      "{\"color\":\"#" + currentHex + "\",\"leds\":" + String(NUM_LEDS) + "}";
  server.send(200, "application/json", json);
}

void handleRoot() {
  server.send(200, "text/plain",
              "omarchy-esp32-leds (simple) activo.\n"
              "GET /color?hex=RRGGBB\n"
              "GET /status");
}

void setup() {
  Serial.begin(115200);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setDither(0);

  applySolidColor(0, 0, 0);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Conectado. IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/color", handleColor);
  server.on("/status", handleStatus);
  server.begin();
  Serial.println("Servidor HTTP iniciado en puerto 80");
}

void loop() { server.handleClient(); }
