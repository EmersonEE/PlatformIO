#include <Arduino.h>
#include "NEMA17.h"
#include "JOYSTICK.h"
// Motor Vertical 35 vueltas
//  Pines del motor
uint8_t EN_PIN = 16;
uint8_t DIR_PIN = 2;
uint8_t STEP_PIN = 4;
uint8_t pinENBase = 23;
uint8_t pinDIRBase = 21;
uint8_t pinSTEPBase = 22;
uint8_t pinENEslabon1 = 19;
uint8_t pinSTEPEslabon1 = 18;
uint8_t pinDIREslabon1 = 17;
// Configuración del motor
const int STEPS_PER_REV = 100; // Pasos por revolución física del motor
const int MICROSTEPS = 16;     // Microsteps configurados en el driver
const int TOTAL_STEPS = STEPS_PER_REV * MICROSTEPS;
int conteovueltas = 0;
// Sensibilidad del joystick
const int DEADZONE = 500;      // Zona muerta en torno al centro (0-4095 ADC)
const int STEP_DELAY_US = 500; // Retraso entre pasos (en microsegundos)
const int STEP_DELAY_US_HIGHT = 250;
// Objetos

NEMA17 motorAltura(EN_PIN, DIR_PIN, STEP_PIN);
NEMA17 motorBase(pinENBase, pinDIRBase, pinSTEPBase);
NEMA17 motorEslabon1(pinENEslabon1, pinDIREslabon1, pinSTEPEslabon1);
String input = "";
void setup()
{
  Serial.begin(115200);
  Serial.println("Control de NEMA17 con Joystick - ESP32");
}

void loop()
{

  delay(2500);
  motorBase.rotateDegrees(360, TOTAL_STEPS, STEP_DELAY_US);
  delay(1500);
  motorAltura.moveDistance(200, TOTAL_STEPS, STEP_DELAY_US);
  delay(500);
  motorAltura.moveDistance(-20, TOTAL_STEPS, STEP_DELAY_US_HIGHT);
  delay(500);
  motorAltura.moveDistance(20, TOTAL_STEPS, STEP_DELAY_US);
  delay(1500);
  motorEslabon1.rotateDegrees(180, TOTAL_STEPS, STEP_DELAY_US);
  delay(1500);
  motorEslabon1.rotateDegrees(-180, TOTAL_STEPS, STEP_DELAY_US);
  delay(1500);
  motorAltura.moveDistance(-200, TOTAL_STEPS, STEP_DELAY_US_HIGHT);
  delay(100);
  motorBase.rotateDegrees(-360, TOTAL_STEPS, STEP_DELAY_US);
  delay(1500);
  // while (Serial.available())
  // {
  //   char c = Serial.read();
  //   if (c == '\n' || c == '\r')4
  //   {
  //     if (input.length() > 0)
  //     {
  //       float degrees = input.toFloat();
  //       Serial.print("Rotando ");
  //       Serial.print(degrees);
  //       Serial.println(" grados...");
  //       // motorBase.rotateDegrees(degrees, TOTAL_STEPS, STEP_DELAY_US);
  //       motorEslabon1.rotateDegrees(degrees, TOTAL_STEPS, STEP_DELAY_US);
  //       delay(500);
  //       // motorAltura.rotateDegrees(degrees,TOTAL_STEPS,STEP_DELAY_US);
  //       // motorAltura.moveDistance(degrees, TOTAL_STEPS, STEP_DELAY_US);
  //       Serial.println("Hecho.");
  //       input = "";
  //     }
  //   }
  //   else
  //   {
  //     input += c;
  //   }
  // }
}

// void loop()
// {
//   if (Serial.available() > 0)
//   {

//     char comando = Serial.read();
//     if (comando == '1')
//     {

//       for (int i = 0; i <= 16; i++)
//       {
//         conteovueltas++;
//         motorAltura.vueltaCompleta(HIGH, TOTAL_STEPS);
//         Serial.print("Una vuelta completa...");
//         Serial.println(conteovueltas);
//       }
//       delay(1000);
//       motorBase.vueltaCompleta(HIGH, TOTAL_STEPS);
//       delay(1000);
//       for (int i = 16; i >= 0; i--)
//       {
//         conteovueltas--;
//         motorAltura.vueltaCompleta(LOW, TOTAL_STEPS);
//         Serial.print("Una vuelta completa...");
//         Serial.println(conteovueltas);
//       }
//       delay(1000);
//       motorBase.vueltaCompleta(LOW, TOTAL_STEPS);
//       delay(1000);
//     }
//     else if (comando == '0')
//     {
//       conteovueltas--;
//       motorAltura.vueltaCompleta(LOW, TOTAL_STEPS);
//       Serial.print("Una vuelta completa...");
//       Serial.println(conteovueltas);
//       delay(1000);
//     }
//   }
// }
// Pausa de 1 segundo
