/*
 * omarchy-esp32-leds - Nivel 2: Experiencia Visual "Pro"
 * Control de tira WS2812B para temas de Omarchy
 *
 * Características Nivel 2:
 *   - Transición suave (crossfade) no bloqueante de 600ms
 *   - Persistencia en NVS (Preferences): arranca con el último color y brillo
 *   - Control de brillo dinámico vía endpoint (/color?hex=...&b=...) y /brightness?val=...
 *   - Soporte mDNS (omarchy-leds.local)
 */

#include <Arduino.h>
#include <FastLED.h>
#include <WebServer.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <Preferences.h>

#define WIFI_SSID "CLARO_h9hU3j"
#define WIFI_PASSWORD "7474FB19FD"

#define LED_PIN 5
#define NUM_LEDS 52
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define DEFAULT_BRIGHTNESS 200
#define FADE_DURATION_MS 600

#define MDNS_HOSTNAME "omarchy-leds"

CRGB leds[NUM_LEDS];
WebServer server(80);
Preferences preferences;

// Variables de estado y color
String currentHex = "000000";
CRGB startColor = CRGB::Black;
CRGB targetColor = CRGB::Black;
uint8_t startBrightness = DEFAULT_BRIGHTNESS;
uint8_t targetBrightness = DEFAULT_BRIGHTNESS;
bool powerOn = true;

// Control de transición suave (crossfade)
unsigned long fadeStartTime = 0;
bool isFading = false;

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

void saveToNVS(const String &hex, uint8_t bri) {
  if (preferences.getString("hex", "") != hex) {
    preferences.putString("hex", hex);
  }
  if (preferences.getUChar("bri", 0) != bri) {
    preferences.putUChar("bri", bri);
  }
}

void startTransition(CRGB newColor, uint8_t newBrightness, const String &hex) {
  if (isFading) {
    startColor = leds[0];
    startBrightness = FastLED.getBrightness();
  } else {
    startColor = targetColor;
    startBrightness = targetBrightness;
  }

  targetColor = newColor;
  targetBrightness = newBrightness;
  currentHex = hex;
  fadeStartTime = millis();
  isFading = true;

  if (powerOn) {
    saveToNVS(hex, newBrightness);
  }
}

void updateFade() {
  if (!isFading)
    return;

  static unsigned long lastFrameTime = 0;
  unsigned long now = millis();

  // Mantener refresco suave a ~60 FPS (cada 16ms)
  if (now - lastFrameTime < 16)
    return;
  lastFrameTime = now;

  unsigned long elapsed = now - fadeStartTime;

  if (elapsed >= FADE_DURATION_MS) {
    fill_solid(leds, NUM_LEDS, targetColor);
    FastLED.setBrightness(targetBrightness);
    FastLED.show();
    isFading = false;
  } else {
    fract8 fraction = (elapsed * 255) / FADE_DURATION_MS;
    CRGB blended = blend(startColor, targetColor, fraction);
    uint8_t blendedBri =
        startBrightness +
        (((int16_t)targetBrightness - startBrightness) * fraction / 255);

    fill_solid(leds, NUM_LEDS, blended);
    FastLED.setBrightness(blendedBri);
    FastLED.show();
  }
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

  uint8_t reqBrightness = targetBrightness;
  if (server.hasArg("b")) {
    int bVal = server.arg("b").toInt();
    if (bVal >= 0 && bVal <= 255) {
      reqBrightness = (uint8_t)bVal;
    }
  }

  powerOn = true;
  preferences.putBool("pwr", true);
  startTransition(CRGB(r, g, b), reqBrightness, hex);

  Serial.printf("Color recibido: #%02X%02X%02X, brillo: %d\n", r, g, b,
                reqBrightness);
  server.send(200, "text/plain", "OK " + hex + " b=" + String(reqBrightness));
}

void handleBrightness() {
  if (!server.hasArg("val")) {
    server.send(400, "text/plain", "Falta parametro ?val=0-255");
    return;
  }

  int bVal = server.arg("val").toInt();
  if (bVal < 0 || bVal > 255) {
    server.send(400, "text/plain", "Brillo invalido (rango 0-255)");
    return;
  }

  powerOn = true;
  preferences.putBool("pwr", true);
  startTransition(targetColor, (uint8_t)bVal, currentHex);
  server.send(200, "text/plain", "OK brillo " + String(bVal));
}

void handlePower() {
  if (server.hasArg("state")) {
    String state = server.arg("state");
    state.toLowerCase();
    if (state == "on" || state == "1" || state == "true") powerOn = true;
    else if (state == "off" || state == "0" || state == "false") powerOn = false;
    else if (state == "toggle") powerOn = !powerOn;
  } else {
    powerOn = !powerOn;
  }

  preferences.putBool("pwr", powerOn);

  if (powerOn) {
    uint8_t r, g, b;
    parseHex(currentHex, r, g, b);
    startTransition(CRGB(r, g, b), targetBrightness, currentHex);
  } else {
    // Atenuar a negro sin alterar el color/brillo guardado
    startTransition(CRGB::Black, 0, currentHex);
  }

  server.send(200, "text/plain", powerOn ? "ON" : "OFF");
}

void handleStatus() {
  String json = "{\"color\":\"#" + currentHex +
                "\",\"brightness\":" + String(targetBrightness) +
                ",\"power\":" + String(powerOn ? "true" : "false") +
                ",\"leds\":" + String(NUM_LEDS) +
                ",\"fade_ms\":" + String(FADE_DURATION_MS) +
                ",\"hostname\":\"" + String(MDNS_HOSTNAME) + ".local\"}";
  server.send(200, "application/json", json);
}

void handleRoot() {
  server.send(200, "text/plain",
              "omarchy-esp32-leds activo.\n"
              "mDNS: http://" MDNS_HOSTNAME ".local\n"
              "GET /color?hex=RRGGBB[&b=0-255]\n"
              "GET /brightness?val=0-255\n"
              "GET /power?state=on|off|toggle\n"
              "GET /status\n");
}

void setup() {
  Serial.begin(115200);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)
      .setCorrection(TypicalLEDStrip);
  FastLED.setDither(0);

  // 1. Recuperar estado guardado en NVS (Flash no volátil)
  preferences.begin("omarchy-leds", false);
  String savedHex = preferences.getString("hex", "00ffff");
  uint8_t savedBri = preferences.getUChar("bri", DEFAULT_BRIGHTNESS);
  powerOn = preferences.getBool("pwr", true);

  uint8_t r, g, b;
  if (parseHex(savedHex, r, g, b)) {
    targetColor = CRGB(r, g, b);
    currentHex = savedHex;
  } else {
    targetColor = CRGB(0, 255, 255);
    currentHex = "00ffff";
  }
  targetBrightness = savedBri;

  // Encender o permanecer apagado según el estado guardado
  if (powerOn) {
    fill_solid(leds, NUM_LEDS, targetColor);
    FastLED.setBrightness(targetBrightness);
  } else {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.setBrightness(0);
  }
  FastLED.show();

  // 2. Conectar WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Conectado con exito. IP asignada: ");
  Serial.println(WiFi.localIP());

  // 3. Iniciar mDNS
  if (!MDNS.begin(MDNS_HOSTNAME)) {
    Serial.println("Error al iniciar servicio mDNS");
  } else {
    Serial.printf("mDNS activo: http://%s.local\n", MDNS_HOSTNAME);
    MDNS.addService("http", "tcp", 80);
  }

  // 4. Registrar rutas HTTP
  server.on("/", handleRoot);
  server.on("/color", handleColor);
  server.on("/brightness", handleBrightness);
  server.on("/power", handlePower);
  server.on("/status", handleStatus);
  server.begin();
  Serial.println("Servidor HTTP iniciado en puerto 80");
}

void loop() {
  server.handleClient();
  updateFade();
}
