// #include "Secuencias.h"
// #include "gpio_M.h"

// // Configuración del motor
// const int STEPS_PER_REV = 100; // Pasos por revolución física del motor
// const int MICROSTEPS = 16;     // Microsteps configurados en el driver
// const int TOTAL_STEPS = STEPS_PER_REV * MICROSTEPS;
// int conteovueltas = 0;
// // Sensibilidad del joystick
// const int STEP_DELAY_US = 500; // Retraso entre pasos (en microsegundos)
// const int STEP_DELAY_US_HIGHT = 250;
// // Objetos

// NEMA17 motorAltura(EN_PIN, DIR_PIN, STEP_PIN);
// NEMA17 motorBase(pinENBase, pinDIRBase, pinSTEPBase);
// NEMA17 motorEslabon1(pinENEslabon1, pinDIREslabon1, pinSTEPEslabon1);
// NEMA17 motorEslabon2(pinENEslabon2, pinDIREslabon2, pinSTEPEslabon2);

// Secuencias::Secuencias(/* args */)
// {
// }

// void Secuencias::rotarMotorBase(float grados)
// {
//     motorBase.rotateDegrees(grados, TOTAL_STEPS, STEP_DELAY_US);
// }

// void Secuencias::moverMotorAltura(float altura)
// {
//     motorAltura.moveDistance(altura, TOTAL_STEPS, STEP_DELAY_US);
// }
// void Secuencias::rotarMotorEslabon1(float grados)
// {
//     motorEslabon1.rotateDegrees(grados, TOTAL_STEPS, STEP_DELAY_US);
// }

// void Secuencias::rotarMotorEslabon2(float grados)
// {
//     motorEslabon2.rotateDegrees(grados, TOTAL_STEPS, STEP_DELAY_US);
// }
// void Secuencias::setSecuencia(int sel)
// {
//     switch (sel)
//     {
//     case 0:
//         Serial.println("Primera Secuencia");
//         for (int i = 0; i < 100; i++)
//         {
//             Serial.println(i);
//             delay(100);
//         }
//         break;
//     case 1:
//         Serial.println("Segunda Secuencia");
//         // do something
//         break;
//     }
// }

// void Secuencias::moverMotor(MOTOR sel)
// {
//     switch (sel)
//     {
//     case MOTOR::BASE:
//         break;

//     default:
//         break;
//     }
// }
