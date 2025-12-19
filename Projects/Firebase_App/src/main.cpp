#include <Arduino.h>
#include "secret.h"

#include <WiFiManager.h>
#include <Firebase_ESP_Client.h>
#include <WiFiCong.h>
#include <GetDate.h>
#include <time.h>

#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

#include <FirebaseJson.h>
#include <string>
#include <string.h>
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

GetDate fechaLT;
FirebaseJsonData data;

unsigned long sendDataPrevMillis = 0;
int contadorLocal = 0;
String key, value;
int type;

void ejecutarAlarma(int grams)
{
  int tiempo_ms = (grams / 20) * 2000;

  Serial.print("Ejecutando alarma, grams=");
  Serial.print(grams);
  Serial.print(" tiempo(ms)=");
  Serial.println(tiempo_ms);

  digitalWrite(LED_BUILTIN, HIGH);
  delay(tiempo_ms);
  digitalWrite(LED_BUILTIN, LOW);
}

long long ahoraMs()
{
  time_t now;
  time(&now);
  return (long long)now * 1000;
}

long long stringToLongLong(const String &s)
{
  return atoll(s.c_str());
}

void setup()
{
  Serial.begin(115200);
  delay(1000);

  wificonfig();

  Serial_Printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  config.token_status_callback = tokenStatusCallback;
  Firebase.reconnectNetwork(true);
  Firebase.setDoubleDigits(5);

  Firebase.begin(&config, &auth);

  fechaLT.init();

  Serial.println("Sistema iniciado correctamente");
}

// ==================================================
void loop()
{
  if (!Firebase.ready())
    return;

  if (millis() - sendDataPrevMillis < 15000 && sendDataPrevMillis != 0)
    return;

  sendDataPrevMillis = millis();

  Firebase.RTDB.setInt(&fbdo, "/contador", contadorLocal);
  contadorLocal++;

  if (Firebase.RTDB.getInt(&fbdo, "/contador"))
  {
    Serial.print("Contador leído: ");
    Serial.println(fbdo.intData());
  }

  Serial.println(fechaLT.getDateLT(1765916000879));

  if (!Firebase.RTDB.get(&fbdo, "/alarms"))
  {
    Serial.println(fbdo.errorReason());
    return;
  }

  // FirebaseJson json = fbdo.jsonObject();
  // size_t len = json.iteratorBegin();

  // Serial.print("Total de nodos en alarms: ");
  // Serial.println(len);

  long long nowMs = ahoraMs();

  FirebaseJson alarmsJson = fbdo.jsonObject();
  size_t alarmCount = alarmsJson.iteratorBegin();

  Serial.print("Total de nodos en alarms: ");
  Serial.println(alarmCount);

  for (size_t i = 0; i < alarmCount; i++)
  {
    String key, value;
    int type;

    alarmsJson.iteratorGet(i, type, key, value);

    // 🔥 FILTRO CLAVE
    if (type != FirebaseJson::JSON_OBJECT)
      continue;

    FirebaseJson alarmJson;
    alarmJson.setJsonData(value);

    FirebaseJsonData d;

    alarmJson.get(d, "id");
    int id = d.intValue;

    alarmJson.get(d, "grams");
    int grams = d.intValue;

    alarmJson.get(d, "time");
    String timeStr = d.stringValue;
    long long alarmTime = stringToLongLong(timeStr);

    alarmJson.get(d, "isEnabled");
    bool enabled = d.boolValue;

    alarmJson.get(d, "title");
    String title = d.stringValue;

    String fecha = fechaLT.getDateLT(alarmTime);

    Serial.println("ALARMA");
    Serial.println(" key: " + key);
    Serial.println(" id: " + String(id));
    Serial.println(" grams: " + String(grams));
    Serial.println(" time: " + timeStr);
    Serial.println(" fecha: " + fecha);
    Serial.println(" enabled: " + String(enabled));
    Serial.println(" title: " + title);
    Serial.println("------------------");
    alarmJson.get(data, "isEnabled");

    // long long alarmTime = stringToLongLong(timeStr);
    Serial.println(nowMs);
    Serial.println(alarmTime);
    if (nowMs >= alarmTime && nowMs <= alarmTime + 1000)
    {
      ejecutarAlarma(grams);
      Serial.println("Alarma Ejecutada");
    }
  }
  alarmsJson.iteratorEnd();
}
