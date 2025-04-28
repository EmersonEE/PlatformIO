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
float lecAUp, LecADown, lecALeft, lecARight = 0;
float promedioUp = 0;
float promedioDown = 0;
float promedioLeft = 0;
float promedioRight = 0;
int pasosEjeY = 155;
int pasosEjeX = 0;
int direccion = 0;
int gradosY, gradosX = 0;
DataReceiver receiver(ldr1, ldr2, ldr3, ldr4);
NEMA17 motorBase(ENA, DIR, STEP);
NEMA17 motorVertical(ENA_V, DIR_V, STEP_V);
JOYSTICK joystick(VRX_PIN, VRY_PIN, SW_PIN);
bool configuracion = false;
PantallitaXD pantallita;
PID pid(1.0f, 0.0f, 0.0f);

void SolarTrackerSetup()
{
    Serial.begin(115200);
    pantallita.init();
}

void SolarTrackerMain()
{
    // Leer los valores del joystick
    int valorX = joystick.mapY(0, 255);
    int valorY = joystick.mapX(0, 255);
    receiver.update();
    promedioUp = receiver.getLDRAverageUp();
    promedioDown = receiver.getLDRAverageDown();
    promedioLeft = receiver.getLDRAverageLeft();
    promedioRight = receiver.getLDRAverageRight();
    gradosY = pantallita.StepsToDegrees(&pasosEjeY, MIN_STEP_Y, MAX_STEP_Y, -90, 90);
    gradosX = pantallita.StepsToDegrees(&pasosEjeX, MIN_STEP_X, MAX_STEP_X, 0, 360);

    if (lecAUp != promedioUp || LecADown != promedioDown || lecALeft != promedioLeft || lecARight != promedioRight)
    {
        leds.ledConf();
        lecALeft = promedioLeft;
        lecARight = promedioRight;
        lecAUp = promedioUp;
        LecADown = promedioDown;
    }
    else
    {
        leds.ledLDR();
    }
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
        // leds.ledConf();
        motorBase.moveAndStopMotor(valorX, MIN_STEP_X, MAX_STEP_X, pasosEjeX, false, &direccion);
        motorVertical.moveAndStopMotor(valorY, MIN_STEP_Y, MAX_STEP_Y, pasosEjeY, true, &direccion);
        pantallita.printConfMode(gradosX, gradosY, &direccion);
    }
    if (!configuracion)
    {
        // leds.ledLDR();
        motorBase.moveAndStopMotorLDR(promedioLeft, promedioRight, 0.20, 0.50, MIN_STEP_X, MAX_STEP_X, pasosEjeX, false, &direccion);
        motorVertical.moveAndStopMotorLDR(promedioDown, promedioUp, 0.20, 0.50, MIN_STEP_Y, MAX_STEP_Y, pasosEjeY, true, &direccion);
        pantallita.printData(promedioUp, promedioDown, promedioLeft, promedioRight, gradosX, gradosY, &direccion);
    }
}
