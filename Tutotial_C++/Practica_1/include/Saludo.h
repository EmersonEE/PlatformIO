#ifndef __SALUDO_H__
#define __SALUDO_H__

#include <Arduino.h>

class Saludo
{
private:
public:
    Saludo();
    void saludo();
    void saludoPersonalizado(String mensaje);
    // Punteros y Referencias
    void duplicarNumero(int *num);
    void incrementarNumeros(int &num);
    float leer(){
        return 25.25;
    }
    ~Saludo();
};

#endif // __SALUDO_H__