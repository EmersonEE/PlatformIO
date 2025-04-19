#include <Arduino.h>

#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include <Adafruit_NeoPixel.h>

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

#include "Secrets.h"
#include "ConfigNeoPixels.h"
// Define Firebase Data object
FirebaseData firebase;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

int Boton = A0;
int Rojo, Verde, Azul = 0;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    digitalWrite(LED_BUILTIN, 0);
    delay(50);
    digitalWrite(LED_BUILTIN, 1);
    delay(50);
  }
  digitalWrite(LED_BUILTIN, 1);
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", ""))
  {
    Serial.println("Conectado a la Base de Datos!!");
    signupOK = true;
  }
  else
  {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Leds.begin();
  Leds.show();
}

void loop()
{

  //////////////////Leyendo Valor del Rojo   //////////////////
  if (Firebase.RTDB.getInt(&firebase, "/Rojo"))
  {
    if (firebase.dataType() == "int")
    {
      Rojo = firebase.intData();
      Serial.print("Rojo: ");
      Serial.println(Rojo);
    }
  }
  else
  {
    Serial.println("Valiod Verga");
  }
  ////////////////Leyendo Valor del Verde  //////////////////
  if (Firebase.RTDB.getInt(&firebase, "/Verde"))
  {
    if (firebase.dataType() == "int")
    {
      Verde = firebase.intData();
      Serial.print("Verde: ");
      Serial.println(Verde);
    }
  }
  else
  {
    Serial.println("Valiod Verga");
  }
  //////////////////Leyendo Valor del Azul   //////////////////
  if (Firebase.RTDB.getInt(&firebase, "/Azul"))
  {
    if (firebase.dataType() == "int")
    {
      Azul = firebase.intData();
      Serial.print("Azul: ");
      Serial.println(Azul);
    }
  }
  else
  {
    Serial.println("Valiod Verga");
  }
  Leds.setBrightness(100);
  // Leds.fill(Leds.Color(255, 0, 0));
  Leds.fill(Leds.Color(Rojo, Verde, Azul));
  Leds.show();
  // Leds.setPixelColor(NumeroPixel, Rojo, Verde, Azul);
  // Serial.print("RGB(");
  // Serial.print(Rojo);
  // Serial.print(",");
  // Serial.print(Verde);
  // Serial.print(",");
  // Serial.print(Azul);
  // Serial.println(")");
}
