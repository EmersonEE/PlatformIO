// #include "Tiras.h"

// Tiras::Tiras(uint8_t _numP, uint8_t _ledP)
//     : numP(_numP), ledP(_ledP), tira(_numP, _ledP, NEO_GRB + NEO_KHZ800)
// {
// }

// void Tiras::init()
// {
//     tira.begin();
//     tira.setBrightness(255);
// }

// void Tiras::effectLED(uint8_t R, uint8_t G, uint8_t B, unsigned long time)
// {
//     tira.clear();
//     for (int i = 0; i < numP; i++)
//     {
//         tira.clear();
//         tira.setPixelColor(i, tira.Color(R, G, B));
//         tira.show();
//         delay(time);
//     }
// }
