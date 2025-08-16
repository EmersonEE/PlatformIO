#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <UniversalTelegramBot.h>

const char *ssid = "ELECTRONICA";
const char *password = "123456789";

// #define BOTtoken "7728354592:AAG5Egb_-28yP30dIJIA9-9RbW4onO_v4tY"
// #define CHAT_ID "1527971462"
#define BOTtoken "8411582875:AAFVtD7UCV2-fcEj_Di9oQFLVx_3NGDEWiU"
#define CHAT_ID "7302926622"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

const int motionSensor = 15;
bool motionDetected = false;
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

const uint8_t PIN_LED_RED = 27;
const uint8_t PIN_LED_GREEN = 14;
const uint8_t PIN_LED_BLUE = 12;
const int pinBuzzer = 17;

bool ledRedState = LOW;
bool ledGreenState = LOW;
bool ledBlueState = LOW;
bool pinBuzzerState = LOW;
unsigned long buzzerStartTime = 0;
const unsigned long BUZZER_DURATION = 10000; // 10 segundos

void controlLed(String chat_ID, String texto, uint8_t pin_led, bool &ledState, const String &ledName)
{
    if (texto == "/led_" + ledName + "_on")
    {
        ledState = HIGH;
        digitalWrite(pin_led, ledState);
        bot.sendMessage(chat_ID, ledName + " LED state set to ON", "");
    }
    else if (texto == "/led_" + ledName + "_off")
    {
        ledState = LOW;
        digitalWrite(pin_led, ledState);
        bot.sendMessage(chat_ID, ledName + " LED state set to OFF", "");
    }
    else if (texto == "/led_" + ledName + "_state")
    {
        String status = ledName + " LED is " + (digitalRead(pin_led) ? "ON" : "OFF");
        bot.sendMessage(chat_ID, status, "");
    }
}

void gpio_on_off(String chat_ID, String texto)
{
    controlLed(chat_ID, texto, PIN_LED_RED, ledRedState, "red");
    controlLed(chat_ID, texto, PIN_LED_GREEN, ledGreenState, "green");
    controlLed(chat_ID, texto, PIN_LED_BLUE, ledBlueState, "blue");

    if (texto == "/state_all")
    {
        String status = "LED States:\n";
        status += "Red: " + String(digitalRead(PIN_LED_RED) ? "ON" : "OFF") + "\n";
        status += "Green: " + String(digitalRead(PIN_LED_GREEN) ? "ON" : "OFF") + "\n";
        status += "Blue: " + String(digitalRead(PIN_LED_BLUE) ? "ON" : "OFF") + "\n";
        status += "Buzzer: " + String(pinBuzzerState ? "ON" : "OFF");
        bot.sendMessage(chat_ID, status, "");
    }
}

void handleNewMessages(int numNewMessages)
{
    Serial.println("handleNewMessages");
    Serial.println(String(numNewMessages));

    for (int i = 0; i < numNewMessages; i++)
    {
        String chat_id = String(bot.messages[i].chat_id);
        if (chat_id != CHAT_ID)
        {
            bot.sendMessage(chat_id, "Unauthorized user", "");
            continue;
        }

        String text = bot.messages[i].text;
        Serial.println(text);

        String from_name = bot.messages[i].from_name;

        if (text == "/start")
        {
            String welcome = "Welcome, " + from_name + ".\n";
            welcome += "Use the following commands to control your LEDs:\n\n";
            welcome += "/led_red_on, /led_red_off, /led_red_state for Red LED\n";
            welcome += "/led_green_on, /led_green_off, /led_green_state for Green LED\n";
            welcome += "/led_blue_on, /led_blue_off, /led_blue_state for Blue LED\n";
            welcome += "/state_all to request all LEDs' states\n";
            bot.sendMessage(chat_id, welcome, "");
        }

        gpio_on_off(chat_id, text);
    }
}

void IRAM_ATTR detectsMovement()
{
    motionDetected = true;
}

void setup()
{
    Serial.begin(115200);

    pinMode(motionSensor, INPUT_PULLUP);
    pinMode(PIN_LED_RED, OUTPUT);
    pinMode(PIN_LED_GREEN, OUTPUT);
    pinMode(PIN_LED_BLUE, OUTPUT);
    pinMode(pinBuzzer, OUTPUT);
    digitalWrite(PIN_LED_RED, ledRedState);
    digitalWrite(PIN_LED_GREEN, ledGreenState);
    digitalWrite(PIN_LED_BLUE, ledBlueState);
    digitalWrite(pinBuzzer, pinBuzzerState);

    attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);

    Serial.print("Connecting Wifi: ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    bot.sendMessage(CHAT_ID, "Bot started up", "");
}

void loop()
{
    if (motionDetected)
    {
        bot.sendMessage(CHAT_ID, "Intruso detectado!!", "");
        pinBuzzerState = HIGH;
        digitalWrite(pinBuzzer, pinBuzzerState);
        buzzerStartTime = millis();
        motionDetected = false;
    }

    if (pinBuzzerState && (millis() - buzzerStartTime >= BUZZER_DURATION))
    {
        pinBuzzerState = LOW;
        digitalWrite(pinBuzzer, pinBuzzerState);
        bot.sendMessage(CHAT_ID, "Intruso Auyentado", "");
    }

    if (millis() > lastTimeBotRan + botRequestDelay)
    {
        int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

        while (numNewMessages)
        {
            Serial.println("got response");
            handleNewMessages(numNewMessages);
            numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        }
        lastTimeBotRan = millis();
    }
}