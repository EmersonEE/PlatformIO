#include "LED.h"

LED::LED(uint8_t _pinLED1, uint8_t _pinLED2){

    pinLED1 = _pinLED1;
    pinLED2 = _pinLED2;
    pinMode(pinLED1, OUTPUT);
    pinMode(pinLED2, OUTPUT);
    digitalWrite(pinLED1, LOW);
    digitalWrite(pinLED2, LOW);
}


void LED::ledLDR(){
    digitalWrite(pinLED1, HIGH);
    digitalWrite(pinLED2, LOW);
}

void LED::ledConf(){
    digitalWrite(pinLED1, LOW);
    digitalWrite(pinLED2, HIGH);
}

void LED::ledsOff(){
    digitalWrite(pinLED1,LOW);
    digitalWrite(pinLED2,LOW);

}