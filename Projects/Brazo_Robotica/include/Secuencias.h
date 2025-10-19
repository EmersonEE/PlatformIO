#ifndef __SECUENCIAS_H__
#define __SECUENCIAS_H__
#include <Arduino.h>

class Secuencias
{
private:
    /* data */
public:
    // enum SECUENCIA
    // {
    //     BI_1_BD_1 = 0,
    //     BI_1_BD_2 = 1,
    //     BI_1_BD_3 = 2,
    //     BI_2_BD_1 = 3,
    //     BI_2_BD_2 = 4,
    //     BI_2_BD_3 = 5,
    //     BI_3_BD_1 = 6,
    //     BI_3_BD_2 = 7,
    //     BI_3_BD_3 = 8,
    //     BD_1_BI_1 = 9,
    //     BD_1_BI_2 = 10,
    //     BD_1_BI_3 = 11,
    //     BD_2_BI_1 = 12,
    //     BD_2_BI_2 = 13,
    //     BD_2_BI_3 = 14,
    //     BD_3_BI_1 = 15,
    //     BD_3_BI_2 = 16,
    //     BD_3_BI_3 = 17,
    // };
    
    Secuencias(/* args */);
    void setSecuencia(int sel);
    
    // Método para convertir string a SECUENCIA
    // SECUENCIA stringToSecuencia(const String& str);
};

#endif // __SECUENCIAS_H__