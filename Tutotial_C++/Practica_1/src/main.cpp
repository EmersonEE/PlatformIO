#include <Arduino.h>
#include "Saludo.h"

Saludo saludo;
const String mensaje = "Hola Mundo";
int x = 3;
int y = 3;
int array[5] ={1,2,3,4,5};
void setup()
{
  Serial.begin(115200);
  saludo.saludo();
  Serial.println();
  Serial.println(mensaje);
  for (int i = 0; i < 5; i++)
  {
    saludo.duplicarNumero(&x);
    Serial.println(x);
  }

  for (int j = 0; j < 10; j++)
  {
    saludo.incrementarNumeros(y);
    Serial.println(y);
  }
  Saludo *sensorPtr = new Saludo();
  Serial.println(sensorPtr->leer());
  for (int &elemento: array)
  {
    elemento *= 2;
    
  }
  Serial.println(array[4]);

  
}

void loop()
{
}