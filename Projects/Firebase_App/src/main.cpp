#include <Arduino.h>
#include "Variables.h"
#include "secret.h"

#include <WiFiManager.h>
#include <Firebase_ESP_Client.h>
#include <WiFiCong.h>
#include <GetDate.h>
#include <time.h>

#include <RTClib.h>

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

RTC_DS3231 rtc;

void procesarComida()
{
  if (foodEnProceso)
    return;

  if (!Firebase.RTDB.getInt(&fbdo, "/manual_food/grams"))
    return;

  int grams = fbdo.intData();

  if (grams < 20 || grams > 100 || grams % 20 != 0)
    return;

  foodEnProceso = true;
  foodInicio = millis();
  gramsPendiente = grams;

  Firebase.RTDB.setInt(&fbdo, "/manual_food/grams", 0);

  Serial.print("Dispensando comida: ");
  Serial.print(grams);
  Serial.println(" g");
}
void ejecutarComida()
{
  if (!foodEnProceso)
    return;

  unsigned long duracion = (gramsPendiente / 20) * 2000UL;

  if (millis() - foodInicio < duracion)
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
    foodEnProceso = false;
    gramsPendiente = 0;

    Firebase.RTDB.setInt(&fbdo, "/manual_food/lastTriggered", millis());

    Serial.println("Comida completada");
  }
}

void procesarAgua()
{
  if (accionEnProceso)
    return;

  if (!Firebase.RTDB.getInt(&fbdo, "/manual_water/ml"))
    return;

  int ml = fbdo.intData();
  if (ml <= 0)
    return;

  if (ml > 200)
    ml = 200;

  accionEnProceso = true;
  accionInicio = millis();
  mlPendiente = ml;

  Firebase.RTDB.setInt(&fbdo, "/manual_water/ml", 0);

  Serial.print("Dispensando agua: ");
  Serial.println(ml);
}
void ejecutarAgua()
{
  if (!accionEnProceso)
    return;

  unsigned long duracion = 150UL * mlPendiente;

  if (millis() - accionInicio < duracion)
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
    accionEnProceso = false;
    mlPendiente = 0;
    Serial.println("Agua completada");
  }
}

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

int diaSemanaActual()
{
  time_t now;
  time(&now);
  struct tm *t = localtime(&now);
  return t->tm_wday;
}

bool diaPermitido(FirebaseJson &alarmJson)
{
  FirebaseJsonData d;
  int day = diaSemanaActual();

  const char *days[] = {"sun", "mon", "tue", "wed", "thu", "fri", "sat"};

  String path = "repeatDays/" + String(days[day]);
  alarmJson.get(d, path);

  return d.success && d.boolValue;
}

void syncRTCwithNTP()
{
  configTime(-6 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 10000))
  {
    Serial.println("❌ No se pudo obtener hora NTP");
    return;
  }

  rtc.adjust(DateTime(
      timeinfo.tm_year + 1900,
      timeinfo.tm_mon + 1,
      timeinfo.tm_mday,
      timeinfo.tm_hour,
      timeinfo.tm_min,
      timeinfo.tm_sec));

  Serial.println("✅ RTC sincronizado con NTP");
}

void setup()
{
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);

  wificonfig();
  if (!rtc.begin())
  {
    Serial.println("No se encontro el RTC");
    Serial.flush();
    while (1)
      delay(10);
  }
  if (rtc.lostPower())
  {
    syncRTCwithNTP();
  }
  else
  {
    syncRTCwithNTP();
  }

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
void loop()
{
  if (!Firebase.ready())
    return;

  procesarAgua();
  ejecutarAgua();

  procesarComida();
  ejecutarComida();
  DateTime now = rtc.now();

  if (millis() - sendDataPrevMillis < 2500 && sendDataPrevMillis != 0)
    return;

  sendDataPrevMillis = millis();
  String yearStr = String(now.year(), DEC);
  String monthStr = (now.month() < 10 ? "0" : "") + String(now.month(), DEC);
  String dayStr = (now.day() < 10 ? "0" : "") + String(now.day(), DEC);
  String hourStr = (now.hour() < 10 ? "0" : "") + String(now.hour(), DEC);
  String minuteStr = (now.minute() < 10 ? "0" : "") + String(now.minute(), DEC);
  String secondStr = (now.second() < 10 ? "0" : "") + String(now.second(), DEC);
  String dayOfWeek = daysOfTheWeek[now.dayOfTheWeek()];
  int currentHour = now.hour();
  int currentMinute = now.minute();
  // Complete time string
  String formattedTime = dayOfWeek + ", " + yearStr + "-" + monthStr + "-" + dayStr + " " + hourStr + ":" + minuteStr + ":" + secondStr;

  if (!Firebase.RTDB.get(&fbdo, "/alarms"))
  {
    Serial.println(fbdo.errorReason());
    return;
  }

  FirebaseJson alarmsJson = fbdo.jsonObject();
  size_t count = alarmsJson.iteratorBegin();

  Serial.print("Alarmas encontradas: ");
  Serial.println(count);

  for (size_t i = 0; i < count; i++)
  {
    String key, value;
    int type;

    alarmsJson.iteratorGet(i, type, key, value);
    if (type != FirebaseJson::JSON_OBJECT)
      continue;

    FirebaseJson alarmJson;
    alarmJson.setJsonData(value);

    FirebaseJsonData d;

    alarmJson.get(d, "isEnabled");
    bool enabled = d.boolValue;
    if (!enabled)
      continue;

    if (!diaPermitido(alarmJson))
      continue;

    alarmJson.get(d, "grams");
    int grams = d.intValue;

    alarmJson.get(d, "time");
    if (!d.success || d.stringValue.length() == 0)
    {
      Serial.println("❌ No existe o vacío el campo 'time'");
      continue;
    }

    String alarmTimeStr = d.stringValue;
    long long alarmTimeMs = atoll(alarmTimeStr.c_str());
    time_t alarmSeconds = alarmTimeMs / 1000;

    struct tm *alarmTm = localtime(&alarmSeconds);

    long alarmSecondsDay = (alarmTm->tm_hour * 3600L) + (alarmTm->tm_min * 60L) + alarmTm->tm_sec;

    alarmJson.get(d, "title");
    String title = d.stringValue;

    alarmJson.get(d, "lastTriggered");
    long long lastTriggeredMs = d.to<long long>();

    DateTime now = rtc.now();
    long currentSecondsDay = (now.hour() * 3600L) + (now.minute() * 60L) + now.second();

    Serial.println("ALARMA");
    Serial.println(" key: " + key);
    Serial.println(" grams: " + String(grams));
    Serial.println(" title: " + title);
    Serial.println(" fecha: " + alarmTimeStr);
    Serial.println(" fecha to Long Long: " + String(alarmTimeMs));
    Serial.println(" Fecha y Hora RTC: " + formattedTime);
    Serial.print(" Hora y Minutos Alarma: ");
    Serial.print(alarmTm->tm_hour);
    Serial.print(":");
    if (alarmTm->tm_min < 10)
      Serial.print("0");
    Serial.println(alarmTm->tm_min);
    Serial.println(" enabled: " + String(enabled));
    Serial.println(" time: " + String(alarmTimeMs));
    Serial.println("------------------");

    bool esHoraDeAlarma = (currentSecondsDay >= alarmSecondsDay && currentSecondsDay < alarmSecondsDay + 60);

    bool yaEjecutadaHoy = false;
    if (lastTriggeredMs > 0)
    {
      time_t lastSeconds = lastTriggeredMs / 1000;
      struct tm *lastTm = localtime(&lastSeconds);
      if (lastTm->tm_year == alarmTm->tm_year && lastTm->tm_yday == alarmTm->tm_yday)
      {
        yaEjecutadaHoy = true;
      }
    }

    if (esHoraDeAlarma && !yaEjecutadaHoy)
    {
      Serial.println("¡¡¡ ES HORA DE ACTIVAR LA ALARMA !!!");
      ejecutarAlarma(grams);

      long long nowMs = (long long)now.unixtime() * 1000LL;
      Firebase.RTDB.setInt(&fbdo, "/alarms/" + key + "/lastTriggered", nowMs);

      Serial.println("ALARMA EJECUTADA");
    }
    else if (yaEjecutadaHoy)
    {
      Serial.println("Alarma ya ejecutada hoy.");
    }
    else
    {
      Serial.println("Aún no es hora.");
    }
  }
  alarmsJson.iteratorEnd();
}
