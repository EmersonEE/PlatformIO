#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

#define SD_CS 5 // Pin CS del lector de la tarjeta SD

const int WIDTH = 128;
const int HEIGHT = 160;
const int FRAME_SIZE = WIDTH * HEIGHT * 2; // 128x160 a 16 bits = 40,960 bytes

// Puntero para la memoria RAM donde guardaremos el fotograma en tránsito
uint8_t *frameBuffer;
File videoFile;

// Control de velocidad del video
const int TARGET_FPS = 15;
const int FRAME_TIME_MS = 1000 / TARGET_FPS; // ~66 milisegundos por fotograma
unsigned long nextFrameTime = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando Reproductor RAW...");

  // 1. Inicializar pantalla
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

  // Como usamos rgb565be en ffmpeg (Big-Endian), la pantalla no necesita
  // invertir nada
  tft.setSwapBytes(false);

  // 2. Reservar los 40.9 KB en la RAM del ESP32
  frameBuffer = (uint8_t *)malloc(FRAME_SIZE);
  if (!frameBuffer) {
    Serial.println("ERROR: No hay suficiente RAM para el búfer de video.");
    while (1)
      yield();
  }

  // 3. Inicializar SD asegurando que la TFT esté apagada momentáneamente
  pinMode(TFT_CS, OUTPUT);
  digitalWrite(TFT_CS, HIGH);

  if (!SD.begin(SD_CS)) {
    Serial.println("Falla montando la SD!");
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("Error de SD", 10, 10, 2);
    while (1)
      yield();
  }

  // 4. Abrir el archivo de video
  videoFile = SD.open("/video.raw");
  if (!videoFile) {
    Serial.println("No se encontro 'video.raw'");
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("Falta video.raw", 10, 10, 2);
    while (1)
      yield();
  }

  Serial.println("Reproduciendo video...");
  nextFrameTime = millis();
}

void loop() {
  // 1. Leer un fotograma completo desde la SD a la RAM
  size_t bytesLeidos = videoFile.read(frameBuffer, FRAME_SIZE);

  // Si leímos menos de un fotograma, llegamos al final del video. Lo
  // reiniciamos.
  if (bytesLeidos < FRAME_SIZE) {
    videoFile.seek(0);
    videoFile.read(frameBuffer,
                   FRAME_SIZE); // Leer el primer fotograma de nuevo
  }

  // 2. Enviar el bloque completo de RAM a la pantalla
  tft.pushImage(0, 0, WIDTH, HEIGHT, (uint16_t *)frameBuffer);

  // 3. Control de FPS (Esperar hasta que sea hora del siguiente fotograma)
  while (millis() < nextFrameTime) {
    // yield() evita que el watchdog del ESP32 se reinicie por inactividad
    yield();
  }

  // Calcular el tiempo para el próximo fotograma
  nextFrameTime = millis() + FRAME_TIME_MS;
}
