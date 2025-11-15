#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ACS712.h>
#include <DHT.h>

// --- Configuración de pantalla OLED ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- Sensores ---
Adafruit_INA219 ina219;
ACS712 currentSensor(34, 3.3, 4095, 185);

// --- Sensor DHT11 ---
#define DHTPIN 4       // Pin donde está conectado el DHT11 (GPIO4)
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// --- Variables ---
int calibration_factor = 120;
float sensibilidad = 0.185;
float I = 0.00;
float ajuste = 0.05;

void setup(void)
{
  Serial.begin(115200);
  currentSensor.autoMidPoint();
  while (!Serial)
  {
    delay(1);
  }

  Serial.println("Iniciando sensores...");

  if (!ina219.begin())
  {
    Serial.println("Error: no se encontró el INA219");
    while (1)
      delay(10);
  }

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("Error: pantalla OLED no encontrada"));
    for (;;);
  }

  dht.begin(); // Inicializar el DHT11

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("Sensores listos!");
  display.display();
  delay(2000);
  display.clearDisplay();
}

float readCurrent()
{
  float average = 0;
  for (int i = 0; i < 100; i++)
  {
    average += currentSensor.mA_DC();
    delay(1);
  }
  float mA = (abs(average / 100.0));
  if (mA <= 5)
    mA = 0;

  Serial.print("Corriente (ACS712): ");
  Serial.print(mA);
  Serial.println(" mA");

  return mA;
}

void loop(void)
{
  // --- Lectura de sensores ---
  float shuntvoltage = ina219.getShuntVoltage_mV();
  float busvoltage = ina219.getBusVoltage_V();
  float current_mA = ina219.getCurrent_mA();
  float power_mW = ina219.getPower_mW();
  float current = readCurrent();

  // --- Lectura del DHT11 ---
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // Celsius

  // Verificar errores del DHT11
  if (isnan(h) || isnan(t))
  {
    Serial.println("Error al leer el sensor DHT11");
    return;
  }

  // --- Mostrar datos en Serial ---
  Serial.println("====================");
  Serial.print("Voltaje Bus: "); Serial.print(busvoltage); Serial.println(" V");
  Serial.print("Corriente INA219: "); Serial.print(current_mA); Serial.println(" mA");
  Serial.print("Potencia: "); Serial.print(power_mW); Serial.println(" mW");
  Serial.print("Corriente ACS712: "); Serial.print(current); Serial.println(" mA");
  Serial.print("Temperatura: "); Serial.print(t); Serial.println(" °C");
  Serial.print("Humedad: "); Serial.print(h); Serial.println(" %");

  // --- Mostrar datos en OLED ---
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(0, 5);
  display.println("Bus Voltage:");
  display.setCursor(80, 5);
  display.print(busvoltage);
  display.println("V");

  display.setCursor(0, 17);
  display.println("Temp:");
  display.setCursor(80, 17);
  display.print(t);
  display.println("C");

  display.setCursor(0, 29);
  display.println("Hum:");
  display.setCursor(80, 29);
  display.print(h);
  display.println("%");

  display.setCursor(0, 41);
  display.println("Power:");
  display.setCursor(80, 41);
  display.print(power_mW);
  display.println("mW");

  display.setCursor(0, 53);
  display.println("ACS712:");
  display.setCursor(80, 53);
  display.print(current);
  display.println("mA");

  display.display();

  delay(2000); // Actualizar cada 2 segundos
}
