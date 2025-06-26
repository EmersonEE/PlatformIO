#include <MSM.h>


MSM::MSM()
{

}
void MSM::MSM_init()
{
    pantalla.init();
    pinMode(ledazul, OUTPUT);
    digitalWrite(ledazul, LOW);
}
void MSM::manejarMensajeSuscribirse(const String &payload)
{
    Serial.print("Tópico: /leds | Mensaje: ");
    Serial.println(payload);

    if (payload == "ON") {
        digitalWrite(ledazul, HIGH);
        Serial.println("LED Azul Encendido");
    } else if (payload == "OFF") {
        digitalWrite(ledazul, LOW);
        Serial.println("LED Azul Apagado");
    } else {
        Serial.println("Comando no reconocido para LEDs");
    }
}
void MSM::manejarMensajePantalla(const String &payload)
{
    Serial.print("Tópico: /pantalla | Mensaje: ");
    Serial.println(payload);

    if (payload != payload_anterior && payload.length() > 2) {
        Serial.println(payload);
        payload_anterior = payload;
        pantalla.showCarnet(payload);
    } else {
        Serial.println("Mensaje repetido, no se actualiza pantalla");
        return; 
    }
}
void MSM::MensajeMQTT(String topic, String payload)
{
    Serial.print("Topic: ");
    Serial.print(topic);
    Serial.print(" | Mensaje: ");
    Serial.println(payload);

    if (topic == "/leds") {
        manejarMensajeSuscribirse(payload);
    } else if (topic == "/pantalla") {
        manejarMensajePantalla(payload);
    } else {
        Serial.println("Tópico no manejado");
    }
}
