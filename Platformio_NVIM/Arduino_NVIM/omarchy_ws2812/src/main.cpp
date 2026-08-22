/*
 * omarchy-esp32-leds v3
 * Anade un modo de BLOQUES SOLIDOS: cada color ocupa un segmento fijo
 * de la tira, sin ningun tipo de mezcla/gradiente entre ellos.
 * Esto evita el efecto de "todo se ve blanco" que produce el blending
 * entre colores complementarios en el modo paleta (v2).
 *
 * Endpoints:
 *   GET /color?hex=RRGGBB                    -> color solido en toda la tira
 *   GET /palette?colors=RRGGBB,RRGGBB,...     -> gradiente animado (v2, con
 * blend) GET /blocks?colors=RRGGBB,RRGGBB,...      -> bloques solidos, SIN
 * blend GET /status
 *
 * Requiere la libreria FastLED (Arduino Library Manager).
 */

#include <Arduino.h>
#include <FastLED.h>
#include <WebServer.h>
#include <WiFi.h>

// ---------- CONFIGURA ESTO ----------
#define WIFI_SSID "CLARO_h9hU3j"
#define WIFI_PASSWORD "7474FB19FD"

#define LED_PIN 5
#define NUM_LEDS 52
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define BRIGHTNESS 255

// Modo paleta (gradiente): velocidad de desplazamiento
#define SCROLL_SPEED 1
// Modo bloques: velocidad de desplazamiento (0 = estatico, recomendado)
#define BLOCK_SCROLL_SPEED 0
#define FRAME_DELAY_MS 40
// -------------------------------------

CRGB leds[NUM_LEDS];
WebServer server(80);

enum Mode { MODE_SOLID, MODE_PALETTE, MODE_BLOCKS };
Mode currentMode = MODE_SOLID;

String currentHex = "000000";
unsigned long lastFrame = 0;

// --- estado modo paleta (gradiente) ---
CRGBPalette16 currentPalette = CRGBPalette16(CRGB::Black);
uint8_t paletteIndex = 0;

// --- estado modo bloques (solido) ---
CRGB blockColors[16];
int numBlockColors = 0;
int blockOffset = 0;

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

int parseColorList(const String &colorsArg, CRGB *out, int maxColors) {
  int count = 0;
  int start = 0;
  while (start < (int)colorsArg.length() && count < maxColors) {
    int comma = colorsArg.indexOf(',', start);
    String token = (comma == -1) ? colorsArg.substring(start)
                                 : colorsArg.substring(start, comma);
    token.replace("#", "");
    token.trim();
    token.toLowerCase();

    uint8_t r, g, b;
    if (parseHex(token, r, g, b)) {
      out[count++] = CRGB(r, g, b);
    }
    if (comma == -1)
      break;
    start = comma + 1;
  }
  return count;
}

// ---------- Modo color solido (un solo color) ----------
void applySolidColor(uint8_t r, uint8_t g, uint8_t b) {
  fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
  FastLED.show();
}

void handleColor() {
  if (!server.hasArg("hex")) {
    server.send(400, "text/plain", "Falta parametro ?hex=RRGGBB");
    return;
  }
  String hex = server.arg("hex");
  hex.replace("#", "");
  hex.toLowerCase();

  uint8_t r, g, b;
  if (!parseHex(hex, r, g, b)) {
    server.send(400, "text/plain", "Formato invalido, usa RRGGBB");
    return;
  }

  currentMode = MODE_SOLID;
  applySolidColor(r, g, b);
  currentHex = hex;
  server.send(200, "text/plain", "OK solido " + hex);
}

// ---------- Modo paleta (gradiente animado, CON blend) ----------
void handlePalette() {
  if (!server.hasArg("colors")) {
    server.send(400, "text/plain", "Falta parametro ?colors=RRGGBB,RRGGBB,...");
    return;
  }
  CRGB parsed[16];
  int count = parseColorList(server.arg("colors"), parsed, 16);
  if (count == 0) {
    server.send(400, "text/plain", "Ningun color valido");
    return;
  }
  for (int i = 0; i < 16; i++) {
    currentPalette[i] = parsed[i % count];
  }
  currentMode = MODE_PALETTE;
  currentHex = "palette(" + String(count) + ")";
  server.send(200, "text/plain", "OK paleta con " + String(count) + " colores");
}

void updatePaletteAnimation() {
  unsigned long now = millis();
  if (now - lastFrame < FRAME_DELAY_MS)
    return;
  lastFrame = now;
  fill_palette(leds, NUM_LEDS, paletteIndex, 255 / NUM_LEDS, currentPalette,
               BRIGHTNESS, LINEARBLEND);
  FastLED.show();
  paletteIndex += SCROLL_SPEED;
}

// ---------- Modo bloques (SOLIDO, sin blend) ----------
void renderBlocks() {
  if (numBlockColors == 0)
    return;
  int segSize = NUM_LEDS / numBlockColors;
  if (segSize < 1)
    segSize = 1;

  for (int i = 0; i < NUM_LEDS; i++) {
    int colorIdx = ((i + blockOffset) / segSize) % numBlockColors;
    leds[i] = blockColors[colorIdx];
  }
  FastLED.show();
}

void handleBlocks() {
  if (!server.hasArg("colors")) {
    server.send(400, "text/plain", "Falta parametro ?colors=RRGGBB,RRGGBB,...");
    return;
  }
  numBlockColors = parseColorList(server.arg("colors"), blockColors, 16);
  if (numBlockColors == 0) {
    server.send(400, "text/plain", "Ningun color valido");
    return;
  }
  currentMode = MODE_BLOCKS;
  blockOffset = 0;
  currentHex = "blocks(" + String(numBlockColors) + ")";
  renderBlocks();
  server.send(200, "text/plain",
              "OK bloques con " + String(numBlockColors) + " colores");
}

void updateBlocksAnimation() {
  if (BLOCK_SCROLL_SPEED == 0)
    return; // estatico: no se mueve, no hace falta redibujar
  unsigned long now = millis();
  if (now - lastFrame < FRAME_DELAY_MS)
    return;
  lastFrame = now;
  blockOffset += BLOCK_SCROLL_SPEED;
  renderBlocks();
}

// ---------- Utilitarios ----------
void handleStatus() {
  String modeStr = (currentMode == MODE_SOLID)     ? "solid"
                   : (currentMode == MODE_PALETTE) ? "palette"
                                                   : "blocks";
  String json = "{\"mode\":\"" + modeStr + "\",\"value\":\"" + currentHex +
                "\",\"leds\":" + String(NUM_LEDS) + "}";
  server.send(200, "application/json", json);
}

void handleRoot() {
  server.send(
      200, "text/plain",
      "omarchy-esp32-leds v3 activo.\n"
      "GET /color?hex=RRGGBB\n"
      "GET /palette?colors=RRGGBB,RRGGBB,...  (gradiente con blend)\n"
      "GET /blocks?colors=RRGGBB,RRGGBB,...   (bloques solidos, sin blend)\n"
      "GET /status");
}

void setup() {
  Serial.begin(115200);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setCorrection(
      TypicalLEDStrip); // corrige gamma para colores mas vivos
  FastLED.setBrightness(BRIGHTNESS);
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
  server.on("/palette", handlePalette);
  server.on("/blocks", handleBlocks);
  server.on("/status", handleStatus);
  server.begin();
  Serial.println("Servidor HTTP iniciado en puerto 80");
}

void loop() {
  server.handleClient();
  if (currentMode == MODE_PALETTE) {
    updatePaletteAnimation();
  } else if (currentMode == MODE_BLOCKS) {
    updateBlocksAnimation();
  }
}
