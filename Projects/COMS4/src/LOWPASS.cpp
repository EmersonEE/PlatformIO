#include "LOWPASS.h"

Filters::Filters(float alpha)
{
    EMA_ALPHA = alpha;
    EMA_LP = 0;
    EMA_HP = 0;
}

int Filters::EMALowPassFilter(int value)
{
    EMA_LP = EMA_ALPHA * value + (1 - EMA_ALPHA) * EMA_LP;
    return EMA_LP;
}

int Filters::EMAHighPassFilter(int value)
{
    EMA_LP = EMA_ALPHA * value + (1 - EMA_ALPHA) * EMA_LP;
    EMA_HP = value - EMA_LP;
    return EMA_HP;
}
