/* =========================================================================
 * Scanner I2C - Deteccion de pantalla OLED SSD1306
 * Board: Arduino Uno (SDA = A4, SCL = A5)
 * Framework: Arduino | PlatformIO
 *
 * El sketch recorre todas las direcciones validas del bus I2C (0x01 - 0x7F)
 * y reporta por el monitor serial los dispositivos que responden.
 * Las direcciones 0x3C y 0x3D se identifican como pantalla OLED SSD1306.
 *
 * Conexion tipica de la OLED:
 *   VCC -> 3.3V / 5V | GND -> GND | SDA -> A4 | SCL -> A5
 *   (El pin SA0/DC del modulo selecciona 0x3C o 0x3D)
 * ========================================================================= */

#include <Arduino.h>
#include <Wire.h>

/* Velocidad del monitor serial y del bus I2C */
#define SERIAL_BAUDIOS 115200
#define I2C_FRECUENCIA 100000UL

/* Direcciones tipicas de la pantalla OLED SSD1306 (pin SA0 = 0 -> 0x3C / 1 -> 0x3D) */
#define SSD1306_DIR_A 0x3C
#define SSD1306_DIR_B 0x3D

/* Contador de dispositivos detectados en cada barrido */
uint8_t dispositivosEncontrados = 0;

/* Imprime un byte en hexadecimal con dos digitos (0x00 - 0xFF) */
void imprimirHexByte(uint8_t valor) {
  Serial.print("0x");
  if (valor < 0x10) {
    Serial.print('0');
  }
  Serial.print(valor, HEX);
}

/* Devuelve el nombre conocido de la direccion, o nullptr si es generica */
const char *nombreDispositivo(uint8_t direccion) {
  switch (direccion) {
    case SSD1306_DIR_A:
    case SSD1306_DIR_B:
      return "Pantalla OLED SSD1306";
    case 0x20: case 0x21: case 0x22: case 0x23:
    case 0x24: case 0x25: case 0x26: case 0x27:
      return "Expansor PCF8574";
    case 0x68:
      return "RTC DS1307 / MPU6050";
    case 0x69:
      return "MPU6050 alterna";
    case 0x76: case 0x77:
      return "Sensor BME280/BMP280";
    case 0x40:
      return "PCA9685 / Si7021";
    case 0x48: case 0x49: case 0x4A: case 0x4B:
      return "ADC ADS1115";
    case 0x57:
      return "EEPROM AT24C32";
    default:
      return nullptr;
  }
}

/* Envia una peticion minima (0 bytes) para saber si hay ACK en la direccion */
bool dispositivoResponde(uint8_t direccion) {
  Wire.beginTransmission(direccion);
  return (Wire.endTransmission() == 0);
}

/* Barrido completo del bus I2C 0x01 - 0x7F */
void escanearBus() {
  dispositivosEncontrados = 0;

  Serial.println(F("Escaneando bus I2C (0x01 - 0x7F)..."));
  Serial.println();

  for (uint8_t direccion = 0x01; direccion < 0x80; direccion++) {
    if (!dispositivoResponde(direccion)) {
      continue;
    }

    dispositivosEncontrados++;

    Serial.print(F("Dispositivo encontrado en "));
    imprimirHexByte(direccion);
    Serial.print(F("  -> "));

    const char *nombre = nombreDispositivo(direccion);
    if (nombre != nullptr) {
      Serial.println(nombre);
    } else {
      Serial.println(F("desconocido"));
    }
  }

  Serial.println();
  Serial.print(F("Total de dispositivos: "));
  Serial.println(dispositivosEncontrados);
}

/* Informe final enfocado en la pantalla OLED SSD1306 */
void informarSSD1306() {
  bool enA = dispositivoResponde(SSD1306_DIR_A);
  bool enB = dispositivoResponde(SSD1306_DIR_B);

  Serial.println(F("--- Resultado OLED SSD1306 ---"));

  if (enA || enB) {
    Serial.print(F("SSD1306 detectada en "));
    imprimirHexByte(enA ? SSD1306_DIR_A : SSD1306_DIR_B);
    Serial.println();
    Serial.println(F("Usa esa direccion para inicializar la pantalla."));
  } else if (dispositivosEncontrados == 0) {
    Serial.println(F("No se detecto ningun dispositivo."));
    Serial.println(F("Revisa SDA(A4)/SCL(A5), VCC, GND y los pull-up."));
  } else {
    Serial.println(F("Sin respuesta en 0x3C ni 0x3D."));
    Serial.println(F("La pantalla puede usar otra direccion (revisa el pin SA0/DC)."));
  }

  Serial.println();
}

void setup() {
  Serial.begin(SERIAL_BAUDIOS);
  delay(200);

  Wire.begin();                    /* Uno: SDA = A4, SCL = A5 */
  Wire.setClock(I2C_FRECUENCIA);   /* 100 kHz, velocidad estandar */
  delay(100);

  Serial.println();
  Serial.println(F("======================================="));
  Serial.println(F("  Scanner I2C + Detector OLED SSD1306"));
  Serial.println(F("  Arduino Uno | SDA=A4  SCL=A5"));
  Serial.println(F("======================================="));
  Serial.println();
}

void loop() {
  escanearBus();
  informarSSD1306();

  Serial.println(F("Nuevo barrido en 5 segundos..."));
  Serial.println();
  delay(5000);
}
