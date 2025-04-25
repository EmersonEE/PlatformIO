#include "LDR.h"
#include "DataReceiver.h"
#include "NEMA17.h"
#include "JOYSTICK.h"
#include "DEF.h"
#include "LED.h"
#include "PantallitaXD.h"
#include "PID.h"
LED leds(LED_LDR, LED_CONF);
// Crear instancias de LDR
LDR ldr1(LDR_SI);
LDR ldr2(LDR_II);
LDR ldr3(LDR_SD);
LDR ldr4(LDR_ID);
float promedioUp = 0;
float promedioDown = 0;
float promedioLeft = 0;
float promedioRight = 0;
int pasosEjeY = 155;
int pasosEjeX = 0;
int direccion = 0;
DataReceiver receiver(ldr1, ldr2, ldr3, ldr4);
NEMA17 motorBase(ENA, DIR, STEP);
NEMA17 motorVertical(ENA_V, DIR_V, STEP_V);
JOYSTICK joystick(VRX_PIN, VRY_PIN, SW_PIN);
bool configuracion = false;
PantallitaXD pantallita;
uint16_t adcValues[4];
//// PID

PID pid(1.0f, 0.0f, 0.0f);

void setup()
{
    Serial.begin(115200);
    pantallita.init();
}

void loop()
{
    // Leer los valores del joystick
    int valorX = joystick.mapY(0, 255);
    int valorY = joystick.mapX(0, 255);
    receiver.update();
    promedioUp = receiver.getLDRAverageUp();
    promedioDown = receiver.getLDRAverageDown();
    promedioLeft = receiver.getLDRAverageLeft();
    promedioRight = receiver.getLDRAverageRight();
    pantallita.printData(promedioUp, promedioDown, promedioLeft, promedioRight, pasosEjeX, pasosEjeY, &direccion);
    if (joystick.wasPressed())
    {
        configuracion = !configuracion;
        if (configuracion)
        {
            Serial.println("Entrando en modo configuración");
        }
        else
        {
            Serial.println("Saliendo de modo configuración");
        }
    }
    if (configuracion)
    {
        leds.ledConf();
        motorBase.moveAndStopMotor(valorX, -360, 360, pasosEjeX, false, &direccion);
        motorVertical.moveAndStopMotor(valorY, 0, 300, pasosEjeY, true, &direccion);
    }
    if (!configuracion)
    {
        leds.ledLDR();
        motorBase.moveAndStopMotorLDR(promedioLeft, promedioRight, 0.20, 0.50, -360, 360, pasosEjeX, false, &direccion);
        motorVertical.moveAndStopMotorLDR(promedioDown, promedioUp, 0.20, 0.50, 0, 300, pasosEjeY, true, &direccion);
    }
}

// // }
// if (valorX > 175 && pasosEjeX < 360)
// {
//     motorBase.moveRight();
//     pasosEjeX++;
//     motorBase.motorStop(pasosEjeX);
//     Serial.println(pasosEjeX);
// }
// else if (valorX < 75 && pasosEjeX > -360)
// {
//     motorBase.moveLeft();
//     pasosEjeX--;
//     motorBase.motorStop(pasosEjeX);
//     Serial.println(pasosEjeX);
// }
// else
// {
//     motorBase.motorStop(pasosEjeX);
// }
// if (valorY > 175 && pasosEjeY < 300)
// {
//     motorVertical.moveUp();
//     pasosEjeY++;
//     Serial.println(pasosEjeY);
//     motorVertical.motorStop(pasosEjeY);
// }
// else if (valorY < 75 && pasosEjeY > 0)
// {
//     motorVertical.moveDown();
//     pasosEjeY--;
//     Serial.println(pasosEjeY);
//     motorVertical.motorStop(pasosEjeY);
// }
// else
// {
//     motorVertical.motorStop(pasosEjeY);
// }
// if ((promedioLeft - promedioRight > 0.20) && pasosEjeX > -360)
// {
//     motorBase.moveLeft();
//     pasosEjeX--;
//     motorBase.motorStop(pasosEjeX);
// }
// else if ((promedioRight - promedioLeft > 0.50) && pasosEjeX < 360)
// {
//     motorBase.moveRight();
//     pasosEjeX++;
//     motorBase.motorStop(pasosEjeX);
// }
// else
// {
//     motorBase.motorStop(pasosEjeX);
// }
// if (promedioDown - promedioUp > 0.20)
// {
//     motorVertical.moveUp();
// }
// else if (promedioUp - promedioDown > 0.50)
// {
//     motorVertical.moveDown();
// }
// else
// {
// }
// float promedioSI = ldr1.readVoltage();  // Superior Derecha; 0
// float promedioSD = ldr2.readVoltage();  // Superior Izquierda 1
// float promedioID = ldr3.readVoltage(); // Inferior Derecha 2
// float promedioII = ldr4.readVoltage();    // Inferior IZquierda 3
// Serial.print("LDR Superior Izquierda: ");
// Serial.print(promedioSI);
// Serial.print(" , ");
// Serial.print("LDR Superior Derecha: ");
// Serial.print(promedioSD);
// Serial.print(" , ");
// Serial.print("LDR Inferior Derecha: ");
// Serial.print(promedioID);
// Serial.print(" , ");
// Serial.print("LDR Inferior Izquierda: ");
// Serial.println(promedioII);
// delay(100);