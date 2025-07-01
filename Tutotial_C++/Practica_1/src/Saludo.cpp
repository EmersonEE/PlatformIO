#include <Saludo.h>

Saludo::Saludo()
{
}

void Saludo::saludo()
{
    Serial.println("Hola Hijo de Puta");
}

void Saludo::saludoPersonalizado(String mensaje)
{
    Serial.println(mensaje);
}

void Saludo::duplicarNumero(int *num)
{
    *num = *num * 2;
}

void Saludo::incrementarNumeros(int &num)
{
    num++;
}

Saludo::~Saludo()
{
}
