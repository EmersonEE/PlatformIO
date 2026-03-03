#include <Arduino.h>

char receivedChar;
boolean newData = false;

void recvOneChar()
{
  if (Serial.available() > 0)
  {                               // Check if a character has arrived in the buffer
    receivedChar = Serial.read(); // Read the character and store it
    newData = true;
  }
}

void showNewData()
{
  if (newData == true)
  {
    Serial.println(receivedChar);
    newData = false;
  }
}

void enviarSecuencia()
{

  int datos[5] = {1, 0, 1, 1, 0};

  Serial.print("$");

  Serial.print("{\"secuencia\":[");
  for (int i = 0; i < 5; i++)
  {
    Serial.print(datos[i]);
    if (i < 4)
      Serial.print(",");
  }
  Serial.print("]}");

  Serial.print("#");
}

void setup()
{
  Serial.begin(9600); // Start serial communication at 9600 baud
}

void loop()
{

  if (Serial.available())
  {
    char c = Serial.read();

    if (c == 'a')
    {
      enviarSecuencia();
    }
  }
}
