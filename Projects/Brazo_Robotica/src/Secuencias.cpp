#include "Secuencias.h"

Secuencias::Secuencias(/* args */)
{
}

void Secuencias::setSecuencia(int sel)
{
    switch (sel)
    {
    case 0:
        Serial.println("Primera Secuencia");
        for (int i = 0; i < 100; i++)
        {
        Serial.println(i);
        delay(100);
        }
        
        break;
    case 1:
        Serial.println("Segunda Secuencia");
        // do something
        break;

    }
}
