#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__


#include <Arduino.h>

class JOYSTICK
{
public:
    // PinX - PinY - PinSW
    JOYSTICK(uint8_t _pinX, uint8_t _pinY, uint8_t _pinSW);

    // Métodos públicos
    int readX();                // Leer el valor del eje X
    int readY();                // Leer el valor del eje Y
    bool isPressed();           // Verificar si el botón está presionado
    bool wasPressed();          // Detectar si el botón fue presionado (transición)
    bool wasReleased();         // Detectar si el botón fue liberado (transición)
    int mapX(int min, int max); // Mapear el valor del eje X a un rango personalizado
    int mapY(int min, int max); // Mapear el valor del eje Y a un rango personalizado

private:
    // Pines del joystick
    uint8_t pinX;
    uint8_t pinY;
    uint8_t pinSW;

    // Variables internas para manejar el botón
    bool lastButtonState;                   // Último estado registrado del botón
    unsigned long lastDebounceTime;         // Tiempo del último cambio en el botón
    const unsigned long debounceDelay = 50; // Retardo para eliminar rebotes
    int valorSW = HIGH;                     // Estado actual del botón
    int lastSWState = HIGH;                 // Último estado registrado del botón
};

#endif // __JOYSTICK_H__