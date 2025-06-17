#ifndef __MSM_H__
#define __MSM_H__

#include <Arduino.h>
#include <PantallitaXD.h>
class MSM
{
public:
    MSM();
    void MSM_init();
    void manejarMensajeSuscribirse(const String &payload);
    void manejarMensajePantalla(const String &payload);
    void MensajeMQTT(String topic, String payload);

private:
    PantallitaXD pantalla;
    uint8_t ledazul = 14;
    String payload_anterior = "";
};

#endif // __MSM_H__